#include "../host/host.h"

class HostSeg : public Host {
public:
    HostSeg(const std::vector<Book>& books_);
    ~HostSeg(); 

    bool setup_conn() override;
};