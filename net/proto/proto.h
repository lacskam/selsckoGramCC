#ifndef PROTO_H
#define PROTO_H
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>
#include<memory>
#include "../encry/encry.h"
#include "../../logs.h"


struct packet_navigation_info {

    uint32_t source_id;
    uint32_t dest_id;

};

#pragma pack(push,1)

typedef struct {
    packet_navigation_info navi_ids;
    uint8_t  type;
    uint32_t payload_size;

} packet_header;

#pragma pack(pop)

#pragma pack(push,1)
struct packet  {
    packet_header header;
    uint8_t nonce[12];
    uint8_t tag[16];
    uint8_t payload[4096];

    std::string get_payload();
} ;
#pragma pack(pop)


const size_t nonce_s = 12;
const size_t tag_s = 16;

enum : uint8_t
{
    SERV_MESSAGE = 0x01,
    USMESSAGE = 0x02,
    BRCAST = 0x03,


};

packet make_packet(uint8_t type,
                                 uint32_t source,
                                 uint32_t dest,
                                 std::string_view message);

bool encr_packet(packet *pkt);

std::shared_ptr<std::vector<uint8_t>> serialise_packet(const packet *pkt);



#endif // PROTO_H
