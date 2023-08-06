#include <tcpConnect/Socket.hpp>

namespace tinyserver
{

Socket::~Socket()
{
    ::close(sockfd_);
}

void Socket::bind(const InetAddress& addr)
{
    int ret = ::bind(sockfd_, (sockaddr*)addr.getSockAddrInet(), sizeof(struct sockaddr_in));
    if (ret == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
}

void Socket::listen()
{
    int ret = ::listen(sockfd_, 1024);
    if (ret == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

int Socket::accept(InetAddress* addr)
{
    sockaddr_in addr_in;
    socklen_t len = sizeof(sockaddr_in);
    int connfd = ::accept4(sockfd_, (sockaddr*)&addr_in, &len, 
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0)
        addr->setSockAddrInet(addr_in);
    return connfd;
}

void Socket::shutdownWrite()
{
    if (::shutdown(sockfd_, SHUT_WR) == -1)
    {
        perror("shutdown");
        exit(EXIT_FAILURE);
    }
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                            &optval, static_cast<socklen_t>(sizeof optval));
    if (ret == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                            &optval, static_cast<socklen_t>(sizeof optval));
    if (ret == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                            &optval, static_cast<socklen_t>(sizeof optval));
    if (ret == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
                            &optval, static_cast<socklen_t>(sizeof optval));
    if (ret == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}
    
} // namespace tinyserver
