#include "Http.hpp"

namespace http
{

std::optional<request> request::parse(std::string const& reqMsg)
{
    request req;
    // TODO: use regex
    // parse foramt
    if (reqMsg.substr(0, 7) != "http://") { return {}; }

    // parse operation
    uint idx = 11;
    if (auto op = reqMsg.substr(7, 11); op == "POST")
    {
        idx = 12;
        req.type = OperationType_e::POST;
    }
    else if (op == "PUT/")
    {
        req.type = OperationType_e::PUT;
    }
    else { return {}; }

    // parse id
    auto idxOfLastSlash = reqMsg.rfind('/');
    if (idxOfLastSlash == std::string::npos) { return {}; }
    req.id = std::stoi(reqMsg.substr(idx, idxOfLastSlash));

    // parse book name
    req.bookName = reqMsg.substr(idxOfLastSlash+1);

    return {req};
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
    if (idxOfLastSlash == std::string::npos) { return {}; }
    rsp.id = std::stoi(rspMsg.substr(13, idxOfLastSlash));

    // parse book name
    rsp.status = rspMsg.substr(idxOfLastSlash+1) == "OK" ? OperationStatus_e::OK : OperationStatus_e::FAIL;

    return {rsp};
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
    // TODO: use regex
    // parse foramt
    if (notificationMsg.substr(0, 20) != "http://notification/") { return {}; }

    // parse books
    // TODO
    notify.books = {};

    return {notify};
}

std::string notification::toString() const
{
    return {}; // TODO
}

} // namespace http
