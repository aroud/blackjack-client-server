#include <enet/enet.h>
#include "json.hpp"

#include <string>
#include <sstream>

std::string PacketToString(const ENetPacket* packet) {
    std::ostringstream oss;
    for (size_t i = 0; i < packet->dataLength; ++i) {
        oss << packet->data[i];
    }
    std::string res = oss.str();
    res.pop_back();
    return res;
}

std::string ToJson(const std::string& type, const std::string& body) {
    nlohmann::json j;
    j["type"] = type;
    j["body"] = body;
    return j.dump();
}