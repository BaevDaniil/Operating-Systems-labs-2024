#include "src/Host/Host.hpp"
#include "src/Client/Client.hpp"
#include "Conn/conn_sock.hpp"
#include "Common/Logger.hpp"

#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 2) // take port
    {
        LOG_ERROR("APP", "Usage: ./host <port>");
        return EXIT_FAILURE;
    }

    alias::port_t port = std::stoi(argv[1]);
    alias::book_container_t books = {
        {"Book 1", 10},
        {"Book 2", 5},
        {"Book 3", 20},
        {"Book 4", 0}
    };
    SemaphoreLocal semaphore(1);

    auto hostSocketConn = ConnSock::crateHostSocket(port);
    if (!hostSocketConn)
    {
        LOG_ERROR(HOST_LOG, "Failed to initialize host socket");
        return EXIT_FAILURE;
    }

    if (pid_t pid = fork(); pid == -1) // error
    {
        LOG_ERROR("APP", "Failed to fork");
        return EXIT_FAILURE;
    }
    else if (pid != 0) // client
    {
        auto clientSocketConn = ConnSock::crateClientSocket(port);
        if (!clientSocketConn)
        {
            LOG_ERROR(CLIENT_LOG, "Failed to initialize client socket");
            return EXIT_FAILURE;
        }

        Client client(pid, semaphore, *clientSocketConn, books);
        return client.start();
    }
    else // host
    {
        auto hostSocketConnAccepted = hostSocketConn->Accept();
        if (!hostSocketConnAccepted)
        {
            LOG_ERROR(HOST_LOG, "Failed to accept connection");
            return EXIT_FAILURE;
        }

        Host host(semaphore, *hostSocketConnAccepted, books);
        return host.start();
    }
}
