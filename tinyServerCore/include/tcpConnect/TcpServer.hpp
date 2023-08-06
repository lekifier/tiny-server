#pragma once

#include <event/Acceptor.hpp>
#include <tcpConnect/InetAddress.hpp>
#include <tcpConnect/TcpConnection.hpp>

namespace tinyserver
{
class TcpServer
{
private:
    const std::string name_;
    const std::string ipPort_;
    Reactor* reactor_;
    std::unique_ptr<Acceptor> acceptor_;

    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
};  
} // namespace tinyserver
