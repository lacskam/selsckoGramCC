#include "session.h"
#include <iostream>

void session::start(message_handler&& on_message, error_handler&& on_error) {
    this->on_message = std::move(on_message);
    this->on_error = std::move(on_error);
    remote_client = socket.remote_endpoint();
    async_read();
}

void session::post(uint8_t type,uint32_t source,uint32_t dest,std::string_view message) {
    bool idle = outgoing.empty();
    auto packet = make_packet(type,source,dest,message);

    outgoing.push_back(packet);

    if (idle) {
        async_write();
    }
}

void session::async_read() {
    std::cout<<"async_read\n";
    async_read_header();

}



void session::async_read_header() {
    std::cout<<"async_read_header\n";
    boost::asio::async_read(
        socket,
        boost::asio::buffer(&read_header,sizeof(read_header)),
        [ self = shared_from_this() ](error_code ec, std::size_t n) {
            self->on_header_read(ec);
        });

}

void session::on_header_read(error_code ec) {
    std::cout<<"on_header_read\n";
    if (ec) {
        std::cout<<"on_header_read ERROR\n";
        socket.close(ec);
        on_error();
        return;
    }

    read_payload.resize(read_header.payload_size);
    async_read_payload();
}


void session::async_read_payload() {
    if (read_payload.empty()) {
        async_read_header();
        return;
    }

    boost::asio::async_read(
        socket,
        boost::asio::buffer(read_payload.data(), read_payload.size()),
        [self = shared_from_this()](error_code ec, std::size_t) {
            self->on_payload_read(ec);
        });
}


void session::on_payload_read(error_code ec) {
    if (ec) {
        socket.close(ec);
        on_error();
        return;
    }


    std::string message(read_payload.begin(), read_payload.end());

    std::cout<<read_header.navi_ids.source_id<<" " +message<<std::endl;
    on_message(read_header.type,remote_client.port(),read_header.navi_ids.dest_id,message);

    session::async_read();
}

void session::async_write() {

    if (outgoing.empty()) return;

    auto& packet = outgoing.front();

    boost::asio::async_write(
        socket,
        boost::asio::buffer(*packet),
        [self = shared_from_this()](error_code ec, std::size_t bytes_transferred) {
            self->on_write(ec, bytes_transferred);
        });
}

void session::on_write(error_code error, std::size_t bytes_transferred) {
    if (!error) {
        outgoing.pop_front();

        if (!outgoing.empty()) {
            async_write();
        }
    } else {
        socket.close(error);
        on_error();
    }
}



