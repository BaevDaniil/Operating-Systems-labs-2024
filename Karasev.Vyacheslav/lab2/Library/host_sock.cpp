#include "src/Host/Host.hpp"
#include "src/Client/Client.hpp"
#include "Conn/conn_sock.hpp"
#include "Common/Logger.hpp"
#include "Common/Reader.hpp"

#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        LOG_ERROR("APP", "Usage: ./host_sock <port> <num_clients>");
        return EXIT_FAILURE;
    }

    alias::port_t port = std::stoi(argv[1]);
    int numClients = std::stoi(argv[2]);

    if (numClients <= 0)
    {
        LOG_ERROR("APP", "Number of clients must be greater than zero");
        return EXIT_FAILURE;
    }

    std::string filePath = "Books.json";
    if (argc == 3)
    {
        filePath = argv[2];
    }

    auto books = Reader::parse("Books.json");
    if (!books)
    {
        LOG_ERROR("APP", "Failed to parse JSON file with books");
        return EXIT_FAILURE;
    }

    SemaphoreLocal semaphore(numClients + 1);

    auto hostSocketConn = ConnSock::craeteHostSocket(port);
    if (!hostSocketConn)
    {
        LOG_ERROR(HOST_LOG, "Failed to initialize host socket");
        return EXIT_FAILURE;
    }

    std::vector<std::unique_ptr<connImpl>> hostConnections;
    std::vector<alias::id_t> clientsId;

    for (int i = 0; i < numClients; ++i)
    {
        if (pid_t pid = fork(); pid == -1) // Error
        {
            LOG_ERROR("APP", "Failed to fork");
            return EXIT_FAILURE;
        }
        else if (pid == 0) // client
        {
            auto clientSocketConn = ConnSock::craeteClientSocket(port);
            if (!clientSocketConn)
            {
                LOG_ERROR(CLIENT_LOG, "Failed to initialize client socket");
                return EXIT_FAILURE;
            }

            Client client(getpid(), semaphore, *clientSocketConn, *books);
            return client.start();
        }
        else // host
        {
            clientsId.push_back(pid);
        }
    }

    for (int i = 0; i < numClients; ++i)
    {
        auto hostSocketConnAccepted = hostSocketConn->accept();
        if (!hostSocketConnAccepted)
        {
            LOG_ERROR(HOST_LOG, "Failed to accept connection");
            return EXIT_FAILURE;
        }

        hostConnections.push_back(std::move(hostSocketConnAccepted));
    }

    Host host(semaphore, clientsId, std::move(hostConnections), *books);
    return host.start();
}
