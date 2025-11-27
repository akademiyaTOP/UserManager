#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject* parent = nullptr);

    void connectToServer(const QString& host, quint16 port);
    void sendJson(const QJsonObject& obj);

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket* m_socket;
};

// CLIENT_H
