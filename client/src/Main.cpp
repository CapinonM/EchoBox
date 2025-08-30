#include "../include/nlohmann/json.hpp"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using json = nlohmann::json;

void ping_server();
void connect_user();
void initialize();
void disconnect(int sock, int user_id);
void handle_user_messages(const int &user_id, int sock);
int handle_user_command(const std::string &command);
int handle_input();
int assign_user_id(int sock);
std::string get_username();

int main()
{
    initialize();
}

int handle_input()
{
    std::unordered_map<std::string, int> commands =
    {
        {"/connect", 1},
        {"/isserverup", 2} // Todo
    };

    std::string user_input = "";
    int command_id = 0;
    bool is_valid = false;

    while ( !is_valid )
    {
        std::getline( std::cin, user_input );

        if ( user_input.size() > 20 )
        {
            std::cout << "The text entered is too long, try again." << std::endl;
            continue;
        }
        auto it = commands.find(user_input);
        if ( it != commands.end() )
        {
            command_id = it->second;
            break;
        }
        else
        {
            std::cout << "Commands not found, please try again" << std::endl;
        }
    }
    return command_id;
}

void initialize()
{
    int user_input = 0;
    bool running = true;

    while ( running )
    {
        user_input = handle_input();

        if ( user_input == 1 )
        {
            connect_user();
        }
        else if ( user_input == 2 )
        {
            ping_server();
        }
    }

}

void connect_user()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(7878);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    connect(sock, (sockaddr*)&server, sizeof(server));
    std::cout << "Connecting to the server!" << std::endl;
    std::string username = get_username();

    json msg;
    msg["NewUser"] = {{"username", username}, {"user_color", 000000}};
    std::string msg_str = msg.dump();
    send(sock, msg_str.c_str(), msg_str.size(), 0);
    
    int user_id = assign_user_id(sock);
    
    if ( user_id == -1 )
    {
        std::cout << "User ID could not be obtained" << std::endl;
        close(sock);
        return;
    }

    std::cout << "Connected!!" << std::endl;

    handle_user_messages(user_id, sock);
}

std::string get_username()
{
    std::string user_input = "";
    while ( true )
    {
        std::cout << "Enter your username: " << std::endl;
        std::getline(std::cin, user_input);
        if ( (int)user_input.size() > 10 )
        {
            std::cout << "The text entered is too long, try again." << std::endl;
            continue;
        }
        else
        {
            int underscore_counter = 0;
            bool spaced = false;
            for ( size_t i=0; i < user_input.size(); ++i )
            {
                if ( user_input[i] == ' ' )
                {
                    spaced = true;
                }
                else if ( user_input[i] == '_' )
                {
                    underscore_counter++;
                }
            }
            if ( underscore_counter > 1 || spaced )
            {
                std::cout << "The text cannot contain spaces or more than one underscore, try again." << std::endl;
                continue;
            }
            break;
        }
    }

    return user_input;
}

void ping_server()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(7878);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    connect(sock, (sockaddr*)&server, sizeof(server));
    
    json msg;
    msg["Command"] = { {"message_type", 1} };
    std::string msg_str = msg.dump();
    send(sock, msg_str.c_str(), msg_str.size(), 0);
    // Todo: Read the ping response from the server!
    // std::cout << "Waiting for server response..." << std::endl;
    close(sock);
}

int assign_user_id(int sock)
{
    char buffer[1024] = {0};

    int bytes_received = recv(sock, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) 
    {
        std::cerr << "Error: Could not receive ID from the server" << std::endl;
        return -1;
    }

    try 
    {
        std::string response(buffer, bytes_received);
        json msg = json::parse(response);

        if ( msg.contains("AssignedId") )
        {
            return msg["AssignedId"]["user_id"].get<int>();
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }

    return -1;
}

void handle_user_messages(const int &user_id, int sock)
{
    std::string message = "";
    while ( true )
    {
        std::getline(std::cin, message);
    
        if ( message[0] == '/')
        {
            int command = handle_user_command(message);
            if ( command == 1 )
            {
                disconnect(sock, user_id);
                return;
            }
            continue;
        }

        json msg;
        msg["Chat"] = { {"user_id", user_id}, {"content", message } };
        std::string msg_str = msg.dump();
        send(sock, msg_str.c_str(), msg_str.size(), 0);
    }
    close(sock);
}

int handle_user_command(const std::string &command)
{
    std::unordered_map<std::string, int> command_list = {
        {"/disconnect", 1}
    };
    int command_id = 0;

    auto it = command_list.find(command);
    if ( it != command_list.end() )
    {
        command_id = it->second;
    }

    return command_id;
}

void disconnect(int sock, int user_id)
{
    std::cout << "Goodbye!" << std::endl;
    json msg;
    msg["Disconnect"] = { {"user_id", user_id } };
    std::string msg_str = msg.dump();
    send(sock, msg_str.c_str(), msg_str.size(), 0);
}