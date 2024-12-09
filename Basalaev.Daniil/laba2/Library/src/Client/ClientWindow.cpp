#include "ClientWindow.hpp"
#include "Common/Logger.hpp"

#include <QMessageBox>
#include <QDockWidget>
#include <QDateTime>

ClientWindow::ClientWindow(alias::id_t id, alias::book_container_t const& books, QWidget* parent)
    : LibraryWindowImpl(id, books, parent)
{
    m_stackedWidget = new QStackedWidget(this);

    createBookView(books);
    createReadingView();

    setCentralWidget(m_stackedWidget);
    setWindowTitle(QString("Client Window [ID=%1]").arg(id));
    resize(400, 300);

    m_stackedWidget->setCurrentIndex(0);
    m_stackedWidget->setStyleSheet(
        "QStackedWidget {"
        "   background-color: #f8f9fa;"
        "   border: 2px solid #87ceeb;"
        "   border-radius: 10px;"
        "   padding: 10px;"
        "}"
    );
}

ClientWindow::~ClientWindow() = default;

void ClientWindow::createBookView(alias::book_container_t const& books)
{
    QWidget* bookView = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(bookView);

    layout->addWidget(m_bookList);

    m_selectButton = new QPushButton("Select Book", this);
    m_selectButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4caf50;"
        "   color: white;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border-radius: 5px;"
        "   padding: 8px;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #cccccc;"
        "   color: #666666;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
    );
    m_selectButton->setEnabled(false);
    layout->addWidget(m_selectButton);

    connect(m_bookList, &QListWidget::itemSelectionChanged, [this]() {
        m_selectButton->setEnabled(m_bookList->currentItem() != nullptr);
    });
    connect(m_selectButton, &QPushButton::clicked, this, &ClientWindow::selectBook);

    m_stackedWidget->addWidget(bookView);
}

void ClientWindow::createReadingView()
{
    QWidget* readingView = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(readingView);

    m_readingLabel = new QLabel("Reading book: ", this);
    QFont readingFont = m_readingLabel->font();
    readingFont.setPointSize(16);
    readingFont.setBold(true);
    m_readingLabel->setFont(readingFont);
    m_readingLabel->setStyleSheet(
        "QLabel {"
        "   color: #333333;"
        "}"
    );
    layout->addWidget(m_readingLabel);

    m_stopReadingButton = new QPushButton("Stop Reading", this);
    m_stopReadingButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #ff6b6b;"
        "   color: white;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border-radius: 5px;"
        "   padding: 8px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #ff4c4c;"
        "}"
    );
    layout->addWidget(m_stopReadingButton);

    connect(m_stopReadingButton, &QPushButton::clicked, this, &ClientWindow::stopReading);

    m_stackedWidget->addWidget(readingView);
}

void ClientWindow::selectBook()
{
    if (m_bookList->currentItem())
    {
        QString bookName = QString::fromStdString(getCurrentBook());
        m_readingLabel->setText("Reading book: " + bookName);

        emit bookSelected(bookName.toStdString(), m_id);
    }
}

void ClientWindow::stopReading()
{
    QString bookName = m_readingLabel->text().split(": ").last();
    emit bookReturned(bookName.toStdString(), m_id);

    m_stackedWidget->setCurrentIndex(0);
}

std::string ClientWindow::getCurrentBook() const
{
    return m_bookList->currentItem()->text().split(": ").first().toStdString();
}

void ClientWindow::onSuccessTakeBook(std::string const& bookName, alias::id_t clientId)
{
    m_stackedWidget->setCurrentIndex(1);
    LibraryWindowImpl::onSuccessTakeBook(bookName, clientId);
}

void ClientWindow::onSuccessReturnBook(std::string const& bookName, alias::id_t clientId)
{
    m_stackedWidget->setCurrentIndex(0);
    LibraryWindowImpl::onSuccessReturnBook(bookName, clientId);
}
