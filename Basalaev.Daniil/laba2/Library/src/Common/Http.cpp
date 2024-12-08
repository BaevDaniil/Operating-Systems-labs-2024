#include "Http.hpp"

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <iostream> //
namespace http
{

std::optional<request> request::parse(std::string const& reqMsg)
{
    request req;
    // TODO: use regex
    // parse foramt
    if (reqMsg.substr(0, 7) != "http://") { return {}; }

    // parse operation
    uint idxOfStartId = 11;
    if (reqMsg.substr(7, 4) == "POST")
    {
        idxOfStartId = 12;
        req.type = OperationType_e::POST;
    }
    else if (reqMsg.substr(7, 3) == "PUT")
    {
        req.type = OperationType_e::PUT;
    }
    else { return {}; }

    // parse id
    auto idxOfLastSlash = reqMsg.rfind('/');
    if (idxOfLastSlash == std::string::npos) { return {}; }
    req.id = std::stoi(reqMsg.substr(idxOfStartId, idxOfLastSlash - idxOfStartId));

    // parse book name
    req.bookName = reqMsg.substr(idxOfLastSlash+1);

    return {std::move(req)};
}

std::string request::toString() const
{
    std::string opType = type == OperationType_e::POST ? "POST" : "PUT";
    std::string idStr = std::to_string(id);
    return std::string("http://" + opType + "/" + idStr + "/" + bookName);
}

std::optional<response> response::parse(std::string const& rspMsg)
{
    response rsp;
    // TODO: use regex
    // parse foramt
    if (rspMsg.substr(0, 12) != "http://head/") { return {}; }

    // parse id
    auto idxOfLastSlash = rspMsg.rfind('/');
    if (idxOfLastSlash == std::string::npos || idxOfLastSlash <= 12) { return {}; }
    rsp.id = std::stoi(rspMsg.substr(12, idxOfLastSlash - 12)); // TODO: catch ex

    // parse book name
    if (auto op = rspMsg.substr(idxOfLastSlash+1); op == "OK")
    {
        rsp.status = OperationStatus_e::OK;
    }
    else if (op == "FAIL")
    {
        rsp.status = OperationStatus_e::FAIL;
    }
    else { return {}; }

    return {std::move(rsp)};
}

std::string response::toString() const
{
    std::string opStatus = status == OperationStatus_e::OK ? "OK" : "FAIL";
    std::string idStr = std::to_string(id);
    return std::string("http://head/" + idStr + "/" + opStatus);
}


std::optional<notification> notification::parse(std::string const& notificationMsg)
{
    notification notify;

    // parse foramt
    std::cout << "FORMAT: " << std::endl;
    if (notificationMsg.substr(0, 20) != "http://notification/") { return {}; }
    std::cout << "COORECT: " << std::endl;

    std::cout << "isArray: " << std::endl;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(QString::fromStdString(notificationMsg.substr(20)).toUtf8());
    if (!jsonDocument.isArray()) { return {}; }
    std::cout << "CORRECT: " << std::endl;

    // parse books
    QJsonArray jsonArray = jsonDocument.array();
    for (auto const& jsonValue : jsonArray)
    {
        std::cout << "isObject: " << std::endl;
        if (!jsonValue.isObject()) { return {}; }
        std::cout << "CORRECT: " << std::endl;

        QJsonObject jsonBook = jsonValue.toObject();
        if (!jsonBook.contains("name") || !jsonBook.contains("amount")) { return {}; }
        std::cout << "name and amount = CORRECT: " << std::endl;

        notify.books.emplace_back(Book{.name = jsonBook["name"].toString().toStdString(), .amount = jsonBook["amount"].toInt()});
    }

    return {std::move(notify)};
}

std::string notification::toString() const
{
    QJsonArray jsonArray;

    for (auto const& book : books)
    {
        QJsonObject jsonBook;
        jsonBook["name"] = QString::fromStdString(book.name);
        jsonBook["amount"] = book.amount;
        jsonArray.append(jsonBook);
    }

    QJsonDocument jsonDocument(jsonArray);
    return "http://notification/" + jsonDocument.toJson(QJsonDocument::Compact).toStdString();
}

} // namespace http
