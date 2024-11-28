#include "conn_shm.hpp"

GeneralFiles::GeneralFiles(const std::string &id, bool create) : name("/shm_" + id)
{
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
}

bool GeneralFiles::Read(std::string &message)
{
    if (!addr)
        return false;

    fd = shm_open(name.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        std::cerr << "Error: Unable to open shared memory: " << strerror(errno) << "\n";
        return false;
    }

    char buffer[max_msg_size];
    memcpy(buffer, addr, max_msg_size);

    if (strnlen(buffer, max_msg_size) == 0)
        return false;

    message.assign(buffer, strnlen(buffer, max_msg_size));
    return true;
}

bool GeneralFiles::Write(const std::string &message)
{
    if (!addr)
        return false;

    fd = shm_open(name.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        std::cerr << "Error: Unable to open shared memory: " << strerror(errno) << "\n";
        return false;
    }
    
    memcpy(addr, message.c_str(), message.size() + 1);
    return true;
}

GeneralFiles::~GeneralFiles()
{
    close(fd);
    shm_unlink(name.c_str());
}