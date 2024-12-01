#pragma once

#include "Alias.hpp"
#include "Conn/conn_impl.hpp"
#include "SemaphoreLocal.hpp"

#include <QObject>

class NetWorkElementImpl : public QObject
{
public:
    NetWorkElementImpl(alias::id_t id, SemaphoreLocal& semaphore, connImpl& connection, QObject* parent = nullptr)
        : QObject(parent)
        , m_id(id)
        , m_semaphore(semaphore)
        , m_connection(connection)
    {}

    virtual ~NetWorkElementImpl() = default;

    virtual int start() = 0;
    virtual void listen() = 0;

    alias::id_t getId() const noexcept { return m_id; }

private slots:
    virtual void handleBookSelected(const std::string& bookName) = 0;
    virtual void handleBookReturned(const std::string& bookName) = 0;

protected:
    alias::id_t m_id;
    SemaphoreLocal& m_semaphore;
    connImpl& m_connection;
};
