#include <tcpConnect/TcpConnection.hpp>

namespace tinyserver
{
TcpConnection::TcpConnection(tinyserver::Reactor* reactor, const std::string& name, int sockfd, 
                             const tinyserver::InetAddress& localAddr, const tinyserver::InetAddress& peerAddr)
    : name_(name),
      state_(StateE::kConnecting),
      reading_(true),
      reactor_(reactor),
      socket_(std::make_unique<tinyserver::Socket>(sockfd)),
      channel_(std::make_unique<tinyserver::Channel>(reactor, sockfd)),
      localAddr_(localAddr),
      peerAddr_(peerAddr)
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection(){

}

void TcpConnection::send(const std::string &message){
    if(state_==StateE::kConnected){
        if(reactor_->isInReactorThread()){
            sendInLoop(message.data(),message.size());
        }
        else{
            reactor_->runInReactor(std::bind(&TcpConnection::sendInLoop,this,message.data(),message.size()));
        }
    }
}

void TcpConnection::sendInLoop(const void *data, size_t len){
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if(state_==StateE::kDisconnected){
        return;
    }
    if(!channel_->isWriting() && outputBuffer_.readableBytes()==0){
        nwrote = ::write(channel_->getFd(),data,len);
        if(nwrote>=0){
            remaining = len - nwrote;
            if(remaining==0 && writeCompleteCallback_){
                reactor_->queueInReactor(
                    std::bind(writeCompleteCallback_, shared_from_this()));
            }
        }
        else{
            nwrote = 0;
            if(errno!=EWOULDBLOCK){
                if(errno==EPIPE || errno==ECONNRESET){
                    faultError = true;
                }
            }
        }
    }
    if(!faultError && remaining>0){
        size_t oldLen = outputBuffer_.readableBytes();
        if(oldLen+remaining>=highWaterMark_ && oldLen<highWaterMark_ && highWaterMarkCallback_){
            reactor_->runInReactor(std::bind(highWaterMarkCallback_,shared_from_this(),oldLen+remaining));
        }
        outputBuffer_.append(static_cast<const char*>(data)+nwrote,remaining);
        if(!channel_->isWriting()){
            channel_->enableWriting();
        }
    }
}

void TcpConnection::shutdown(){
    if(state_==StateE::kConnected){
        setState(StateE::kDisconnecting);
        reactor_->runInReactor(std::bind(&TcpConnection::shutdownInLoop,this));
    }
}

void TcpConnection::shutdownInLoop(){
    if(!channel_->isWriting()){
        socket_->shutdownWrite();
    }
}

void TcpConnection::connectEstablished(){
    setState(StateE::kConnected);
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed(){
    if(state_==StateE::kConnected){
        setState(StateE::kDisconnected);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}

void TcpConnection::handleRead(){
    int saveErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->getFd(), &saveErrno);
    if(n>0){
        messageCallback_(shared_from_this(), &inputBuffer_);
    }
    else if(n==0){
        handleClose();
    }
    else{
        handleError();
    }
}

void TcpConnection::handleWrite(){
    if(channel_->isWriting()){
        int saveErrno = 0;
        ssize_t n = outputBuffer_.writeFd(channel_->getFd(), &saveErrno);
        if(n>0){
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes()==0){
                channel_->disableWriting();
                if(writeCompleteCallback_){
                    reactor_->queueInReactor(std::bind(writeCompleteCallback_, shared_from_this()));
                }
                if(state_==StateE::kDisconnecting){
                    shutdownInLoop();
                }
            }
        }
    }
}

void TcpConnection::handleClose(){
    setState(StateE::kDisconnected);
    channel_->disableAll();
    TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis);
    closeCallback_(guardThis);
}

void TcpConnection::handleError(){
    int optval;
    socklen_t optlen = sizeof(optval);
    int err = ::getsockopt(channel_->getFd(), SOL_SOCKET, SO_ERROR, &optval, &optlen);
    if(err<0){
        //LOG_SYSERR << "TcpConnection::handleError";
    }
    else{
        //LOG_ERROR << "TcpConnection::handleError [" << name_
        //          << "] - SO_ERROR = " << optval << " " << strerror_tl(optval);
    }
}

}


