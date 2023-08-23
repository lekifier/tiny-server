#pragma once

#include <atomic>
#include <mutex>
#include <sys/eventfd.h>

#include <event/nio/Poller.hpp>
#include <event/nio/EpollPoller.hpp>
#include <event/Channel.hpp>

#include <thread/CurrentThread.hpp>
#include <utils/noncopyable.hpp>
#include <utils/Callbacks.hpp>
#include <utils/Containers.hpp>

namespace tinyserver
{
class Reactor : noncopyable
{
private:
    const pid_t threadId_;
    std::atomic<bool> running_;
    std::atomic<bool> quit_;

    std::unique_ptr<tinyserver::Pollor> poller_;
    int wakeupFd_;
    std::unique_ptr<tinyserver::Channel> wakeupChannel_;

    ChannelList activeChannels_;
    std::atomic<bool> callingPendingFunctors_;
    std::vector<UserEventCallback> pendingFunctors_;
    std::mutex mutex_;

    void handleRead();
    void doPendingFunctors();

public:
    Reactor();
    ~Reactor();

    void loop();
    void quit();
    void wakeup();

    void runInReactor(UserEventCallback cb);
    void queueInReactor(UserEventCallback cb);

    void updateChannel(tinyserver::Channel* channel);
    void removeChannel(tinyserver::Channel* channel);
    bool hasChannel(tinyserver::Channel* channel);

    bool isInReactorThread() const{ return threadId_ == CurrentThread::tid();};
};
}