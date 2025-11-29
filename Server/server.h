#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QObject>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(QObject* parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    Database m_db;
};


#endif // SERVER_H
