#pragma once

#include "client.h"
#include "../connection/conn_seg.h"


class ClientSeg : public Client {
public:
    ClientSeg(std::vector<Book> books);
    ~ClientSeg();
    
    bool setup_conn() override;
};

