#include "src/Host/Host.hpp"
#include "src/Client/Client.hpp"
#include "Conn/conn_fifo.hpp"
#include "Common/Logger.hpp"
#include "Common/Reader.hpp"

#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        LOG_ERROR("APP", "Usage: ./host_fifo <num_clients> [optional: path_to_file]");
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

    auto books = Reader::parse(filePath);
    if (!books)
    {
        LOG_ERROR("APP", "Failed to parse JSON file with books");
        return EXIT_FAILURE;
    }

    SemaphoreLocal semaphore(numClients + 1);

    std::vector<std::unique_ptr<connImpl>> hostConnections;
    for (int i = 0; i < numClients; ++i)
    {
        auto hostFifoConn = ConnFifo::crateHostFifo("/tmp/my_fifo_" + std::to_string(i));
        if (!hostFifoConn)
        {
            LOG_ERROR(HOST_LOG, "Failed to initialize fifo by host");
            return EXIT_FAILURE;
        }

        hostConnections.push_back(std::move(hostFifoConn));
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
            auto clientFifoConn = ConnFifo::crateClientFifo("/tmp/my_fifo_" + std::to_string(i));
            if (!clientFifoConn)
            {
                LOG_ERROR(CLIENT_LOG, "Failed to initialize client socket");
                return EXIT_FAILURE;
            }

            Client client(getpid(), semaphore, *clientFifoConn, *books);
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
