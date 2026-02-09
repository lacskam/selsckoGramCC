#ifndef SERVER_H
#define SERVER_H

#include <optional>
#include <unordered_set>
#include "session.h"

class server {
public:
    server(boost::asio::io_context& io_context, std::uint16_t port) :
        io_context(io_context),
        acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {}

    void async_accept();

    void post(uint8_t type,uint32_t source,uint32_t dest,const std::string& message);

private:
    boost::asio::io_context& io_context;
    tcp::acceptor acceptor;
    std::optional<tcp::socket> socket;
    std::unordered_set<std::shared_ptr<session>> clients;
};

#endif // SERVER_H
