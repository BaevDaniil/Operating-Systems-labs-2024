#ifndef CONNFIFO_H
#define CONNFIFO_H

#include "includes.h"
#include "conn.h"

class ConnFifo : public Conn
{
public:
    ConnFifo(const std::string& path, bool create);
    ~ConnFifo();
    bool is_valid() const override;

private:
    bool read(const std::string& buf, unsigned size);
    bool write(const std::string& buf);

    const std::string path;
    int fd = -1;
    bool valid {false};
};

#endif // CONNFIFO_H
