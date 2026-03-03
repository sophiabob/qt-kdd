#include "db_config.h"

DbConfig::Env DbConfig::s_currentEnv = DbConfig::Ini;  // По умолчанию
DbConfig::Env DbConfig::currentEnv() { return s_currentEnv; }
void DbConfig::setCurrentEnv(Env env) { s_currentEnv = env; }

//{ demoWindows, demoLinux, Ini };
QString DbConfig::databaseName(Env env) {
    switch (env) {
    case demoWindows: return "data/dev.db";
    case demoLinux: return "stage_db";
    case Ini: return "prod_db";

    default: return "data/app.db";
    }
}

QString DbConfig::host(Env env) {
    switch (env) {
    case demoWindows: return "localhost";
    case demoLinux: return "localhost";
    case Ini: return "stage.db.server";

    default: return "localhost";
    }
}

int DbConfig::port(Env env) {
    switch (env) {
    case demoWindows: return 5432;
    case demoLinux: return 5432;
    case Ini: return 5432;

    default: return 0;
    }
}

QString DbConfig::username(Env env) {
    if (env == demoWindows) return "postgres";
    if (env == demoLinux) return "postgres";
    if (env == Ini) return "stage_user"; // кошмар

    return "";
}

QString DbConfig::password(Env env) {
    if (env == demoWindows) return "0";
    if (env == demoLinux) return "postgres1";
    if (env == Ini) return "stage_pass";

    return "";
}
