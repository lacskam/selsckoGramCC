#include "proto.h"

packet make_packet(
    uint8_t type,
    uint32_t source,
    uint32_t dest,
    std::string_view message
    ) {
    const uint32_t payload_size = static_cast<uint32_t>(message.size());

    std::vector<uint8_t> payload(payload_size);
    std::memcpy(payload.data(), message.data(), payload_size);

    packet_header header{
        .navi_ids = { source, dest },
        .type = type,
        .payload_size = payload_size
    };
    packet packet {
        .header = header,
        .payload =payload
    };

    return packet;
}


std::shared_ptr<std::vector<uint8_t>> serialise_packet(const packet *pkt) {

    const size_t total_size = sizeof(packet_header) + pkt->header.payload_size;
    auto buffer = std::make_shared<std::vector<uint8_t>>(total_size);

    std::memcpy(buffer->data(), &pkt->header, sizeof(packet_header));
    if (pkt->header.payload_size) {
        std::memcpy(buffer->data()+sizeof(packet_header), pkt->payload.data(), pkt->header.payload_size);
    } else {
        //LOG_WARNING_MSG("payload is empty - [serialise_packet]");
    }

    return buffer;
}
