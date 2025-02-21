#include "conn_socket.hpp"

#include "conn_socket.hpp"

SocketConnection::SocketConnection(const std::string &id, bool create)
    : socket_name(std::filesystem::current_path() / "tmp" / id), is_host(create)
{
    std::filesystem::path tmpDir = std::filesystem::path(socket_name).parent_path();
    if (!std::filesystem::exists(tmpDir))
    {
        std::filesystem::create_directory(tmpDir);
    }

    struct sockaddr_un address;
    memset(&address, 0, sizeof(struct sockaddr_un));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socket_name.c_str(), sizeof(address.sun_path) - 1);

    if (is_host)
    {
        if (std::filesystem::exists(socket_name))
            unlink(socket_name.c_str());

        host_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (host_socket < 0)
            throw std::runtime_error("Host socket creation error");

        if (bind(host_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
            throw std::runtime_error("Bind socket error");

        if (listen(host_socket, SOMAXCONN) < 0)
            throw std::runtime_error("Listen socket error");

        client_socket = accept(host_socket, nullptr, nullptr);
        if (client_socket < 0)
            throw std::runtime_error("Accept client socket error");
    }
    else
    {
        while (!std::filesystem::exists(socket_name))
        {
            std::cout << "Waiting for socket to be created: " << socket_name << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        client_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (client_socket < 0)
            throw std::runtime_error("Client socket creation error");


        if (connect(client_socket, (const struct sockaddr *)&address, sizeof(struct sockaddr_un)) == -1)
        {
            std::cout << "Error connecting sockets: " << strerror(errno) << std::endl;
            throw std::runtime_error("Connect sockets error");
        }
    }
}

bool SocketConnection::Read(std::string &message)
{
    char buffer[max_msg_size];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0)
    {
        std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
        return false;
    }

    message.assign(buffer, bytesReceived);
    return true;
}

bool SocketConnection::Write(const std::string &message)
{
    if (send(client_socket, message.c_str(), message.size(), MSG_NOSIGNAL) == -1)
    {
        std::cerr << "Error writing to socket: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

SocketConnection::~SocketConnection()
{
    if (is_host)
    {
        close(client_socket);
        close(host_socket);
        unlink(socket_name.c_str());
    }
    else
    {
        close(client_socket);
    }
}