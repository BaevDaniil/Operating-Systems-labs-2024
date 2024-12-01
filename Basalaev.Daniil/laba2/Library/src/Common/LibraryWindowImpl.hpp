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

    virtual void onSuccessTakeBook() = 0;
    virtual void onFailedTakeBook() = 0;
    virtual void onSuccessReturnBook() = 0;
    virtual void onFailedReturnBook() = 0;

signals:
    void bookSelected(const std::string& bookName);
    void bookReturned(const std::string& bookName);

protected:
    void createHistoryView();

    QListWidget* m_historyList;
    QListWidget* m_bookList;
    alias::id_t m_id;
};
