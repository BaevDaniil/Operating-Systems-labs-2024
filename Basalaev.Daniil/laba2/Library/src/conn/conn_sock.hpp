#pragma once

#include "connImpl.hpp"
#include "Common/Alias.hpp"

#include <sys/socket.h>
#include <netinet/in.h>

class ConnSock : public connImpl
{
public:
    using namespace alias;
    ~ConnSock() override;

    static std::unique_ptr<ConnSock*> crateHostSocket(port_t hostPort);
    static std::unique_ptr<ConnSock*> crateClientSocket(port_t hostPort);

    std::unique_ptr<ConnSock*> Accept();

    bool Read(void* buf, size_t count) override;
    bool Write(const void* buf, size_t count) override;

    bool IsInitialized() const override;

private:
    desriptor_t m_socketFileDesriptor = -1;
    address_t m_address;
};
