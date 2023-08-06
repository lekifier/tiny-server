#include<event/Acceptor.hpp>

namespace tinyserver
{
static int createNonblocking()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0)
    {
        // LOG_SYSFATAL << "Failed in socket";
        abort();
    }
    return sockfd;
}

Acceptor::Acceptor(Reactor* reactor, const InetAddress& listenAddr, bool reuseport)
    : reactor_(reactor),
      acceptSocket_(createNonblocking()),
      acceptChannel_(reactor_, acceptSocket_.fd()),
      newConnectionCallback_(nullptr),
      listenning_(false)
{
    acceptSocket_.bind(listenAddr);
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reuseport);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}


Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::listen()
{
    listenning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0)
    {
        if (newConnectionCallback_)
        {
            newConnectionCallback_(connfd, peerAddr);
        }
        else
        {
            ::close(connfd);
        }
    }
    else
    {
        // LOG_SYSERR << "in Acceptor::handleRead";
        // Read the section named "The special problem of
        // accept()ing when you can't" in libev's doc.
        // By Marc Lehmann, author of livev.
        // LOG_SYSERR << "in Acceptor::handleRead";
        // Read the section named "The special problem of
        // accept()ing when you can't" in libev's doc.
        // By Marc Lehmann, author of livev.
        // if (errno == EMFILE)
        // {
        //     ::close(idleFd_);
        //     idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
        //     ::close(idleFd_);
        //     idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        // }
    }
}


}