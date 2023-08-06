#include <thread/ReactorThreadPool.hpp>

namespace tinyserver
{
ReactorThreadPool::ReactorThreadPool(Reactor *masterReactor, const std::string &name)
    : masterReactor_(masterReactor),
      name_(name),
      started_(false),
      numThreads_(0),
      next_(0)
{}
ReactorThreadPool::~ReactorThreadPool()
{}

void ReactorThreadPool::start(const tinyserver::ThreadInitCallback &cb) {
    started_ = true;
    for (int i = 0; i < numThreads_; ++i) {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);
        ReactorThread *t = new ReactorThread(cb, buf);
        threads_.push_back(std::unique_ptr<ReactorThread>(t));
        reactors_.push_back(t->start());
    }
}

Reactor *ReactorThreadPool::getNextReactor() {
    Reactor *reactor = masterReactor_;
    if(!reactors_.empty()){
        reactor = reactors_[next_];
        ++next_;
        if(next_ >= reactors_.size()){
            next_ = 0;
        }
    }
    return reactor;
}

std::vector<Reactor*> ReactorThreadPool::getAllReactors() {
    if (reactors_.empty()) {
        return std::vector<Reactor *>(1, masterReactor_);
    } else {
        return reactors_;
    }
}

} // namespace tinyserver

