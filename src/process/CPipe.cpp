#include <unistd.h>
#include "CPipe.hpp"

CPipe::CPipe()
{
    /* Create pipes */
    int rv = ::pipe(this->descriptors);
    if (rv)
    {
        throw std::runtime_error("Failed to create pipes");
    }
}

CPipe::~CPipe()
{
    this->close_all();
}

void CPipe::close_all()
{
    /* Close read side of pipe */
    (void)::close(this->descriptors[0]);
    /* Close write side of pipe */
    (void)::close(this->descriptors[1]);
}

void CPipe::close_read()
{
    /* Close read side of pipe */
    (void)::close(this->descriptors[0]);
}

void CPipe::close_write()
{
    /* Close write side of pipe */
    (void)::close(this->descriptors[1]);
}
