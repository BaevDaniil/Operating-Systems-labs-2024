#include "HostWindow.hpp"
#include "Common/Logger.hpp"

#include <QMessageBox>
#include <QApplication>
#include <QDockWidget>
#include <QDateTime>
#include <cstdlib>
#include <signal.h>

static int secondsLeft = 100;

HostWindow::HostWindow(std::string const& hostTitle, alias::book_container_t const& books, QWidget* parent)
    : LibraryWindowImpl(alias::HOST_ID, books, parent)
{
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    m_hostTitle = new QLabel(QString::fromStdString(hostTitle), this);
    layout->addWidget(m_hostTitle);

    layout->addWidget(m_bookList);

    m_hostKillButton = new QPushButton("Terminate Host", this);
    layout->addWidget(m_hostKillButton);

    m_timerText = new QLabel("Time left: 100 seconds", this);
    layout->addWidget(m_timerText);

    m_clientTimer = new QTimer(this);
    m_clientTimer->setInterval(1000);
    connect(m_clientTimer, &QTimer::timeout, this, [this]() {
        if (!m_clientTimer->isActive()) return;

        if (--secondsLeft <= 0) {
            terminateClient();
        } else {
            m_timerText->setText(QString("Time left: %1 seconds").arg(secondsLeft));
        }
    });

    connect(m_hostKillButton, &QPushButton::clicked, this, &HostWindow::terminateHost);
    connect(this, &HostWindow::resetSignalTimer, this, &HostWindow::resetTimer);
    connect(this, &HostWindow::stopSignalTimer, this, [this]() {
        m_timerText->setText("Client reading");
        m_clientTimer->stop();
    });

    setCentralWidget(centralWidget);
    setWindowTitle("Host Window");
    resize(600, 450);

    m_clientTimer->start();
}

HostWindow::~HostWindow() = default;

void HostWindow::terminateClient()
{
    m_clientTimer->stop();
    m_timerText->setText("Client terminated");
    QMessageBox::information(this, "Terminate Client", "Client terminated.");
    LOG_INFO(HOST_LOG, "Terminate Client");
    // kill(clientPid, SIGKILL);
}

void HostWindow::terminateHost()
{
    QMessageBox::information(this, "Terminate Host", "Host terminated.");
    LOG_INFO(HOST_LOG, "Terminate Host");
    // kill(clientPid, SIGKILL); // and kill client too
    std::exit(0);
}

void HostWindow::resetTimer()
{
    secondsLeft = 100;
    m_timerText->setText("Time left: 100 seconds");
    m_clientTimer->start();
}

void HostWindow::signalResetTimer()
{
    emit resetSignalTimer();
}

void HostWindow::signalStopTimer()
{
    emit stopSignalTimer();
}
