#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>

#include "CustomSocket.hpp"
#include "Initialize.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

ClientApp::ClientApp() :
CustSock(7878, "127.0.0.1"), user_name(""), user_id(-1)
{
    if (!CustSock.connect_to_server())
    {
        std::cout << "Couldn't connect to server" << std::endl;
    }
}

void ClientApp::run_menu()
{
    int user_input = 0;
    bool running = true;

    while (running)
    {
        user_input = this->get_menu_command();

        if (user_input == 1)
        {
            this->connect_to_server();
        }
        else if (user_input == 2)
        {
            this->ping_server();
        }
        else if (user_input == 3)
        {
            break;
        }
    }

    std::cout << "Bye!" << std::endl;
}

int ClientApp::get_menu_command()
{
    std::unordered_map<std::string, int> command_list = {
        {"/join", 1},
        {"/ping", 2},
        {"/exit", 3}
    };

    std::string user_input = "";
    int command_id = 0;
    bool is_valid = false;

    while (!is_valid)
    {
        std::getline(std::cin, user_input);
        
        if (user_input.size() > 20)
        {
            std::cout << "The text entered is too long, try again." << std::endl;
            continue;
        }
        else if (user_input[0] != '/')
        {
            std::cout << "Commands need to start with \'/\', please try again" << std::endl;
            continue;
        }
        else if (user_input.empty())
        {
            std::cout << "Line can't be empty" << std::endl;
            continue;
        }
        else 
        {
            //fixed coredumped if command wasn't found
            if(!(command_list.find(user_input) != command_list.end()))
            {
                continue;
            }
            is_valid = true;
        }
    }
    
    auto it = command_list.find(user_input);
    command_id = it->second;
    
    return command_id;
}

void ClientApp::chat_loop()
{
    std::string user_input = "";
    while (true)
    {
        std::getline(std::cin, user_input);
    
        if (user_input[0] == '/' && user_input.size() < 20)
        {
            int command = this->parse_chat_command(user_input);
            
            if (command == 0)
            {
                std::cout << "Command not found" << std::endl;
            }
            else if (command == 1)
            {
                disconnect();
                return;
            }
            else
            {
                std::cout << "You typed a command!" << std::endl;
            }
            continue;
        }
        
        json msg;
        int u_id = this->user_id;
        msg["Chat"] = { {"user_id", u_id}, {"content", user_input } };
        std::string msg_str = msg.dump();

        try
        {
            CustSock.send_data(msg_str);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error sending message: " << e.what() << std::endl;
        }
    }
}

template<typename T>
bool ClientApp::parse_incoming_json(const std::vector<std::pair<std::string, std::string>> &atributes, std::string &response, std::vector<T> &declarations)
{
    json msg = json::parse(response);
    int index = 0;
    
    for (std::pair<std::string, std::string> it : atributes)
    {
        if (!msg.contains(it.first))
        {
            return false;
        }
        declarations[index] = msg[it.first][it.second].get<T>();
        index++;
    }

    return true;
}

int ClientApp::parse_chat_command(const std::string &command)
{
    std::unordered_map<std::string, int> chat_commands = {
        {"/leave", 1},
        {"/server_time", 2},
        {"/server_uptime", 3},
        {"/my_uptime", 4},
        {"/server_time", 5}
    };

    int command_id = 0;

    auto it = chat_commands.find(command);

    if (it != chat_commands.end())
    {
        command_id = it->second;
    }
    else
    {
        std::cout << "Commands not found, please try again" << std::endl;
    }

    return command_id;
}

void ClientApp::disconnect()
{
    const int u_id = this->user_id;
    json msg;
    msg["Disconnect"] = { {"user_id", u_id} };
    std::string msg_str = msg.dump();

    try
    {
        CustSock.send_data(msg_str);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
}

void ClientApp::ping_server()
{
    std::string u_name = this->user_name;
    json msg;
    msg["Command"] = { {"message_type", 1} };
    std::string msg_str = msg.dump();

    try
    {
        CustSock.send_data(msg_str);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }

    char buffer[1024] = {0};
    int bytes_received = CustSock.receive_data(buffer, sizeof(buffer));
    
    if (bytes_received <= 0)
    {
        std::cout << "Couldn't get response from server" << std::endl;
        return;
    }
    else
    {
        std::cout << "Server is up! Join using \'/join\' command!" << std::endl;
    }
}

void ClientApp::prompt_for_username()
{
    std::string user_input = "";
    bool valid_user_name = false;
    while (!valid_user_name)
    {
        std::cout << "Enter your username: " << std::endl;
        std::getline(std::cin, user_input);

        if ((int)user_input.size() > 15)
        {
            std::cout << "The text entered is too long, try again." << std::endl;
            continue;
        }

        int underscore_counter = 0;
        bool spaced = false;

        for (size_t i = 0; i < user_input.size(); ++i)
        {
            if (user_input[i] == ' ')
            {
                spaced = true;
                break;
            }
            else if (user_input[i] == '_')
            {
                underscore_counter++;
                if (underscore_counter > 2) break;
            }
        }
        if (underscore_counter > 2 || spaced)
        {
            std::cout << "The text cannot contain spaces or more than two underscores, nor have spaces, try again." << std::endl;
            continue;
        }
        break;
    }

    this->user_name = user_input;
}

void ClientApp::connect_to_server()
{
    prompt_for_username();

    assign_user_id();

    if (this->user_id < 0)
    {
        std::cout << "Couldn't get user id from server, try connecting again" << std::endl;
        return;
    }

    this->chat_loop();
}

void ClientApp::assign_user_id()
{
    std::string u_name = this->user_name;
    json msg;
    msg["NewUser"] = { {"username", u_name}, {"user_color", 000000} };
    std::string msg_str = msg.dump();

    try
    {
        CustSock.send_data(msg_str);
        std::cout << "Connected!!" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error sending message: " << e.what() << std::endl;
    }

    char buffer[1024] = {0};
    int bytes_received = CustSock.receive_data(buffer, sizeof(buffer));

    if (bytes_received <= 0)
    {
        this->user_id = -1;
        return;
    }

    std::string response(buffer, bytes_received);

    std::vector<std::pair<std::string, std::string>> atributes = {{"AssignedId", "user_id"}};
    std::vector<int> u_id = { -1 };
    
    if (this->parse_incoming_json(atributes, response, u_id))
    {
        this->user_id = u_id[0];
    }
    else
    {
        this->user_id = -1;
    }
}