#pragma once

#include "Http.hpp"

#include <QString>
#include <QDateTime>
#include <string>

namespace utils
{
struct HistoryBookInfo
{
    QString toQString() const;

    QDateTime timeStamp;
    int clientId;
    std::string name;
    operation op;
};

QString HistoryBookInfo::toQString() const
{
    QString timeStr = timestamp.toString("yyyy-MM-dd HH:mm:ss");
    QString clientIdStr = QString::number(clientId);
    QString opType = op.type == OperationType_e::POST ? "POST" : "PUT";
    QString status = op.status == OperationType_e::OK ? "OK" : "FAIL";
    return QString("[%1][ID=%2]: %3/%4 [%5], %2 \"%3\": %4").arg(timeStr, clientIdStr, opType, name, status);
}

} // namespace utils
