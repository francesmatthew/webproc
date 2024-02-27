#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include "process/Process.hpp"
#include "daemon/Daemon.hpp"
#include "daemon/Logger.hpp"
#include "argumentParser/ArgumentParser.hpp"
#include "service/Service.hpp"
#include "client/Client.hpp"

int main(int argc, const char** argv)
{
    ArgumentParser argParser;
    argParser.parse(argc, argv);

    /* run client */
    if (argParser.mIsClient)
    {
        Client client;
        return client.run();
    }

    /* check that a command was actually provided*/
    if (argParser.mProcArgv == nullptr)
    {
        std::cerr << "Command to run not specified" << std::endl;
        argParser.printHelp();
        return 1;
    }

    auto logger = std::make_shared<Logger>(Service::SYSLOG_IDENT, argParser.mIsDaemon);

    std::shared_ptr<Daemon> daemon;
    if (argParser.mIsDaemon)
    {
        daemon = std::shared_ptr<Daemon>(Daemon::daemonize(Service::PID_FILE));
        logger->Notice("Daemon thread started.");
    }

    auto process = std::make_shared<Process>(logger, argParser.mProcArgv, true);

    /* check that process started correctly */
    if (process->getPid() < 0)
    {
        /* error will be logged from child process explaining issue */
        logger->Crit("Process failed to start, service will not be started");
        ::exit(1);
    }

    Service service(daemon, logger, process);
    if (argParser.mIsDaemon)
    {
        service.runSocket();
    }
    else
    {
        service.run();
    }

    return 0;
}
