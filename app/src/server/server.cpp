#include <iostream>
#include <tcpConnect/TcpServer.hpp>

class EchoServer{
private:
    tinyserver::Reactor *reactor_;
    tinyserver::TcpServer server_;
    void onConnection(const tinyserver::TcpConnectionPtr &conn){
        if(conn->connected()){
            std::cout << "onConnection(): new connection [" << conn->getName() << "] from " << conn->getPeerAddr().toIpPort() << std::endl;
        }else{
            std::cout << "onConnection(): connection [" << conn->getName() << "] is down" << std::endl;
        }
    }
    void onMessage(const tinyserver::TcpConnectionPtr &conn, tinyserver::Buffer *buf){
        std::cout << "onMessage(): received from connection [" << conn->getName() << "]" << std::endl;
        conn->send(buf->retrieveAllAsString());
    }
public:
    EchoServer(tinyserver::Reactor *reactor, const tinyserver::InetAddress &addr)
        : reactor_(reactor),
          server_(reactor, addr, "EchoServer")
    {
        server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(
            std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
        server_.setThreadNum(4);
    }
    void start(){
        std::cout << "Start" << server_.getServerName() << " at " << server_.getIpPort() << std::endl;
        server_.start();
    }
};
int main(){
    tinyserver::Reactor reactor;
    tinyserver::InetAddress addr(12345, "0.0.0.0");
    EchoServer server(&reactor, addr);
    server.start();
    reactor.loop();
    return 0;
}