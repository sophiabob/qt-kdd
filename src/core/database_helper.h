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
    qDebug() << "[HELPER] START withDbConnection";  // ← Отладка

    // Уникальное имя подключения
    QString connName = "TEMP_" + QUuid::createUuid().toString();

    // Создаём и настраиваем подключение
    QSqlDatabase db = QSqlDatabase::addDatabase(DbConfig::driver(), connName);
    db.setDatabaseName(DbConfig::databaseName());
    db.setHostName(DbConfig::host());
    db.setPort(DbConfig::port());
    db.setUserName(DbConfig::username());
    db.setPassword(DbConfig::password());
    db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=5000");

    // Открываем
    if (!db.open()) {
        qCritical() << "[HELPER] ❌ open() failed:" << db.lastError().text();
        QSqlDatabase::removeDatabase(connName);
        return ResultType{};  // ← Возвращаем пустой результат
    }

    qDebug() << "[HELPER] ✅ DB opened, calling operation...";

    // === ВЫЗОВ ЛЯМБДЫ (ОБЯЗАТЕЛЬНО!) ===
    ResultType result = operation(db);  // ← ← ← ЗДЕСЬ ВЫПОЛНЯЕТСЯ ТВОЙ КОД!

    qDebug() << "[HELPER] ✅ Operation done, closing DB...";

    // Закрываем и удаляем
    db.close();
    QSqlDatabase::removeDatabase(connName);

    qDebug() << "[HELPER] END withDbConnection";
    return result;
}

// === Специализация для void ===
inline void withDbConnection(std::function<void(QSqlDatabase&)> operation)
{
    QString connName = "TEMP_" + QUuid::createUuid().toString();
    QSqlDatabase db = QSqlDatabase::addDatabase(DbConfig::driver(), connName);
    db.setDatabaseName(DbConfig::databaseName());
    db.setHostName(DbConfig::host());
    db.setPort(DbConfig::port());
    db.setUserName(DbConfig::username());
    db.setPassword(DbConfig::password());
    db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=5000");

    if (db.open()) {
        operation(db);  // ← Вызов лямбды
        db.close();
    }
    QSqlDatabase::removeDatabase(connName);
}


#endif // DATABASE_HELPER_H
