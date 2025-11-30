#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonArray>
#include <QMutex>

// Класс-обёртка над PostgreSQL, один на весь сервер.
class Database
{
public:
    Database();

    bool addUser   (const QString& username, const QString& email);
    bool deleteUser(int userId);
    bool editUser  (int userId, const QString& newUsername, const QString& newEmail);
    QJsonArray getUsers();

private:
    QSqlDatabase db;
    QMutex dbMutex;
};

#endif // DATABASE_H
