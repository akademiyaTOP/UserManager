#include "server.h"
#include "clienthandler.h"

Server::Server(QObject* parent) : QTcpServer(parent)
{
    quint16 port = 12345;
    listen(QHostAddress::Any, port);

    //qDebug() << "QHostAddress::Any " << port;
}

void Server::incomingConnection(qintptr descriptor)
{
    auto* handler = new ClientHandler(descriptor,  m_db, this);
    if(handler == nullptr)
        return;


    connect(handler, QThread::finished, this, QObject::deleteLater);
    handler->start();
}
