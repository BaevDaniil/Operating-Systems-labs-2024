#include "host_gui.h"


HostWindow::HostWindow(const std::vector<Book>& books, QWidget* parent)
    : QMainWindow(parent) {

    QWidget* book_view = new QWidget(this);
    QHBoxLayout* h_layout = new QHBoxLayout(book_view);

    QWidget* book_list_w = new QWidget(this);
    QVBoxLayout* layout1 = new QVBoxLayout(book_list_w);

    book_table = new QTableWidget(this);
    book_table->setColumnCount(2);
    book_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    QStringList book_table_headers;
    book_table_headers << "book title" << "amount";
    book_table->setHorizontalHeaderLabels(book_table_headers);

    for (const auto& book : books) {
        int rows = book_table->rowCount();
        book_table->insertRow (rows);
        QTableWidgetItem *item_title = new QTableWidgetItem(QString::fromStdString(book.name));
        QTableWidgetItem *item_amount = new QTableWidgetItem(QString::number(book.count));
        book_table->setItem(rows, 0, item_title);
        book_table->setItem(rows, 1, item_amount);
    }
    left_label = new QLabel("Book list", this);

    layout1->addWidget(left_label);
    layout1->addWidget(book_table);


    QWidget* history_list_w = new QWidget(this);
    QVBoxLayout* layout2 = new QVBoxLayout(history_list_w);

    right_label = new QLabel("History list", this);
    
    table = new QTableWidget(this);
    table->setColumnCount(4);
    table->horizontalHeader()->setStretchLastSection(true);
    QStringList headers;
    headers << "time" << "user" << "status" << "book title";
    table->setHorizontalHeaderLabels(headers);

    layout2->addWidget(right_label);
    layout2->addWidget(table);


    h_layout->addWidget(book_list_w);
    h_layout->addWidget(history_list_w);
    
    setCentralWidget(book_view);
    setWindowTitle("Host Control Panel");
    resize(900, 400);
}

HostWindow::~HostWindow() {}

void HostWindow::update_books(const std::vector<Book>& books, std::string state, std::string book_name, std::string client_name, std::string time, bool flag) {
    if (flag) {
        book_table->clear();
        QStringList book_table_headers;
        book_table_headers << "book title" << "amount";
        book_table->setHorizontalHeaderLabels(book_table_headers);
        book_table->setRowCount(0);
        for (const auto& book : books) {
            int book_table_rows = book_table->rowCount();
            book_table->insertRow(book_table_rows);
            QTableWidgetItem *item_title = new QTableWidgetItem(QString::fromStdString(book.name));
            QTableWidgetItem *item_amount = new QTableWidgetItem(QString::number(book.count));
            book_table->setItem(book_table_rows, 0, item_title);
            book_table->setItem(book_table_rows, 1, item_amount);
        }
        
        int table_rows = table->rowCount();
        table->insertRow(table_rows);
        QTableWidgetItem *item_time = new QTableWidgetItem(QString::fromStdString(time));
        QTableWidgetItem *item_state = new QTableWidgetItem(QString::fromStdString(state));
        QTableWidgetItem *item_title = new QTableWidgetItem(QString::fromStdString(book_name));
        QTableWidgetItem *item_client = new QTableWidgetItem(QString::fromStdString(client_name));
        table->setItem(table_rows, 0, item_time);
        table->setItem(table_rows, 1, item_client);
        table->setItem(table_rows, 2, item_state);
        table->setItem(table_rows, 3, item_title); 
    }
    else{
        int table_rows = table->rowCount();
        table->insertRow (table_rows);
        QTableWidgetItem *item_time = new QTableWidgetItem(QString::fromStdString(time));
        QTableWidgetItem *item_state = new QTableWidgetItem(QString::fromStdString("FAILED"));
        QTableWidgetItem *item_title = new QTableWidgetItem(QString::fromStdString(book_name));
        QTableWidgetItem *item_client = new QTableWidgetItem(QString::fromStdString(client_name));
        table->setItem(table_rows, 0, item_time);
        table->setItem(table_rows, 1, item_client);
        table->setItem(table_rows, 2, item_state);
        table->setItem(table_rows, 3, item_title); 
    }
}


