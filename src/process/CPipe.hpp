#ifndef C_PIPE_HPP
#define C_PIPE_HPP

#include <stdexcept>

/* Simple class wrapping a c-style pipe with destructor */
class CPipe
{
public:
    CPipe();
    ~CPipe();
    const int read_fd() const { return this->descriptors[0]; }
    const int write_fd() const { return this->descriptors[1]; }
    void close_all();
    void close_read();
    void close_write();

private:
    /* File descriptors of created pipes */
    int descriptors[2];
};

#endif /* C_PIPE_HPP */
