#include "session.h"


void session::start(message_handler&& on_message, error_handler&& on_error) {
    this->on_message = std::move(on_message);
    this->on_error = std::move(on_error);

    async_read();
}

void session::post(uint8_t type,uint32_t source,uint32_t dest,std::string_view message) {
    LOG_DEBUG_MSG("post - [session "+std::to_string(state.session_id())+"]");
    bool idle = outgoing.empty();

    packet pkt = make_packet(type,source,dest,message);
    auto s_packet = serialise_packet(&pkt);

    outgoing.push_back(s_packet);

    if (idle) {
        async_write();
    }
}

void session::async_read() {
    LOG_DEBUG_MSG("async_read - [session "+std::to_string(state.session_id())+"]");
    async_read_header();
}



void session::async_read_header() {
    LOG_DEBUG_MSG("async_read_header - [session "+std::to_string(state.session_id())+"]");
    boost::asio::async_read(
        socket,
        boost::asio::buffer(&read_header,sizeof(read_header)),
        [ self = shared_from_this() ](error_code ec, std::size_t n) {
            self->on_header_read(ec);
        });
}

void session::on_header_read(error_code ec) {
    LOG_DEBUG_MSG("on_header_read - [session "+std::to_string(state.session_id())+"]");
    if (ec) {
        LOG_ERROR_MSG("on_header_read - [session "+std::to_string(state.session_id())+"]");
        socket.close(ec);
        on_error(ec);
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
    LOG_DEBUG_MSG("async_read_payload - [session "+std::to_string(state.session_id())+"]");
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
        on_error(ec);
        return;
    }

    std::string message(read_payload.begin(), read_payload.end());
    LOG_DEBUG_MSG("async_read_payload - [session "+std::to_string(state.session_id())+"]");
    std::cout<<read_header.navi_ids.source_id<<" " +message<<std::endl;
    on_message(read_header.type,this->state.session_id(),read_header.navi_ids.dest_id,message);

    session::async_read();
}

void session::async_write() {

    if (outgoing.empty()) return;
    LOG_DEBUG_MSG("async_write - [session "+std::to_string(state.session_id())+"]");
    auto& packet = outgoing.front();

    boost::asio::async_write(
        socket,
        boost::asio::buffer(*packet),
        [self = shared_from_this()](error_code ec, std::size_t bytes_transferred) {
            self->on_write(ec, bytes_transferred);
        });
}

void session::on_write(error_code ec, std::size_t bytes_transferred) {
    if (!ec) {
        outgoing.pop_front();
        LOG_DEBUG_MSG("on_write - [session "+std::to_string(state.session_id())+"]");
        if (!outgoing.empty()) {
            async_write();
        }
    } else {
        socket.close(ec);
        on_error(ec);
    }
}


//------------------------------------------------------------------------------------------


session::session_state& session::_state() {
    return state;
}













