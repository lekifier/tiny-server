#include<thread/ReactorThread.hpp>
#include<iostream>

namespace tinyserver
{

ReactorThread::ReactorThread(const tinyserver::ThreadInitCallback& cb,
                             const std::string& name)
    : reactor_(nullptr),
      thread_(std::bind(&ReactorThread::threadFunc, this), name),
      mutex_(),
      cond_(),
      started_(false),
      callback_(cb)
{
}

ReactorThread::~ReactorThread()
{
    if (started_)
    {
        reactor_->quit();
        thread_.join();
    }
}

Reactor* ReactorThread::start()
{
    started_ = true;
    thread_.start();
    Reactor* reactor = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (reactor_ == nullptr)
        {
            cond_.wait(lock);
        }
        reactor = reactor_;
    }
    std::cout << "EventLoopThread::startLoop() loop = " << reactor << std::endl;
    return reactor;
}

void ReactorThread::threadFunc()
{
    Reactor reactor;
    if (callback_)
    {
        callback_(&reactor);
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        reactor_ = &reactor;
        cond_.notify_one();
    }

    reactor.loop();
    std::unique_lock<std::mutex> lock(mutex_);
    reactor_ = nullptr;
}

} // namespace tinyserver   