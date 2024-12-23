#include "host_interface.h"


HostWindow::HostWindow(const std::vector<Book>& books, QWidget* parent)
    : QMainWindow(parent) {

    QWidget* book_view = new QWidget(this);
    QHBoxLayout* h_layout = new QHBoxLayout(book_view);

    QWidget* book_list_w = new QWidget(this);
    QVBoxLayout* layout1 = new QVBoxLayout(book_list_w);

    book_qlist = new QListWidget(this);
    for (const auto& book : books) {
        book_qlist->addItem(QString::fromStdString(book.name) + " [" + QString::number(book.count) + "]");
    }
    book_list_label = new QLabel("Book list", this);

    layout1->addWidget(book_list_label);
    layout1->addWidget(book_qlist);


    QWidget* history_list_w = new QWidget(this);
    QVBoxLayout* layout2 = new QVBoxLayout(history_list_w);

    history_list = new QListWidget(this);

    history_list_label = new QLabel("History list", this);
    layout2->addWidget(history_list_label);
    layout2->addWidget(history_list);


    h_layout->addWidget(book_list_w);
    h_layout->addWidget(history_list_w);
    
    setCentralWidget(book_view);
    setWindowTitle("Host");
    resize(800, 600);
}

HostWindow::~HostWindow() {}

void HostWindow::update_books(const std::vector<Book>& books, std::string state, std::string book_name, std::string client_name, std::string time, bool flag) {
    if (flag) {
        book_qlist->clear();
        for (const auto& book : books) {
            book_qlist->addItem(QString::fromStdString(book.name) + " [" + QString::number(book.count) + "]");
        }
        history_list->addItem(QString::fromStdString(time) + " " + QString::fromStdString(state) + " " + QString::fromStdString(client_name) + " - " + QString::fromStdString(book_name));
    }
    else{
        history_list->addItem(QString::fromStdString(time) + " [FAILED TO TAKE] " + QString::fromStdString(client_name) + " - " + QString::fromStdString(book_name));
    }
}

