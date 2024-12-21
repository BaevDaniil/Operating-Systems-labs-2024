#pragma once

#include "client.h"
#include "../connection/conn_mq.h"

class ClientMq : public Client {
public:
    ClientMq(std::vector<Book> books);
    ~ClientMq();
    
    bool setup_conn() override;
};

