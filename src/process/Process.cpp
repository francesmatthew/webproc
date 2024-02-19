#include <iostream>
#include <string>
#include "Process.hpp"

Process::Process(const char* const argv[], bool search_path, const char* const envp[])
{
    this->pid = ::fork();
    if (this->pid > 0)
    {
        /* parent process*/
        /* close the writing end of stdout pipe */
        this->stdout_pipe.close_write();
        /* close the reading end of stdin pipe */
        this->stdin_pipe.close_read();
    }
    else if (this->pid == 0)
    {
        /* In child process */
        /* close the reading end of stdout pipe */
        this->stdout_pipe.close_read();
        /* close the writing end of stdin pipe */
        this->stdin_pipe.close_write();
        /* rename stdin and stdout file descriptors to those of the pipes */
        if (::dup2(this->stdin_pipe.read_fd(), STDIN_FILENO) ||
            ::dup2(this->stdout_pipe.write_fd(), STDOUT_FILENO)
        )
        {
            std::runtime_error("Failed to redirect stdin/stdout");
        }
        /* close old file descriptors that got reassigned */
        this->stdin_pipe.close_read();
        this->stdout_pipe.close_write();

        /* Run the executable */
        int result;
        if (search_path)
        {
            if (envp != 0) result = execvpe(argv[0], const_cast<char* const*>(argv), const_cast<char* const*>(envp));
            else result = execvp(argv[0], const_cast<char* const*>(argv));
        }
        else
        {
            if (envp != 0) result = execve(argv[0], const_cast<char* const*>(argv), const_cast<char* const*>(envp));
            else result = execv(argv[0], const_cast<char* const*>(argv));
        }
        if (result == -1)
        {
            /* Note: no point writing to stdout here, it has been redirected */
            std::cerr << "Error: Failed to launch program" << std::endl;
            ::exit(1);
        }
    }
    else
    {
        /* fork error */
        std::runtime_error("Failed to fork child process");
    }
}

const Process& operator<<(const Process& proc, std::string str)
{
    (void)::write(proc.stdin_pipe.write_fd(), &str[0], str.length());
    return proc;
}

const Process& operator<<(const Process& proc, char ch)
{
    (void)::write(proc.stdin_pipe.write_fd(), &ch, 1);
    return proc;
}
