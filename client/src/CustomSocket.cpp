#include "CustomSocket.hpp"
#include "nlohmann/json.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

CustomSocket::CustomSocket(const int port, const std::string &ip_address)
{
    try
    {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0)
        {
            throw std::runtime_error("Socket creation failed");
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        if (inet_pton(AF_INET, ip_address.c_str(), &serv_addr.sin_addr) <= 0)
        {
            throw std::runtime_error("Invalid address / Address not supported");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "CustomSocket error: " << e.what() << std::endl;
        socket_fd = -1;
    }
}

int CustomSocket::send_data(const std::string &data)
{
    try
    {
        if (socket_fd < 0)
            throw std::runtime_error("Socket not initialized");
        int sent = send(socket_fd, data.c_str(), data.size(), 0);
        if (sent < 0)
            throw std::runtime_error("Send failed");
        return sent;
    }
    catch (const std::exception &e)
    {
        std::cerr << "CustomSocket send error: " << e.what() << std::endl;
        return -1;
    }
}

int CustomSocket::receive_data(char *buffer, int size)
{
    if (socket_fd < 0)
    {
        throw std::runtime_error("Socket not initialized");
        return -1;
    }
    try
    {
        int received = recv(socket_fd, buffer, size, 0);
        if (received < 0)
            throw std::runtime_error("Receive failed");
        return received;
    }
    catch (const std::exception &e)
    {
        std::cerr << "CustomSocket receive error: " << e.what() << std::endl;
        return -1;
    }
}

CustomSocket::~CustomSocket()
{
    if (socket_fd > 0)
        close(socket_fd);
}

bool CustomSocket::connect_to_server()
{
    try
    {
        if (socket_fd < 0)
            throw std::runtime_error("Sock_fdet not initialized");
        if (connect(socket_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)) != 0)
        {
            throw std::runtime_error("Connection to server failed");
        }
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "CustomSocket connect error: " << e.what() << std::endl;
        return false;
    }
}