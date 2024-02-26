#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <syslog.h>
#include "Daemon.hpp"
#include "process/CPipe.hpp"
#include <memory>

Daemon* Daemon::daemonize(const char* pidFile, const char* chRoot)
{
    Daemon* daemon = new Daemon();
    strncpy(daemon->mPidFile, pidFile, Daemon::maxStrSize);
    strncpy(daemon->mChRoot, chRoot, Daemon::maxStrSize);

    /* https://www.freedesktop.org/software/systemd/man/latest/daemon.html */

    /* close all file descriptors */
    struct ::rlimit rlim;
    if (::getrlimit(RLIMIT_NOFILE, &rlim) < 0)
    {
        ::exit(EXIT_FAILURE);
    }
    for (int fd = 3; fd <rlim.rlim_cur; fd++)
    {
        (void) ::close(fd);
    }

    /* reset all signal handlers and implement SIGTERM and SIGHUP */
    /* reset signal mask */

    /* create a pipe to receive status of daemon starting */
    CPipe reportingPipe;

    /* run in background */
    ::pid_t pid;
    pid = ::fork();
    if (pid > 0)
    {
        /* parent process, wait for result from reporting pipe then exit */
        reportingPipe.close_write();
        int reportedStatus;
        if (::read(reportingPipe.read_fd(), &reportedStatus, sizeof(reportedStatus)) < sizeof(reportedStatus))
        {
            ::exit(EXIT_FAILURE);
        }
        reportingPipe.close_all();
        ::exit(reportedStatus);
    }
    if (pid < 0)
    {
        /* fork failure */
        ::exit(EXIT_FAILURE);
    }
    /* child process */
    reportingPipe.close_read();


    /* create independent session */
    if(::setsid() < 0)
    {
        ::syslog(LOG_CRIT, "Failed to create independent session");
        /* report status through pipe to the original process */
        int status = SYSTEMD_STATUS_DEAD;
        ::write(reportingPipe.write_fd(), &status, sizeof(status));
        ::exit(EXIT_FAILURE);
    }

    /* currently PID==SID, fork again so PID!=SID and process will not be bound to tty again */
    pid = ::fork();
    if (pid > 0)
    {
        /* exit in first child (not original process) */
        ::exit(EXIT_SUCCESS);
    }
    if (pid < 0)
    {
        ::syslog(LOG_CRIT, "Failed to fork from SID");
        /* report status through pipe to the original process */
        int status = SYSTEMD_STATUS_DEAD;
        ::write(reportingPipe.write_fd(), &status, sizeof(status));
        ::exit(EXIT_FAILURE);
    }

    /* connect /dev/null to stdin, stdout, and stderr */
    int nullFd = ::open("/dev/null", O_RDWR);
    if (nullFd < 0)
    {
        ::syslog(LOG_CRIT, "Failed to open /dev/null");
        /* report status through pipe to the original process */
        int status = SYSTEMD_STATUS_DEAD;
        ::write(reportingPipe.write_fd(), &status, sizeof(status));
        ::exit(EXIT_FAILURE);
    }
    if (::dup2(nullFd, STDIN_FILENO)    < 0 ||
        ::dup2(nullFd, STDOUT_FILENO)   < 0 ||
        ::dup2(nullFd, STDERR_FILENO)   < 0 )
    {
        ::syslog(LOG_CRIT, "Failed to connect /dev/null to stdin, stdout, and/or stderr");
        /* report status through pipe to the original process */
        int status = SYSTEMD_STATUS_DEAD;
        ::write(reportingPipe.write_fd(), &status, sizeof(status));
        ::exit(EXIT_FAILURE);
    }
    (void) ::close(nullFd);

    /* set file mode */
    ::umask(0);

    /* change root directory */
    if (::chdir(daemon->mChRoot) < 0)
    {
        ::syslog(LOG_CRIT, "Failed to write to change dir");
        /* report status through pipe to the original process */
        int status = SYSTEMD_STATUS_DEAD;
        ::write(reportingPipe.write_fd(), &status, sizeof(status));
        ::exit(EXIT_FAILURE);
    }

    /* write daemon PID to PID file */
    if (daemon->create_pid_file() < 0)
    {
        ::syslog(LOG_CRIT, "Failed to write to PID file");
        /* report status through pipe to the original process */
        int status = SYSTEMD_STATUS_DEAD;
        ::write(reportingPipe.write_fd(), &status, sizeof(status));
        ::exit(EXIT_FAILURE);
    }

    /* drop privileges */

    /* notify original process of completion */
    int status = SYSTEMD_STATUS_OK;
    ::write(reportingPipe.write_fd(), &status, sizeof(status));
    reportingPipe.close_all();

    return daemon;
}

int Daemon::create_pid_file()
{
    mPid = ::getpid();
    int pidFd = ::open(mPidFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (pidFd < 1)
    {
        return -1;
    }
    /* convert pid to string */
    char pidBuf[10];
    ::sprintf(pidBuf, "%u", mPid);
    if (::write(pidFd, &pidBuf, ::strlen(pidBuf)) < ::strlen(pidBuf))
    {
        return -1;
    }
    /* change pidfile permissions */
    if (::fchmod(pidFd, 0644) < 0)
    {
        return -1;
    }
    if(::close(pidFd))
    {
        return -1;
    }
    return 0;
}

void Daemon::remove_pid_file()
{
    struct ::stat st {0};
    /* check if /var/run/webprocd/pid exists */
    if (::stat(mPidFile, &st) < 0)
    {
        /* does not exist */
        return;
    }
    (void) remove(mPidFile);
}
