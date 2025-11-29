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

    QSqlQuery query(db);
    const QString createTableSql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id SERIAL PRIMARY KEY,"
        "username TEXT NOT NULL,"
        "email TEXT NOT NULL"
        ")";

    if (!query.exec(createTableSql))
    {
        qDebug() << "Database: failed to create table 'users':"
                 << query.lastError().text();
    } else {
        qDebug() << "Database table 'users' is create";
    }


}

bool DataBase::addUser(const QString username, const QString &email)
{
    QMutexLocker locker(&dbMutex);


    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, email) VALUES (:username, :email)");
    query.bindValue(":username", username);
    query.bindValue(":email", email);


    if(!query.exec()) {
        qDebug() << "Database adduser error:"
                 << query.lastError().text();
        return false;
    }
    return true;
}


