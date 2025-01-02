#include "conn_seg.h"


ConnSeg::ConnSeg(key_t key, bool isHost) {
    _isHost = isHost;
    int f = _isHost ? IPC_CREAT | O_EXCL | 0666 : 0666;
    _shmid = shmget(key, MAX_SIZE, f);
    _segptr = shmat(_shmid, 0, 0);
    if (_segptr == (void*)-1) {
        if (_isHost)
            shmctl(_shmid, IPC_RMID, 0);
    }
}

bool ConnSeg::read(std::string& data, size_t max_size) {
    if (is_valid()) {
        if (max_size > MAX_SIZE)
            return false;
        char buf[MAX_SIZE];
        memcpy(buf, _segptr, max_size);
        data = buf;
        memcpy(_segptr, "lalala", 6);
        if (data.substr(0, 10) == "written###") {
            data = data.substr(10);
            return true;
        }
    }

    return false;
}

bool ConnSeg::write(const std::string& data) {
    if (data.size() > MAX_SIZE)
        return false;
    std::string new_data = "written###" + data + '\0';
    memcpy(_segptr, new_data.c_str(), new_data.size());

    return true;
}

ConnSeg::~ConnSeg(void) {
    shmdt(_segptr);
    if (_isHost)
        shmctl(_shmid, IPC_RMID, 0);
}

bool ConnSeg::is_valid() const {
    return (_segptr != nullptr);
}
