#pragma once

#include "Common/Book.hpp"
#include "Common/LibraryWindowImpl.hpp"

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
#include <QTimer>

class HostWindow : public LibraryWindowImpl
{
    Q_OBJECT

public:
    HostWindow(const std::string& hostTitle, const std::vector<Book>& books, QWidget* parent = nullptr);
    ~HostWindow() override;

    void onSuccessTakeBook() override;
    void onFailedTakeBook() override;
    void onSuccessReturnBook()override;
    void onFailedReturnBook() override;

    void signalResetTimer();
    void signalStopTimer();

    pid_t clientPid; // for kill

signals:
    void resetSignalTimer();
    void stopSignalTimer();

private slots:
    void terminateClient();
    void terminateHost();
    void resetTimer();

private:
    QLabel* portLabel;
    QPushButton* terminateClientButton;
    QPushButton* terminateHostButton;

    QTimer* clientTimer;
    QLabel* timerLabel;
};
