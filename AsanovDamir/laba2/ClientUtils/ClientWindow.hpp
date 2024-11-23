#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QListWidget>
#include <QStackedWidget>
#include <QString>
#include <vector>
#include <string>
#include <QWidget>
#include "Book.hpp"

class ClientWindow : public QMainWindow {
    Q_OBJECT

public:
    ClientWindow(const std::vector<Book>& books, QWidget* parent = nullptr);
    ~ClientWindow();

    void onSuccessTakeBook();
    void onFailedTakeBook();

signals:
    void bookSelected(const QString& bookName);
    void bookReturned(const QString& bookName);

private slots:
    void selectBook();
    void cancelReading();
    void terminateClient();

private:
    void createBookView(const std::vector<Book>& books);
    void createReadingView();

    QStackedWidget* stackedWidget; // For switch windows (started <-> reading)

    QListWidget* bookList;
    QPushButton* selectButton;

    QLabel* readingLabel;
    QPushButton* cancelReadingButton;

    QPushButton* terminateClientButton;
};
