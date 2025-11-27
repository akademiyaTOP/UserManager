#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QJsonArray>
#include <QMutex>

class DataBase
{
public:
    DataBase();

    bool addUser(const QString username, const QString& email);
    bool deleteUser(int userId);
    bool editUser(int userId, const QString& newUsername, const QString& newEmail);
    QJsonArray getUsers();

private:
    QSqlDatabase db;
    QMutex dbMutex;
};

#endif // DATABASE_H
