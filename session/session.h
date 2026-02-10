#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <queue>
#include "../net/proto/proto.h"
#include "../logs.h"

using tcp = boost::asio::ip::tcp;
using error_code = boost::system::error_code;


using message_handler = std::function<void(uint8_t,uint32_t,uint32_t,std::string)>;
using error_handler = std::function<void(error_code&)>;


class session: public std::enable_shared_from_this<session> {
public:
    explicit session(tcp::socket&& socket)  : socket(std::move(socket))  {}

    void start(message_handler&& on_message, error_handler&& on_error);

    void post(uint8_t type, uint32_t source, uint32_t dest, std::string_view message);

private:
    void async_read();
    void async_read_header();
    void async_read_payload();

    void on_header_read(error_code ec);
    void on_payload_read(error_code ec);


    void async_write();

    void on_write(error_code error, std::size_t bytes_transferred);

    struct session_state
    {
    private:
        bool _is_active = false;
        int _session_id = -1;
    public:
        bool is_active() {return _is_active;}
        void activate() {_is_active=true;}
        void unactivate() {_is_active=false;}

        int session_id() {return _session_id;}
        void set_session_id(int id) {_session_id = id;}
    };
    session_state state;

    tcp::socket socket;

    packet_header read_header{};
    std::vector<uint8_t> read_payload;

    std::deque<std::shared_ptr<std::vector<uint8_t>>> outgoing;
    message_handler on_message;
    error_handler on_error;

public:
    session_state& _state();
};

#endif // SESSION_H
