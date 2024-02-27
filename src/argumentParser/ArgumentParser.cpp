#include "ArgumentParser.hpp"
#include <string>
#include <string.h>
#include <iostream>

void ArgumentParser::parse(int argc, const char* const* argv)
{
    for (int ii = 1; ii < argc; ii++)
    {
        std::string arg = argv[ii];
        if (!arg.compare("-d") || !arg.compare("--daemon"))
        {
            mIsDaemon = true;
        }
        if (!arg.compare("-c") || !arg.compare("--client"))
        {
            mIsClient = true;
        }
        else if (!arg.compare("-h") || !arg.compare("--help"))
        {
            this->printHelp();
            ::exit(EXIT_SUCCESS);
        }
        else if (!arg.compare("-w") || !arg.compare("--workingdir"))
        {
            if (argc > ii+1)
            {
                ::strncpy(mChRoot, argv[ii+1], sizeof(mChRoot));
                ii++;
            }
            else
            {
                std::cerr << "No working directory provided" << std::endl;
                ::exit(EXIT_FAILURE);
            }
        }
        else if (!arg.compare("--"))
        {
            /* the rest of the args are the command string */
            if (argc > ii+1)
            {
                mProcArgv = &argv[ii+1];
            }
            return;
        }
        /* once we've reached the end of the args, the rest are args for the command to run */
        else if (arg[0] != '-')
        {
            mProcArgv = argv + ii;
        }
    }
}

void ArgumentParser::printHelp()
{
    std::cerr << "Usage: webproc [-d] [-h] [-w <working directory>] [--] <command to run> [args for command to run]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "    -d, --daemon: run in daemon mode" << std::endl;
    std::cerr << "    -w, --workingdir: set the working directory of the daemon" << std::endl;
    std::cerr << "    -h, --help: display this message" << std::endl;
}
