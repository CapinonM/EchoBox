#ifndef INITIALIZE_HPP
#define INITIALIZE_HPP

#include "CustomSocket.hpp"
#include <string>
#include <utility>
#include <vector>

class ClientApp
{
public:
    ClientApp();
    void run_menu();
    int get_menu_command();
    void prompt_for_username();
    void connect_to_server();
    void chat_loop();
    void assign_user_id();
    template<typename T>
    bool parse_incoming_json(const std::vector<std::pair<std::string, std::string>> &atributes, std::string &response, std::vector<T> &declarations);
    void send_parsed_json(const std::vector<std::pair<std::string, std::string>> &data);
    int parse_chat_command(const std::string &command);
private:
    CustomSocket CustSock;
    std::string user_name;
    int user_id;
};

#endif //INITIALIZE_HPP