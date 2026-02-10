#include "server.h"
#include <iostream>

void server::async_accept() {
    socket.emplace(io_context);


    acceptor.async_accept(*socket, [&](error_code error) {
        auto client = std::make_shared<session>(std::move(*socket));
        client->post(servmessage,0,0,"Welcome to chat\n\r");
        post(servmessage,0,0,"We have a newcomer\n\r");
        std::cout<<"New client\n";
        client->_state().set_session_id(++id_count);
        auto res = clients.insert({client->_state().session_id(), client});
        if (!res.second) {
            std::cout << "Session с таким id уже есть!\n";
        }

        client->start(
            [this](uint8_t type,uint32_t source,uint32_t dest,const std::string &message) {
                post(type, source, dest, message);
            },
            [&, weak = std::weak_ptr(client)](boost::system::error_code& ec) {
                LOG_ERROR_MSG(ec.message() + " [CODE]: " + std::to_string(ec.value()));
                if (auto shared = weak.lock();
                    shared && clients.erase(shared->_state().session_id())) {
                    post(servmessage,0,0,"We are one less\n\r");
                }
            });

        async_accept();
    });
}

void server::post(uint8_t type,uint32_t source,uint32_t dest,const std::string& message) {

    for (auto& client : clients) {
        client.second->post(type,source,dest,message);
    }
}



