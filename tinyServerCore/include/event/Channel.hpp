#pragma once

#include <sys/epoll.h>

#include <event/Reactor.hpp>
#include <utils/Callbacks.hpp>
#include <utils/noncopyable.hpp>

namespace tinyserver
{

class Channel : noncopyable{
private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
    tinyserver::Reactor *reactor_;
    const int fd_;
    int events_;
    int revents_;
    int idx_;

    tinyserver::ReadEventCallback readCallback_;
    tinyserver::EventCallback writeCallback_;
    tinyserver::EventCallback closeCallback_;
    tinyserver::EventCallback errorCallback_;

    void update();
    void handleEventWithGuard();
public:
    Channel(tinyserver::Reactor *reactor, int fd);
    ~Channel();

    void handleEvent();
    void setReadCallback(ReadEventCallback cb){readCallback_ = std::move(cb);};
    void setWriteCallback(EventCallback cb){writeCallback_ = std::move(cb);};
    void setCloseCallback(EventCallback cb){closeCallback_ = std::move(cb);};
    void setErrorCallback(EventCallback cb){errorCallback_ = std::move(cb);};

    int getFd() const {return fd_;};
    int getEvents() const {return events_;};
    void setRevents(int revt){revents_ = revt;};

    void enableReading(){events_ |= kReadEvent; update();};
    void disableReading(){events_ &= ~kReadEvent; update();};
    void enableWriting(){events_ |= kWriteEvent; update();};
    void disableWriting(){events_ &= ~kWriteEvent; update();};
    void disableAll(){events_ = kNoneEvent; update();};

    bool isWriting() const {return events_ & kWriteEvent;};
    bool isReading() const {return events_ & kReadEvent;};
    bool isNoneEvent() const {return events_ == kNoneEvent;};

    int getIndex() const {return idx_;};
    void setIndex(int idx){idx_ = idx;};

    tinyserver::Reactor *getReactor(){return reactor_;};
    void remove();
};
}