#include "clienthandler.h"
#include <QJsonParseError>
#include <QDebug>

ClientHandler::ClientHandler(qintptr descriptor, DataBase &db, QObject* parent)
    : QThread(parent),
      m_descriptor(descriptor),
      m_socket(nullptr),
    m_db(db)
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
        sendError(QStringLiteral("Invalid JSON: ") + parseError.errorString());
        return;
    }

    QJsonObject request = doc.object();
    const QString action = request.value("action").toString();

    if (action == "ping") {
        QJsonObject responce;
        responce["status"] = "ok";
        responce["message"] = "pong";
        sendJson(responce);
        return;
    }

    if (action == "add_user")
        handlerAddUser(obj);
    else if (action == "delete_user")
        handlerDeleteUser(obj);
    else if (action == "edit_user")
        handleEditUser(obj);
    else if (action == "get_users")
        handleGetUsers();
    else
        sendError("Unkown action: " + action);
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

void ClientHandler::handlerAddUser(const QJsonObject& obj)
{
    if (!responce.contains("username") || !responce.contains("email")) {
        sendError("nichego netu, ni email, ni username");
        return;
    }

    const QString username = obj.value("username").toString();
    const QString email = obj.value("email").toString();

    if (!m_db.addUser(username, email)) {
        sendError("Failed to addUser");
        return;
    }

    QJsonObject responce;
    responce["status"] = "success";
    responce["message"] = "User added seccessfully";
    responce["users"] = m_db.getUsers();
    sendJson(responce);
}

void ClientHandler::sendJson(const QJsonObject& obj)
{
    if (!m_socket)
        return;

    QJsonDocument doc(obj);
    QByteArray out = doc.toJson(QJsonDocument::Compact);

    m_socket->write(out);
    m_socket->flush();

    qDebug() << "Server sent:" << out;
}

void ClientHandler::sendSuccess()
{
    QJsonObject obj;
    obj["status"] = "success";
    sendJson();
}

void ClientHandler::sendError(const QString& message)
{
    QJsonObject obj;
    obj["status"] = "error";
    obj["message"] = message;
    sendJson(obj);
}
