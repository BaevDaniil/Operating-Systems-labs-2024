#include "conn_mq.hpp"

MQConnection::MQConnection(const std::string &id, bool create)
    : name("/queue_" + id), attr()
{
    // Настройка атрибутов очереди сообщений
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    attr.mq_maxmsg = max_msg_count;
    attr.mq_msgsize = max_msg_size;


    // Открытие очереди сообщений с флагом создания или без
    if (create)
    {
        mq = mq_open(name.c_str(), O_CREAT | O_RDWR | O_NONBLOCK, 0777, &attr);
    }
    else
    {
        mq = mq_open(name.c_str(), O_RDWR | O_NONBLOCK);
    }

    if (mq == (mqd_t)-1)
    {
        std::cerr << "Error opening message queue: " << strerror(errno) << std::endl;
        throw std::runtime_error("Error opening message queue");
    }
}

bool MQConnection::Read(std::string &message)
{
    char buffer[max_msg_size];
    memset(buffer, 0, sizeof(buffer));

    // Открытие очереди сообщений для чтения
    mq = mq_open(name.c_str(), O_RDWR | O_NONBLOCK);
    if (mq == (mqd_t)-1)
    {
        std::cerr << "Error opening message queue: " << strerror(errno) << std::endl;
        throw std::runtime_error("Error opening message queue");
    }

    // Чтение сообщения из очереди
    ssize_t bytesReceived = mq_receive(mq, buffer, sizeof(buffer), nullptr);
    if (bytesReceived == -1)
    {
        return false;  // Ошибка чтения
    }

    // Присваивание прочитанного сообщения
    message.assign(buffer, bytesReceived);
    return true;
}

bool MQConnection::Write(const std::string &message)
{
    // Открытие очереди сообщений для записи
    mq = mq_open(name.c_str(), O_RDWR | O_NONBLOCK);
    if (mq == (mqd_t)-1)
    {
        std::cerr << "Error opening message queue: " << strerror(errno) << std::endl;
        throw std::runtime_error("Error opening message queue");
    }

    // Отправка сообщения в очередь
    if (mq_send(mq, message.c_str(), message.size(), 0) == -1)
    {
        return false;  // Ошибка отправки
    }

    return true;
}

MQConnection::~MQConnection()
{
    // Закрытие очереди сообщений
    mq_close(mq);
}