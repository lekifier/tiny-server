#pragma once

#include <condition_variable>

#include <thread/Thread.hpp>
#include <event/Reactor.hpp>
#include <utils/Callbacks.hpp>

namespace tinyserver
{
class ReactorThread
{
private:
    Reactor* reactor_;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    bool started_;
    ThreadInitCallback callback_;

    void threadFunc();

public:
    ReactorThread(const tinyserver::ThreadInitCallback& cb = tinyserver::ThreadInitCallback(),
                  const std::string& name = std::string());
    ~ReactorThread();
    Reactor* start();
    
};


}

