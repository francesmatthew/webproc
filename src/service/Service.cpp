#include <unistd.h>
#include <sstream>
#include <iostream>
#include <signal.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <errno.h>
#include <syslog.h>
#include "Service.hpp"

const char* Service::SOCKET_DIR      = "/var/run/webprocd.socket";
const char* Service::PID_FILE        = "/var/run/webprocd.pid";
const char* Service::SYSLOG_IDENT    = "webprocd";

int Service::mSockFd = -1;
std::mutex Service::mBufferLock;
std::array<std::array<char, Service::mLineBufferSize>, Service::mBufferLines> Service::mBuffer {};
size_t Service::mBufferIndex = 0;
size_t Service::mLineIndex = 0;
bool Service::mLineInvalid = false;

std::size_t mLineWriteIndex = 0;

Service::Service(std::shared_ptr<Daemon> &daemon, std::shared_ptr<Logger> &logger, std::shared_ptr<Process> &process):
    mDaemon(daemon), mLogger(logger), mProcess(process)
{
}

void Service::sigint_handler(int signal)
{
    ::syslog(LOG_NOTICE, "Received SIGINT, shutting down.");
    ::shutdown(Service::mSockFd, 2);
    if (::unlink(Service::SOCKET_DIR) < 0)
    {
        ::syslog(LOG_ERR, "Failure when unlinking PID file, errno: %d", errno);
    }
    ::exit(EXIT_SUCCESS);
}

void Service::runSocket()
{
    CPipe procReadLoopReporting;
    if (this->runProcessReadLoop(procReadLoopReporting) < 0)
    {
        mLogger->Crit("Failed to fork service child process.");
        return;
    }
    procReadLoopReporting.close_read();

    /* create socket */
    int sockFd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockFd < 0)
    {
        mLogger->Crit("Failed to create socket.");
        return;
    }

    /* allow reuse */
    int option = 1;
    struct ::linger lin;
    lin.l_onoff = 1;		/* non-zero value enables linger option in kernel */
    lin.l_linger = 0;	    /* timeout interval */

    if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option))   < 0     ||
        setsockopt(sockFd, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin))            < 0     )
    {
        mLogger->Crit("Failed to set socket options.");
        return;
    }

    /* bind socket */
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    ::strncpy(addr.sun_path, Service::SOCKET_DIR, sizeof(addr.sun_path));
    if (bind(sockFd, (struct sockaddr *) &addr, strlen(addr.sun_path) + sizeof (addr.sun_family)) < 0)
    {
        std::stringstream strBuilder;
        strBuilder << "Failed to bind to socket, errno: ";
        strBuilder << errno;
        mLogger->Crit(&(strBuilder.str()[0]));
        return;
    }

    /* change socket permissions */
    if (::fchmod(sockFd, 00666) < 0)
    {
        mLogger->Crit("Failed to change socket mode");
        return;
    }

    /* listen on socket, allow queue of 5 connections */
    if (::listen(sockFd, 5) < 0)
    {
        mLogger->Crit("Failed to listen on socket.");
        return;
    }
    mSockFd = sockFd;
    ::signal(SIGINT, this->sigint_handler);

    /* close pipe to send EOF */
    procReadLoopReporting.close_write();

    /* read from connection */
    struct sockaddr_un client;
    ::socklen_t clientLen;
    while(1) /* this loop will end when this thread is killed */
    {
        /* accept a connection */
        clientLen = sizeof(client);
        int connFd = ::accept(sockFd, (struct sockaddr *) &client, &clientLen); /* blocking */
        if (connFd < 0)
        {
            std::stringstream strBuilder;
            strBuilder << "Failed to accept process, errno: ";
            strBuilder << errno;
            mLogger->Crit(&(strBuilder.str()[0]));
            continue;
        }
        else
        {
            std::stringstream strBuilder;
            strBuilder << "Accepted connection; ";
            strBuilder << "sun_family: " << client.sun_family;
            strBuilder << "; sun_path: " << client.sun_path;
            mLogger->Notice(&(strBuilder.str()[0]));
        }

        this->readConnection(connFd);
    }
}

