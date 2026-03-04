#include "database_manager.h"
#include <QSqlQuery>
#include <QFile>
#include <QDebug>

DatabaseManager::DatabaseManager() {
    m_db = QSqlDatabase::addDatabase(DbConfig::driver(), "MAIN_CONNECTION");
    m_db.setDatabaseName(DbConfig::databaseName());
    m_db.setHostName(DbConfig::host());
    m_db.setPort(DbConfig::port());
    m_db.setUserName(DbConfig::username());
    m_db.setPassword(DbConfig::password());
    m_db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=5000");  // Для SQLite
}
/*
//DatabaseManager&
DatabaseManager::instance() {
    static DatabaseManager instance;  // Singleton (ленивая инициализация)
    return instance;
}*/

bool DatabaseManager::connect() {
    if (m_db.isOpen()) return true;

    if (!m_db.open()) {
        m_error = m_db.lastError().text();
        qWarning() << "DB connect failed:" << m_error;
        return false;
    }

    qDebug() << "DB connected:" << DbConfig::databaseName();
    return true;
}

bool DatabaseManager::isConnected() const {
    return m_db.isOpen() && m_db.isValid();
}

QSqlDatabase DatabaseManager::connection() {
    return m_db;  // Возвращаем копию (QSqlDatabase — lightweight handle)
}

QString DatabaseManager::lastError() const {
    return m_error;
}

bool DatabaseManager::reconnect() {
    if (m_db.isOpen()) m_db.close();
    m_error.clear();
    return connect();
}

bool DatabaseManager::ping() {
    if (!isConnected()) return false;

    QSqlQuery test(m_db);
    return test.exec("SELECT 1");  // Универсальный тест для любой БД
}
