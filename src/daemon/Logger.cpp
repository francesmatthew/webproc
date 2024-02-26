#include "Logger.hpp"
#include <syslog.h>
#include <iostream>

Logger::Logger(const char* ident, bool isDaemon):
mIsDaemon(isDaemon)
{
    if (isDaemon)
    {
        ::openlog(ident, LOG_PID, LOG_DAEMON);
    }
}

Logger::~Logger()
{
    if (mIsDaemon)
    {
        ::closelog();
    }
}

void Logger::Debug(const char* msg)
{
    if (mIsDaemon)
    {
        ::syslog(LOG_DEBUG, msg);
    }
    else
    {
        std::cout << "DEBUG: " << msg << std::endl;
    }
}

void Logger::Info(const char* msg)
{
    if (mIsDaemon)
    {
        ::syslog(LOG_INFO, msg);
    }
    else
    {
        std::cout << "INFO: " << msg << std::endl;
    }
}

void Logger::Notice(const char* msg)
{
    if (mIsDaemon)
    {
        ::syslog(LOG_NOTICE, msg);
    }
    else
    {
        std::cout << "NOTICE: " << msg << std::endl;
    }
}

void Logger::Warn(const char* msg)
{
    if (mIsDaemon)
    {
        ::syslog(LOG_WARNING, msg);
    }
    else
    {
        std::cout << "WARN: " << msg << std::endl;
    }
}

void Logger::Error(const char* msg)
{
    if (mIsDaemon)
    {
        ::syslog(LOG_ERR, msg);
    }
    else
    {
        std::cout << "ERROR: " << msg << std::endl;
    }
}

void Logger::Crit(const char* msg)
{
    if (mIsDaemon)
    {
        ::syslog(LOG_CRIT, msg);
    }
    else
    {
        std::cout << "CRITICAL: " << msg << std::endl;
    }
}

