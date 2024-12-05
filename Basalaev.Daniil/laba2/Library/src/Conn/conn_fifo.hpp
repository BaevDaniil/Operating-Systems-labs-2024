#pragma once

#include "Common/Alias.hpp"
#include "Conn/conn_impl.hpp"

#include <string>
#include <memory>

class ConnFifo : public connImpl
{
public:
    ~ConnFifo() override;

    static std::unique_ptr<ConnFifo> crateHostFifo(std::string const& fifoPath);
    static std::unique_ptr<ConnFifo> crateClientFifo(std::string const& fifoPath);

    bool Read(void* buf, size_t count) override;
    bool Write(const void* buf, size_t count) override;

    bool isValid() const override;

private:

    std::string m_fifoPath;
    alias::desriptor_t m_readFileDisriptor = -1;
    alias::desriptor_t m_writeFileDisriptor = -1;
};
