#ifndef CONNSOCKET_H
#define CONNSOCKET_H

#include "includes.h"
#include "conn.h"

class ConnSocket : public Conn
{
public:
    ConnSocket(const std::string& path, bool create);
    ~ConnSocket();
    bool is_valid() const override;

    void setup_conn(bool create);
    void accept_conn();
    void connect_to_server();
    bool read(std::string& buf, unsigned size);
    bool write(const std::string& buf);

private:
    const std::string path;
    int fd = -1;
    bool valid {false};
};

#endif // CONNSOCKET_H
