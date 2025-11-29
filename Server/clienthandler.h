#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H
#include <QTcpSocket>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>

#include "database.h"

class ClientHandler : public QThread
{
    Q_OBJECT

public:
    explicit ClientHandler(qintptr descriptor,
                           DataBase& db,
                           QObject* parent = nullptr);

protected:
    void run() override; //запуск потока

private slots:
    void onDisconnected();
    void onReadyRead();

private:
    void handlerAddUser(const QJsonObject& obj);

    void sendJson(const QJsonObject& obj);
    void sendSuccess();
    void sendError(const QString& message);

private:
    qintptr m_descriptor;
    QTcpSocket* m_socket;
    Database& m_db;
};

#endif // CLIENTHANDLER_H
