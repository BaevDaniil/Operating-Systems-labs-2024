#include "conn_pipe.hpp"
#include "Common/Logger.hpp"

#include <unistd.h>
#include <cstring>

ConnPipe::~ConnPipe()
{
    if (m_readFileDescriptor != -1)
    {
        close(m_readFileDescriptor);
    }

    if (m_writeFileDescriptor != -1)
    {
        close(m_writeFileDescriptor);
    }
}

std::pair<std::unique_ptr<ConnPipe>, std::unique_ptr<ConnPipe>> ConnPipe::createPipeConns()
{
    // [0] - read, [1] - write
    std::array<alias::desriptor_t, 2> hostToClient;
    std::array<alias::desriptor_t, 2> clientToHost;

    if (pipe(hostToClient.data()) == -1 || pipe(clientToHost.data()) == -1)
    {
        LOG_ERROR("PIPE", "Failed to create pipes");
        return std::make_pair<std::unique_ptr<ConnPipe>, std::unique_ptr<ConnPipe>>(nullptr, nullptr);
    }

    ConnPipe* pipeHost = new ConnPipe();
    pipeHost->m_readFileDescriptor = clientToHost[0];
    pipeHost->m_writeFileDescriptor = hostToClient[1];

    ConnPipe* pipeClient = new ConnPipe();
    pipeClient->m_readFileDescriptor = hostToClient[0];
    pipeClient->m_writeFileDescriptor = clientToHost[1];

    return std::make_pair<std::unique_ptr<ConnPipe>, std::unique_ptr<ConnPipe>>(std::unique_ptr<ConnPipe>(pipeHost), std::unique_ptr<ConnPipe>(pipeClient));
}

bool ConnPipe::isValid() const
{
    return !(m_writeFileDescriptor == -1 || m_readFileDescriptor == -1);
}

bool ConnPipe::Read(void* buf, size_t maxSize)
{
    if (!isValid()) { return false; }

    ssize_t bytesRead = read(m_readFileDescriptor, buf, maxSize);
    if (bytesRead == -1) { return false; }

    return bytesRead > 0;
}

bool ConnPipe::Write(const void* buf, size_t count)
{
    if (!isValid()) { return false; }

    ssize_t bytesWritten = write(m_writeFileDescriptor, buf, count);
    if (bytesWritten == -1) { return false; }

    return bytesWritten == static_cast<ssize_t>(count);
}
