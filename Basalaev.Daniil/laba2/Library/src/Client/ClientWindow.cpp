#include "ClientWindow.hpp"
#include "Common/Logger.hpp"

#include <QMessageBox>
#include <QDockWidget>
#include <QDateTime>

ClientWindow::ClientWindow(alias::id_t id, alias::book_container_t const& books, QWidget* parent)
    : LibraryWindowImpl(id, books, parent)
{
    stackedWidget = new QStackedWidget(this);

    createBookView(books);
    createReadingView();

    setCentralWidget(stackedWidget);
    setWindowTitle("Client Window");
    resize(400, 300);

    stackedWidget->setCurrentIndex(0);
}

ClientWindow::~ClientWindow() = default;

void ClientWindow::createBookView(alias::book_container_t const& books)
{
    QWidget* bookView = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(bookView);

    layout->addWidget(m_bookList);

    selectButton = new QPushButton("Select Book", this);
    selectButton->setEnabled(false);
    layout->addWidget(selectButton);

    connect(m_bookList, &QListWidget::itemSelectionChanged, [this]() {
        selectButton->setEnabled(m_bookList->currentItem() != nullptr);
    });
    connect(selectButton, &QPushButton::clicked, this, &ClientWindow::selectBook);

    stackedWidget->addWidget(bookView);
}

void ClientWindow::createReadingView()
{
    QWidget* readingView = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(readingView);

    readingLabel = new QLabel("Reading book: ", this);
    layout->addWidget(readingLabel);

    cancelReadingButton = new QPushButton("Cancel Reading", this);
    layout->addWidget(cancelReadingButton);

    connect(cancelReadingButton, &QPushButton::clicked, this, &ClientWindow::cancelReading);

    stackedWidget->addWidget(readingView);
}

void ClientWindow::selectBook()
{
    if (m_bookList->currentItem())
    {
        QString bookName = m_bookList->currentItem()->text().split(": ").first();
        readingLabel->setText("Reading book: " + bookName);

        emit bookSelected(bookName.toStdString());
    }
}

void ClientWindow::cancelReading()
{
    QString bookName = readingLabel->text().split(": ").last();
    emit bookReturned(bookName.toStdString());

    stackedWidget->setCurrentIndex(0);
}

void ClientWindow::onSuccessTakeBook()
{
    stackedWidget->setCurrentIndex(1);

    utils::HistoryBookInfo bookInfo
    {
        .timeStamp = QDateTime::currentDateTime(),
        .clientId = m_id,
        .name = m_bookList->currentItem()->text().split(": ").first().toStdString(),
        .op = {.type = http::OperationType_e::POST, .status = http::OperationStatus_e::OK}
    };

    addHistoryEntry(bookInfo);
}

void ClientWindow::onFailedTakeBook()
{
    LOG_ERROR(CLIENT_LOG, "Failed to take book");

    utils::HistoryBookInfo bookInfo
    {
        .timeStamp = QDateTime::currentDateTime(),
        .clientId = m_id,
        .name = m_bookList->currentItem()->text().split(": ").first().toStdString(),
        .op = {.type = http::OperationType_e::POST, .status = http::OperationStatus_e::FAIL}
    };

    addHistoryEntry(bookInfo);
}

void ClientWindow::onSuccessReturnBook()
{
    utils::HistoryBookInfo bookInfo
    {
        .timeStamp = QDateTime::currentDateTime(),
        .clientId = m_id,
        .name = m_bookList->currentItem()->text().split(": ").first().toStdString(),
        .op = {.type = http::OperationType_e::PUT, .status = http::OperationStatus_e::OK}
    };

    addHistoryEntry(bookInfo);
}

void ClientWindow::onFailedReturnBook()
{
    LOG_ERROR(CLIENT_LOG, "Failed to return book");

    utils::HistoryBookInfo bookInfo
    {
        .timeStamp = QDateTime::currentDateTime(),
        .clientId = m_id,
        .name = m_bookList->currentItem()->text().split(": ").first().toStdString(),
        .op = {.type = http::OperationType_e::PUT, .status = http::OperationStatus_e::FAIL}
    };

    addHistoryEntry(bookInfo);
}

void ClientWindow::terminateClient()
{
    QMessageBox::information(this, "Terminate Client", "Client terminated.");
    std::exit(0); // TODO: exit only qapp, but don't whole process
}
