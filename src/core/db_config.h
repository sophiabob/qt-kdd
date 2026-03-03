// db_config.h
#pragma once
#include <QString>

struct DbConfig {
    enum Env { demoWindows, demoLinux, Ini };  // Среды

    // Текущая среда (можно менять в runtime)
    static Env currentEnv();
    static void setCurrentEnv(Env env);

    // Настройки для каждой среды
    static QString driver() { return "QPSQL"; }
    static QString databaseName(Env env = currentEnv());
    static QString host(Env env = currentEnv());
    static int port(Env env = currentEnv());
    static QString username(Env env = currentEnv());
    static QString password(Env env = currentEnv());

private:
    static Env s_currentEnv;  // Статическое поле
};
