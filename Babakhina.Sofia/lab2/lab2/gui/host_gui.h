#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QListWidget>
#include <QString>
#include <vector>
#include <string>
#include <QWidget>
#include "../book.h"

class HostWindow : public QMainWindow {
    Q_OBJECT

public:
    HostWindow(const std::vector<Book>& books, QWidget* parent = nullptr);
    virtual ~HostWindow();

    void updateBooks(const std::vector<Book>& books);

    // pid_t clientPid; // for kill

// private slots:
//     void terminateClient();
//     void terminateHost();

private:
    QLabel* portLabel;
    QListWidget* bookList;
    QPushButton* terminateClientButton;
    QPushButton* terminateHostButton;
    int hostPort;
};