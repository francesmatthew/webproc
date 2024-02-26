#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <memory>
#include <array>
#include <mutex>
#include "daemon/Daemon.hpp"
#include "daemon/Logger.hpp"
#include "process/Process.hpp"
#include "process/CPipe.hpp"

class Service
{
public:
    Service() = delete;
    Service(std::shared_ptr<Daemon> &daemon, std::shared_ptr<Logger> &logger, std::shared_ptr<Process> &process);

    /* start a socket and go into a read loop from the socket, for running a daemon */
    void runSocket();
    /* go into a read loop taking input from stdin, for running in non-daemon mode */
    void run();

    /* index into line history, where rIndex is the reverse index of lines into the history */
    std::string getLastLine(std::size_t rIndex);

    static const char* SOCKET_DIR;
    static const char* PID_FILE;
    static const char* SYSLOG_IDENT;

private:
    /* fork and start a read loop from the process stdout, and store the results in the buffer */
    int runProcessReadLoop(CPipe &procReadLoopReporting);
    void storeChar(char ch);
    static void sigint_handler(int signal);
    static int mSockFd;
    void readConnection(int connFd);

    std::shared_ptr<Daemon> mDaemon;
    std::shared_ptr<Logger> mLogger;
    std::shared_ptr<Process> mProcess;

    static const std::size_t mBufferLines = 10; /* number of lines to hold in the buffer */
    static const std::size_t mLineBufferSize = 200; /* number of chars to hold in a line */

    static std::mutex mBufferLock;
    static std::array<std::array<char, mLineBufferSize>, mBufferLines> mBuffer;
    static std::size_t mBufferIndex; /* index of next line to write to, and one after the previous line written */
    static std::size_t mLineIndex; /* index of next line to write to, and one after the previous line written */
    static bool mLineInvalid;
};

#endif /* SERVICE_HPP */
