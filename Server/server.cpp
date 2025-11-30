#include "server.h"
#include "clienthandler.h"
#include <QHostAddress>
#include <QDebug>

Server::Server(QObject* parent) : QTcpServer(parent)
{
    quint16 port = 12345;
    if(!listen(QHostAddress::Any, port))
        qDebug() << "Server: failed to listen on port" << port << ":" << errorString();
    else
        qDebug() << "Server: listening on port" << port;
}

void Server::incomingConnection(qintptr descriptor)
{
    qDebug() << "Server: new connection, descriptor =" << descriptor;

    auto* handler = new ClientHandler(descriptor,  db, this);
    if(handler == nullptr)
        return;

    // Когда поток завершится - удаляем обработчик
    connect(handler, QThread::finished, this, QObject::deleteLater);

    handler->start();

    qDebug() << "New connect intalled";
}
