#ifndef SERVER_H
#define SERVER_H

#include <unordered_map>
#include "session.h"

class server {
public:
    int id_count=0;
    server(boost::asio::io_context& io_context, std::uint16_t port) :
        io_context(io_context),
        ssl_ctx(boost::asio::ssl::context::tlsv13_server),
        acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {configure_ssl();}


    void async_accept();

    void post(uint8_t type,uint32_t source,uint32_t dest,const std::string& message);
    void broadcast(uint8_t type,uint32_t source,uint32_t dest,const std::string& message);

private:
    void configure_ssl();

    boost::asio::io_context& io_context;

    ssl::context ssl_ctx;
    tcp::acceptor acceptor;
    std::unordered_map<int, std::shared_ptr<session>> clients;
};

#endif // SERVER_H
