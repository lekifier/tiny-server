#include <event/Channel.hpp>

namespace tinyserver{

const int Channel::kNoneEvent=0;
const int Channel::kReadEvent=EPOLLIN|EPOLLPRI;
const int Channel::kWriteEvent=EPOLLOUT;

Channel::Channel(tinyserver::Reactor* reactor,int fd)
    :reactor_(reactor),
    fd_(fd),
    events_(0),
    revents_(0),
    idx_(-1)
{}

Channel::~Channel()
{
}

void Channel::tie(const std::shared_ptr<void> &obj) {
    tie_ = obj;
    tied_ = true;
}

void Channel::update() {
    reactor_->updateChannel(this);
}

void Channel::remove() {
    reactor_->removeChannel(this);
}

void Channel::handleEvent() {
    if(tied_){
        std::shared_ptr<void> guard = tie_.lock();
        if(guard){
            handleEventWithGuard();
        }
    }
    else{
        handleEventWithGuard();
    }
}

void Channel::handleEventWithGuard() {
    //close
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
        if(closeCallback_) closeCallback_();
    }
    //error
    if(revents_ & EPOLLERR){
        if(errorCallback_) errorCallback_();
    }
    //read
    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
        if(readCallback_) readCallback_();
    }
    //write
    if(revents_ & EPOLLOUT){
        if(writeCallback_) writeCallback_();
    }
}



}