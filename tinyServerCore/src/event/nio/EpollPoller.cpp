#include<event/nio/EpollPoller.hpp>

namespace tinyserver
{
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EpollPoller::EpollPoller(Reactor* reactor)
    : Pollor(reactor),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize)
{
    if(epollfd_ < 0)
    {
        // LOG_FATAL << "EpollPoller::EpollPoller";
    }
}

EpollPoller::~EpollPoller()
{
    ::close(epollfd_);
}

void EpollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int savedErrno = errno;
    if(numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
        if(static_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if(numEvents == 0)
    {
        // LOG_TRACE << "nothing happended";
    }
    else
    {
        if(savedErrno != EINTR)
        {
            errno = savedErrno;
            // LOG_ERROR << "EpollPoller::poll()";
        }
    }
}

void EpollPoller::updateChannel(Channel *channel){
    // Pollor::assertInReactorThread();
    const int index = channel->getIndex();
    if(index == kNew || index == kDeleted)
    {
        int fd = channel->getFd();
        if(index == kNew)
        {
            channels_[fd] = channel;
        }
        else
        {
            // LOG_TRACE << "fd = " << fd << " channel = " << channel->name();
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        int fd = channel->getFd();
        if(!channel->isNoneEvent())
        {
            update(EPOLL_CTL_MOD, channel);
        }
        else
        {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        }
    }
}

void EpollPoller::removeChannel(Channel* channel){
    // Pollor::assertInReactorThread();
    int fd = channel->getFd();
    // LOG_TRACE << "fd = " << fd;
    int index = channel->getIndex();

    channels_.erase(fd);
    if(index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    for(int i = 0; i < numEvents; ++i)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EpollPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    ::memset(&event, 0, sizeof(event));
    event.events = channel->getEvents();
    event.data.ptr = channel;
    int fd = channel->getFd();
    if(::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if(operation == EPOLL_CTL_DEL)
        {
            // LOG_ERROR << "epoll_ctl op = " << operation << " fd = " << fd;
        }
        else
        {
            // LOG_FATAL << "epoll_ctl op = " << operation << " fd = " << fd;
        }
    }
}


}
