#include "conn_socket.h"

ConnSocket::ConnSocket(const std::string& path, bool create): path(path)
{
    if(create) {
        setup_conn(true);
    }
    else {
        setup_conn(false);
    }
}

ConnSocket::~ConnSocket()
{
    if(valid) {
        if(close(d) == -1) {
            fprintf(stderr, "Ошибка при закрытии socket.\n");
        }
    }
    unlink(path.c_str());
}

void ConnSocket::setup_conn(bool create)
{
    if(create)
    {
        int len;
        struct sockaddr_un sa;

        if((d = socket (AF_UNIX, SOCK_STREAM, 0)) < 0)
        {
            std::perror ("client: socket");
            return;
        }

        sa.sun_family = AF_UNIX;
        strcpy(sa.sun_path, path.c_str());
        unlink(path.c_str());
        len = sizeof(sa.sun_family) + strlen(sa.sun_path);

        if (bind(d, (struct sockaddr*)&sa, len) < 0)
        {
            std::perror ("server: bind");
            return;
        }
        if (listen(d, 5) < 0)
        {
            std::perror ("server: listen");
            return;
        }
    }
    else {
        int len;
        struct sockaddr_un sa;

        if ((d = socket (AF_UNIX, SOCK_STREAM, 0)) < 0)
        {
            perror ("client: socket");
            return;
        }
        sa.sun_family = AF_UNIX;
        strcpy (sa.sun_path, path.c_str());

        len = sizeof(sa.sun_family) + strlen(sa.sun_path);
        if (connect(d, (struct sockaddr*)&sa, len) < 0 )
        {
            perror ("client: connect");
            return;
        }
    }
    valid = true;
}

void ConnSocket::accept_conn()
{
    socklen_t len;
    struct sockaddr_un ca;

    if ((d = accept(d, (struct sockaddr*)&ca, &len)) < 0)
    {
        std::perror ("server: accept");
        return;
    }
}

bool ConnSocket::read(std::string& buf, unsigned size)
{
    if (!is_valid())
    {
      std::perror("Socket not available for reading\n");
      return false;
    }

    char buffer[size];
    memset(buffer, '\0', size);

    int bytes_read = recv(d, buffer, size - 1, 0);
    if (bytes_read == 0)
    {
      std::perror("Connection closed by peer\n");
      return false;
    }

    if (bytes_read == -1)
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return false;
      }
      else if (errno == EINTR)
      {
        std::cerr << "Read interrupted by a signal, retrying\n";
        return read(buf, size);
      }
      else {
        std::cerr << "Error receiving msg: " << strerror(errno) << " (errno: " << errno << ")\n";
      }
      return false;
    }

    buf.assign(buffer, bytes_read);
    return true;
}

bool ConnSocket::write(const std::string& buf)
{
    if (!is_valid())
    {
        std::perror("Socket not available for writing\n");
        return false;
    }

    ssize_t bytes_written = send(d, buf.c_str(), buf.size(), 0);
    if (bytes_written < 0)
    {
        std::perror("Error sending msg\n");
        return false;
    }
    return true;
}

bool ConnSocket::is_valid() const
{
    return valid;
}
