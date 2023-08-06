#pragma once

#include <atomic>
#include <memory>

#include <utils/noncopyable.hpp>
#include <event/Reactor.hpp>
#include <event/Channel.hpp>
#include <tcpConnect/InetAddress.hpp>
#include <tcpConnect/Socket.hpp>
#include <tcpConnect/Buffer.hpp>


namespace tinyserver
{
class TcpConnection : noncopyable
{
private:
    enum class StateE 
    { 
        kDisconnected, 
        kConnecting, 
        kConnected, 
        kDisconnecting 
    };
    const std::string name_;
    std::atomic<StateE> state_;
    bool reading_;

    tinyserver::Reactor* reactor_;

    std::unique_ptr<tinyserver::Socket> socket_;
    std::unique_ptr<tinyserver::Channel> channel_;

    const tinyserver::InetAddress localAddr_;
    const tinyserver::InetAddress peerAddr_;

    tinyserver::ConnectionCallback connectionCallback_;
    tinyserver::MessageCallback messageCallback_;
    tinyserver::WriteCompleteCallback writeCompleteCallback_;
    tinyserver::CloseCallback closeCallback_;
    tinyserver::HighWaterMarkCallback highWaterMarkCallback_;

    size_t highWaterMark_;

    tinyserver::Buffer inputBuffer_;
    tinyserver::Buffer outputBuffer_;

    void setState(StateE s) { state_ = s; }
    void handleRead();
    void handleWrite();
    void handleError();
    void handleClose();
    
    void sendInLoop(const void *data, size_t len);
    void shutdownInLoop();
public:
    TcpConnection(tinyserver::Reactor* reactor, const std::string& name, int sockfd, 
                  const tinyserver::InetAddress& localAddr, const tinyserver::InetAddress& peerAddr);
    ~TcpConnection();

    tinyserver::Reactor* getReactor() const { return reactor_; }
    const std::string& getName() const { return name_; }
    const tinyserver::InetAddress& getLocalAddr() const { return localAddr_; }
    const tinyserver::InetAddress& getPeerAddr() const { return peerAddr_; }

    bool connected() const { return state_ == StateE::kConnected; }
    bool disconnected() const { return state_ == StateE::kDisconnected; }

    void setConnectionCallback(const tinyserver::ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const tinyserver::MessageCallback& cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const tinyserver::WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }
    void setCloseCallback(const tinyserver::CloseCallback& cb) { closeCallback_ = cb; }
    void setHighWaterMarkCallback(const tinyserver::HighWaterMarkCallback& cb, size_t highWaterMark) 
    { 
        highWaterMarkCallback_ = cb; 
        highWaterMark_ = highWaterMark;
    }

    void connectEstablished();
    void connectDestroyed();

    void send(const std::string &message);
    void shutdown();

};
} // namespace tinyserver
