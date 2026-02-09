#include "server.h"
#include <iostream>

void server::async_accept() {
    socket.emplace(io_context);

    acceptor.async_accept(*socket, [&](error_code error) {
        auto client = std::make_shared<session>(std::move(*socket));
        client->post("Welcome to chat\n\r");
        post("We have a newcomer\n\r");
        std::cout<<"New client\n";
        clients.insert(client);

        client->start(
            std::bind(&server::post, this, std::placeholders::_1),
            [&, weak = std::weak_ptr(client)] {
                if (auto shared = weak.lock();
                    shared && clients.erase(shared)) {
                    post("We are one less\n\r");
                }
            });

        async_accept();
    });
}

void server::post(const std::string& message) {

    for (auto& client : clients) {
        client->post(message);
    }
}



