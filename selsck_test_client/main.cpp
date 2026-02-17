#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <deque>
#include <cstring>
#include "../net/proto/proto.h"
using tcp = boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;


class async_client : public std::enable_shared_from_this<async_client> {
public:
    async_client(boost::asio::io_context& io_context, ssl::context& ssl_ctx)
        : socket(io_context,ssl_ctx) {}

    void start(const tcp::resolver::results_type& endpoints) {
        boost::asio::async_connect(socket.lowest_layer(),
                                   endpoints,
                                   [self = shared_from_this()](boost::system::error_code ec, const tcp::endpoint&) {
                                       if (!ec) {
                                           self->socket.async_handshake(ssl::stream_base::client,
                                                                            [self](boost::system::error_code ec) {
                                                                                if (!ec) {
                                                                                    std::cout << "TLS handshake success: "
                                                                                              << SSL_get_cipher(self->socket.native_handle()) << "\n";
                                                                                    std::cout << "Connected to server!\n";
                                                                                    self->async_read_header();
                                                                                } else {
                                                                                    std::cerr << "Handshake error: " << ec.message() << "\n";
                                                                                }
                                                                            }
                                                                        );

                                       } else {
                                           std::cerr << "Connect error: " << ec.message() << "\n";
                                       }
                                   });
    }

    void send(uint8_t type,uint32_t source,
              uint32_t dest,std::string_view message) {

        packet *pkt = new packet;
        *pkt = make_packet(type,source,dest,message);

        auto s_packet = serialise_packet(pkt);
        delete pkt;

        bool idle = outgoing.empty();
        outgoing.push_back(s_packet);

        if (idle) {
            async_write();
        }
    }

private:
    ssl::stream<tcp::socket> socket;
    packet_header read_header;
    packet temp_packet;
    std::vector<uint8_t> read_payload;
    std::deque<std::shared_ptr<std::vector<uint8_t>>> outgoing;

    void async_read_header() {
        boost::asio::async_read(socket,
                                boost::asio::buffer(&temp_packet, sizeof(packet_header)),
                                [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
                                    if (!ec) {
                                        self->read_payload.resize(self->temp_packet.header.payload_size);
                                        self->async_read_payload();
                                    } else {
                                        std::cerr << "Header read error: " << ec.message() << "\n";
                                    }
                                });
    }

    void async_read_payload() {
        if (read_payload.empty()) {
            async_read_header();
            return;
        }

        boost::asio::async_read(socket,
                                boost::asio::buffer(read_payload.data(), read_payload.size()),
                                [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
                                    if (!ec) {
                                        std::string msg(self->read_payload.begin(), self->read_payload.end());
                                        std::cout << "User " << self->temp_packet.header.navi_ids.source_id <<": "<< msg << "\n";
                                        self->async_read_header();
                                    } else {
                                        std::cerr << "Payload read error: " << ec.message() << "\n";
                                    }
                                });
    }

    void async_write() {
        if (outgoing.empty()) return;

        auto& packet = outgoing.front();
        boost::asio::async_write(socket, boost::asio::buffer(*packet),
                                 [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
                                     if (!ec) {
                                         self->outgoing.pop_front();
                                         if (!self->outgoing.empty())
                                             self->async_write();
                                     } else {
                                         std::cerr << "Write error: " << ec.message() << "\n";
                                     }
                                 });
    }
};

int main() {
    try {
        boost::asio::io_context io_context;
        ssl::context ssl_ctx(ssl::context::tlsv13_client);

        ssl_ctx.set_verify_mode(ssl::verify_none);

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "1488");

        auto client = std::make_shared<async_client>(io_context,ssl_ctx);
        client->start(endpoints);


        std::thread input_thread([client]() {
            std::string line;
            while (std::getline(std::cin, line)) {
                if (!line.empty() && line[0] == ':') {
                    auto first = line.find(":");
                    auto second = line.find(":", first + 1);
                    std::string id_str = line.substr(first + 1, second - first - 1);
                    uint32_t dest_id = std::stoi(id_str);
                    std::string msg = line.substr(second + 1);
                    client->send(USMESSAGE, 0, dest_id, msg);
                } else {
                    client->send(BRCAST, 0, 0, line);
                }
            }
        });

        io_context.run();
        input_thread.join();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
