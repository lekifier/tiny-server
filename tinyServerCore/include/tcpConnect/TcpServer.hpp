#pragma once

#include <event/Acceptor.hpp>
#include <tcpConnect/InetAddress.hpp>
#include <tcpConnect/TcpConnection.hpp>
#include <thread/ReactorThreadPool.hpp>
#include <utils/Containers.hpp>
#include <utils/Callbacks.hpp>

namespace tinyserver
{
class TcpServer
{
private:
    const std::string name_;
    const std::string ipPort_;
    Reactor* reactor_;
    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<ReactorThreadPool> threadPool_;

    ThreadInitCallback threadInitCallback_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    ConnectionMap connections_;
    std::atomic<bool> started_;
    int nextConnId_;
public:
    enum class Option
    {
        kNoReusePort,
        kReusePort
    };
    TcpServer(Reactor* reactor,
              const InetAddress& listenAddr, 
              const std::string& nameArg, 
              Option option = Option::kNoReusePort);
    ~TcpServer();

    void setThraedInitCallback(const ThreadInitCallback& cb)
    {
        threadInitCallback_ = cb;
    }

    void setConnectionCallback(const ConnectionCallback& cb)
    {
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback& cb)
    {
        messageCallback_ = cb;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    {
        writeCompleteCallback_ = cb;
    }
    void setThreadNum(int numThreads);
    void start();
};  
} // namespace tinyserver
