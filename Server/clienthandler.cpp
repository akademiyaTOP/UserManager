#include "clienthandler.h"
#include "database.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QDebug>
#include <QHostAddress>

ClientHandler::ClientHandler(qintptr socketDescriptor, Database& database, QObject* parent)
    : QThread(parent)
    , m_descriptor(socketDescriptor)
    , m_socket(nullptr)
    , db(database)
{
}

// Запускается в отдельном потоке при вызове handler->start().
void ClientHandler::run()
{
    // Создаём сокет БЕЗ родителя — он создаётся в этом потоке и "принадлежит" ему.
    m_socket = new QTcpSocket;

    if (!m_socket->setSocketDescriptor(m_descriptor)) {
        qDebug() << "ClientHandler: failed to set socket descriptor";
        delete m_socket;
        m_socket = nullptr;
        return;
    }

    qDebug() << "ClientHandler: new connection from"
             << m_socket->peerAddress().toString()
             << ":" << m_socket->peerPort();

    // Подписываемся на сигналы сокета.
    // Qt::DirectConnection — слот выполняется в том же потоке, где сокет,
    // т.е. в нашем worker-потоке.
    connect(m_socket, &QTcpSocket::readyRead,
            this, &ClientHandler::onReadyRead,
            Qt::DirectConnection);

    connect(m_socket, &QTcpSocket::disconnected,
            this, &ClientHandler::onDisconnected,
            Qt::DirectConnection);

    // Запускаем цикл обработки событий в этом потоке.
    exec();

    // После выхода из exec() поток завершается.
    // Здесь можно подчистить, если ещё не подчистили в onDisconnected.
    if (m_socket) {
        m_socket->close();
        delete m_socket;
        m_socket = nullptr;
    }

    qDebug() << "ClientHandler: thread finished";
}

void ClientHandler::onReadyRead()
{
    if (!m_socket)
        return;

    QByteArray requestData = m_socket->readAll();
    if (requestData.isEmpty())
        return;

    qDebug() << "ClientHandler: received raw =" << requestData;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(requestData, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        qDebug() << "ClientHandler: JSON parse error =" << parseError.errorString();

        QJsonObject errorResp = makeError(
            QStringLiteral("Invalid JSON: ") + parseError.errorString()
            );

        m_socket->write(QJsonDocument(errorResp).toJson(QJsonDocument::Compact));
        m_socket->flush();
        return;
    }

    QJsonObject request = doc.object();
    const QString action = request.value("action").toString();
    QJsonObject response;

    // Маршрутизация по action → handle-методы, по желанию можно заменить на switch/case
    if (action == "ping") {
        response = handlePing(request);
    }
    else if (action == "add_user") {
        response = handleAddUser(request);
    }
    else if (action == "delete_user") {
        response = handleDeleteUser(request);   // ← ДЗ-метод
    }
    else if (action == "edit_user") {
        response = handleEditUser(request);
    }
    else if (action == "get_users") {
        response = handleGetUsers(request);
    }
    else {
        response = makeError(QStringLiteral("Unknown action: ") + action);
    }

    const QByteArray out = QJsonDocument(response).toJson(QJsonDocument::Compact);
    m_socket->write(out);
    m_socket->flush();

    qDebug() << "ClientHandler: sent response =" << out;
}

void ClientHandler::onDisconnected()
{
    qDebug() << "ClientHandler: client disconnected";

    if (m_socket) {
        m_socket->close();
        delete m_socket;
        m_socket = nullptr;
    }

    // Останавливаем event loop потока → run() завершится.
    quit();
}

// ---------- Вспомогательные и handle-методы ----------

QJsonObject ClientHandler::makeError(const QString& message)
{
    QJsonObject resp;
    resp["status"]  = "error";
    resp["message"] = message;
    return resp;
}

QJsonObject ClientHandler::handlePing(const QJsonObject& /*request*/)
{
    QJsonObject resp;
    resp["status"]  = "ok";
    resp["message"] = "pong";
    return resp;
}

QJsonObject ClientHandler::handleAddUser(const QJsonObject& request)
{
    const QString username = request.value("username").toString();
    const QString email    = request.value("email").toString();

    if (username.isEmpty() || email.isEmpty()) {
        return makeError("username or email is empty");
    }

    if (db.addUser(username, email)) {
        QJsonObject resp;
        resp["status"]  = "success";
        resp["message"] = "User added";
        return resp;
    } else {
        return makeError("Failed to add user");
    }
}

// ==== ВАЖНО: версия ДЛЯ СТУДЕНТОВ — с TODO ====
QJsonObject ClientHandler::handleDeleteUser(const QJsonObject& request)
{
    // TODO (ДЗ):
    // 1) Прочитать id: int id = request["id"].toInt(-1);
    // 2) Проверить, что id > 0.
    //    Если нет — вернуть makeError("Invalid id");
    // 3) Вызвать db.deleteUser(id):
    //    - если true  → {"status":"success","message":"User deleted"}
    //    - если false → {"status":"error","message":"Failed to delete user"}

    Q_UNUSED(request);
    return makeError("handleDeleteUser is not implemented yet");
}

QJsonObject ClientHandler::handleEditUser(const QJsonObject& request)
{
    const int id = request.value("id").toInt(-1);
    const QString username = request.value("username").toString();
    const QString email    = request.value("email").toString();

    if (id <= 0 || username.isEmpty() || email.isEmpty()) {
        return makeError("Invalid data for edit_user");
    }

    if (db.editUser(id, username, email)) {
        QJsonObject resp;
        resp["status"]  = "success";
        resp["message"] = "User updated";
        return resp;
    } else {
        return makeError("Failed to update user");
    }
}

QJsonObject ClientHandler::handleGetUsers(const QJsonObject& /*request*/)
{
    QJsonObject resp;
    resp["status"]  = "success";
    resp["message"] = "Users list";
    resp["users"]   = db.getUsers(); // QJsonArray из Database
    return resp;
}
