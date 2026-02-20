#include "user_repository.h"
//вывод пользователя по логину

//авторизация пользователя
//список в формате фио - логин
//список в формате id

UserRepository::UserRepository() {}




// ✅ ХОРОШО: Валидация в репозитории (при загрузке из БД)
User UserRepository::findById(int id) {
    QSqlQuery q(...);
    // ... выполнение запроса ...

    QDateTime birthDate = q.value("birth_date").toDateTime();

    if (!birthDate.isValid()) {
        qWarning() << "Invalid birth_date for user" << id;
        // Решаем: пропустить запись, поставить дефолт, или вернуть пустой User
    }

    return User(..., birthDate, ...);
}



/*
User u(1, "admin", QDate(1990, 1, 1));
// u.setBirthDate(...);  // ❌ Нет такого метода — дата рождения неизменна

u.touch();  // ✅ Обновляем lastModified при любом изменении
*/
