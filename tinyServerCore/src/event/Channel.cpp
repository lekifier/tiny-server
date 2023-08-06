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

void Channel::remove() {
    reactor_->removeChannel(this);
}


}