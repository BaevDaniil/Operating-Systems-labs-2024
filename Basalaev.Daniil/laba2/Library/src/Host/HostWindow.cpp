#include "HostWindow.hpp"
#include "Common/Logger.hpp"

#include <QMessageBox>
#include <QApplication>
#include <QDockWidget>
#include <QDateTime>
#include <cstdlib>
#include <signal.h>

static int secondsLeft = 100;

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QListWidget>
#include <QDockWidget>
#include <QPalette>
#include <QFont>

HostWindow::HostWindow(std::string const& hostTitle, alias::book_container_t const& books, QWidget* parent)
    : LibraryWindowImpl(alias::HOST_ID, books, parent)
{
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    m_hostTitle = new QLabel(QString::fromStdString(hostTitle), this);
    QFont titleFont = m_hostTitle->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    m_hostTitle->setFont(titleFont);
    m_hostTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_hostTitle);

    layout->addWidget(m_bookList);

    m_hostKillButton = new QPushButton("Terminate Host", this);
    layout->addWidget(m_hostKillButton);

    m_timerText = new QLabel("Time left: 100 seconds", this);
    layout->addWidget(m_timerText);

    m_clientList = new QListWidget(this);
    m_clientList->setMinimumHeight(200);
    m_clientList->setMinimumWidth(400);
    QDockWidget* clientDock = new QDockWidget("Clients", this);
    clientDock->setWidget(m_clientList);
    addDockWidget(Qt::RightDockWidgetArea, clientDock);

    setCentralWidget(centralWidget);
    setWindowTitle("Host Window");
    resize(800, 600);

    QPalette palette = centralWidget->palette();
    palette.setColor(QPalette::Window, QColor(240, 240, 240));
    centralWidget->setAutoFillBackground(true);
    centralWidget->setPalette(palette);

    m_clientTimer = new QTimer(this);
    m_clientTimer->setInterval(1000);
    connect(m_clientTimer, &QTimer::timeout, this, [this]() {
        if (!m_clientTimer->isActive())
            return;

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

    m_clientTimer->start();
}

HostWindow::~HostWindow() = default;

void HostWindow::updateClientsInfo(std::vector<utils::ClientInfo> const& clientsInfo)
{
    m_clientList->clear();
    for (auto const& clientInfo : clientsInfo)
    {
        QListWidgetItem* clientItem = new QListWidgetItem(clientInfo.toQString(), m_clientList);

        if (clientInfo.readingBook.empty())
        {
            clientItem->setForeground(QColor(Qt::red));
        }
        else
        {
            clientItem->setForeground(QColor(Qt::green));
        }

        m_clientList->addItem(clientItem);
    }
}

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
