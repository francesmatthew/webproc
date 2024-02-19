#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <stdexcept>
#include <unistd.h>
#include <string>
#include "CPipe.hpp"

class Process
{
public:
    Process(const char* const argv[], bool search_path = false, const char* const envp[] = 0);
    friend const Process& operator<<(const Process& proc, std::string str);
    friend const Process& operator<<(const Process& proc, char ch);
    void send_eof() { this->stdin_pipe.close_write(); }

    const int stdout_fd() const { return this->stdout_pipe.read_fd(); }

private:
    int pid = -1;
    CPipe stdin_pipe, stdout_pipe;
};

#endif /* PROCESS_HPP */