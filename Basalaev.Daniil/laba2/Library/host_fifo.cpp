#include "src/Host/Host.hpp"
#include "src/Client/Client.hpp"
#include "Conn/conn_fifo.hpp"
#include "Common/Logger.hpp"

#include <unistd.h>

int main(int argc, char* argv[])
{
    // // initialize some stuff
    // std::vector<Book> books = {
    //     {"Book 1", 10},
    //     {"Book 2", 5},
    //     {"Book 3", 20},
    //     {"Book 4", 0}
    // };
    // SemaphoreLocal semaphore(1);

    // auto hostFifoConn = ConnFifo::crateHostFifo("/tmp/my_fifo");
    // if (!hostFifoConn)
    // {
    //     LOG_ERROR(HOST_LOG, "Failed to initialize fifo by host");
    //     return EXIT_FAILURE;
    // }

    // if (pid_t pid = fork(); pid == -1) // error
    // {
    //     LOG_ERROR("APP", "Failed to fork");
    //     return EXIT_FAILURE;
    // }
    // else if (pid == 0) // client
    // {
    //     auto clientFifoConn = ConnFifo::crateClientFifo("/tmp/my_fifo");
    //     if (!clientFifoConn)
    //     {
    //         LOG_ERROR(CLIENT_LOG, "Failed to initialize client socket");
    //         return EXIT_FAILURE;
    //     }

    //     Client client(pid, semaphore, *clientFifoConn, books);
    //     return client.start();
    // }
    // else // host
    // {
    //     Host host(semaphore, {std::move(hostFifoConn)}, books);
    //     return host.start();
    // }
}
