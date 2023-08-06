#include <event/nio/Poller.hpp>
#include <event/Channel.hpp>

namespace tinyserver
{
    Pollor::Pollor(Reactor* reactor)
        : ownerReactor_(reactor)
    {
    }

    Pollor::~Pollor()
    {
    }

    bool Pollor::hasChannel(Channel* channel) const
    {
        auto it = channels_.find(channel->getFd());
        return it != channels_.end() && it->second == channel;
    }
} // namespace tinyserver