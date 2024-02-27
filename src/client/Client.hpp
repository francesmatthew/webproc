#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client
{
    public:
    Client() = default;
    int run();
    void stdinReadLoop(int sockFd);
};

#endif /* CLIENT_HPP */
