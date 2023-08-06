#pragma once
#include <utils/noncopyable.hpp>
#include <utils/Callbacks.hpp>
#include <tcpConnect/Socket.hpp>
#include <event/Reactor.hpp>
#include <event/Channel.hpp>

namespace tinyserver
{
class Acceptor
{
private:
    Reactor* reactor_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;

    void handleRead();
public:
    Acceptor(Reactor* reactor, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb) { newConnectionCallback_ = cb; }
    bool listenning() const { return listenning_; }
    void listen();
};

}