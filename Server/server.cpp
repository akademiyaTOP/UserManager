#include "server.h"
#include "clienthandler.h"

Server::Server(QObject* parent) : QTcpServer(parent)
{
    quint16 port = 12345;
    listen(QHostAddress::Any, port);
}

void Server::incomingConnection(qintptr descriptor)
{
    auto* handler = new ClientHandler(descriptor, this);
    if(handler == nullptr)
        return;

    handler->start();
}
