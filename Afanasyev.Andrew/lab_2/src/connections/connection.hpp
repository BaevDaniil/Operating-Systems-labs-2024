#pragma once

#include <type_traits>
#include <thread>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <iostream>
#include <csignal>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <exception>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdexcept>
#include <utility>
#include <semaphore.h>
#include <errno.h>
#include <mqueue.h>
#include <semaphore>
#include <functional>
#include <queue>
#include <utility>
#include <memory>
#include <sys/mman.h>
#include <sys/un.h>

class Connection
{
public:
    virtual bool Read(std::string&) = 0;
    virtual bool Write(const std::string&) = 0;
    virtual ~Connection() = default;
    Connection() = default;
};