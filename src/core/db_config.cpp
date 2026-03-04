#include "db_config.h"
#include <QFile>
#include <QDir>
#include <QDebug>

QSettings* DbConfig::s_settings = nullptr;

bool DbConfig::loadFromIni() {
    // Очистка предыдущего
    unload();

    QString resourcesPath = QCoreApplication::applicationDirPath() + "/../resources";
    resourcesPath = QDir::cleanPath(resourcesPath);

    QString filename = "settings.ini";
    // 2. Список путей для поиска (в порядке приоритета)
    QStringList searchPaths = {
        resourcesPath + "/" + filename,                    // 1. resources/
        QCoreApplication::applicationDirPath() + "/" + filename,  // 2. Рядом с exe
        QDir::currentPath() + "/" + filename,              // 3. Текущая папка
        QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation) + "/" + filename  // 4. Системный конфиг
    };

    // 3. Ищем первый существующий файл
    QString foundPath;
    for (const QString& path : searchPaths) {
        if (QFileInfo::exists(path)) {
            foundPath = path;
            break;
        }
    }

    // Загружаем файл
    s_settings = new QSettings(foundPath, QSettings::IniFormat);
    if (s_settings->status() != QSettings::NoError) {
        qWarning() << "Failed to load config:" << s_settings->status();
        delete s_settings;
        s_settings = nullptr;
        return false;
    }

    qDebug() << "✓ Config loaded:" << QFileInfo(foundPath).absoluteFilePath();
    return true;
}

void DbConfig::unload() {
    if (s_settings) {
        delete s_settings;
        s_settings = nullptr;
    }
}

bool DbConfig::isLoaded() {
    return s_settings != nullptr;
}

// === Шаблонный геттер: INI → default → fallback ===
template<typename T>
T DbConfig::getValue(const QString& key, T defaultValue) {
    if (s_settings && s_settings->contains("database/" + key))
        return s_settings->value("database/" + key).value<T>();
    return defaultValue;
}

// === Публичные геттеры ===
QString DbConfig::driver() {
    if (s_settings && s_settings->contains("database/driver"))
        return s_settings->value("database/driver").toString();
    return defaultDriver();
}

QString DbConfig::databaseName() {
    if (s_settings && s_settings->contains("database/name"))
        return s_settings->value("database/name").toString();
    return defaultDatabase();
}

QString DbConfig::host() {
    if (s_settings && s_settings->contains("database/host"))
        return s_settings->value("database/host").toString();
    return defaultHost();
}

int DbConfig::port() {
    if (s_settings && s_settings->contains("database/port"))
        return s_settings->value("database/port").toInt();
    return defaultPort();
}

QString DbConfig::username() {
    if (s_settings && s_settings->contains("database/user"))
        return s_settings->value("database/user").toString();
    return defaultUsername();
}

QString DbConfig::password() {
    //  Пароль: env-переменная > INI > default (только Dev)
   // QString envPwd = qgetenv("APP_DB_PASSWORD");
    //if (!envPwd.isEmpty()) return envPwd;

    if (s_settings && s_settings->contains("database/password"))
        return s_settings->value("database/password").toString();
    return defaultPassword();  // Пусто для Prod = безопасно
}

int DbConfig::connectTimeout() {
    if (s_settings && s_settings->contains("database/timeout"))
        return s_settings->value("database/timeout").toInt();
    return defaultTimeout();
}

// === Дефолтные значения ===
QString DbConfig::defaultDriver() { return "QPSQL"; }

QString DbConfig::defaultDatabase() { return "postgres"; }

QString DbConfig::defaultHost() { return "localhost"; }

int DbConfig::defaultPort() { return 5432; }

QString DbConfig::defaultUsername() { return "postgres"; }

QString DbConfig::defaultPassword() {

#ifdef QT_DEBUG
    return "";  // Только для отладки
#else
    return "0";  // Пусто = ошибка подключения (безопасно)
#endif
}

int DbConfig::defaultTimeout() { return 5000; }  // 5 секунд
