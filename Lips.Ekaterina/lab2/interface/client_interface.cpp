#include "client_interface.h"


ClientWindow::ClientWindow(const std::vector<Book>& books, pid_t pid, QWidget* parent)
    : QMainWindow(parent) {

    create_base_view(books);

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

    setWindowTitle("Client");
    resize(800, 600);
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

    reading_label = new QLabel("Reading book list", this);
    reading_qlist = new QListWidget(this);
    for (const auto& book : reading_books) {
        book_qlist->addItem(QString::fromStdString(book));
    }
    cancel_button = new QPushButton("Cancel reading", this);
    cancel_button->setEnabled(false);

    layout1->addWidget(book_list_label);
    layout1->addWidget(book_qlist);
    layout1->addWidget(select_button);

    layout1->addWidget(reading_label);
    layout1->addWidget(reading_qlist);
    layout1->addWidget(cancel_button);

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

    connect(reading_qlist, &QListWidget::itemSelectionChanged, [this]() {
        cancel_button->setEnabled(reading_qlist->currentItem() != nullptr);
    });
    connect(cancel_button, &QPushButton::clicked, this, &ClientWindow::cancel_reading);
    
    setCentralWidget(book_view);
}


void ClientWindow::select_book() {
    std::cout << "select_book" << std::endl;

    if (book_qlist->currentItem()) {
        QString book_name = book_qlist->currentItem()->text().split(" [").first();

        emit book_selected_signal(book_name);        
    }
}


void ClientWindow::cancel_reading() {
    if (reading_qlist->currentItem()) {
        QString book_name = reading_qlist->currentItem()->text();
        reading_books.erase(std::remove(reading_books.begin(), reading_books.end(), book_name.toStdString()));

        emit book_returned_signal(book_name);
    }
}


void ClientWindow::success_take_book(const std::string& book_name) {
    std::cout << "Success take book\n";

    reading_books.push_back(book_name);
}


void ClientWindow::fail_take_book() {
    std::cout << "Failed to take book\n";
}


void ClientWindow::update_history(const std::vector<Book>& books, const std::string& state, const std::string& book_name, const std::string& time, bool flag) {
    std::cout << "update_history: " << book_name << std::endl;
    if (flag) {
        book_qlist->clear();
        for (const auto& book : books) {
            book_qlist->addItem(QString::fromStdString(book.name) + " [" + QString::number(book.count) + "]");
        }
        history_list->addItem(QString::fromStdString(time) + " " + QString::fromStdString(state) + " " + QString::fromStdString(book_name));

        reading_qlist->clear();
        for (auto& book : reading_books) {
            reading_qlist->addItem(QString::fromStdString(book));
        }
    }
    else {
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
    if (reading_books.empty()){
        timekill = conn_timeout;
        timer_label->setText(QString("Timer: %1 sec").arg(conn_timeout));
        timer.start();
    }
}


void ClientWindow::restart_timer_wrap() {
    emit restart_timer_signal();
}


void ClientWindow::stop_timer_wrap() {
    timer_label->setText(QString("Timer: [stopped]"));
    emit stop_timer_signal();
}

