#ifndef SERVER_H
#define SERVER_H
#include "database.h"
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
    Database db;
};


#endif // SERVER_H
