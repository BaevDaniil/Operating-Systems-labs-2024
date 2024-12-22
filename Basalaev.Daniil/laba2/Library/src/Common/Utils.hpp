#pragma once

#include "Alias.hpp"
#include "Http.hpp"

#include <QString>
#include <QDateTime>
#include <QTimer>
#include <memory>
#include <string>

namespace utils
{
struct HistoryBookInfo
{
    QString toQString() const;

    QDateTime timeStamp;
    alias::id_t clientId;
    std::string name;
    http::operation op;
};

struct ClientInfo
{
    QString toQString() const;

    alias::id_t clientId;
    std::string readingBook;
    int secondsToKill = 5;
};

struct ClientInfoWithTimer
{
    ClientInfo info;
    std::unique_ptr<QTimer> timer;
};

} // namespace utils
