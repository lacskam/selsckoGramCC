#include "proto.h"

std::shared_ptr<std::vector<uint8_t>> make_packet(
    uint8_t type,
    uint32_t source,
    uint32_t dest,
    std::string_view message
    ) {
    const uint32_t payload_size = static_cast<uint32_t>(message.size());
    const size_t total_size = sizeof(packet_header) + payload_size;

    auto buffer = std::make_shared<std::vector<uint8_t>>(total_size);

    packet_header header{
        .navi_ids = { source, dest },
        .type = type,
        .payload_size = payload_size
    };

    std::memcpy(buffer->data(), &header, sizeof(header));
    if (!message.empty()) {
        std::memcpy(buffer->data() + sizeof(header), message.data(), payload_size);
    }

    return buffer;
}

