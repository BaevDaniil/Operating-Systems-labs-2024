#pragma once

#include "conn_impl.hpp"
#include "Semaphore.hpp"
#include "Common/Alias.hpp"

#include <QObject>

class NetWorkElementImpl : public QObject
{
    Q_OBJECT

public:
    NetWorkElementImpl(alias::id_t, Semaphore&, conn&, QObject* parent = nullptr);
    virtual ~NetWorkElementImpl() = default;

    virtual int start() = 0;
    virtual void listen() = 0;

    alias::id_t getId() const noexcept { return m_id; }

private slots:
    virtual void handleBookSelected(const std::string& bookName) = 0;
    virtual void handleBookReturned(const std::string& bookName) = 0;

protected:
    alias::id_t m_id;
    Semaphore& m_semaphore;
    conn& m_connection;
};

NetWorkElementImpl::NetWorkElementImpl(alias::id_t id, Semaphore& semaphore, conn& connection, QObject* parent)
    : QObject(parent)
    , m_id(id);
    , m_semaphore(semaphore)
    , m_connection(connection)
{}
