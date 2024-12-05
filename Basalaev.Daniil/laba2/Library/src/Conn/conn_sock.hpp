#pragma once

#include "Common/Alias.hpp"
#include "Conn/conn_impl.hpp"

#include <memory>

class ConnSock : public connImpl
{
public:
    ~ConnSock() override;

    static std::unique_ptr<ConnSock> crateHostSocket(alias::port_t hostPort);
    static std::unique_ptr<ConnSock> crateClientSocket(alias::port_t hostPort);

    std::unique_ptr<ConnSock> Accept();

    bool Read(void* buf, size_t count) override;
    bool Write(const void* buf, size_t count) override;

    bool isValid() const override;

private:
    alias::desriptor_t m_socketFileDesriptor = -1;
    alias::address_t m_address;
};
