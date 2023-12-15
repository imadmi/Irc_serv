/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idouni <idouni@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 19:07:04 by idouni            #+#    #+#             */
/*   Updated: 2023/12/15 14:02:33 by idouni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Irc.hpp"
#include "../headers/Channel.hpp"
#include "../headers/commands.hpp"

std::string trim(std::string &str, std::string charstotrim) {
    while (!str.empty() && (str.back() == '\r' || str.back() == '\n')) {
        str.pop_back();
    }
    return (str);
};

std::vector<std::string> parser(std::string &full_command, char dilimeter){
    std::vector<std::string> args;
    std::stringstream        stream(trim(full_command, "\r\n"));
    std::string              token;
    
    if (full_command.empty())
        return args;
    while (!std::getline(stream , token,  dilimeter).eof()){
        if (!token.empty())
            args.push_back(token);
    }
    if (!token.empty()){
        args.push_back(token);
    }
    return args;
};

long int contains_only_nums(std::string string){
    if (string.empty())
        return (-1);
    for (size_t i = 0; i < string.length(); i++){
        if (string.c_str()[0] == '+')
            continue ;
        if (!isdigit(string.c_str()[i]) || i == 10)
            return (0);
    }
    return (std::atol(string.c_str()));
};

void send_message(int client_socket, std::string message) {
    if (send(client_socket, message.c_str(), message.length(), 0) == -1)
        std::cerr << "Err: failling sending message to the client !" << std::endl;
    usleep(300);
};


int main(int argc, char *argv[]) {
    if (argc != 3){
        std::cerr << "Usage: <Ip> <port>!" << std::endl;
        return 0;    
    }
    struct sockaddr_in       addr;
    std::string              message;
    std::vector<std::string> args;
    std::string              play;
    char                     buff[512];
    int                      read_bytes = 0;


    bzero(&addr, sizeof(addr));

    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port        = htons(contains_only_nums(argv[2]));

    int socket_end = socket(PF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);

    if (connect(socket_end, (struct sockaddr *)&addr, sizeof(addr)) == -1){
        std::cerr << "Err: failling to connect socket!" << std::endl;
        return 0;
    }
    std::cout << socket_end << std::endl;    
    std::cout << "BOT CONNECTED TO TE SERVER SUCCESSFULLY !" << std::endl;    

    send_message(socket_end, "PASS pass\r\n");
    send_message(socket_end, "NICK Emet\r\n");
    send_message(socket_end, "USER 1 0 *\r\n");
    read_bytes = recv(socket_end, buff, 512, 0);
    buff[read_bytes] = '\0';
    play = buff;
    if (play.find("001 Emet :Welcome Emet") != std::string::npos)
        std::cout << "BOT LOGED TO THE SERVER SUCCESSFULLY !" << std::endl;
    else{
        std::cout << "PROBLEM LOGING TO THE SERVER  !" << std::endl;
        close(socket_end);
        return (1);
    }

    while (true){
        read_bytes = recv(socket_end, buff, 512, 0);
        if (read_bytes > 0){
            std::cout << "PING" << std::endl;
            buff[read_bytes] = '\0';
            play = buff;
            if (play.find("MSG_TO_SD") != std::string::npos){
                play.erase(0, play.find("MSG_TO_SD"));
                args = parser(play, ' ');
                if (args.size() > 1){
                    message = "PRIVMSG " + args[1] + " :You have been pinged, you are too noisy a zamel \r\n";
                    send_message(socket_end, message);
                }
            }
        }
        else if (!read_bytes){
            std::cout << "SOCKET CLOSED !" << std::endl;
            close(socket_end);
            break ;
        }
        else if (read_bytes == -1){
            std::cout << "RECV FAILED !" << std::endl; 
            close(socket_end);
            break ; 
        }
        bzero(buff, sizeof(buff));
        play.clear();
        message.clear();
    }
    

    return (0);
}