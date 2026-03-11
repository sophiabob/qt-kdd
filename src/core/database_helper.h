/*#ifndef DATABASE_HELPER_H
#define DATABASE_HELPER_H

class database_helper
{
public:
    database_helper();
};

#endif // DATABASE_HELPER_H

*/
#pragma once
#ifndef DATABASE_HELPER_H
#define DATABASE_HELPER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>
#include <functional>
#include "db_config.h"

template<typename ResultType>
ResultType withDbConnection(std::function<ResultType(QSqlDatabase&)> operation)
{
    qDebug() << "[HELPER.H] START withDbConnection";  // ← Отладка

    // Уникальное имя подключения
    QString connName = "TEMP_" + QUuid::createUuid().toString();
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", connName);

    db.setHostName(DbConfig::host());
    db.setPort(DbConfig::port());

    //QString dbName = DbConfig::databaseName();  // Просто имя, например "postgres"
    //db.setDatabaseName(QString("dbname=%1 options='-c client_encoding=UTF8'").arg(dbName));
    db.setDatabaseName(DbConfig::databaseName());

    db.setUserName(DbConfig::username());
    db.setPassword(DbConfig::password());

    //db.setConnectOptions("CONNECT_TIMEOUT=10");

    qDebug() << "=== [DB] CONNECT DEBUG ===";
    qDebug() << "Host:" << DbConfig::host();
    qDebug() << "Port:" << DbConfig::port();
    qDebug() << "User:" << DbConfig::username();
    qDebug() << "DB Name:" << DbConfig::databaseName();
    qDebug() << "Password:" << DbConfig::password();  // ← Увидишь реальный пароль
    qDebug() << "=========================";


    if (!db.open()) {
        qCritical() << "[HELPER.H] open() failed:" << db.lastError().text();
        QSqlDatabase::removeDatabase(connName);
        return ResultType{}; // ← Возвращаем пустой результат
    }

    qDebug() << "[HELPER.H] DB opened, calling operation...";

    //operation(db);
    ResultType result = operation(db);  //тут всё происходит
    {
        QSqlQuery cleanup(db);  // Пустой запрос для "очистки" драйвера
    }
    db.close();
    QSqlDatabase::removeDatabase(connName);

    qDebug() << "[HELPER.H] END withDbConnection";
    return result;
}

// === Специализация для void ===
inline void withDbConnection(std::function<void(QSqlDatabase&)> operation)
{

    //qDebug() << "withDbConnection(std::function<void(QSqlDatabase&)> operation)";

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");

    db.setHostName(DbConfig::host());
    db.setPort(DbConfig::port());
    db.setDatabaseName(DbConfig::databaseName());
    db.setUserName(DbConfig::username());
    db.setPassword(DbConfig::password());
    //db.setConnectOptions("CONNECT_TIMEOUT=10");

    if (db.open()) {
        operation(db);  // ← Вызов лямбды
        db.close();
    }
    QSqlDatabase::removeDatabase("default");


    /*// 2. При переподключении (смена БД)
    QSqlDatabase::removeDatabase("oldConn");
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", "newConn");*/
}


#endif // DATABASE_HELPER_H
