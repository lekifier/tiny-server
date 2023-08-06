#pragma once
#include <thread>
#include <memory>
#include <thread/CurrentThread.hpp>
#include <utils/Callbacks.hpp>

namespace tinyserver
{

class Thread
{
private:
    std::string name_;
    pid_t tid_;
    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_;
    ThreadFunc func_;
    static int numCreated_;

    void setDefalutName();

public:
    explicit Thread(ThreadFunc, const std::string &name = std::string());
    ~Thread();

    void start();
    void join();
    bool started() const { return started_; }
    pid_t tid() const { return tid_; }
    const std::string& name() const { return name_; }
    static int numCreated() { return numCreated_; }
};
    
} // namespace tinyserver
