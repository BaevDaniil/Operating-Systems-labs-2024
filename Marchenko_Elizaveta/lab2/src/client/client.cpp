#ifdef FIFO_Connection
#include "../conn_templates/fifo_temp.hpp"
#elif defined(MQ_Connection)
#include "../conn_templates/mq_temp.hpp"
#elif defined(SEG_Connection)
#include "../conn_templates/seg_temp.hpp"
#endif
#include "client.hpp"

#include <QApplication>

#include "ClientWindow.hpp"
#include "../config.hpp"



using namespace config;
namespace{
    Client<ConnType>& client = Client<ConnType>::getInstance(host_pid_path);
    ClientWindow *window_ptr;
}

void client_signal_handler(const int sig, siginfo_t *info, void *context){
    std::cerr<<"client_signal_handler"<<std::endl;
    std::string status;

    if (!window_ptr)
        return;
    switch (sig)
    {
        case SIGUSR2:
            client.read_game_status(status);
            if (std::stoi(status) == 1) {
                window_ptr->startNewTurn();
            }
            else {
                exit(0);
            }
            status.clear();
        break;
        case SIGUSR1:
            sem_post(client.game_sem);
            client.read_life_status(status);
            window_ptr->updateStatus(client.getStatus());
            status.clear();
            break;
        default:
            break;
    }
 }

void ClientWindow::onSendButtonClicked() const {
    std::cout<<"ClientWindow::SendButtonClicked"<<std::endl;
    QString chosenNumber = inputField->text().isEmpty() ? "—" : inputField->text();
    chosenNumberLabel->setText("Выбранное число: " + chosenNumber);
    client.send_move_to_host(chosenNumber.toStdString());
    sendButton->setEnabled(false);
    inputField->setEnabled(false);
    turnTimer->stop(); // Остановка таймера после отправки
}

void ClientWindow::onTimeout() const {
    std::cout<<"ClientWindow::onTimeout"<<std::endl;
    chosenNumberLabel->setText("Выбранное число: —");
    client.send_move_to_host("");
    sendButton->setEnabled(false);
    inputField->setEnabled(false);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ClientWindow window;
    window_ptr = &window;

    window.show();

    return app.exec();
}