#pragma once

#include "Alias.hpp"
#include "Utils.hpp"

#include <string>
#include <QMainWindow>
#include <QListWidget>
#include <QWidget>

class LibraryWindowImpl : public QMainWindow
{
    Q_OBJECT

public:
    LibraryWindowImpl(alias::id_t, alias::book_container_t const&, QWidget* parent = nullptr);
    virtual ~LibraryWindowImpl() = default;

    void updateBooks(alias::book_container_t const&);
    void addHistoryEntry(utils::HistoryBookInfo const&);

    virtual void onSuccessTakeBook(std::string const& bookName, alias::id_t clientId);
    virtual void onFailedTakeBook(std::string const& bookName, alias::id_t clientId);
    virtual void onSuccessReturnBook(std::string const& bookName, alias::id_t clientId);
    virtual void onFailedReturnBook(std::string const& bookName, alias::id_t clientId);

protected:
    void createHistoryView();

    QListWidget* m_historyList;
    QListWidget* m_bookList;
    alias::id_t m_id;
};
