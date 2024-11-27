#include <string>

class Conn {
public:
    Conn() = default;
    virtual ~Conn() = default;
    virtual bool Read(std::string& message) = 0;
    virtual bool Write(std::string const& message) = 0;
}