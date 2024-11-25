#include "host_interface.h"


#include <QMessageBox>
#include <QApplication>
#include <cstdlib>
#include <signal.h>

HostWindow::HostWindow(const std::vector<Book>& books, QWidget* parent)
    : QMainWindow(parent) {

    QWidget* central_widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central_widget);

    left_label = new QLabel("Book list", this);
    layout->addWidget(left_label);

    book_qlist = new QListWidget(this);
    for (const auto& book : books) {
        book_qlist->addItem(QString::fromStdString(book.name) + " [" + QString::number(book.count) + "]");
    }
    layout->addWidget(book_qlist);

    setCentralWidget(central_widget);
    setWindowTitle("Host Control Panel");
    resize(400, 300);
}

HostWindow::~HostWindow() {}

void HostWindow::update_books(const std::vector<Book>& books) {
    book_qlist->clear();
    for (const auto& book : books) {
        book_qlist->addItem(QString::fromStdString(book.name) + " [" + QString::number(book.count) + "]");
    }
}

