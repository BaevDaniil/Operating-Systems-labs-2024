#include "conn.h"


constexpr int MAX_SIZE = 1024;

class ConnSeg : public Conn {
public:
    ConnSeg(key_t key, bool isHost);

    bool read(std::string& data, size_t max_size) override;
    bool write(const std::string& data) override;
    bool is_valid() const;
    void close(){}

    ~ConnSeg();

private:
    bool _isHost;
    void *_segptr = nullptr;
    int _shmid = -1;
};