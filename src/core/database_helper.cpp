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
        db.setDatabaseName(DbConfig::databaseName());
        db.setHostName(DbConfig::host());
        db.setPort(DbConfig::port());
        db.setUserName(DbConfig::username());
        db.setPassword(DbConfig::password());
        db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=5000");
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
