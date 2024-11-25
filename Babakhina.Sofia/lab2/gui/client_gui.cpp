#include "client_gui.h"
#include <QMessageBox>
#include <iostream>

ClientWindow::ClientWindow(const std::vector<Book>& books, QWidget* parent)
    : QMainWindow(parent) {
    stackedWidget = new QStackedWidget(this);

    createBookView(books);
    createReadingView();

    setCentralWidget(stackedWidget);
    setWindowTitle("Client Control Panel");
    resize(400, 300);

    stackedWidget->setCurrentIndex(0);
}

ClientWindow::~ClientWindow() {}

void ClientWindow::createBookView(const std::vector<Book>& books) {
    QWidget* bookView = new QWidget(this);
    QHBoxLayout* h_layout = new QHBoxLayout(bookView);

    QWidget* book_list_w = new QWidget(this);
    QVBoxLayout* layout1 = new QVBoxLayout(book_list_w);

    bookList = new QListWidget(this);
    for (const auto& book : books) {
        bookList->addItem(QString::fromStdString(book.name));
    }
    portLabel = new QLabel("Book list", this);
    selectButton = new QPushButton("Pick up a book", this);
    selectButton->setEnabled(false);

    layout1->addWidget(portLabel);
    layout1->addWidget(bookList);
    layout1->addWidget(selectButton);


    QWidget* history_list_w = new QWidget(this);
    QVBoxLayout* layout2 = new QVBoxLayout(history_list_w);

    history_list = new QListWidget(this);
    for (const auto& book : books) {
        history_list->addItem(QString::fromStdString(book.name) + " - " + QString::number(book.count) + " copies");
    }
    right_label = new QLabel("History list", this);
    layout2->addWidget(right_label);
    layout2->addWidget(history_list);

    h_layout->addWidget(book_list_w);
    h_layout->addWidget(history_list_w);

    connect(bookList, &QListWidget::itemSelectionChanged, [this]() {
        selectButton->setEnabled(bookList->currentItem() != nullptr);
    });

    connect(selectButton, &QPushButton::clicked, this, &ClientWindow::selectBook);

    stackedWidget->addWidget(bookView);
}

void ClientWindow::createReadingView() {
    QWidget* readingView = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(readingView);

    readingLabel = new QLabel("Reading book: ", this);
    layout->addWidget(readingLabel);

    cancelReadingButton = new QPushButton("Cancel Reading", this);
    layout->addWidget(cancelReadingButton);

    connect(cancelReadingButton, &QPushButton::clicked, this, &ClientWindow::cancelReading);

    stackedWidget->addWidget(readingView);
}

void ClientWindow::selectBook() {
    std::cout << "selectBook\n";
    if (bookList->currentItem()) {
        QString bookName = bookList->currentItem()->text().split(" - ").first();
        readingLabel->setText("Reading book: " + bookName);
        emit bookSelected(bookName);
    }
}

void ClientWindow::cancelReading() {
    QString bookName = readingLabel->text().split(": ").last();
    emit bookReturned(bookName);
    stackedWidget->setCurrentIndex(0);
}

void ClientWindow::onSuccessTakeBook() {
    std::cout << "onSuccessTakeBook\n";
    stackedWidget->setCurrentIndex(1);
}

void ClientWindow::onFailedTakeBook() {
    std::cout << "Failed to take book\n";
}
