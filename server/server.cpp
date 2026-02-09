#include "server.h"
#include <iostream>

void server::async_accept() {
    socket.emplace(io_context);

    acceptor.async_accept(*socket, [&](error_code error) {
        auto client = std::make_shared<session>(std::move(*socket));
        client->post(1,0,0,"Welcome to chat\n\r");
        post(1,0,0,"We have a newcomer\n\r");
        std::cout<<"New client\n";
        clients.insert(client);

        client->start(
            [this](uint8_t type,uint32_t source,uint32_t dest,const std::string &message) {
                post(type, source, dest, message);
            },
            [&, weak = std::weak_ptr(client)] {
                if (auto shared = weak.lock();
                    shared && clients.erase(shared)) {
                    post(1,0,0,"We are one less\n\r");
                }
            });

        async_accept();
    });
}

void server::post(uint8_t type,uint32_t source,uint32_t dest,const std::string& message) {

    for (auto& client : clients) {
        client->post(type,source,dest,message);
    }
}



