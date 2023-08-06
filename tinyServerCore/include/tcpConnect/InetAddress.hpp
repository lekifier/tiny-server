#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <string.h>


namespace tinyserver {

class InetAddress {
private :
    struct sockaddr_in addr_;
public:
    explicit InetAddress(uint16_t port = 0, std::string ip = "0.0.0.0");
    explicit InetAddress(const struct sockaddr_in& addr) : addr_(addr) {}

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const sockaddr_in *getSockAddrInet() const { return &addr_; }
    void setSockAddrInet(const sockaddr_in& addr) { addr_ = addr; }
};

}