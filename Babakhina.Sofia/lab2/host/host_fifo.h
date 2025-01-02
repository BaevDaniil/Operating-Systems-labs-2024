#include "../host/host.h"


class HostFifo : public Host {
  public:
    HostFifo(const std::vector<Book>& books_);

    ~HostFifo();

    bool setup_conn() override;
};