#include "clienthandler.h"

ClientHandler::ClientHandler(qintptr descriptor, QObject* parent)
    : QThread(parent),
      m_descriptor(descriptor),
      m_socket(nullptr)
{}

void ClientHandler::run()
{
    m_socket = new QTcpSocket();
    m_socket->setSocketDescriptor(m_descriptor);

    connect(m_socket, &QTcpSocket::disconnected,
            this, &ClientHandler::onDisconnected);

    connect(m_socket, &QTcpSocket::readyRead,
            this, &ClientHandler::onReadyRead);


    exec();
    // while(true)
    //     жду события -> вызываю слоты
}

void ClientHandler::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    QString cmd = obj["command"].toString();

    if (cmd == "ping")
    {
        QJsonObject resp{{"status", "ok"}};
        m_socket->write(QJsonDocument(resp).toJson());
    }
}

void ClientHandler::onDisconnected()
{
    m_socket->deleteLater();
    quit();
}


