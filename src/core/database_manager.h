#pragma once
#include <QSqlDatabase>
#include <QSqlError>
#include <optional>
#include "db_config.h"

class DatabaseManager {
public:
    static DatabaseManager& instance();  // Singleton

    bool connect();                      // Подключиться
    bool isConnected() const;            // Статус
    QSqlDatabase connection();           // Получить соединение
    QString lastError() const;           // Текст последней ошибки
    bool reconnect();                    // Переподключиться

    // Тестовый запрос (проверка "живости")
    bool ping();

private:
    DatabaseManager();  // Приватный конструктор для Singleton
    QSqlDatabase m_db;
    QString m_error;

    // Запрет копирования
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};
