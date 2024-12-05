#include "Utils.hpp"

namespace utils
{

QString HistoryBookInfo::toQString() const
{
    QString timeStr = timeStamp.toString("yyyy-MM-dd HH:mm:ss");
    QString clientIdStr = QString::number(clientId);
    QString opType = op.type == http::OperationType_e::POST ? "POST" : "PUT";
    QString status = op.status == http::OperationStatus_e::OK ? "OK" : "FAIL";
    return QString("[%1][ID=%2]: %3/%4 [%5], %2 \"%3\": %4").arg(timeStr, clientIdStr, opType, QString::fromStdString(name), status);
}

QString ClientInfo::toQString() const
{
    QString clientIdStr = QString::number(clientId);
    if (!readingBook.empty())
    {
        return QString("[Client][ID=%1] reading %2").arg(clientIdStr, QString::fromStdString(readingBook));
    }

    QString secondsToKillStr = QString::number(secondsToKill);
    return QString("[Client][ID=%1] no read, time to unlink: %2 s").arg(clientIdStr, secondsToKillStr);    
}

} // namespace utils
