#include "host_window.h"
#include "input_dialog.h"
#include "client_window.h"

HostChatWindow* HostChatWindow::instance = nullptr;

HostChatWindow::HostChatWindow(const std::vector<__pid_t>& clients_pid, void* reserve, QWidget *parent)
    : QMainWindow(parent), clients_pid(clients_pid), reserve(reserve)
{
    init_gui();
    setup_conn();
    init_timers();
    init_signals();

    instance = this;
}

HostChatWindow::~HostChatWindow()
{
    for (auto& chat : private_chats)
    {
        if(chat.first) {
            delete chat.first;
        }
        chat.second->close();
    }
}

void HostChatWindow::init_gui()
{
    QScrollArea *scrollArea = new QScrollArea(&private_chats_widget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidget(&clients_list);

    QVBoxLayout *scrollLayout = new QVBoxLayout(&private_chats_widget);
    scrollLayout->addWidget(scrollArea);

    for(auto& client_pid : clients_pid)
    {
        auto itemText = QStringLiteral("Клиент: %1").arg(client_pid);
        auto *item = new QListWidgetItem(itemText, &clients_list);
        private_chats[item] = new InputDialog(this);
        connect(private_chats[item], &InputDialog::send_msg, this, &HostChatWindow::send_private_msg);
    }

    tab_widget.addTab(&public_chat, "Общий чат");
    tab_widget.addTab(&private_chats_widget, "Личный чат");

    auto centralWidget = new QWidget(this);
    auto mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addWidget(&tab_widget);
    setCentralWidget(centralWidget);

    setGeometry(100, 100, 800, 600);
    setWindowTitle("Сервер");

    connect(&public_chat, &InputDialog::send_msg, this, [this](const std::string& msg)
    {
        send_public_msg(msg, -1);
    });

    connect(&clients_list, &QListWidget::itemClicked, [this](QListWidgetItem *item)
    {
        if (private_chats.contains(item) && private_chats[item]) {
            private_chats[item]->show();
        }
    });
}

void HostChatWindow::init_timers()
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &HostChatWindow::read_msg);
    timer->start(10);

    for(auto& client_pid : clients_pid)
    {
        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, &client_pid]
        {
            kill(client_pid, SIGKILL);
            for(auto& chat : private_chats)
            {
                int pid;
                std::string  str = chat.first->text().toStdString();
                sscanf(str.c_str(), "Клиент: %d", &pid);
                if(pid == client_pid)
                {
                    delete chat.second;
                    private_chats.erase(chat.first);
                    delete chat.first;
                    break;
                }
            }
        });
        timer->start(60000);
        timers[client_pid] = timer;
    }
}

void HostChatWindow::init_signals()
{
    struct sigaction act;
    act.sa_sigaction = [](int sig, siginfo_t *info, void *context) {
        instance->handle_signal(sig, info, context);
    };
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &act, NULL) == -1)
    {
        perror("Sigaction");
        exit(EXIT_FAILURE);
    }
}

void HostChatWindow::handle_signal(int signo, siginfo_t *info, void *context)
{
    if (signo == SIGUSR1)
    {
        for(auto& chat : private_chats)
        {
            int pid;
            std::string  str = chat.first->text().toStdString();
            sscanf(str.c_str(), "Клиент: %d", &pid);
            if(pid == info->si_pid)
            {
                delete chat.second;
                private_chats.erase(chat.first);
                delete chat.first;
                break;
            }
        }
    }
}
