#include "conn_fifo.hpp"

ConnFifo::ConnFifo(int id, bool create) : id(id), create(create) {
    if (create) {
        mkfifo("/tmp/myfifo", 0666);
        fifo = open("/tmp/myfifo", O_RDWR);
    } else {
        fifo = open("/tmp/myfifo", O_RDWR);
    }
}

void ConnFifo::Read(char *buf, size_t count) {
    read(fifo, buf, count);
}

void ConnFifo::Write(const char *buf, size_t count) {
    write(fifo, buf, count);
}

ConnFifo::~ConnFifo() {
    close(fifo);
    if (create) {
        unlink("/tmp/myfifo");
    }
}
