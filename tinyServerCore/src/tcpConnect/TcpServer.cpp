#include <tcpConnect/TcpServer.hpp>

namespace tinyserver
{
TcpServer::TcpServer(Reactor* reactor,
                     const InetAddress& listenAddr, 
                     const std::string& nameArg, 
                     Option option)
    : reactor_(reactor),
      name_(nameArg),
      ipPort_(listenAddr.toIpPort()),
      acceptor_(std::make_unique<Acceptor>(reactor, listenAddr, option == Option::kReusePort)),
      threadInitCallback_([](Reactor*){}),
      connectionCallback_([](const TcpConnectionPtr&){}),
      messageCallback_([](const TcpConnectionPtr&, Buffer*){}),
      writeCompleteCallback_([](const TcpConnectionPtr&){})
{
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    for (auto& item : connections_)
    {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getReactor()->runInReactor(
            std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::setThreadNum(int numThreads)
{
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start()
{
    if (started_.exchange(true) == false)
    {
        threadPool_->start(threadInitCallback_);
        reactor_->runInReactor(
            std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
    Reactor *ioReactor = threadPool_->getNextReactor();

    std::string connName = name_ + "-" + peerAddr.toIpPort() + "#" + std::to_string(nextConnId_);
    ++nextConnId_;
    sockaddr_in local;
    ::memset(&local, 0, sizeof(local));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(local));
    InetAddress localAddr(local);
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(
                                reactor_, connName,
                                sockfd, localAddr, peerAddr);
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);

    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    
    ioReactor->runInReactor(
        std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    reactor_->runInReactor(
        std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    connections_.erase(conn->getName());
    Reactor *ioReactor = conn->getReactor();
    ioReactor->runInReactor(
        std::bind(&TcpConnection::connectDestroyed, conn));
}




} // namespace 
