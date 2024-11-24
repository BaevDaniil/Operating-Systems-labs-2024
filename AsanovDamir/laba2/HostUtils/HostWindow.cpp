#include "HostWindow.hpp"
#include "logger.hpp"
#include <QMessageBox>
#include <QApplication>
#include <cstdlib>
#include <signal.h>

HostWindow::HostWindow(const std::string& hostTitle, const std::vector<Book>& books, QWidget* parent)
    : QMainWindow(parent) {

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    portLabel = new QLabel(QString::fromStdString(hostTitle), this);
    layout->addWidget(portLabel);

    bookList = new QListWidget(this);
    for (const auto& book : books) {
        bookList->addItem(QString::fromStdString(book.name) + " - " + QString::number(book.count) + " copies");
    }
    layout->addWidget(bookList);

    terminateClientButton = new QPushButton("Terminate Client", this);
    layout->addWidget(terminateClientButton);

    terminateHostButton = new QPushButton("Terminate Host", this);
    layout->addWidget(terminateHostButton);

    connect(terminateClientButton, &QPushButton::clicked, this, &HostWindow::terminateClient);
    connect(terminateHostButton, &QPushButton::clicked, this, &HostWindow::terminateHost);

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

void HostWindow::terminateClient() {
    QMessageBox::information(this, "Terminate Client", "Client terminated.");
    LoggerHost::get_instance().log(Status::INFO, "Terminate Client");
    kill(clientPid, SIGKILL);
}

void HostWindow::terminateHost() {
    QMessageBox::information(this, "Terminate Host", "Host terminated.");
    LoggerHost::get_instance().log(Status::INFO, "Terminate Host");
    std::exit(0);
}
