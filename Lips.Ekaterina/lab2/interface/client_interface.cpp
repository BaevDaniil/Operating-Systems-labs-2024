#include "client_interface.h"


#include <QMessageBox>
#include <iostream>

ClientWindow::ClientWindow(const std::vector<Book>& books, QWidget* parent)
    : QMainWindow(parent) {
    stacked_widget = new QStackedWidget(this);

    create_base_view(books);
    create_reading_view();

    setCentralWidget(stacked_widget);
    setWindowTitle("Client Control Panel");
    resize(400, 300);

    stacked_widget->setCurrentIndex(0);
}

ClientWindow::~ClientWindow() {}

void ClientWindow::create_base_view(const std::vector<Book>& books) {
    QWidget* book_view = new QWidget(this);
    QHBoxLayout* h_layout = new QHBoxLayout(book_view);

    QWidget* book_list_w = new QWidget(this);
    QVBoxLayout* layout1 = new QVBoxLayout(book_list_w);

    book_qlist = new QListWidget(this);
    for (const auto& book : books) {
        book_qlist->addItem(QString::fromStdString(book.name));
    }
    left_label = new QLabel("Book list", this);
    select_button = new QPushButton("Take a book", this);
    select_button->setEnabled(false);

    layout1->addWidget(left_label);
    layout1->addWidget(book_qlist);
    layout1->addWidget(select_button);


    QWidget* history_list_w = new QWidget(this);
    QVBoxLayout* layout2 = new QVBoxLayout(history_list_w);

    history_list = new QListWidget(this);
    for (const auto& book : books) {
        history_list->addItem(QString::fromStdString(book.name) + " [" + QString::number(book.count) + "]");
    }
    right_label = new QLabel("History list", this);
    layout2->addWidget(right_label);
    layout2->addWidget(history_list);

    h_layout->addWidget(book_list_w);
    h_layout->addWidget(history_list_w);

    connect(book_qlist, &QListWidget::itemSelectionChanged, [this]() {
        select_button->setEnabled(book_qlist->currentItem() != nullptr);
    });

    connect(select_button, &QPushButton::clicked, this, &ClientWindow::select_book);

    stacked_widget->addWidget(book_view);
}

void ClientWindow::create_reading_view() {
    QWidget* readingView = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(readingView);

    reading_label = new QLabel("Reading book: ", this);
    layout->addWidget(reading_label);

    cancel_reading_button = new QPushButton("Cancel Reading", this);
    layout->addWidget(cancel_reading_button);

    connect(cancel_reading_button, &QPushButton::clicked, this, &ClientWindow::cancel_reading);

    stacked_widget->addWidget(readingView);
}

void ClientWindow::select_book() {
    std::cout << "select_book\n";
    if (book_qlist->currentItem()) {
        QString book_name = book_qlist->currentItem()->text().split(" [").first();
        reading_label->setText("Reading book: " + book_name);

        emit bookSelected(book_name);
    }
}

void ClientWindow::cancel_reading() {
    QString book_name = reading_label->text().split(": ").last();
    emit bookReturned(book_name);

    stacked_widget->setCurrentIndex(0);
}

void ClientWindow::success_take_book() {
    std::cout << "success_take_book\n";

    stacked_widget->setCurrentIndex(1);
}

void ClientWindow::fail_take_book() {
    std::cout << "Failed to take book\n";
    // QMessageBox::warning(nullptr, "FAIL", "Failed to take book");
}
