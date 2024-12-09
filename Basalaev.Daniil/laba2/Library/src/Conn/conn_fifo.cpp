#include "conn_fifo.hpp"
#include "Common/Logger.hpp"

#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

std::unique_ptr<ConnFifo> ConnFifo::crateHostFifo(std::string const& fifoPath)
{
    ConnFifo* fifo = new ConnFifo();
    fifo->m_isHost = true;
    fifo->m_fifoPath = fifoPath;

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

    return std::unique_ptr<ConnFifo>(fifo);
}

std::unique_ptr<ConnFifo> ConnFifo::crateClientFifo(std::string const& fifoPath)
{
    ConnFifo* fifo = new ConnFifo();
    fifo->m_isHost = false;
    fifo->m_fifoPath = fifoPath;

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

    return std::unique_ptr<ConnFifo>(fifo);
}

bool ConnFifo::isValid() const
{
    return !(m_writeFileDisriptor == -1 || m_readFileDisriptor == -1);
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

    if (!m_fifoPath.empty() && m_isHost)
    {
        unlink(m_fifoPath.c_str());
    }
}

bool ConnFifo::Read(void* buf, size_t maxSize)
{
    if (!isValid()) { return false; }

    char tmpBuf[alias::MAX_MSG_SIZE];
    ssize_t bytesRead = read(m_readFileDisriptor, tmpBuf, maxSize);
    if (bytesRead == -1) { return false; }

    if ((tmpBuf[0] == '0' && m_isHost) || (tmpBuf[0] == '1' && !m_isHost))
    {
        std::memcpy(buf, &tmpBuf[1], --bytesRead);
    }
    else
    {
        Write(&tmpBuf[1], bytesRead - 1);
        return false;
    }

    return bytesRead > 0;
}

bool ConnFifo::Write(const void* buf, size_t count)
{
    if (!isValid()) { return false; }

    char tmpBuf[alias::MAX_MSG_SIZE];
    tmpBuf[0] = m_isHost ? '1' : '0';
    std::memcpy(&tmpBuf[1], buf, count);
    ssize_t bytesWritten = write(m_writeFileDisriptor, tmpBuf, count + 1);
    if (bytesWritten == -1) { return false; }

    return static_cast<size_t>(bytesWritten) == count + 1;
}
