#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "Client.hpp"
#include "service/Service.hpp"

int Client::run()
{
    /* create socket */
    int sockFd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockFd < 0)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    /* connect to socket */
    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    ::strncpy(server.sun_path, Service::SOCKET_DIR, sizeof(server.sun_path));
    if(::connect(sockFd, (struct sockaddr *)&server, strlen(server.sun_path) + sizeof(server.sun_family)))
    {
        std::cerr << "Failed to connect to server socket" << std::endl;
        return 1;
    }

    /* fork and read from stdin in the child process */
    ::pid_t pid = fork();
    if (pid < 0)
    {
        return 1;
    }
    if (pid == 0)
    {
        /* in child process, read from stdin and send to socket */
        std::cout << "Connected, press Ctrl+C to exit." << std::endl;
        std::string str;
        while (1)
        {
            std::cin >> str;
            str = str + "\n";
            ::write(sockFd, &str[0], str.length());
        }
    }
    else
    {
        /* in parent process, read from file */
        char ch;
        while (1)
        {
            ::read(sockFd, &ch, sizeof(char));
            std::cout << ch;
        }
    }

}
