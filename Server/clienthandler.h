#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QThread>
#include <QTcpSocket>
#include <QJsonObject>

class Database;

// Один поток на одного клиента. Внутри — QTcpSocket + сигналы/слоты.
class ClientHandler : public QThread
{
    Q_OBJECT
public:
    explicit ClientHandler(qintptr socketDescriptor, Database& database, QObject* parent = nullptr);

protected:
    void run() override;

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    // Обработчики конкретных команд протокола:
    QJsonObject handlePing      (const QJsonObject& request);
    QJsonObject handleAddUser   (const QJsonObject& request);
    QJsonObject handleDeleteUser(const QJsonObject& request);  // ← ДЗ
    QJsonObject handleEditUser  (const QJsonObject& request);
    QJsonObject handleGetUsers  (const QJsonObject& request);

    QJsonObject makeError(const QString& message);

private:
    qintptr m_descriptor;  // socketDescriptor из incomingConnection
    QTcpSocket* m_socket;      // сокет, живущий в этом потоке
    Database& db;            // ссылка на общий объект базы
};

#endif // CLIENTHANDLER_H
