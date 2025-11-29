#include "client.h"
#include <QJsonDocument>
#include <QJsonParseError>
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
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Client: not connected";
        return;
    }

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

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Client JSON parse error: " << parseError.errorString();
        return;
    }

    QJsonObject responce = doc.object();
    const QString status = responce.value("status").toString();
    const QString message = responce.value("message").toString();

    qDebug() << "Client parsed responce. Status: " << status
             << "Message: " << message;
}

void Client::onDisconnected()
{
    qDebug() << "Client disconnected";
}
