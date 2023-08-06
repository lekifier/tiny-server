#pragma once

#include <sys/epoll.h>
#include <cstring>

#include <event/Channel.hpp>
#include <event/nio/Poller.hpp>
#include <utils/Containers.hpp>

namespace tinyserver
{
class EpollPoller : public tinyserver::Pollor
{
private:
    int epollfd_;
    static const int kInitEventListSize = 16;

    std::vector<struct epoll_event> events_;

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    void update(int operation, Channel* channel);
public:
    EpollPoller(Reactor* reactor);
    ~EpollPoller() override;

    void poll(int timeoutMs, ChannelList* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;
};

}


