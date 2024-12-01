#pragma once

#include <vector>

struct Book;

namespace alias
{

using id_t = uint;
using port_t = int;
using desriptor_t = int;
using address_t = struct sockaddr_in;
using book_container_t = std::vector<Book>;

inline constexpr auto HOST_ID = 0;

} // namespace alias
