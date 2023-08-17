#pragma once

#include <memory>
#include <functional>

#include <utils/Containers.hpp>

namespace tinyserver
{
    class Buffer;
    class InetAddress;
    class TcpConnection;
    class Reactor;

    using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
    using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
    using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
    using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr&, size_t)>;
    using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer*)>;

    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void()>;

    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress &)>;
    using UserEventCallback = std::function<void()>;

    using ThreadFunc = std::function<void()>;
    using ThreadCallback = std::function<void(int sockfd, const InetAddress &)>;
    using ThreadInitCallback = std::function<void(tinyserver::Reactor *)>;
}