#ifdef USE_FIFO_FILE
#include "fifo_names.hpp"
#elif defined(USE_MQ_FILE)
#include "mq_names.hpp"
#elif defined(USE_SHM_FILE)
#include "shm_names.hpp"
#endif

#include <QApplication>

#include "client_gui.hpp"

using namespace client_namespace;

namespace
{
    TempClient& client = TempClient::get_instance(host_pid_path, identifier);
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
        client.read_take_answer_from_host(answer);

        status = answer.substr(0, answer.find(' '));
        bookName = answer.substr(answer.find(' ') + 1);
        if (status == "OK"){
            mainwindow_pointer->addBookToClientList(bookName.data());
        }
        mainwindow_pointer->addRequestToTable("Take", bookName.data(), status.data());

        answer.clear();
        break;
        case SIGUSR1:
            std::cerr<<"read return answer" << std::endl;
            client.read_return_answer_from_host(answer);
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
    client.send_request_to_host(SIGUSR1, request);
}

void ClientWindow::send_take_request_to_host(const std::string &request) {
    std::cerr << "send_take_request_to_host" << std::endl;
    client.send_request_to_host(SIGUSR2, request);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ClientWindow window;
    mainwindow_pointer = &window;

    window.show();

    return app.exec();
}