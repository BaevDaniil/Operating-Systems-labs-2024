#include "LibraryWindowImpl.hpp"

#include <QDockWidget>
#include <QCloseEvent>

LibraryWindowImpl::LibraryWindowImpl(alias::id_t id, alias::book_container_t const& books, QWidget* parent)
    : QMainWindow(parent)
    , m_id(id)
{
    m_bookList = new QListWidget(this);
    m_bookList->setFixedWidth(250);
    m_bookList->setStyleSheet(
        "QListWidget {"
        "   background-color: #f4f4f4;"
        "   border: 1px solid #c0c0c0;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   font-size: 14px;"
        "   color: #333333;"
        "}"
        "QListWidget::item {"
        "   padding: 5px;"
        "   border-bottom: 1px solid #d0d0d0;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #87cefa;"
        "   color: #ffffff;"
        "}"
    );

    updateBooks(books);
    createHistoryView();
    setWindowFlags(Qt::CustomizeWindowHint);
}

std::string LibraryWindowImpl::getCurrentBook() const
{
    return m_bookList->currentItem() ? m_bookList->currentItem()->text().split(": ").first().toStdString() : "";
}

void LibraryWindowImpl::updateBooks(alias::book_container_t const& books)
{
    std::string const currentBook = getCurrentBook();

    m_bookList->clear();
    for (auto const& book : books)
    {
        m_bookList->addItem(QString::fromStdString(book.name) + ": " + QString::number(book.amount)); 
    }

    for (int i = 0; i < m_bookList->count(); ++i)
    {
        auto* item = m_bookList->item(i);
        std::string itemBookName = item->text().split(": ").first().toStdString();
        if (itemBookName == currentBook)
        {
            m_bookList->setCurrentItem(item);
            break;
        }
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
    m_historyList->setFixedWidth(400);
    m_historyList->setStyleSheet(
        "QListWidget {"
        "   background-color: #fff8e1;"
        "   border: 1px solid #c0c0c0;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   font-size: 14px;"
        "   color: #333333;"
        "}"
        "QListWidget::item {"
        "   padding: 5px;"
        "   border-bottom: 1px solid #d0d0d0;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #ffd54f;"
        "   color: #ffffff;"
        "}"
    );

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

void LibraryWindowImpl::closeEvent(QCloseEvent* event)
{
    if (m_letClose)
    {
        QMainWindow::closeEvent(event);
    }
    else
    {
        event->ignore();
    }
}
