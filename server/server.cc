#include "server.h"
#include <iostream>
#include "db_proccessing.h"

void server::async_accept() {
    socket.emplace(io_context);
    db_conection db;

    acceptor.async_accept(*socket, [&](error_code error) {
        auto client = std::make_shared<session>(std::move(*socket));
        client->post(BRCAST,0,0,"welcome to chat\n\r");

        broadcast(BRCAST,0,0,"we have a newcomer\n\r");

        LOG_INFO_MSG("new client - [server::async_accept]");
        client->_state().set_session_id(++id_count);
        auto res = clients.insert({client->_state().session_id(), client});
        if (!res.second) {
            LOG_WARNING_MSG("session with this id is busy - [server::async_accept]");
        }

        client->start(
            [this](uint8_t type,uint32_t source,uint32_t dest,const std::string &message) {
                switch (type) {
                case BRCAST:
                    broadcast(type, source, dest, message);
                    break;
                case USMESSAGE:
                    post(type, source, dest, message);
                    break;
                default:
                    break;
                }

            },
            [&, weak = std::weak_ptr(client)](boost::system::error_code& ec) {
                LOG_ERROR_MSG(ec.message() + " [CODE]: " + std::to_string(ec.value()));
                if (auto shared = weak.lock();
                    shared && clients.erase(shared->_state().session_id())) {
                    // post(servmessage,0,0,"We are one less\n\r");
                }
            });

        async_accept();
    });
}

void server::post(uint8_t type,uint32_t source,
                  uint32_t dest,const std::string& message)
{
    if (clients.contains(dest)) {
        clients.at(dest)->post(type,source,dest,message);
    } else LOG_ERROR_MSG("dest is not found - [server::post]");
}


void server::broadcast(uint8_t type,uint32_t source,
                       uint32_t dest,const std::string& message)
{
    for (auto& client : clients) {
        client.second->post(type,source,dest,message);
    }
}
