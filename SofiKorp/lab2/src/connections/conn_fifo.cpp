#include "conn_fifo.hpp"

FIFOConnection::FIFOConnection(const std::string &name, bool create)
{
    // Указываем путь к FIFO файлу
    std::cerr << name << std::endl;
    pathname = std::filesystem::current_path() / name;
    std::cerr << pathname << std::endl;

    // Создание FIFO файла, если флаг create установлен
    if (create)
    {
        std::cerr << "Creating " << pathname << std::endl;
        if (mkfifo(pathname.c_str(), 0777) == -1)
        {
            std::cerr << "Already created " << pathname << std::endl;
            // Если файл уже существует, игнорируем ошибку
            if (errno != EEXIST)
            {
                std::cerr << std::strerror(errno) << std::endl;
                throw std::runtime_error("Failed to create FIFO channel");
            }
        }
        std::cerr << "Created: " << pathname << std::endl;

    }

    // Открытие FIFO канала
    fd = open(pathname.c_str(), O_RDWR);
    if (fd == -1)
    {
        std::cout << pathname << " " << std::strerror(errno) << std::endl;
        throw std::runtime_error("Failed to open FIFO channel");
    }
}

bool FIFOConnection::Read(std::string &message)
{
    char buffer[max_msg_size];
    memset(buffer, '\0', max_msg_size);

    // Открытие FIFO канала для чтения
    fd = open(pathname.c_str(), O_RDWR);
    if (fd == -1)
    {
        std::cout << pathname << " " << std::strerror(errno) << std::endl;
        throw std::runtime_error("Failed to open FIFO channel");
    }

    // Чтение данных из FIFO канала
    ssize_t bytes_read = read(fd, buffer, max_msg_size - 1);
    if (bytes_read == -1)
    {
        return false; // Ошибка чтения
    }

    // Запись прочитанного сообщения в строку
    message.assign(buffer, bytes_read);
    return true;
}

bool FIFOConnection::Write(const std::string &message)
{
    // Открытие FIFO канала для записи
    fd = open(pathname.c_str(), O_RDWR);
    if (fd == -1)
    {
        std::cout << pathname << " " << std::strerror(errno) << std::endl;
        throw std::runtime_error("Failed to open FIFO channel");
    }

    // Запись сообщения в FIFO канал
    ssize_t bytesWritten = write(fd, message.c_str(), message.size());
    if (bytesWritten == -1)
    {
        std::cout << "Error writing to FIFO: " << strerror(errno) << "\n";
        return false; // Ошибка записи
    }
    return true;
}

FIFOConnection::~FIFOConnection()
{
    // Закрытие канала и удаление FIFO файла
    close(fd);
    unlink(pathname.c_str());
    std::cerr<<"removed " << pathname << std::endl;
}