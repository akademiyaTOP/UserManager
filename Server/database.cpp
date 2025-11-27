#include "database.h"
#include <QJsonObject>
#include <QDebug>
#include <QMutexLocker>

DataBase::DataBase()
{
    db = QSqlDatabase::addDatabase("QPSQL");

    db.setHostName("127.0.0.1");
    db.setPort(5432);
    db.setDatabaseName("usermanager");
    db.setUserName("postgres");
    db.setPassword("postgres");


    if (!db.open()) {
        qDebug() << "Database: failed to open:" << db.lastError().text();
        return;
    }

    qDebug() << "Database: connection opened";


}
