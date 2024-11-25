#include "host_gui.h"
#include <QMessageBox>
#include <QApplication>
#include <cstdlib>
#include <signal.h>

HostWindow::HostWindow(const std::vector<Book>& books, QWidget* parent)
    : QMainWindow(parent) {

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    portLabel = new QLabel("Book list", this);
    layout->addWidget(portLabel);

    bookList = new QListWidget(this);
    for (const auto& book : books) {
        bookList->addItem(QString::fromStdString(book.name) + " - " + QString::number(book.count) + " copies");
    }
    layout->addWidget(bookList);

    setCentralWidget(centralWidget);
    setWindowTitle("Host Control Panel");
    resize(400, 300);
}

HostWindow::~HostWindow() {}

void HostWindow::updateBooks(const std::vector<Book>& books) {
    bookList->clear();
    for (const auto& book : books) {
        bookList->addItem(QString::fromStdString(book.name) + " - " + QString::number(book.count) + " copies");
    }
}
