#include "LibraryWindowImpl.hpp"

#include <QDockWidget>

LibraryWindowImpl::LibraryWindowImpl(alias::id_t id, alias::book_container_t const& books, QWidget* parent)
    : QMainWindow(parent)
    , m_id(id)
{
    m_bookList = new QListWidget(this);
    updateBooks(books);
    createHistoryView();
}

void LibraryWindowImpl::updateBooks(alias::book_container_t const& books)
{
    m_bookList->clear();
    for (const auto& book : books)
    {
        m_bookList->addItem(QString::fromStdString(book.name) + ": " + QString::number(book.count));
    }
}

void LibraryWindowImpl::addHistoryEntry(utils::HistoryBookInfo const& booksInfo)
{
    m_historyList->addItem(booksInfo.toQString());
}

void LibraryWindowImpl::createHistoryView()
{
    m_historyList = new QListWidget(this);
    m_historyList->setFixedWidth(350);
    auto* historyWidget = new QDockWidget("History", this);
    historyWidget->setWidget(m_historyList);
    addDockWidget(Qt::LeftDockWidgetArea, historyWidget);
}