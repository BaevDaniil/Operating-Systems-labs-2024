#include "client_interface.h"


ClientWindow::ClientWindow(const std::vector<Book>& books, pid_t pid, QWidget* parent)
    : QMainWindow(parent) {
    stacked_widget = new QStackedWidget(this);

    create_base_view(books);
    create_reading_view();

    client_pid = pid;

    connect(&timer, &QTimer::timeout, this, [this]() {
        if (!timer.isActive()) return;

        if (--timekill <= 0) {
            terminate_client();
        } 
        else {
            timer_label->setText(QString("Timer: %1 sec").arg(timekill));
        }
    });

    connect(this, &ClientWindow::restart_timer_signal, this, &ClientWindow::restart_timer);
    connect(this, &ClientWindow::stop_timer_signal, this, [this]() {
        timer.stop();
    });

    timer.setInterval(1000);
    timer.start();

    setCentralWidget(stacked_widget);
    setWindowTitle("Client");
    resize(800, 600);

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
        book_qlist->addItem(QString::fromStdString(book.name) + " [" + QString::number(book.count) + "]");
    }
    book_list_label = new QLabel("Book list", this);
    select_button = new QPushButton("Take a book", this);
    select_button->setEnabled(false);

    layout1->addWidget(book_list_label);
    layout1->addWidget(book_qlist);
    layout1->addWidget(select_button);


    QWidget* history_list_w = new QWidget(this);
    QVBoxLayout* layout2 = new QVBoxLayout(history_list_w);

    history_list = new QListWidget(this);
    history_list_label = new QLabel("History list", this);
    timer_label = new QLabel("Timer: 7 sec", this);
    timer_label->setText(QString("Timer: %1 sec").arg(conn_timeout));
    layout2->addWidget(history_list_label);
    layout2->addWidget(history_list);
    layout2->addWidget(timer_label);

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
    if (book_qlist->currentItem()) {
        QString book_name = book_qlist->currentItem()->text().split(" [").first();
        reading_label->setText("Reading book: " + book_name);

        emit book_selected_signal(book_name);
    }
}


void ClientWindow::cancel_reading() {
    QString book_name = reading_label->text().split(": ").last();
    emit book_returned_signal(book_name);

    stacked_widget->setCurrentIndex(0);
}


void ClientWindow::success_take_book() {
    std::cout << "success_take_book\n";
    stacked_widget->setCurrentIndex(1);
}


void ClientWindow::fail_take_book() {
    std::cout << "Failed to take book\n";
}


void ClientWindow::update_books(const std::vector<Book>& books, std::string state, std::string book_name, std::string time, bool flag) {
    std::cout << "ClientWindow::update_books: " << book_name << std::endl;
    if (flag) {
        book_qlist->clear();
        for (const auto& book : books) {
            book_qlist->addItem(QString::fromStdString(book.name) + " [" + QString::number(book.count) + "]");
        }
        history_list->addItem(QString::fromStdString(time) + " " + QString::fromStdString(state) + " " + QString::fromStdString(book_name));
    }
    else{
        std::string fail_state = "[FAILED TO TAKE]";
        history_list->addItem(QString::fromStdString(time) + " " + QString::fromStdString(fail_state) + " " + QString::fromStdString(book_name));
    }
}


void ClientWindow::terminate_client() {
    timer.stop();
    timer_label->setText("Client terminated");
    QMessageBox::information(this, "Terminate", "Client terminated");
    kill(client_pid, SIGKILL);
}


void ClientWindow::restart_timer() {
    timekill = conn_timeout;
    timer_label->setText(QString("Timer: %1 sec").arg(conn_timeout));
    timer.start();
}


void ClientWindow::restart_timer_wrap() {
    emit restart_timer_signal();
}


void ClientWindow::stop_timer_wrap() {
    emit stop_timer_signal();
}

