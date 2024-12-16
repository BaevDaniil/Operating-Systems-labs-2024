#include "conn_pipe.h"

ConnPipe::ConnPipe(const std::string& path, bool create): path(path)
{
}

ConnPipe::~ConnPipe()
{
}

void ConnPipe::setup_conn(bool create)
{
}

bool ConnPipe::read(std::string& buf, unsigned size)
{
    return true;
}

bool ConnPipe::write(const std::string& buf)
{
    return true;
}

bool ConnPipe::is_valid() const
{
    return valid;
}
