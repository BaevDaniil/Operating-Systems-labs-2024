#pragma once

#include "Common/Alias.hpp"
#include "Conn/conn_impl.hpp"

#include <memory>

class ConnSock : public connImpl
{
public:
    ~ConnSock() override;

    static std::unique_ptr<ConnSock> craeteHostSocket(alias::port_t hostPort);
    static std::unique_ptr<ConnSock> craeteClientSocket(alias::port_t hostPort);

    std::unique_ptr<ConnSock> accept();

    bool read(void* buf, size_t maxSize = alias::MAX_MSG_SIZE) override;
    bool write(const void* buf, size_t count) override;

    void close() override;

    bool isValid() const override;

private:
    alias::desriptor_t m_socketFileDesriptor = -1;
    alias::address_t m_address;
};
