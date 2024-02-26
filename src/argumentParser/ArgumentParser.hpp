#ifndef ARGUMENT_PARSER_HPP
#define ARGUMENT_PARSER_HPP

class ArgumentParser
{
public:
    void parse(int argc, const char* const* argv);
    void printHelp();

    bool                mIsDaemon   {false};
    bool                mIsClient   {false};
    const char* const*  mProcArgv   {nullptr};

};


#endif /* ARGUMENT_PARSER_HPP */
