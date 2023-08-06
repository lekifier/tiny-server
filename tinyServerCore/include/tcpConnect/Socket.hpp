#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include<utils/noncopyable.hpp>
#include<tcpConnect/InetAddress.hpp>

namespace tinyserver
{

class Socket : noncopyable
{
private:
    const int sockfd_;

public:
    explicit Socket(int sockfd) : sockfd_(sockfd) {}
    ~Socket();

    int fd() const { return sockfd_; }
    void bind(const InetAddress& addr);
    void listen();
    int accept(InetAddress* addr);

    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);
};
    
}
