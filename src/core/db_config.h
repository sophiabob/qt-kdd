#pragma once
#ifndef DB_CONFIG_H
#define DB_CONFIG_H

#include <QString>
#include <QSettings>
#include <QStandardPaths>

#include <QCoreApplication>  // ← Для applicationDirPath()
#include <QFileInfo>         // ← Для QFileInfo::exists()
#include <QDir>              // ← Для QDir::cleanPath()

class DbConfig {
public:
    // Загрузка конфига (вызвать один раз в main)
    static bool loadFromIni();

    // Геттеры настроек
    static QString driver();
    static QString databaseName();
    static QString host();
    static int port();
    static QString username();
    static QString password();  // 🔐 + проверка env-переменной
    static int connectTimeout();

    // Проверка: загружен ли конфиг
    static bool isLoaded();

    // Очистка (для тестов)
    static void unload();

private:
    static QSettings* s_settings;  // Указатель на настройки

    // Дефолтные значения (если INI не загружен или ключа нет)
    static QString defaultDriver();
    static QString defaultDatabase();
    static QString defaultHost();
    static int defaultPort();
    static QString defaultUsername();
    static QString defaultPassword();
    static int defaultTimeout();

    // Вспомогательный геттер
    template<typename T>
    static T getValue(const QString& key, T defaultValue);
};

#endif // DB_CONFIG_H
