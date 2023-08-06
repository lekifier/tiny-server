#pragma once

#include<vector>
#include<unordered_map>
#include<memory>

namespace tinyserver
{
class Channel;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using EventList = std::vector<Channel*>;
using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;
using ChannelList = std::vector<Channel*>;
using ChannelMap = std::unordered_map<int, Channel*>;
}