#pragma once
#include <vector>
#include <string>
#include <memory>

#include <event/Reactor.hpp>
#include <thread/ReactorThread.hpp>
#include <utils/Callbacks.hpp>
#include <utils/Containers.hpp>

namespace tinyserver
{

class ReactorThreadPool 
{
private:
    Reactor *masterReactor_;
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    ThreadList threads_;
    ReactorList reactors_;
public:
    ReactorThreadPool(Reactor *masterReactor, const std::string &name);
    ~ReactorThreadPool();

    void setThreadNum(int numThreads){numThreads_ = numThreads;}
    void start(const ThreadInitCallback &cb = ThreadInitCallback());

    Reactor *getNextReactor();

    std::vector<Reactor *> getAllReactors();
    bool started() const {return started_;}
    const std::string name() const {return name_;}
    
};

} // namespace tinyserver
