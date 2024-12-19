#ifndef CONNPIPE_H
#define CONNPIPE_H

#include "includes.h"
#include "conn.h"

class ConnPipe : public Conn
{
public:
    ConnPipe(int pipe);
    ~ConnPipe();
    bool is_valid() const override;

    void setup_conn(bool create) {}
    bool read(std::string& buf, unsigned size);
    bool write(const std::string& buf);

private:
    int pipe;
    bool valid {false};
};

#endif // CONNPIPE_H
