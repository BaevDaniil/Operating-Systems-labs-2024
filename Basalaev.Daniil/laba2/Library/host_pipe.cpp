#include "src/Host/Host.hpp"
#include "src/Client/Client.hpp"
#include "Conn/conn_pipe.hpp"
#include "Common/Logger.hpp"
#include "Common/Reader.hpp"

#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        LOG_ERROR("APP", "Usage: ./host_pipe <num_clients>");
        return EXIT_FAILURE;
    }

    int numClients = std::stoi(argv[1]);

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

    std::vector<std::unique_ptr<connImpl>> hostConnections;
    std::vector<std::unique_ptr<connImpl>> clientsConnections;
    for (int i = 0; i < numClients; ++i)
    {
        auto [hostPipeConn, clientPipeConn] = ConnPipe::createPipeConns();
        if (!hostPipeConn && !clientPipeConn)
        {
            LOG_ERROR(HOST_LOG, "Failed to initialize pipe by host");
            return EXIT_FAILURE;
        }

        hostConnections.push_back(std::move(hostPipeConn));
        clientsConnections.push_back(std::move(clientPipeConn));
    }

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
            Client client(getpid(), semaphore, *clientsConnections[i], *books);
            return client.start();
        }
        else // host
        {
            clientsId.push_back(pid);
        }
    }

    Host host(semaphore, clientsId, std::move(hostConnections), *books);
    return host.start();
}
