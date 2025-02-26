#pragma once
#include "connection.hpp"

class MQConnection: public Connection
{
private:
    std::string name;
    struct mq_attr attr;
    static const int MAX_MESSAGE_SIZE = 1024;
    static const int MAX_MESSAGE_COUNT = 10;
    mqd_t mq;
    static std::string typeToString() { return "mq"; }

public:
    explicit MQConnection(const std::string &id, bool create);
    virtual bool Read(std::string &) override;
    virtual bool Write(const std::string &) override;

    virtual ~MQConnection() override;

    MQConnection() = default;
    MQConnection(const MQConnection &) = default;
    MQConnection &operator=(const MQConnection &) = default;
    MQConnection(MQConnection &&) = default;
    MQConnection &operator=(MQConnection &&) = default;

    static std::string GenerateFilename(int num1, int num2)
    {
        return typeToString() + std::to_string(num1) + '_' + std::to_string(num2);
    }

    static std::string GenerateGeneralFilename(int num1, int num2)
    {
        return "general" + GenerateFilename(num1, num2) ;
    }
};