void Service::readConnection(int connFd)
{
    char sockBuf[100];
    std::size_t sockBufIndex = 0;

    int bytesRead = 0;
    while (1)
    {
        /* read char into buffer */
        bytesRead = ::read(connFd, sockBuf+sockBufIndex, sizeof(char));

        /* if no bytes read, terminal may be empty */
        if (bytesRead <= 0)
        {
            ::close(connFd);
            return;
        }
        /* when newline is received, send command */
        if (sockBuf[sockBufIndex] == '\n')
        {
            sockBuf[sockBufIndex] = 0;

            std::stringstream strBuilder;
            strBuilder << "Sending command: \'";
            strBuilder << sockBuf;
            strBuilder << "\'";
            mLogger->Notice(&(strBuilder.str()[0]));

            sockBuf[sockBufIndex] = '\n';
            sockBuf[sockBufIndex+1] = 0;

            if (::write(mProcess->stdin_fd(), sockBuf, sockBufIndex+1) < sockBufIndex+1)
            {
                mLogger->Warn("Error in writing to process stdin pipe.");
                ::close(connFd);
                return;
            }

            /* write OK to socket */
            (void)::write(connFd, "OK.\n", 4);

            /* reset to beginning of buffer*/
            sockBufIndex = 0;
        }
        /* if buffer if fulled, terminate connection */
        else if (sockBufIndex == sizeof(sockBuf) -2)
        {
            mLogger->Notice("Connection exceeded buffer line size, terminating connection");
            ::close(connFd);
            return;
        }
        /* otherwise, get the next character */
        else
        {
            sockBufIndex++;
        }
    }
}

void Service::run()
{
    CPipe procReadLoopReporting;
    if (this->runProcessReadLoop(procReadLoopReporting) < 0)
    {
        mLogger->Crit("Failed to fork service child process.");
        return;
    }
    /* close write to send EOF */
    procReadLoopReporting.close_all();

    /* read from stdin */
    std::string str;
    while(1) /* this loop will end when this thread is killed */
    {
        std::cin >> str;

        std::stringstream strBuilder;
        strBuilder << "Sending command: \'";
        strBuilder << str;
        strBuilder << "\'";
        mLogger->Notice(&(strBuilder.str()[0]));

        str = str + "\n";

        ::write(mProcess->stdin_fd(), &str[0], str.length());
    }
}

int Service::runProcessReadLoop(CPipe &procReadLoopReporting)
{
    ::pid_t pid = ::fork();
    if (pid == 0)
    {
        /* child process */
        return 0;
    }
    else if (pid < 0)
    {
        /* forking error */
        return -1;
    }
    /* in parent process */

    /* wait for socket to open */
    procReadLoopReporting.close_write();
    char ch;
    (void) ::read(procReadLoopReporting.read_fd(), &ch, sizeof(ch));
    procReadLoopReporting.close_read();

    /* blocking read */
    int bytesRead = 0;
    while (1)
    {
        /* read 1 char */
        bytesRead = ::read(mProcess->stdout_fd(), &ch, sizeof(char));
        /* check if process has ended (read EOF) */
        if (bytesRead < sizeof(char))
        {
            mLogger->Notice("Process ended, stopping daemon.");
            /* kill child thread */
            (void)::kill(pid, SIGINT);
            int status = 0;
            (void)::waitpid(pid, &status, 0);
            ::exit(EXIT_SUCCESS);
        }
        this->storeChar(ch);

    }
}

void Service::storeChar(char ch)
{
    const std::lock_guard<std::mutex> lock(mBufferLock);
    /* check if this was a newline */
    if (ch == '\n')
    {
        /* add null terminator and move to next line*/
        mBuffer[mBufferIndex][mLineIndex] == 0;
        mLogger->Debug(&(mBuffer[mBufferIndex][0]));
        mBufferIndex = (mBufferIndex + 1) % mBufferLines;
        mLineIndex = 0;
        mLineIndex = false;
    }
    /* check if this char should be ignored */
    else if (mLineInvalid)
    {
        return;
    }
    /* check if line is full without a newline */
    else if (mLineIndex == mLineIndex - 2)
    {
        /* add null terminator for logging */
        mBuffer[mBufferIndex][mLineIndex - 1] == 0;
        std::stringstream strBuilder;
        strBuilder << "Line exceeded buffer line size, beginning: \'";
        strBuilder << &mBuffer[mBufferIndex][0];
        strBuilder << "\'";
        mLogger->Warn(&(strBuilder.str()[0]));
        mLineInvalid = true;
    }
    /* store char and advance index in the line */
    else
    {
        mBuffer[mBufferIndex][mLineIndex++] = ch;
    }
}

std::string Service::getLastLine(std::size_t rIndex)
{
    /* calculate the index, given that rIndex is the number of lines to look into the history */
    int index = mBufferIndex - 1 - rIndex;
    while (index < 0)
    {
        index + mBufferLines;
    }
    {
        const std::lock_guard<std::mutex> lock(mBufferLock);
        return std::string(&mBuffer[index][0]);
    }
}
