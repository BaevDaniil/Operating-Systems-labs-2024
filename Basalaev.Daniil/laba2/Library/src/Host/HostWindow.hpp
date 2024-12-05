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
    HostWindow(std::string const& hostTitle, alias::book_container_t const& books, QWidget* parent = nullptr);
    ~HostWindow() override;

    void signalResetTimer();
    void signalStopTimer();

signals:
    void resetSignalTimer();
    void stopSignalTimer();

private slots:
    void terminateClient();
    void terminateHost();
    void resetTimer();

private:
    QLabel* m_hostTitle;
    QPushButton* m_hostKillButton;

    QTimer* m_clientTimer;
    QLabel* m_timerText;
};
