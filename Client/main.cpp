#include <QApplication>
#include <QWidget>
#include "client.h"
#include <QTimer>

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);

    // QWidget w;
    // w.setWindowTitle("Окно клиента");
    // w.resize(400,200);
    // w.show();

    Client client;
    client.connectToServer("127.0.0.1", 12345);

    QTimer::singleShot(1000, [&client](){
        QJsonObject obj;
        obj["command"] = "ping";
        client.sendJson(obj);
    });

    return app.exec();
}
