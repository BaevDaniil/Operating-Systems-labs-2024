#pragma once

#include <QMainWindow>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QTableWidgetItem>
#include <QDateTime>
#include <QLabel>

class QLabel;

class ClientWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ClientWindow(int client_pid, QWidget *parent = nullptr) : ClientWindow(parent)
    {
        pid = client_pid;
    }

    explicit ClientWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        // Установка главного виджета
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        // Основной вертикальный макет
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        // Таблица запросов
        requestTable = new QTableWidget(this);
        requestTable->setColumnCount(4);
        requestTable->setHorizontalHeaderLabels({"Тип запроса", "Книга", "Дата и время", "Статус"});
        requestTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        requestTable->setSelectionMode(QAbstractItemView::NoSelection);
        mainLayout->addWidget(requestTable);

        // Список книг клиента
        QHBoxLayout *bookListLayout = new QHBoxLayout();
        QLabel *bookListLabel = new QLabel("Список книг клиента:", this);
        bookList = new QListWidget(this);
        bookListLayout->addWidget(bookListLabel);
        bookListLayout->addWidget(bookList);
        mainLayout->addLayout(bookListLayout);

        // Поле ввода для названия книги
        QHBoxLayout *inputLayout = new QHBoxLayout();
        QLabel *inputLabel = new QLabel("Название книги:", this);
        bookInput = new QLineEdit(this);
        inputLayout->addWidget(inputLabel);
        inputLayout->addWidget(bookInput);
        mainLayout->addLayout(inputLayout);

        // Кнопки для отправки запросов
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *getBookButton = new QPushButton("Получить книгу", this);
        QPushButton *returnBookButton = new QPushButton("Вернуть книгу", this);
        buttonLayout->addWidget(getBookButton);
        buttonLayout->addWidget(returnBookButton);
        mainLayout->addLayout(buttonLayout);

        // Подключение сигналов и слотов
        connect(getBookButton, &QPushButton::clicked, this, &ClientWindow::onGetBookClicked);
        connect(returnBookButton, &QPushButton::clicked, this, &ClientWindow::onReturnBookClicked);
    }

    void send_return_request_to_host(const std::string &request);
    void send_take_request_to_host(const std::string &request);

    void addRequestToTable(const QString &type, const QString &book, const QString &status) const {
        const int row = requestTable->rowCount();
        requestTable->insertRow(row);

        requestTable->setItem(row, 0, new QTableWidgetItem(type));
        requestTable->setItem(row, 1, new QTableWidgetItem(book));
        requestTable->setItem(row, 2, new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
        requestTable->setItem(row, 3, new QTableWidgetItem(status));
    }
    void addBookToClientList(std::string& bookName) {
        if (bookName.length() == 0) return;  // Защита от добавления пустых строк
        QString QbookName = QString::fromStdString(strip(bookName));
        bookList->addItem(QbookName);
    }

    void deleteBookFromClientList(const QString &bookName) const {
        if (bookName.isEmpty()) return; // Защита от пустого ввода

        // Поиск книги в списке клиента
        for (int i = 0; i < bookList->count(); ++i) {
            QListWidgetItem *item = bookList->item(i);
            if (item->text() == bookName) {
                delete bookList->takeItem(i); // Удаляем книгу из списка
                return; // Завершаем после удаления
            }
        }
    }

private slots:
    void onGetBookClicked() {
        QString bookName = bookInput->text().trimmed();
        if (bookName.isEmpty()) {
            addRequestToTable("Take", bookName, "Ошибка: Название книги не указано");
            return;
        }

        std::string str_bookName = strip(bookName.toUtf8().constData());
        send_take_request_to_host(str_bookName);
        bookInput->clear();
    }

    void onReturnBookClicked() {
        QListWidgetItem *selectedItem = bookList->currentItem();
        if (!selectedItem) {
            addRequestToTable("Return", "", "Ошибка: Книга не выбрана");
            return;
        }

        std::string bookName = strip(selectedItem->text().toUtf8().constData());
        send_return_request_to_host(bookName);
        delete selectedItem;
    }


private:
    int pid{};

    std::string strip(std::string str) {
        size_t first = str.find_first_not_of(' ');
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(' ');
        str = str.substr(first, last - first + 1);

        while (std::iscntrl(str[str.size() - 1])) {
            str.erase(str.size() - 1);
        }

        return str;
    }

    QTableWidget *requestTable;
    QListWidget *bookList;
    QLineEdit *bookInput;
};
