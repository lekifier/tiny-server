#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
class TCPClient{
private:
    int client_socket_;
    struct sockaddr_in server_addr_;
public:
    TCPClient(std::string ip, int port){
        client_socket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket_ < 0){
            throw std::runtime_error("socket error");
        }
        server_addr_.sin_family = AF_INET;
        server_addr_.sin_port = htons(port);
        server_addr_.sin_addr.s_addr = inet_addr(ip.c_str());
        int connect_errno = connect(client_socket_, (struct sockaddr*)&server_addr_, sizeof(server_addr_));
        if (connect_errno < 0){
            throw std::runtime_error("connect error");
            close(client_socket_);
        }
    }
    ~TCPClient(){
        close(client_socket_);
    }
    void msg_send(const std::string &msg){
        int send_errno = send(client_socket_, msg.c_str(), msg.size(), 0);
        if (send_errno < 0){
            throw std::runtime_error("send error");
        }
    }
    void msg_recv(char *buf, int len){
        int recv_errno = recv(client_socket_, buf, sizeof(buf), 0);
        if (recv_errno < 0){
            throw std::runtime_error("recv error");
        }else{
            buf[recv_errno] = '\0';
        }
    }
    void tcp_close(){
        close(client_socket_);
    }
};
//tcp client
int main(){
    TCPClient conn("0.0.0.0", 12345);
    conn.msg_send("hello");
    char buf[1024];
    conn.msg_recv(buf, sizeof(buf));
    std::cout << buf << std::endl;
    return 0;
}