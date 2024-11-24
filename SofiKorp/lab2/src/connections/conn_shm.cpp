#include "conn_shm.hpp"

SHMConnection::SHMConnection(const std::string &id, bool create)
    : name("/shm_" + id)
{
    // Открытие разделяемой памяти
    fd = shm_open(name.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        std::cerr << "Error shm opening: " << strerror(errno) << std::endl;
        throw std::runtime_error("Error opening shm");
    }

    // Установка размера разделяемой памяти
    res = ftruncate(fd, max_msg_size);
    if (res == -1)
    {
        std::cerr << "Error shm ftruncate: " << strerror(errno) << std::endl;
        throw std::runtime_error("Error opening ftruncate");
    }

    // Отображение разделяемой памяти в адресное пространство
    addr = mmap(NULL, max_msg_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
        std::cerr << "Error shm mmap: " << strerror(errno) << std::endl;
        throw std::runtime_error("Error opening mmap");
    }
}

bool SHMConnection::Read(std::string &message)
{
    char buffer[max_msg_size];

    // Копирование данных из разделяемой памяти в буфер
    memcpy(buffer, addr, max_msg_size);

    // Присваивание строки с учетом длины данных
    message.assign(buffer, strnlen(buffer, max_msg_size));
    return true;
}

bool SHMConnection::Write(const std::string &message)
{
    // Копирование данных в разделяемую память
    memcpy(addr, message.c_str(), message.size() + 1);
    return true;
}

SHMConnection::~SHMConnection()
{
    // Удаление разделяемой памяти
    shm_unlink(name.c_str());
}