#pragma once

#include "../includes/includes.hpp"
#include <QMainWindow>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QDateTime>
#include <QHeaderView>
#include <qlistwidget.h>

class HostWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit HostWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        // Установка центрального виджета
        auto *centralWidget = new QWidget(this);
        this->resize(1200, 600);
        setCentralWidget(centralWidget);

        // Основной вертикальный макет
        auto *mainLayout = new QVBoxLayout(centralWidget);

        // Таблица книг
        bookTable = new QTableWidget(this);
        bookTable->setColumnCount(4);
        bookTable->setHorizontalHeaderLabels({"Название", "Доступно", "Всего", "История выдачи"});
        bookTable->horizontalHeader()->setStretchLastSection(true);
        bookTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        mainLayout->addWidget(bookTable);

        // Заполнение начальных данных
        ReadStartBooksList();
    }

    void ReturnBook(int client_id, std::string book_name) {
        std::cerr<<"return book script"<<std::endl;
        std::lock_guard lock(mtx);
        std::string answer = "NOTFOUND " + book_name;
        for (int row = 0; row < bookTable->rowCount(); ++row) {
            if (bookTable->item(row, 0)->text().toUtf8().data() == book_name) {
                // Ищем запись о данном клиенте
                std::cerr<<"book found"<<std::endl;
                QWidget *cellWidget = bookTable->cellWidget(row, 3);
                if (auto *listWidget = qobject_cast<QListWidget *>(cellWidget)) {
                    // Перебираем все элементы QListWidget
                    for (int i = 0; i < listWidget->count(); ++i) {
                        QListWidgetItem *item = listWidget->item(i);
                        if (item && item->text().contains(QString("Client%1 ").arg(client_id))) {
                            // Элемент найден, выделяем его
                            QListWidgetItem *removedItem = listWidget->takeItem(i);
                            delete removedItem;
                            break;
                        }
                    }

                    // Увеличиваем доступное количество экземпляров
                    int available = bookTable->item(row, 1)->text().toInt();
                    bookTable->item(row, 1)->setText(QString::number(available + 1));

                    answer = "OK " + book_name;
                    break;
                }
            }
        }
        send_return_answer_to_client(client_id, answer);
    }
    void GiveBook(int client_id, std::string book_name) {
        // TODO: отдать книгу. Доступных экземпляров меньше, клиент в списке должников + сообщение в конце
        std::lock_guard lock(mtx);
        std::cerr<<"Try to give book"<<std::endl;
        std::cerr<<bookTable->rowCount()<<std::endl;
        std::string answer = "NOBOOK " + book_name;

        for (int i = 0; i < bookTable->rowCount(); ++i) {
            std::string curBookName = bookTable->item(i, 0)->text().toUtf8().constData();
            std::cerr<<curBookName<<std::endl;
            if (curBookName == book_name) {

                int countAvailable = std::stoi(bookTable->item(i, 1)->text().toUtf8().constData());
                std::cerr<<countAvailable<<std::endl;
                // Проверяем есть ли эта книга уже у человека
                QWidget *cellWidget = bookTable->cellWidget(i, 3);
                if (auto *listWidget = qobject_cast<QListWidget *>(cellWidget)) {
                    // Перебираем все элементы QListWidget
                    bool f = false;
                    for (int i = 0; i < listWidget->count(); ++i) {
                        QListWidgetItem *item = listWidget->item(i);
                        if (item && item->text().contains(QString("Client%1 ").arg(client_id))) {
                            // Элемент найден
                            answer = "ALREADY " + book_name;
                            f = true;
                            break;
                        }
                    }
                    if (f) break;
                }

                if (countAvailable > 0) {
                    answer = "OK " + curBookName;
                    countAvailable --;
                    bookTable -> setItem(i, 1, new QTableWidgetItem(QString::number(countAvailable)));
                    // Добавляем запись в историю выдачи
                    QString newEntry = "Client" + QString::number(client_id) + " " + QString(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
                    std::cerr<<newEntry.toUtf8().constData()<<std::endl;
                    QWidget *cellWidget = bookTable->cellWidget(i, 3);
                    if (auto *listWidget = qobject_cast<QListWidget *>(cellWidget)) {
                        // Изменяем содержимое
                        listWidget->addItem(newEntry);
                    }
                }
                else {
                    answer = "NO " + curBookName;
                }
                break;
            }
        }
        send_take_answer_to_client(client_id, answer);
    }

    void send_take_answer_to_client(int client_id, std::string answer);
    void send_return_answer_to_client(int client_id, std::string answer);

private:
    std::mutex mtx;
    void ReadStartBooksList() const {
        std::ifstream file("/home/annka/CLionProjects/lab2.2/src/books_for_start.txt");
        if (!file.is_open()) {
            std::cerr<<"M";
            return;
        }
        std::string line;
        while (std::getline(file, line)){
            if (line[0] == '#')
                continue;
            std::string bookName = line.substr(0, line.find(':'));

            int count ;
            try{
                count = std::stoi(line.substr(line.find(':') + 1, line.length()));
              }
            catch(...){
                count = 0;
            }
            addBook(bookName.data(), count, count);
        }
    }

    void addBook(const QString &title, const int available, const int total) const {
        const int row = bookTable->rowCount();
        bookTable->insertRow(row);

        bookTable->setItem(row, 0, new QTableWidgetItem(title));
        bookTable->setItem(row, 1, new QTableWidgetItem(QString::number(available)));
        bookTable->setItem(row, 2, new QTableWidgetItem(QString::number(total)));
        QListWidget *historyList = new QListWidget();
        bookTable->setCellWidget(row, 3, historyList); // История выдачи пустая изначально
    }

    QTableWidget *bookTable;
};
