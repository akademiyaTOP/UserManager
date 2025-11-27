#include "client.h"
#include <QJsonDocument>
#include <qDebug>

Client::Client(QObject* parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::connected,
            this, &Client::onConnected);

    connect(m_socket, &QTcpSocket::readyRead,
            this, &Client::onReadyRead);

    connect(m_socket, &QTcpSocket::disconnected,
            this, &Client::onDisconnected);
}

void Client::connectToServer(const QString& host, quint16 port)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Client: already connected";
        return;
    }

    qDebug() << "Client connecting to " << host << ":" << port;
    m_socket->connectToHost(host, port);
}

void Client::onConnected()
{
    qDebug() << "Client connected!";
}

void Client::sendJson(const QJsonObject &obj)
{
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    m_socket->write(data);
    m_socket->flush();

    qDebug() << "Client send:" << data;
}

void Client::onReadyRead()
{
    QByteArray data = m_socket->readAll();

    qDebug() << "Client received: " << data;
}

void Client::onDisconnected()
{
    qDebug() << "Client disconnected";
}
