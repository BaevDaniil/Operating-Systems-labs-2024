#include "conn_shm.hpp"

GeneralFiles::GeneralFiles(const std::string &id, bool create) : name("/shm_" + id)
{
    std::cout << "start opening" << std::endl;
    fd = shm_open(name.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        std::cout << "Error shm opening : " << strerror(errno) << std::endl;
        throw std::runtime_error("Error opening shm");
    }

    res = ftruncate(fd, max_msg_size);
    if (res == -1)
    {
        std::cout << "Error shm ftruncate : " << strerror(errno) << std::endl;
        throw std::runtime_error("Error opening ftruncate");
    }
    addr = mmap(NULL, max_msg_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
        std::cout << "Error shm mmap : " << strerror(errno) << std::endl;
        throw std::runtime_error("Error opening mmap");
    }
    std::cout << "end opening" << std::endl;
}

bool GeneralFiles::Read(std::string &message)
{
    char buffer[max_msg_size];
    memcpy(buffer, addr, max_msg_size);
    message.assign(buffer, max_msg_size);
    return true;
}

bool GeneralFiles::Write(const std::string &message)
{
    memcpy(addr, message.c_str(), message.size() + 1);
    return true;
}

GeneralFiles::~GeneralFiles()
{
    shm_unlink(name.c_str());
}