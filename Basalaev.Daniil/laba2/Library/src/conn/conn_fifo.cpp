#include "conn_fifo.hpp"
#include "Logger.hpp"

#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

std::unique_ptr<ConnFifo*> ConnFifo::crateHostFifo(std::string const& fifoPath)
{
    ConnFifo* fifo = new ConnFifo();

    if (mkfifo(fifo->m_fifoPath.c_str(), 0666) == -1)
    {
        if (errno != EEXIST)
        {
            LOG_ERROR("ConnFifo", "Host failed to create FIFO");
            return nullptr;
        }
    }

    fifo->m_readFileDisriptor = open(fifo->m_fifoPath.c_str(), O_RDONLY | O_NONBLOCK);
    if (fifo->m_readFileDisriptor == -1)
    {
        LOG_ERROR("ConnFifo", "Host failed to open FIFO for reading");
        return nullptr;
    }

    fifo->m_writeFileDisriptor = open(fifo->m_fifoPath.c_str(), O_WRONLY);
    if (fifo->m_writeFileDisriptor == -1)
    {
        LOG_ERROR("ConnFifo", "Host failed to open FIFO for writing");
        close(fifo->m_readFileDisriptor);
        return nullptr;
    }

    return fifo;
}

std::unique_ptr<ConnFifo*> ConnFifo::crateClientFifo(std::string const& fifoPath)
{
    ConnFifo* fifo = new ConnFifo();

    fifo->m_readFileDisriptor = open(fifo->m_fifoPath.c_str(), O_RDONLY | O_NONBLOCK);
    if (fifo->m_readFileDisriptor == -1)
    {
        LOG_ERROR("ConnFifo", "Client failed to open FIFO for reading");
        return nullptr;
    }

    fifo->m_writeFileDisriptor = open(fifo->m_fifoPath.c_str(), O_WRONLY);
    if (fifo->m_writeFileDisriptor == -1)
    {
        LOG_ERROR("ConnFifo", "Client failed to open FIFO for writing");
        close(fifo->m_readFileDisriptor);
        return nullptr;
    }

    return fifo;
}

bool ConnFifo::IsInitialized() const
{
    return m_writeFileDisriptor == -1 || m_readFileDisriptor == -1;
}

ConnFifo::~ConnFifo()
{
    if (m_readFileDisriptor != -1)
    {
        close(m_readFileDisriptor);
    }

    if (m_writeFileDisriptor != -1)
    {
        close(m_writeFileDisriptor);
    }

    if (!m_fifoPath.empty())
    {
        unlink(m_fifoPath.c_str());
    }
}

bool ConnFifo::Read(void* buf, size_t count)
{
    if (!IsInitialized()) { return false; }

    ssize_t bytesRead = read(m_readFileDisriptor, buf, count);
    if (bytesRead == -1) { return false; }

    return bytesRead > 0;
}

bool ConnFifo::Write(const void* buf, size_t count)
{
    if (!IsInitialized()) { return false; }

    ssize_t bytesWritten = write(m_writeFileDisriptor, buf, count);
    if (bytesWritten == -1) { return false; }

    return static_cast<size_t>(bytesWritten) == count;
}
