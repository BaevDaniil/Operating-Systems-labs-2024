#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QListWidget>
#include <QStackedWidget>
#include <QString>
#include <vector>
#include <string>
#include <QWidget>
#include "Book.hpp"

class LibraryWindowImpl : public QMainWindow
{
    Q_OBJECT

public:
    LibraryWindowImpl(alias::book_container_t const&, QWidget* parent = nullptr);
    virtual ~LibraryWindowImpl() = default;

    virtual void onSuccessPostBook() = 0;
    virtual void onFailedPostBook() = 0;
    virtual void onSuccessPutBook() = 0;
    virtual void onFailedPutBook() = 0;
    virtual void addHistoryEntry(utils::HistoryBookInfo const&) = 0;

signals:
    void bookSelected(const QString& bookName);
    void bookReturned(const QString& bookName);

private:
    void createBookView(const std::vector<Book>& books);
    void createReadingView();
    void createHistoryView();

    QListWidget* historyList;
    QListWidget* bookList;
};

LibraryWindowImpl::LibraryWindowImpl(alias::book_container_t const& books, QWidget* parent)
    : QMainWindow(parent)
{
    // TODO
}
