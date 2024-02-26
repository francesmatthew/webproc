#ifndef LOGGER_HPP
#define LOGGER_HPP

class Logger
{
    public:
    Logger() = delete;
    Logger(const char* ident, bool isDaemon);
    ~Logger();

    void Debug(const char* msg);
    void Info(const char* msg);
    void Notice(const char* msg);
    void Warn(const char* msg);
    void Error(const char* msg);
    void Crit(const char* msg);

    private:
        bool mIsDaemon  {false};
};

#endif /* LOGGER_HPP */
