#pragma once

#include "Alias.hpp"

#include <optional>
#include <string>

namespace http
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

//! format request: http://{operation_type}/{id}/{bookName}
struct request
{
    static std::optional<request> parse(std::string const& reqMsg);
    std::string toString() const;

    OperationType_e type;
    int id;
    std::string bookName;
};

//! format response: http://head/{id}/{status}
struct response
{
    static std::optional<response> parse(std::string const& rspMsg);
    std::string toString() const;

    int id;
    OperationStatus_e status;
};

//! format notification: http://notification/{books}
struct notification
{
    static std::optional<notification> parse(std::string const& notificationMsg);
    std::string toString() const;

    alias::book_container_t books;
};

} // namespace http
