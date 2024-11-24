#include <QApplication>
#include "host_gui.hpp"
#include "client_gui.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    if (argc > 1 && QString(argv[1]) == "host") {
        HostGUI host;
        host.show();
    } else {
        ClientGUI client;
        client.show();
    }

    return app.exec();
}
