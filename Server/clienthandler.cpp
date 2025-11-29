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
        sendError("nichego netu");
        return;
    }

    QString username = obj["username"].toString();
    QString email = obj["email"].toString();

    bool ok = db.addUser(username, email);

    if (!ok) {
        qDebug() << "Failed to addUser";
        return;
    }

    sendSeccuess();
}

void ClientHandler::sendJson(const QJsonObject& obj)
{
    QJsonDocument doc(obj);
    m_socket->write(doc.toJson(QJsonDocument::Compact));
}

void ClientHandler::sendSeccuess()
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
