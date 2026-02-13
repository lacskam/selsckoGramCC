#ifndef PROTO_H
#define PROTO_H
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>
#include<memory>
//#include "logs.h"


typedef struct {

    uint32_t source_id;
    uint32_t dest_id;


} packet_navigation_info;

#pragma pack(push,1)

typedef struct {
    packet_navigation_info navi_ids;
    uint8_t  type;
    uint32_t payload_size;

} packet_header;

#pragma pack(pop)

typedef struct
{
    packet_header header;
    std::vector<uint8_t> payload;
} packet;

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


std::shared_ptr<std::vector<uint8_t>> serialise_packet(const packet *pkt);

#endif // PROTO_H
