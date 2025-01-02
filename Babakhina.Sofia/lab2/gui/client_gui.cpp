#include "client_gui.h"


ClientWindow::ClientWindow(const std::vector<Book>& books, QWidget* parent)
    : QMainWindow(parent) {
    stacked_widget = new QStackedWidget(this);
    create_general_view(books);
    create_reading_view();
    setCentralWidget(stacked_widget);
    setWindowTitle("Client Control Panel");
    resize(800, 400);
    stacked_widget->setCurrentIndex(0);
    connect(&timer, &QTimer::timeout, this, [this]() {
        if (!timer.isActive()) return;
        if (--timekill <= 0) {
            terminate_client();
        }
        else {
            timer_label->setText(QString("Time left: %1 sec...").arg(timekill));
        }
    });
    connect(this, &ClientWindow::signal_reset_timer, this, &ClientWindow::reset_timer);
    connect(this, &ClientWindow::signal_stop_timer, this, [this]() {
        timer.stop();
    });

    timer.setInterval(1000);
    timer.start();

}

ClientWindow::~ClientWindow() {}

void ClientWindow::create_general_view(const std::vector<Book>& books) {
    QWidget* general_view = new QWidget(this);
    QVBoxLayout* v_layout = new QVBoxLayout(general_view);
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
    select_button = new QPushButton("SELECT THE BOOK", this);
    select_button->setEnabled(false);
    layout1->addWidget(left_label);
    layout1->addWidget(book_table);
    layout1->addWidget(select_button);
    QWidget* history_list_w = new QWidget(this);
    QVBoxLayout* layout2 = new QVBoxLayout(history_list_w);
    right_label = new QLabel("History", this);
    table = new QTableWidget(this);
    table->setColumnCount(3);
    table->horizontalHeader()->setStretchLastSection(true);
    QStringList headers;
    headers << "time" << "status" << "book title";
    table->setHorizontalHeaderLabels(headers);
    timer_label = new QLabel("Time left: 10 sec...", this);
    timer_label->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout2->addWidget(right_label);
    layout2->addWidget(table);
    h_layout->addWidget(book_list_w);
    h_layout->addWidget(history_list_w);
    v_layout->addWidget(timer_label);
    v_layout->addWidget(book_view);
    connect(book_table, &QTableWidget::itemSelectionChanged, [this]() {
        select_button->setEnabled(book_table->currentItem() != nullptr);
    });
    connect(select_button, &QPushButton::clicked, this, &ClientWindow::select_book);
    stacked_widget->addWidget(general_view);
}

void ClientWindow::create_reading_view() {
    QWidget* readingView = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(readingView);
    reading_label = new QLabel("Reading book: ", this);
    reading_label->setAlignment(Qt::AlignCenter);
    layout->addWidget(reading_label);
    cancel_reading_button = new QPushButton("Cancel Reading", this);
    layout->addWidget(cancel_reading_button);
    connect(cancel_reading_button, &QPushButton::clicked, this, &ClientWindow::cancel_reading);
    stacked_widget->addWidget(readingView);
}

void ClientWindow::select_book() {
    if (book_table->currentItem()) {
        QString book_name = book_table->currentItem()->text();
        reading_label->setText("NOW READING: " + book_name);
        emit book_selected(book_name);
    }
}

void ClientWindow::cancel_reading() {
    QString book_name = reading_label->text().split(": ").last();
    emit book_returned(book_name);
    stacked_widget->setCurrentIndex(0);
}

void ClientWindow::success_take_book() {
    std::cout << "Succeded to take book\n";
    stacked_widget->setCurrentIndex(1);
}

void ClientWindow::fail_take_book() {
    std::cout << "Failed to take book\n";
}

void ClientWindow::update_books(const std::vector<Book>& books, std::string state, std::string book_name, std::string time, bool flag) {
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
        table->setItem(table_rows, 0, item_time);
        table->setItem(table_rows, 1, item_state);
        table->setItem(table_rows, 2, item_title); 
    }
    else{
        std::string fail_state = "FAILED";
        int table_rows = table->rowCount();
        table->insertRow (table_rows);
        QTableWidgetItem *item_time = new QTableWidgetItem(QString::fromStdString(time));
        QTableWidgetItem *item_state = new QTableWidgetItem(QString::fromStdString(fail_state));
        QTableWidgetItem *item_title = new QTableWidgetItem(QString::fromStdString(book_name));
        table->setItem(table_rows, 0, item_time);
        table->setItem(table_rows, 1, item_state);
        table->setItem(table_rows, 2, item_title); 
    }
}

void ClientWindow::terminate_client() {
    timer.stop();
    QMessageBox::information(this, "Terminate Client", "Client terminated.");
    kill(client_pid, SIGKILL);
}

void ClientWindow::reset_timer() {
    timekill = conn_timeout;
    timer_label->setText(QString("Time left: %1 sec").arg(conn_timeout));
    timer.start();
}

void ClientWindow::wrap_reset_timer() {
    emit signal_reset_timer();
}

void ClientWindow::wrap_stop_timer() {
    emit signal_stop_timer();
}
