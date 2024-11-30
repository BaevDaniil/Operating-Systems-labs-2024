#include "conn_fifo.hpp"

FifoConnection::FifoConnection(const std::string &fifo_name, bool should_create)
{
    fifo_path = std::filesystem::current_path() / "tmp" / fifo_name;

    if (should_create)
    {
        if (mkfifo(fifo_path.c_str(), 0777) != 0 && errno != EEXIST)
        {
            std::cerr << "Error creating FIFO: " << std::strerror(errno) << "\n";
            throw std::runtime_error("FIFO creation failed");
        }
        std::cout << "FIFO created: " << fifo_path << "\n";
    }

    fifo_fd = open(fifo_path.c_str(), O_RDWR);
    if (fifo_fd == -1)
    {
        std::cerr << "Error opening FIFO: " << std::strerror(errno) << "\n";
        throw std::runtime_error("FIFO opening failed");
    }
}

bool FifoConnection::Read(std::string &message)
{
    char buffer[MAX_MESSAGE_SIZE];
    memset(buffer, '\0', MAX_MESSAGE_SIZE);

    fifo_fd = open(fifo_path.c_str(), O_RDWR);
    if (fifo_fd == -1)
    {
        std::cout << fifo_path << " " << std::strerror(errno) << std::endl;
        return false;
    }

    ssize_t bytes_read = read(fifo_fd, buffer, MAX_MESSAGE_SIZE - 1);
    if (bytes_read == -1)
        return false;

    message.assign(buffer, bytes_read);
    return true;
}

bool FifoConnection::Write(const std::string &message)
{
    fifo_fd = open(fifo_path.c_str(), O_RDWR);
    if (fifo_fd == -1)
    {
        std::cout << fifo_path << " " << std::strerror(errno) << std::endl;
        return false;
    }

    ssize_t bytesWritten = write(fifo_fd, message.c_str(), message.size());
    if (bytesWritten == -1)
    {
        std::cout << "Error writing to FIFO: " << strerror(errno) << "\n";
        return false;
    }
    return true;
}

FifoConnection::~FifoConnection()
{
    close(fifo_fd);
    unlink(fifo_path.c_str());
}