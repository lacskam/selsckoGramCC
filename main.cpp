#include "server.h"

int main() {
    boost::asio::io_context io_context;
    server srv(io_context, 1488);
    srv.async_accept();
    io_context.run();
    return 0;
}
