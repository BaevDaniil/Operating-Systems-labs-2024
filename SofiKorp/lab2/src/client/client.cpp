#ifdef FIFO_COMMUNICATION
#include "fifo_names.hpp"
#elif defined(MQ_COMMUNICATION)
#include "mq_names.hpp"
#elif defined(SHM_COMMUNICATION)
#include "shm_names.hpp"
#endif

#include <QApplication>
#include "client.hpp"

#include "ClientWindow.hpp"

using namespace client_config;

namespace
{
    TempClient& client = TempClient::getInstance(pid_file_path, identifier);
    ClientWindow *mainwindow_pointer;
}

void client_signal_handler(int sig, siginfo_t *info, void *context)
{
    std::cerr<<"client_signal_handler"<<std::endl;
    static std::string answer;
    std::string status, bookName;
    if (!mainwindow_pointer)
        return;
    switch (sig)
    {
    case SIGUSR2:
        client.readResponse(answer, false);

        status = answer.substr(0, answer.find(' '));
        bookName = answer.substr(answer.find(' ') + 1);
        if (status == "OK"){
            mainwindow_pointer->addBookToClientList(bookName);
        }
        mainwindow_pointer->addRequestToTable("Take", bookName.data(), status.data());

        answer.clear();
        break;
        case SIGUSR1:
            std::cerr<<"read return answer" << std::endl;
            client.readResponse(answer, true);
            status = answer.substr(0, answer.find(' '));
            bookName = answer.substr(answer.find(' ') + 1);
            std::cerr<< status << ' ' << bookName << std::endl;
            if (status == "OK"){
                mainwindow_pointer->deleteBookFromClientList(bookName.data());
            }
            std::cerr<<"remove book from client list"<<std::endl;
            mainwindow_pointer->addRequestToTable("Return", bookName.data(), status.data());

            answer.clear();
        break;
    default:
        break;
    }
}


void ClientWindow::send_return_request_to_host(const std::string &request){
    std::cerr << "send_return_request_to_host" << std::endl;
    client.sendRequest(SIGUSR1, request);
}

void ClientWindow::send_take_request_to_host(const std::string &request) {
    std::cerr << "send_take_request_to_host" << std::endl;
    client.sendRequest(SIGUSR2, request);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ClientWindow window;
    mainwindow_pointer = &window;

    window.show();

    return app.exec();
}