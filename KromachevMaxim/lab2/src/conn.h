#ifndef CONN_H
#define CONN_H

class Conn
{
public:
    Conn(int id , bool create);
    ~Conn();
    bool Read(void *buf, size_t count);
    bool Write(void *buf, size_t count);
};

#endif // CONN_H
