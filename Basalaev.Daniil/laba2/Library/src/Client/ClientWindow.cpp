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

    cancelReadingButton = new QPushButton("Stop Reading", this);
    layout->addWidget(cancelReadingButton);

    connect(cancelReadingButton, &QPushButton::clicked, this, &ClientWindow::stopReading);

    stackedWidget->addWidget(readingView);
}

void ClientWindow::selectBook()
{
    if (m_bookList->currentItem())
    {
        QString bookName = m_bookList->currentItem()->text().split(": ").first();
        readingLabel->setText("Reading book: " + bookName);

        emit bookSelected(bookName.toStdString(), m_id);
    }
}

void ClientWindow::stopReading()
{
    QString bookName = readingLabel->text().split(": ").last();
    emit bookReturned(bookName.toStdString(), m_id);

    stackedWidget->setCurrentIndex(0);
}

std::string ClientWindow::getCurrentBook() const
{
    return m_bookList->currentItem()->text().split(": ").first().toStdString();
}

void ClientWindow::onSuccessTakeBook(std::string const& bookName, alias::id_t clientId)
{
    stackedWidget->setCurrentIndex(1);
    LibraryWindowImpl::onSuccessTakeBook(bookName, clientId);
}
void ClientWindow::onSuccessReturnBook(std::string const& bookName, alias::id_t clientId)
{
    stackedWidget->setCurrentIndex(0);
    LibraryWindowImpl::onSuccessReturnBook(bookName, clientId);
}

void ClientWindow::terminateClient()
{
    QMessageBox::information(this, "Terminate Client", "Client terminated.");
    std::exit(0); // TODO: exit only qapp, but don't whole process
}
