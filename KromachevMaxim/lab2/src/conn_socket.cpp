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
        if(close(fd) == -1) {
            std::perror("Ошибка при закрытии socket");
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
        memset(&sa, 0, sizeof(sa));

        if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        {
            std::perror ("Ошибка client socket:");
            return;
        }

        sa.sun_family = AF_UNIX;
        strcpy(sa.sun_path, path.c_str());
        unlink(path.c_str());
        len = sizeof(sa.sun_family) + strlen(sa.sun_path);

        if (bind(fd, (struct sockaddr*)&sa, len) < 0)
        {
            std::perror ("Ошибка server bind");
            return;
        }
        if (listen(fd, 5) < 0)
        {
            std::perror ("Ошибка server listen");
            return;
        }
    }
    else
    {
        if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        {
            std::perror ("Ошибка client socket");
            return;
        }
    }

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        std::perror("Ошибка получения флагов сокета");
        return;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        std::perror("Ошибка установки неблокирующего режима");
        return;
    }
}

void ConnSocket::accept_conn()
{
    socklen_t len = sizeof(sockaddr_un);
    struct sockaddr_un ca;
    int client_socket;

    if ((client_socket = accept(fd, (struct sockaddr*)&ca, &len)) < 0)
    {
        std::perror ("Ошибка accept сервера");
        return;
    }

    close(fd);
    fd = client_socket;
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        std::perror("Ошибка получения флагов сокета");
        return;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        std::perror("Ошибка установки неблокирующего режима");
        return;
    }
    valid = true;
}

void ConnSocket::connect_to_server()
{
    int len;
    struct sockaddr_un sa;
    memset(&sa, 0, sizeof(sa));
    sa.sun_family = AF_UNIX;
    strcpy (sa.sun_path, path.c_str());

    len = sizeof(sa.sun_family) + strlen(sa.sun_path);
    if (connect(fd, (struct sockaddr*)&sa, len) < 0 )
    {
        std::perror ("Ошибка connect клиента");
        return;
    }
    valid = true;
}

bool ConnSocket::read(std::string& buf, unsigned size)
{
    if (!is_valid())
    {
        std::perror("Сокет не доступен для чтения");
        return false;
    }

    char buffer[size];
    memset(buffer, '\0', size);

    int bytes_read = recv(fd, buffer, size - 1, 0);

    if (bytes_read == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return false;
        }
        else if (errno == EINTR)
        {
            std::perror("Ошибка, чтение было прервано сигналом");
            return read(buf, size);
        }
        else {
            std::perror("Ошибка receiving msg:");
            return false;
        }
    }

    buf.assign(buffer, bytes_read);
    return true;
}

bool ConnSocket::write(const std::string& buf)
{
    if (!is_valid())
    {
        std::perror("Сокет не доступен для записи");
        return false;
    }

    ssize_t bytes_written = send(fd, buf.c_str(), buf.size(), 0);
    if (bytes_written < 0)
    {
        std::perror("Ошибка отправки сообщения");
        return false;
    }
    return true;
}

bool ConnSocket::is_valid() const
{
    return valid;
}
