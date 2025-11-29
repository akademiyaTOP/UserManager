#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H
#include <QTcpSocket>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>

class ClientHandler : public QThread
{
    Q_OBJECT

public:
    explicit ClientHandler(qintptr descriptor, QObject* parent = nullptr);

protected:
    void run() override; //запуск потока

private slots:
    void onDisconnected();
    void onReadyRead();

private:
    qintptr m_descriptor;
    QTcpSocket* m_socket;
    Database& m_db;

};

#endif // CLIENTHANDLER_H
