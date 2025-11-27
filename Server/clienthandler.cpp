#include "clienthandler.h"

ClientHandler::ClientHandler(qintptr descriptor, QObject* parent)
    : QThread(parent),
      m_descriptor(descriptor),
      m_socket(nullptr)
{}

void ClientHandler::run()
{
    m_socket = new QTcpSocket();

    if (!m_socket->setSocketDescriptor(m_descriptor)) {
        qDebug() << "ClientHandler: failed to set socket descriptor";
        m_socket->deleteLater();
        m_socket = nullptr;
        return;
    }

    qDebug() << "ClientHandler start " <<QThread::currentThread();

    //m_socket->setSocketDescriptor(m_descriptor);

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
    if(!m_socket)
        return;


    QByteArray data = m_socket->readAll();
    qDebug() << "Server received " << data;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Server JSON parse error: " << parseError.errorString();

        QJsonObject errResponce;
        errResponce["status"] = "error";
        errResponce["message"] = QStringLiteral("Invalid JSON: ") + parseError.errorString();

        QByteArray out = QJsonDocument(errResponce).toJson(QJsonDocument::Compact);
        m_socket->write(out);
        m_socket->flush();
        return;
    }

    QJsonObject request = doc.object();
    const QString action = request.value("action").toString();

    QJsonObject responce;

    if (action == "ping") {
        responce["status"] = "ok";
        responce["message"] = "pong";
    } else {
        responce["status"] = "error";
        responce["message"] = QStringLiteral("unkown action: ") + action;
    }

    QJsonDocument respDoc(responce);
    QByteArray out = QJsonDocument(responce).toJson(QJsonDocument::Compact);
    m_socket->write(out);
    m_socket->flush();

    qDebug() << "Server sent:" << out;
}

void ClientHandler::onDisconnected()
{
    qDebug() << "ClientHandler: client disconnected";

    if (m_socket) {
        m_socket->deleteLater();
        m_socket = nullptr;
    }

    quit();
}


