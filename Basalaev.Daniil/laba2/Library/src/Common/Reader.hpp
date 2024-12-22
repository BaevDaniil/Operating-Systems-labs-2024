#include "Alias.hpp"

#include <optional>

class Reader
{
public:

static std::optional<alias::book_container_t> parse(std::string const& filePath);

};