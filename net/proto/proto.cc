#include "proto.h"



std::string packet::get_payload()  {
    return std::string(
        reinterpret_cast<const char*>(payload),
        header.payload_size
        );
}


packet make_packet(
    uint8_t type,
    uint32_t source,
    uint32_t dest,
    std::string_view message
    ) {
    const uint32_t payload_size = static_cast<uint32_t>(message.size());

    packet pkt;

    packet_header header{
        .navi_ids = { source, dest },
        .type = type,
        .payload_size = payload_size
    };

    pkt.header = header;
    if (pkt.header.payload_size) {
        std::memcpy(pkt.payload, message.data(), payload_size);
    } else {
        LOG_WARNING_MSG("payload is empty - [make_packet]");
    }


    return pkt;
}

std::vector<uint8_t> key_g(32);

bool encr_packet(packet *pkt) {

    uint8_t payload_cha[pkt->header.payload_size];

    RAND_bytes(key_g.data(), key_g.size());
    RAND_bytes(pkt->nonce, nonce_s);

    if (!encrypt_packet(pkt->payload,pkt->header.payload_size, key_g, pkt->nonce, payload_cha, pkt->tag))
    {
        LOG_ERROR_MSG("Encrypt error - [encr_packet]");
        return 1;
    }

    std::memcpy(pkt->payload, payload_cha, pkt->header.payload_size);

    return 0;
}

std::shared_ptr<std::vector<uint8_t>> serialise_packet(const packet *pkt) {

    constexpr const size_t header_s = sizeof(packet_header);
    constexpr const size_t other_size = header_s + nonce_s + tag_s;

    const size_t total_size = other_size + pkt->header.payload_size;

    auto buffer = std::make_shared<std::vector<uint8_t>>(total_size);

    if (pkt->header.payload_size) {
        size_t offset = 0;

        std::memcpy(buffer->data() + offset, &pkt->header, header_s);
        offset += header_s;

        std::memcpy(buffer->data() + offset, pkt->nonce, nonce_s);
        offset += nonce_s;

        std::memcpy(buffer->data() + offset, pkt->tag, tag_s);
        offset += tag_s;

        std::memcpy(buffer->data() + offset, pkt->payload, pkt->header.payload_size);

    } else {
        LOG_WARNING_MSG("payload is empty - [serialise_packet]");
    }

    return buffer;
}
