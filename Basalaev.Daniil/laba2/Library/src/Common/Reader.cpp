#include "Reader.hpp"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

std::optional<alias::book_container_t> Reader::parse(std::string const& filePath)
{
    QFile file(QString::fromStdString(filePath));

    if (!file.open(QIODevice::ReadOnly)) { return {}; }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData, &parseError);

    if (parseError.error != QJsonParseError::NoError) { return {}; }

    if (!jsonDoc.isObject()) { return {}; }

    QJsonObject rootObj = jsonDoc.object();

    if (!rootObj.contains("books") || !rootObj["books"].isArray()) { return {}; }

    QJsonArray booksArray = rootObj["books"].toArray();
    alias::book_container_t books;

    for (const QJsonValue& bookValue : booksArray)
    {
        if (!bookValue.isObject()) { return {}; }

        QJsonObject bookObj = bookValue.toObject();

        if (!bookObj.contains("name") || !bookObj["name"].isString() ||
            !bookObj.contains("amount") || !bookObj["amount"].isDouble())
        {
            return {};
        }

        books.emplace_back(Book{.name = bookObj["name"].toString().toStdString(), .amount = static_cast<int>(bookObj["amount"].toDouble())});
    }

    return books;
}
