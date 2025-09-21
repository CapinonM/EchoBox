#ifndef CUSTOMSOCKET_HPP
#define CUSTOMSOCKET_HPP

#include <string>
#include <arpa/inet.h>

class CustomSocket
{
public:
    int socket_fd;
    sockaddr_in serv_addr;
    CustomSocket(const int port, const std::string &ip);
    ~CustomSocket();
    bool connect_to_server();
    int send_data(const std::string &data);
    int receive_data(char *buffer, int size);
};

#endif