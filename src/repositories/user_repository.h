#pragma once
#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <QString>
#include <QVariant>
#include <optional>
#include <QMap>

#include "../models/user.h"
#include "../repositories/result.h"

class Welcome;

#include "../ui/welcome.h"

#include <QApplication>
#include "../core/db_config.h"
#include "../core/database_manager.h"

#include "database_helper.h"

/*
// === Результат операции с возможностью ошибки ===
template<typename T>
struct RepoResult {
    bool success;
    T data;
    QString errorMessage;

    static RepoResult<T> ok(T value) { return {true, value, {}}; }
    static RepoResult<T> err(const QString& msg) { return {false, T{}, msg}; }
};*/



class UserRepository
{
public:
    explicit UserRepository(QSqlDatabase db);

    User* mapQueryToUser(const QSqlQuery& query) const;
    bool loginExists(const QString& login);
    Result<int> createUser(const User& user); // Create → возвращает ID




private:
    QSqlDatabase m_db;
   /* QSqlDatabase m_db() const {
        return DatabaseManager::instance().connection();
    }*/

    void logDatabaseError(const QString& context, const QSqlError& error) const;

    void logDatabaseError(const QString& context, const QSqlError& error);

    //изменить расположение

    //Ui::MainWindow *ui; // указатель на сгенерированный UI
    Welcome *welcomeWindow = nullptr; // окно приветствия (может быть nullptr)

};





#endif // USER_REPOSITORY_H
