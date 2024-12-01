#pragma once

#include "conn.hpp"
#include "Common/Alias.hpp"

#include <string>

class ConnFifo : public conn
{
public:
    ~ConnFifo() override;

    static std::unique_ptr<ConnFifo*> crateHostFifo(std::string const& fifoPath);
    static std::unique_ptr<ConnFifo*> crateClientFifo(std::string const& fifoPath);

    bool Read(void* buf, size_t count) override;
    bool Write(const void* buf, size_t count) override;

    bool IsInitialized() const override;

private:
    using namespace alias;

    std::string m_fifoPath;
    desriptor_t m_readFileDisriptor = -1;
    desriptor_t m_writeFileDisriptor = -1;
};
