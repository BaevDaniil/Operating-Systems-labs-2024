#include "LibraryWindowImpl.hpp"

#include <QDockWidget>

LibraryWindowImpl::LibraryWindowImpl(alias::id_t id, alias::book_container_t const& books, QWidget* parent)
    : QMainWindow(parent)
    , m_id(id)
{
    m_bookList = new QListWidget(this);
    m_bookList->setFixedWidth(250);
    updateBooks(books);
    createHistoryView();
}

void LibraryWindowImpl::updateBooks(alias::book_container_t const& books)
{
    m_bookList->clear();
    for (auto const& book : books)
    {
        m_bookList->addItem(QString::fromStdString(book.name) + ": " + QString::number(book.amount));
    }
}

void LibraryWindowImpl::addHistoryEntry(utils::HistoryBookInfo const& booksInfo)
{
    auto* listItem = new QListWidgetItem(booksInfo.toQString());

    if (booksInfo.op.status == http::OperationStatus_e::OK)
    {
        listItem->setForeground(QColor(Qt::green));
    }
    else
    {
        listItem->setForeground(QColor(Qt::red));
    }

    m_historyList->addItem(listItem);
}

void LibraryWindowImpl::createHistoryView()
{
    m_historyList = new QListWidget(this);
    m_historyList->setFixedWidth(550);
    auto* historyWidget = new QDockWidget("History", this);
    historyWidget->setWidget(m_historyList);
    addDockWidget(Qt::LeftDockWidgetArea, historyWidget);
}

void LibraryWindowImpl::onSuccessTakeBook(std::string const& bookName, alias::id_t clientId)
{
    utils::HistoryBookInfo bookInfo
    {
        .timeStamp = QDateTime::currentDateTime(),
        .clientId = clientId,
        .name = bookName,
        .op = {.type = http::OperationType_e::POST, .status = http::OperationStatus_e::OK}
    };

    addHistoryEntry(bookInfo);
}

void LibraryWindowImpl::onFailedTakeBook(std::string const& bookName, alias::id_t clientId)
{
    utils::HistoryBookInfo bookInfo
    {
        .timeStamp = QDateTime::currentDateTime(),
        .clientId = clientId,
        .name = bookName,
        .op = {.type = http::OperationType_e::POST, .status = http::OperationStatus_e::FAIL}
    };

    addHistoryEntry(bookInfo);
}

void LibraryWindowImpl::onSuccessReturnBook(std::string const& bookName, alias::id_t clientId)
{
    utils::HistoryBookInfo bookInfo
    {
        .timeStamp = QDateTime::currentDateTime(),
        .clientId = clientId,
        .name = bookName,
        .op = {.type = http::OperationType_e::PUT, .status = http::OperationStatus_e::OK}
    };

    addHistoryEntry(bookInfo);
}

void LibraryWindowImpl::onFailedReturnBook(std::string const& bookName, alias::id_t clientId)
{
    utils::HistoryBookInfo bookInfo
    {
        .timeStamp = QDateTime::currentDateTime(),
        .clientId = clientId,
        .name = bookName,
        .op = {.type = http::OperationType_e::PUT, .status = http::OperationStatus_e::FAIL}
    };

    addHistoryEntry(bookInfo);
}