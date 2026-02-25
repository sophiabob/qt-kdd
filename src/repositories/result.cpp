#include "result.h"

//Result::Result() {}


Result<int> UserRepository::insert(const User& user)
{
    // Проверка уникальности
    if (loginExists(user.login())) {
        return Result<int>::failure(
            ErrorInfo::validation("login", "Логин уже занят"));
    }

    QSqlQuery q(m_db);
    q.prepare("INSERT INTO users (...) VALUES (...)");
    user.bindToQuery(q);

    if (!q.exec()) {
        return Result<int>::failure(
            ErrorInfo::database("Ошибка БД: " + q.lastError().text()));
    }

    return Result<int>::success(q.lastInsertId().toInt());
}
