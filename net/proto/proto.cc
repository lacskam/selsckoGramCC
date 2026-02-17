#include "proto.h"



std::string_view packet::get_payload() const   {
    return {
        reinterpret_cast<const char*>(payload),
        header.payload_size
    };
}


packet make_packet(
    uint8_t type,
    uint32_t source,
    uint32_t dest,
    std::string_view message
    ) {
    const uint32_t payload_size = static_cast<uint32_t>(message.size());

    packet pkt;

    packet_header header {
        .navi_ids = { source, dest },
        .type = type,
        .payload_size = payload_size
    };

    pkt.header = header;
    if (pkt.header.payload_size) [[likely]] {
        std::memcpy(pkt.payload, message.data(), payload_size);
    } else [[unlikely]] {

    }


    return pkt;
}



const std::vector<uint8_t> generate_key() {
    std::vector<uint8_t> session_key(32);
    RAND_bytes(session_key.data(), 32);
    return session_key;

}

bool encr_packet(packet *pkt, std::vector<uint8_t> key) {

    auto payload_cha = std::make_unique<uint8_t[]>(pkt->header.payload_size);



    RAND_bytes(pkt->nonce, nonce_s);

    if (!encrypt_packet(pkt->payload,pkt->header.payload_size, key, pkt->nonce, payload_cha.get(), pkt->tag)) [[unlikely]]
    {
        LOG_ERROR_MSG("Encrypt error - [encr_packet]");
        return 1;
    }

    std::memcpy(pkt->payload, payload_cha.get(), pkt->header.payload_size);

    return 0;
}

std::shared_ptr<std::vector<uint8_t>> serialise_packet(const packet *pkt) {

    constexpr const size_t header_s = sizeof(packet_header);
    const size_t total_size = header_s + pkt->header.payload_size;

    auto buffer = std::make_shared<std::vector<uint8_t>>(total_size);

    if (pkt->header.payload_size) [[likely]] {
        size_t offset = 0;

        std::memcpy(buffer->data() + offset, &pkt->header, header_s);
        offset += header_s;

        std::memcpy(buffer->data() + offset, pkt->payload, pkt->header.payload_size);

    } else [[unlikely]] {
        LOG_WARNING_MSG("payload is empty - [serialise_packet]");
    }

    return buffer;
}
