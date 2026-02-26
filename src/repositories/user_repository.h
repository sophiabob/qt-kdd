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
#include "../models/user.h"

#include "../repositories/result.h"

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

    User mapQueryToUser(const QSqlQuery& query) const;
    bool loginExists(const QString& login);
    RepoResult<int> createUser(const User& user); // Create → возвращает ID
    void UserRepository::updateDateTime(); // есть вопросики


private:
    QSqlDatabase m_db;

    void logDatabaseError(const QString& context, const QSqlError& error) const;
};


#endif // USER_REPOSITORY_H
