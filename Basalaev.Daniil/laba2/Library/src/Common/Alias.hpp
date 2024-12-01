#pragma once

#include "Book.hpp"

#include <netinet/in.h>
#include <vector>

namespace alias
{

using id_t = unsigned int;
using port_t = int;
using desriptor_t = int;
using address_t = sockaddr_in;
using book_container_t = std::vector<Book>;

inline constexpr id_t HOST_ID = 0;

} // namespace alias
