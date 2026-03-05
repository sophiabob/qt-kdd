#pragma once
#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QSqlDatabase>
#include "db_config.h"
#include "database_helper.h"

class DatabaseManager {
public:
    // Синглтон: возвращает экземпляр (но не подключение!)
    static DatabaseManager& instance() {
        static DatabaseManager inst;
        return inst;
    }

    // Метод для получения настроек (если нужно вне helper)
    static QSqlDatabase createConnection(const QString& connName = "DEFAULT") {
        QSqlDatabase db = QSqlDatabase::addDatabase(DbConfig::driver(), connName);
        //QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
        db.setHostName(DbConfig::host());
        db.setPort(DbConfig::port());
        db.setDatabaseName(DbConfig::databaseName());
        db.setUserName(DbConfig::username());
        db.setPassword(DbConfig::password());
        db.setConnectOptions("CONNECT_TIMEOUT=10");  // Таймаут подключения (сек)

        return db;
    }

private:
    DatabaseManager() = default;  // Конструктор пустой — настройки в helper
    ~DatabaseManager() = default;

    // Запрет копирования
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};

#endif // DATABASE_MANAGER_H
