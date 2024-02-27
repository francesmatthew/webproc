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
        this->stdinReadLoop(sockFd);
        ::close(sockFd);
    }
    else
    {
        /* in parent process, read from file */
        char ch;
        int bytesRead = 1;
        while (bytesRead > 0)
        {
            bytesRead = ::read(sockFd, &ch, sizeof(char));
            if (bytesRead > 0)
            {
                std::cout << ch;
            }
        }
        std::cout << "Connection closed." << std::endl;
    }
    return 0;
}

void Client::stdinReadLoop(int sockFd)
{
    char buf[100] = {0};
    std::size_t bufIndex = 0;

    int bytesRead = 0;
    char ch;
    while (1)
    {
        /* read char into buffer */
        bytesRead = ::read(STDIN_FILENO, &ch, sizeof(char));

        /* if no bytes read, there is an issue */
        if (bytesRead <= 0)
        {
            return;
        }
        /* when newline is received, send command */
        if (ch == '\n')
        {
            buf[bufIndex] = '\n';

            if (::write(sockFd, buf, bufIndex+1) < bufIndex+1)
            {
                std::cerr << "Error in writing to socket." << std::endl;
                return;
            }

            /* reset to beginning of buffer*/
            bufIndex = 0;
        }
        /* if buffer if fulled, terminate connection */
        else if (bufIndex == sizeof(buf) - 2)
        {
            std::cerr << "Line too long, will not be sent." << std::endl;
            return;
        }
        /* if char is valid ASCII, store it and go to next char */
        else if (ch >= ' ' && ch <= '~')
        {
            buf[bufIndex++] = ch;
        }
    }
}
