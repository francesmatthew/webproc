#ifndef DAEMON_HPP
#define DAEMON_HPP

#include <sys/types.h>
#include <sys/socket.h>

#define SYSTEMD_STATUS_OK               0
#define SYSTEMD_STATUS_DEAD_PID_FILE    1
#define SYSTEMD_STATUS_DEAD_LOCK_FILE   2
#define SYSTEMD_STATUS_DEAD             3
#define SYSTEMD_STATUS_UNKNOWN          4

class Daemon
{
public:
    Daemon(const Daemon&) = default;
    ~Daemon() { this->remove_pid_file(); }

    static Daemon* daemonize(const char* pidFile, const char* chRoot = "/");

    static const ::size_t maxStrSize = 100;
private:
    Daemon() = default;
    /* create/delete pid file */
    int create_pid_file();
    void remove_pid_file();

    char mPidFile[maxStrSize]   {0};
    char mChRoot[maxStrSize]    {0};
    ::pid_t mPid                {0};
};

#endif /* DAEMON_HPP */
