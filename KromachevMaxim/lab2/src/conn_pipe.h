#ifndef CONNPIPE_H
#define CONNPIPE_H

#include "includes.h"
#include "conn.h"

class ConnPipe : public Conn
{
public:
    ConnPipe(const std::string& path, bool create);
    ~ConnPipe();
    bool is_valid() const override;

    void setup_conn(bool create);
    bool read(std::string& buf, unsigned size);
    bool write(const std::string& buf);

private:
    const std::string path;
    int fd = -1;
    bool valid {false};
};

#endif // CONNPIPE_H
