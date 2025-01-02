#include "../host/host.h"

class HostMq : public Host {
public:

    HostMq(const std::vector<Book>& books_);
    ~HostMq();

    bool setup_conn() override;
};