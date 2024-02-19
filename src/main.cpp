#include <iostream>
#include <string>
#include "process/Process.hpp"

int main(void)
{
    std::cout << "Hello, World!" << std::endl;

    const char* const argv[] = {"/bin/cat", (const char*)0};
    Process cat(argv);
    cat << "Hello, World!" << '\n';

    char buf[2048];
    (void)::read(cat.stdout_fd(), buf, 2048);
    std::cout << "Read from program: '" << buf << "'" << std::endl;
    cat.send_eof();
    return 0;
}
