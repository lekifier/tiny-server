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

}

