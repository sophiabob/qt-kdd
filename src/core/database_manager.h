#pragma once
#include <QSqlDatabase>
#include <QSqlError>
#include <optional>
#include "db_config.h"

class DatabaseManager {
public:
   // static DatabaseManager& instance();  // Singleton

    static DatabaseManager& instance() {
        static DatabaseManager inst;  // ← Meyers singleton (потокобезопасный в C++11)
        return inst;
    }

    bool connect();                      // Подключиться
    bool disconnect();
    bool isConnected() const;            // Статус
    QSqlDatabase connection();           // Получить соединение
    QString lastError() const;           // Текст последней ошибки
    bool reconnect();                    // Переподключиться

    // Тестовый запрос (проверка "живости")
    bool ping();

    bool open(const QString& dbPath = ":memory:") {
        if (m_db.isOpen()) return true;

        m_db = QSqlDatabase::addDatabase("QSQLITE");  // ← Или "QPSQL", "QMYSQL"
        m_db.setDatabaseName(dbPath);

        if (!m_db.open()) {
            qCritical() << "DB open failed:" << m_db.lastError().text();
            return false;
        }
        qDebug() << "DB opened:" << dbPath;
        return true;
    }

private:
    //DatabaseManager();  // Приватный конструктор для Singleton
    DatabaseManager();// = default;  // ← Приватный конструктор
    ~DatabaseManager() { if (m_db.isOpen()) m_db.close(); }

    QSqlDatabase m_db;
    QString m_error;

    // Запрет копирования
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};
