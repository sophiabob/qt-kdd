#include "db_config.h"
#include <QFile>
#include <QDir>
#include <QDebug>

QSettings* DbConfig::s_settings = nullptr;

bool DbConfig::loadFromIni() { //не запускается
    // Очистка предыдущего
    unload();
/*
    QString osName;

#ifdef Q_OS_WIN
    osName = "Windows";
#elif defined(Q_OS_LINUX)
    osName = "Linux";
#elif defined(Q_OS_UNIX)
    osName = "Unix";
#elif defined(Q_OS_MAC)
    osName = "macOS";
#else
    osName = "Unknown OS";
#endif*/


/*
    QString Path = "";
    QString currentFile = __FILE__;
    QFileInfo fileInfo(currentFile);
    QDir sourceDir = fileInfo.dir();

    qDebug() << "foundPath: " << sourceDir;
    //QString os = Welcome::detectOS();
    sourceDir.cdUp();
    sourceDir.cdUp();

    //foundPath:  "C:/Users/User/Documents/kdd_nice/kdd_nice_2/src/core/resources/settings_win.ini"
    //foundPath:  "C:/Users/User/Documents/kdd_nice/kdd_nice_2/src/core/resources/settings_win.ini"
    if (osName == "Windows") {
        sourceDir.cdUp();
    }
    qDebug() << "foundPath: " << sourceDir;

    Path = sourceDir.absolutePath() +"/resources/";
    QString foundPath = Path + "settings_win.ini";

    qDebug() << "foundPath: " << foundPath;*/





    QString appDir = QCoreApplication::applicationDirPath();  // .../build/Debug
    QString projectRoot = QDir::cleanPath(appDir + "/../../");  // .../kdd_nice_2/
    QString foundPath = projectRoot + "src/core/resources/settings_win.ini";

    qDebug() << "Looking for:" << foundPath;
    qDebug() << "Exists:" << QFileInfo::exists(foundPath);

    /*QString currentFile = __FILE__;  // .../src/core/file.cpp
    QDir dir = QFileInfo(currentFile).dir();  // .../src/core/

    // Пытаемся войти в resources относительно текущего файла
    if (dir.cd("resources")) {
        QString path = dir.absoluteFilePath("settings_win.ini");

        qDebug() << "foundPath: " << path;/*}
        if (QFileInfo::exists(path)) {
            //return path;  // ✅ Нашли!
            qDebug() << "foundPath: " << path;}
    }*/






    /*QString resourcesPath = QCoreApplication::applicationDirPath() + "/../resources";
    resourcesPath = QDir::cleanPath(resourcesPath);

    QString filename = "settings_win.ini";

    // 2. Список путей для поиска (в порядке приоритета)
    QStringList searchPaths = {
        resourcesPath + "/" + filename,                             // 1. resources/
        QCoreApplication::applicationDirPath() + "/" + filename,    // 2. Рядом с exe
        QDir::currentPath() + "/" + filename,                       // 3. Текущая папка
        QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation) + "/" + filename     // 4. Системный конфиг
    };

    // 3. Ищем первый существующий файл
    QString foundPath;
    for (const QString& path : searchPaths) {
        if (QFileInfo::exists(path)) {
            foundPath = path;
            break;
        }
    }*/
   // QString foundPath;

    // Загружаем файл
    s_settings = new QSettings(foundPath, QSettings::IniFormat);

    if (s_settings->status() != QSettings::NoError) {
        qWarning() << "Failed to load config:" << s_settings->status();
        delete s_settings;
        s_settings = nullptr;
        return false;
    }

    qDebug() << "Config loaded from:" << QFileInfo(foundPath).absoluteFilePath() << "| Exists:" << QFileInfo(foundPath).exists();
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
    if (s_settings && s_settings->contains("Settings/" + key)) {
        T value = s_settings->value("Settings/" + key).value<T>();
#ifdef QT_DEBUG
        qDebug() << "[Config.cpp] INI:" << key << "=" << value; //почему ini то ??
#endif
        return value;
    }
#ifdef QT_DEBUG
    qDebug() << "[Config.cpp] DEFAULT:" << key << "=" << defaultValue;
#endif
    return defaultValue;
}

// === Публичные геттеры ===
QString DbConfig::driver() {
    if (s_settings && s_settings->contains("Settings/driver"))
        return s_settings->value("Settings/driver").toString();
    return defaultDriver();
}

QString DbConfig::databaseName() {
    if (s_settings && s_settings->contains("Settings/DatabaseName"))
        return s_settings->value("Settings/DatabaseName").toString();
    return defaultDatabase();
}

QString DbConfig::host() {
    if (s_settings && s_settings->contains("Settings/ip_database"))
        return s_settings->value("Settings/ip_database").toString();
    return defaultHost();
}

int DbConfig::port() {
    if (s_settings && s_settings->contains("Settings/port"))
        return s_settings->value("Settings/port").toInt();
    return defaultPort();
}

QString DbConfig::username() {
    if (s_settings && s_settings->contains("Settings/UserName"))
        return s_settings->value("Settings/UserName").toString();
    return defaultUsername();
}

QString DbConfig::password() {
    if (s_settings && s_settings->contains("Settings/Password"))
        return s_settings->value("Settings/Password").toString();
    return defaultPassword();
}

int DbConfig::connectTimeout() {
    if (s_settings && s_settings->contains("Settings/timeout"))
        return s_settings->value("Settings/timeout").toInt();
    return defaultTimeout();
}

// === Дефолтные значения ===
QString DbConfig::defaultDriver() { return "QPSQL"; }

QString DbConfig::defaultDatabase() { return "postgres"; }

QString DbConfig::defaultHost() { return "localhost"; }

int DbConfig::defaultPort() { return 5432; }

QString DbConfig::defaultUsername() { return "postgres"; }

QString DbConfig::defaultPassword() { return "0"; }
/*
#ifdef QT_DEBUG
    return "";  // Только для отладки
#else
    return "0";  // Пусто = ошибка подключения (безопасно)
#endif
}*/

int DbConfig::defaultTimeout() { return 5000; }  // 5 секунд
