#include "server.h"
#include <iostream>
#include "db_proccessing.h"


void server::configure_ssl() {
    ssl_ctx.set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 |
        ssl::context::no_sslv3 |
        ssl::context::no_tlsv1 |
        ssl::context::no_tlsv1_1|
        ssl::context::no_tlsv1_2
        );

    ssl_ctx.use_certificate_chain_file("serv.crt");
    ssl_ctx.use_private_key_file("serv.key", ssl::context::pem);

    SSL_CTX_set_ciphersuites(
        ssl_ctx.native_handle(),
        "TLS_CHACHA20_POLY1305_SHA256"
    );

}


void server::async_accept() {

    db_conection db;
    auto s_socket = std::make_shared<ssl::stream<tcp::socket>>(io_context, ssl_ctx);

    acceptor.async_accept(s_socket->lowest_layer(), [&, s_socket](error_code error) {
        if (error) {
            LOG_ERROR_MSG(error.message()+" - [async_accept]");
            async_accept();
            return;
        }

        s_socket->async_handshake(
            ssl::stream_base::server,
            [&,s_socket](error_code ec) {

                if (!ec) {
                    LOG_DEBUG_MSG("TLS handshake success: "+std::string(SSL_get_cipher(s_socket->native_handle())));

                    auto client = std::make_shared<session>(std::move(s_socket));

                    client->post(BRCAST,1488,1488,"welcome to chat\n\r");
                    broadcast(BRCAST,1488,1488,"we have a newcomer\n\r");

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
                                case HANDSHAKE:

                                    break;
                                default:
                                    break;
                            }

                        },
                        [&, weak = std::weak_ptr(client)](const boost::system::error_code& ec) {

                            LOG_ERROR_MSG(ec.message() + " [CODE]: " + std::to_string(ec.value()));

                            if (auto shared = weak.lock();
                                shared && clients.erase(shared->_state().session_id())) {

                                // post(servmessage,0,0,"We are one less\n\r");
                            }
                        });
                }
            }
        );

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
