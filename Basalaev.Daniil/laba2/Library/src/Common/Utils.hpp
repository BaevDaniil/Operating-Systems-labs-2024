#pragma once

#include "Alias.hpp"
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
    alias::id_t clientId;
    std::string name;
    http::operation op;
};

} // namespace utils
