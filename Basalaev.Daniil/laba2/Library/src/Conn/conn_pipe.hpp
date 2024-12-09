#pragma once

#include "Common/Alias.hpp"
#include "Conn/conn_impl.hpp"

#include <memory>
#include <array>
#include <string>

class ConnPipe : public connImpl
{
public:
    ~ConnPipe() override;

    static std::pair<std::unique_ptr<ConnPipe>, std::unique_ptr<ConnPipe>> createPipeConns();

    bool Read(void* buf, size_t maxSize = alias::MAX_MSG_SIZE) override;
    bool Write(const void* buf, size_t count) override;

    bool isValid() const override;

private:
    alias::desriptor_t m_readFileDescriptor = -1;
    alias::desriptor_t m_writeFileDescriptor = -1;
};
