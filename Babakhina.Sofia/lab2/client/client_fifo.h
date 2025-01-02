#pragma once

#include "client.h"
#include "../connection/conn_fifo.h"


class ClientFifo : public Client {  
public:
    ClientFifo(std::vector<Book> books);
    ~ClientFifo();
    
    bool setup_conn() override;
};

