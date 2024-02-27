#ifndef ARGUMENT_PARSER_HPP
#define ARGUMENT_PARSER_HPP

#include "daemon/Daemon.hpp"

class ArgumentParser
{
public:
    void parse(int argc, const char* const* argv);
    void printHelp();

    bool                mIsDaemon   {false};
    bool                mIsClient   {false};
    const char* const*  mProcArgv   {nullptr};
    char mChRoot[Daemon::maxStrSize]    {0};

};


#endif /* ARGUMENT_PARSER_HPP */
