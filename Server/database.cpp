#include "database.h"

#include <QCoreApplication>
#include <QJsonObject>
#include <QDebug>
#include <QMutexLocker>

Database::Database()
{
#ifdef Q_OS_WIN
    // Если Qt вдруг не находит плагины, можно добавить путь от каталога приложения.
    const QString pluginsPath = QCoreApplication::applicationDirPath() + "/plugins";
    QCoreApplication::addLibraryPath(pluginsPath);
#endif

    qDebug() << "Database: available drivers =" << QSqlDatabase::drivers();

    if (!QSqlDatabase::isDriverAvailable("QPSQL")) {
        qDebug() << "Database: ERROR - driver QPSQL is not available!";
        return;
    }

    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("127.0.0.1");
    db.setPort(5432);

    // TODO: подставь реальные настройки учебной БД
    db.setDatabaseName("usermanager");
    db.setUserName("usermanager");
    db.setPassword("usermanager");

    if (!db.open()) {
        qDebug() << "Database: failed to open:" << db.lastError().text();
        return;
    }

    qDebug() << "Database: connection opened";

    // Создаём таблицу, если её нет
    QSqlQuery query(db);
    const QString createTableSql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id SERIAL PRIMARY KEY,"
        "username TEXT NOT NULL,"
        "email TEXT NOT NULL"
        ")";

    if (!query.exec(createTableSql)) {
        qDebug() << "Database: failed to create table 'users':"
                 << query.lastError().text();
    } else {
        qDebug() << "Database: table 'users' is ready";
    }
}

bool Database::addUser(const QString& username, const QString& email)
{
    QMutexLocker locker(&dbMutex);

    if (!db.isOpen()) {
        qDebug() << "Database::addUser: DB not open";
        return false;
    }

    // Проверка, что email уникален
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM users WHERE email = :email");
    checkQuery.bindValue(":email", email);

    if (!checkQuery.exec()) {
        qDebug() << "Database::addUser: email check error:"
                 << checkQuery.lastError().text();
        return false;
    }

    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        qDebug() << "Database::addUser: email already exists";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, email) VALUES (:username, :email)");
    query.bindValue(":username", username);
    query.bindValue(":email", email);

    if (!query.exec()) {
        qDebug() << "Database::addUser: insert error:" << query.lastError().text();
        return false;
    }

    return true;
}

bool Database::deleteUser(int userId)
{
    QMutexLocker locker(&dbMutex);

    if (!db.isOpen()) {
        qDebug() << "Database::deleteUser: DB not open";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM users WHERE id = :id");
    query.bindValue(":id", userId);

    if (!query.exec()) {
        qDebug() << "Database::deleteUser: error:" << query.lastError().text();
        return false;
    }

    return true;
}

bool Database::editUser(int userId, const QString& newUsername, const QString& newEmail)
{
    QMutexLocker locker(&dbMutex);

    if (!db.isOpen()) {
        qDebug() << "Database::editUser: DB not open";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE users "
                  "SET username = :username, email = :email "
                  "WHERE id = :id");
    query.bindValue(":username", newUsername);
    query.bindValue(":email",    newEmail);
    query.bindValue(":id",       userId);

    if (!query.exec()) {
        qDebug() << "Database::editUser: error:" << query.lastError().text();
        return false;
    }

    return true;
}

QJsonArray Database::getUsers()
{
    QMutexLocker locker(&dbMutex);

    QJsonArray usersArray;

    if (!db.isOpen()) {
        qDebug() << "Database::getUsers: DB not open";
        return usersArray;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT id, username, email FROM users ORDER BY id")) {
        qDebug() << "Database::getUsers: query error:" << query.lastError().text();
        return usersArray;
    }

    while (query.next()) {
        QJsonObject userObj;
        userObj["id"]       = query.value(0).toInt();
        userObj["username"] = query.value(1).toString();
        userObj["email"]    = query.value(2).toString();
        usersArray.append(userObj);
    }

    return usersArray;
}
