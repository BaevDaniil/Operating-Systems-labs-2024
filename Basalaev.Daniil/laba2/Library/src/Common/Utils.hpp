#pragma once

#include <QString>
#include <QDateTime>
#include <string>

namespace utils
{

enum class OperationType_e : bool
{
    POST, // Take book
    PUT // Return book
};

enum class OperationStatus_e : bool
{
    OK,
    FAIL
};

struct operation
{
    OperationType_e type;
    OperationStatus_e status;
};

struct HistoryBookInfo
{
    QString toQString();

    QDateTime timeStamp;
    int clientId;
    std::string name;
    operation op;
};

HistoryBookInfo::toQString()
{
    QString timeStr = timestamp.toString("yyyy-MM-dd HH:mm:ss");
    QString clientIdStr = QString::number(clientId);
    QString opType = op.type == OperationType_e::POST ? "POST" : "PUT";
    QString status = op.status == OperationType_e::OK ? "OK" : "FAIL";
    return QString("[%1][ID=%2]: %3/%4 [%5], %2 \"%3\": %4").arg(timeStr, clientIdStr, opType, name, status);
}

} // namespace utils
