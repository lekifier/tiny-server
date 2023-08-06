#pragma once
#include <unordered_map>
#include <utils/Containers.hpp>

namespace tinyserver
{
class Reactor;

class Pollor
{
private:
    Reactor* ownerReactor_;

protected:
    ChannelMap channels_;

public:
    Pollor(Reactor* reactor);
    virtual ~Pollor();

    virtual void poll(int timeoutMs, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    virtual bool hasChannel(Channel* channel) const;

};
    
} // namespace tinyserver