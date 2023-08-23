#include <event/Reactor.hpp>

thread_local tinyserver::Reactor* t_reactorInThisThread = nullptr;

namespace tinyserver
{
const int kPollTimeMs = 10000;
int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        // LOG_SYSERR << "Failed in eventfd";
        abort();
    }
    return evtfd;
}
Reactor::Reactor()
    : threadId_(CurrentThread::tid()),
      running_(false),
      quit_(false),
      poller_(new EpollPoller(this)),
      wakeupFd_(createEventfd()),
      wakeupChannel_(new Channel(this, wakeupFd_))
{
    if (t_reactorInThisThread)
    {
        // LOG_FATAL << "Another Reactor " << t_reactorInThisThread
        //           << " exists in this thread " << threadId_;
    }
    else
    {
        t_reactorInThisThread = this;
    }
}

Reactor::~Reactor()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_reactorInThisThread = nullptr;
}
void Reactor::loop()
{
    running_ = true;
    quit_ = false;
    while (!quit_)
    {
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, &activeChannels_);
        for (auto channel : activeChannels_)
        {
            channel->handleEvent();
        }
        doPendingFunctors();
    }
    running_ = false;
}
void Reactor::quit()
{
    quit_ = true;
    if (!isInReactorThread())
    {
        wakeup();
    }
}

void Reactor::runInReactor(UserEventCallback cb)
{
    if (isInReactorThread())
    {
        cb();
    }
    else
    {
        queueInReactor(std::move(cb));
    }
}
void Reactor::queueInReactor(UserEventCallback cb)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingFunctors_.push_back(std::move(cb));
    }
    if (!isInReactorThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}
void Reactor::handleRead()
{
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        // LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
}

void Reactor::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        // LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void Reactor::updateChannel(tinyserver::Channel* channel)
{
    poller_->updateChannel(channel);
}

void Reactor::removeChannel(tinyserver::Channel* channel)
{
    poller_->removeChannel(channel);
}

bool Reactor::hasChannel(tinyserver::Channel* channel)
{
    return poller_->hasChannel(channel);
}

void Reactor::doPendingFunctors()
{
    std::vector<UserEventCallback> functors;
    callingPendingFunctors_ = true;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for (const auto& functor : functors)
    {
        functor();
    }
    callingPendingFunctors_ = false;
}

}