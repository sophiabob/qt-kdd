#include "user_repository.h"

#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>
#include <QRegularExpression>




// вывод пользователя по логину

// авторизация пользователя
// список в формате фио - логин
// список в формате id - фио
// создания нового пользователя
// изменение старого пользователя
// проверка валидности переменных

UserRepository::UserRepository(QSqlDatabase db)
    : m_db(db)
{
    // Проверка, что БД открыта
    if (!m_db.isOpen()) {
        qWarning() << "UserRepository: database is not open!";
    }
}

// обработчик ошибок
void UserRepository::logDatabaseError(const QString& context, const QSqlError& error)
{

#ifdef QT_DEBUG
    qDebug() << "[Repository]" << context << ":" << error.text();
#else
    /*// 2. Пишем в файл логов (для продакшена)
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs/db.log";
    QFile f(logPath);
    if (f.open(QIODevice::Append)) {
        QTextStream(&f) << QDateTime::currentDateTime().toString(Qt::ISODate)
                        << " [" << context << "] " << error.text() << "\n";
    }*/
#endif
}



Result<bool> UserRepository::validation(const User& user)
{
    if (loginExists(user.login()))
        return Result<bool>::err(ErrorInfo::validation("login", "Логин уже занят"));
    \
    if (tabNumExists(user.employeeNumber()))
        return Result<bool>::err(ErrorInfo::validation("tabNum", "Табельный номер уже занят"));

    if (cardIdExists(user.cardId()))
        return Result<bool>::err(ErrorInfo::validation("cardId", "Идентификатор карты уже зарегистрирован"));

    if (codeExists(user.accessCode()))
        return Result<bool>::err(ErrorInfo::validation("code", "Код пропуска уже занят"));

    return Result<bool>::ok(true);
}

// === Проверки уникальности в БД ===
bool UserRepository::loginExists(const QString& login)
{
    //if (login.isEmpty()) return false;
    return withDbConnection<bool>([&](QSqlDatabase& db) -> bool {
        QSqlQuery q(db);  // ← Используем гарантированно открытый db
        q.prepare("SELECT COUNT(*) FROM users WHERE login = :login");
        q.bindValue(":login", login);

        if (!q.exec()) {
            qCritical() << "Query failed:" << q.lastError().text();
            return false;
        }
        return q.next() && q.value(0).toInt() > 0;
    });
}

bool UserRepository::tabNumExists(int tabNum)
{
    //if (tabNum <= 0) return false;
    return withDbConnection<bool>([&](QSqlDatabase& db) -> bool {
        QSqlQuery q(db);
        q.prepare("SELECT COUNT(*) FROM users WHERE tab_num = :tab_num");
        q.bindValue(":tab_num", tabNum);
        return q.exec() && q.next() && q.value(0).toInt() > 0;
    });
}

bool UserRepository::cardIdExists(const QString& cardId)
{
    //if (cardId.isEmpty()) return false;
    return withDbConnection<bool>([&](QSqlDatabase& db) -> bool {
        QSqlQuery q(db);
        q.prepare("SELECT COUNT(*) FROM users WHERE card_id = :card_id");
        q.bindValue(":card_id", cardId);
        return q.exec() && q.next() && q.value(0).toInt() > 0;
    });
}

bool UserRepository::codeExists(const QString& code)
{
    //if (code.isEmpty()) return false;
    return withDbConnection<bool>([&](QSqlDatabase& db) -> bool {
        QSqlQuery q(db);
        q.prepare("SELECT COUNT(*) FROM users WHERE code = :code");
        q.bindValue(":code", code);
        return q.exec() && q.next() && q.value(0).toInt() > 0;
    });
}



//создать пользователя
Result<int> UserRepository::createUser(const User& user)
{
    return withDbConnection<Result<int>>([&](QSqlDatabase& db) -> Result<int> {
        try {
            QSqlQuery(db).exec("SET client_encoding TO 'UTF8'");
            QSqlQuery query(db);

            auto validateResult = validation(user);
            if (!validateResult.isOk){
                qCritical() << "[validateResult ERROR]:" << validateResult.error.message;
                //return Result<int>::err(validateResult.error);
                throw std::runtime_error(validateResult.error.message.toStdString());
            }

            if (!query.prepare(R"(
                INSERT INTO users (login, password, name_0, name_1, name_2, snils, gender, birthday,
                    role, tab_num, department, card_id, set_ID, kas_ID, mesh_ID,
                    doz_tld_id, cell_date, dose_year, dose_year_now, dose_year_now_ppd,
                    code, block, last_update)
                VALUES (:login, :password, :name_0, :name_1, :name_2, :snils, :gender, :birthday,
                    :role, :tab_num, :department, :card_id, :set_ID, :kas_ID, :mesh_ID,
                    :doz_tld_id, :cell_date, :dose_year, :dose_year_now, :dose_year_now_ppd,
                    :code, :block, :last_update)
            )")) {
                throw std::runtime_error("Prepare failed: " + query.lastError().text().toStdString());
            }

            user.bindToQueryUser(query);  // Привязка данных

            if (!query.exec()) {//проверка на уникальность
                throw std::runtime_error("Exec failed: " + query.lastError().text().toStdString());
            }

            int newId = query.lastInsertId().toInt();
            return Result<int>::ok(newId);
        }
        catch (const std::exception& e) {
            qCritical() << "[DB ERROR]:" << e.what();
            return Result<int>::err(ErrorInfo::database(QString::fromStdString(e.what())));
        }
    });
}


// если fromQuery не работает
User* UserRepository::mapQueryToUser(const QSqlQuery& query) const
{
    User* user(nullptr);
    // Используем friend-доступ к private полям
    user -> m_id = query.value("user_id").toInt();
    user -> m_login = query.value("login").toString();
    user -> m_passwordHash = query.value("password").toString();  // Осторожно: не передавай в UI!
    user -> m_firstName = query.value("name_0").toString();
    user -> m_surname = query.value("name_1").toString();
    user -> m_patronymic = query.value("name_2").toString();
    user -> m_snils = query.value("snils").toInt();
    user -> m_gender = query.value("gender").toString();
    user -> m_birthDate = query.value("birthday").toDateTime();
    user -> m_role = query.value("role").toString();
    user -> m_employeeNumber = query.value("tab_num").toInt();
    user -> m_department = query.value("department").toString();
    user -> m_cardId = query.value("card_id").toString();
    user -> m_setId = query.value("set_ID").toInt();
    user -> m_kasId = query.value("kas_ID").toInt();
    user -> m_meshId = query.value("mesh_ID").toInt();
    user -> m_dosimetrTldId = query.value("doz_tld_id").toInt();
    user -> m_lastCellUpdate = query.value("cell_date").toDateTime();
    user -> m_annualDose = query.value("dose_year").toFloat();
    user -> m_currentYearDose = query.value("dose_year_now").toFloat();
    user -> m_currentYearDosePPD = query.value("dose_year_now_ppd").toFloat();
    user -> m_accessCode = query.value("code").toString();
    user -> m_blockReason = query.value("block").toString();
    user -> m_lastUpdate = query.value("last_update").toDateTime();


    //query.clear();
    //delete user; / обязательно при использовании
    return user;
}





/*
//обновление пользователя - доработать
bool UserRepository::update(const User& user)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE users SET login = :login, email = :email, cell_date = :cell_date WHERE id = :id");

    user.bindToQuery(q);

    // Ручная привязка для WHERE (id не в Q_PROPERTY с USER, или просто явно)
    q.bindValue(":id", user.id());

    if (!q.exec()) {
        logDatabaseError("UserRepository::update", q.lastError());
        return false;
    }
    return true;
}


//вывод пользователя по логину - доработаь запрос
bool UserRepository::readUserByLogin(const User& user, QString login)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT login, email, cell_date FROM users WHERE id = :id");
    q.bindValue(":login", login);
    q.exec();

    if (q.next()) {
        User loadedUser = User::fromQuery(q);  // loadedUser.surname() == "Петров"
    }


    //закрыть бд
    //query.close();
    return q.exec();
}

// Существует ли логин
bool UserRepository::loginExists(const QString& login)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT COUNT(*) FROM users WHERE login = :login");
    q.bindValue(":login", login);

    if (q.exec() && q.next()) {
        return q.value(0).toInt() > 0;
    }
    return false;
}




//==================================================================================


// === READ: Поиск по логину ===
std::optional<User> UserRepository::readUserByLogin(const QString& login)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM users WHERE login = :login LIMIT 1");
    q.bindValue(":login", login);

    if (!q.exec()) {
        logDatabaseError("UserRepository::readUserByLogin", q.lastError());
        return std::nullopt;
    }

    if (q.next()) {
        // Вариант А: Авто-маппинг через fromQuery (если настроен в User)
        return User::fromQuery(q);

        // Вариант Б: Ручной маппинг (если авто не работает)
        // return mapQueryToUser(q);
    }

    return std::nullopt;  // Не найдено
}

// === READ: Поиск по ID ===
std::optional<User> UserRepository::readUserById(int id)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM users WHERE user_id = :id LIMIT 1");
    q.bindValue(":id", id);

    if (!q.exec()) {
        logDatabaseError("UserRepository::readUserById", q.lastError());
        return std::nullopt;
    }

    if (q.next()) {
        return User::fromQuery(q);
    }
    return std::nullopt;
}

// === READ: Все пользователи (с пагинацией!) ===
RepoResult<QVector<User>> UserRepository::readAllUsers(int limit, int offset)
{
    QVector<User> users;
    QSqlQuery q(m_db);

    // LIMIT/OFFSET для экономии памяти при больших таблицах
    q.prepare("SELECT * FROM users ORDER BY name_0, name_1 LIMIT :limit OFFSET :offset");
    q.bindValue(":limit", limit);
    q.bindValue(":offset", offset);

    if (!q.exec()) {
        logDatabaseError("UserRepository::readAllUsers", q.lastError());
        return RepoResult<QVector<User>>::err(q.lastError().text());
    }

    while (q.next()) {
        users.append(User::fromQuery(q));
    }

    return RepoResult<QVector<User>>::ok(users);
}

// === UPDATE: Обновление пользователя ===
bool UserRepository::updateUser(const User& user)
{
    QSqlQuery q(m_db);

    // Важно: перечисляем только те поля, которые разрешено менять
    q.prepare(R"(
        UPDATE users SET
            login = :login,
            password = :password,
            name_0 = :name_0,
            name_1 = :name_1,
            name_2 = :name_2,
            snils = :snils,
            gender = :gender,
            birthday = :birthday,
            role = :role,
            tab_num = :tab_num,
            department = :department,
            card_id = :card_id,
            set_ID = :set_ID,
            kas_ID = :kas_ID,
            mesh_ID = :mesh_ID,
            doz_tld_id = :doz_tld_id,
            cell_date = :cell_date,
            dose_year = :dose_year,
            dose_year_now = :dose_year_now,
            dose_year_now_ppd = :dose_year_now_ppd,
            code = :code,
            block = :block,
            last_update = :last_update
        WHERE user_id = :user_id
    )");

    // Авто-привязка всех полей
    user.bindToQuery(q);

    // Явная привязка ID для WHERE (если он не в Q_PROPERTY с USER)
    q.bindValue(":user_id", user.id());

    if (!q.exec()) {
        logDatabaseError("UserRepository::updateUser", q.lastError());
        return false;
    }

    return true;
}

// === DELETE: Удаление (мягкое — лучше не удалять физически) ===
bool UserRepository::deleteUser(int id)
{
    // Вариант А: Физическое удаление

    QSqlQuery q(m_db);
    q.prepare("DELETE FROM users WHERE user_id = :id");
    q.bindValue(":id", id);
    return q.exec();




    // Вариант Б: Мягкое удаление (рекомендую!)
    QSqlQuery q(m_db);
    q.prepare("UPDATE users SET block = 'deleted', last_update = NOW() WHERE user_id = :id");
    q.bindValue(":id", id);

    if (!q.exec()) {
        logDatabaseError("UserRepository::deleteUser", q.lastError());
        return false;
    }
    return true;
}

// === AUTH: Авторизация ===
std::optional<User> UserRepository::authenticate(const QString& login, const QString& passwordHash)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM users WHERE login = :login AND password = :pwd AND block IS NULL LIMIT 1");
    q.bindValue(":login", login);
    q.bindValue(":pwd", passwordHash);

    if (!q.exec()) {
        logDatabaseError("UserRepository::authenticate", q.lastError());
        return std::nullopt;
    }

    if (q.next()) {
        User user = User::fromQuery(q);
        // Не возвращаем хэш пароля обратно в UI!
        return user;
    }

    return std::nullopt;  // Неверный логин/пароль или пользователь заблокирован
}*/




//написать мэп(словарь) для виджетов пользователя


//проверка валидности перемннных
/*

// === READ (by Login) ===
User UserRepository::findByLogin(const QString& login)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM users WHERE login = :login LIMIT 1");
    q.bindValue(":login", login);

    if (q.exec() && q.next()) {
        return mapRow(q);
    }
    return User();
}







/*
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


//----------------------------------------------------------------------------------------------------------------
// копия юез изменений mainwindow - FROM users - 36

/*
// Сначала привязываем значение
query.prepare("SELECT name_0, name_1, name_2 FROM users WHERE user_id = :user_id");
query.bindValue(":user_id", currentUserId);

if (query.exec()) {
    if (query.next()) {
        QString name0 = query.value("name_0").toString();
        QString name1 = query.value("name_1").toString();
        QString name2 = query.value("name_2").toString();

        QString userId_text = name0 + " " + name1 + " " + name2;
        ui->label_userId->setText(userId_text);

        checkRole();
    } else {
        //qDebug() << "Пользователь с ID" << currentUserId << "не найден";
        ui->label_userId->setText("Пользователь не найден");
    }
} else {
    //qDebug() << "Ошибка выполнения запроса1:" << query.lastError().text();
    ui->label_userId->setText("Ошибка загрузки данных currentUserId");
}

*/

/*
//на будущее - для большого количества пользователей

// repositories/user_repository.cpp
Result<QVector<User>> UserRepository::findFiltered(
    const UserFilter& filter,
    const Pagination& pagination)
{
    QVector<User> users;
    QSqlQuery q(m_db);

    // Динамический WHERE
    QStringList conditions;
    if (!filter.loginContains.isEmpty()) {
        conditions << "login LIKE :login";
        q.bindValue(":login", "%" + filter.loginContains + "%");
    }
    if (!filter.emailContains.isEmpty()) {
        conditions << "email LIKE :email";
        q.bindValue(":email", "%" + filter.emailContains + "%");
    }
    // ... другие фильтры

    QString where = conditions.isEmpty() ? "" : "WHERE " + conditions.join(" AND ");

    // Запрос с LIMIT/OFFSET
    QString sql = QString(
                      "SELECT id, login, email, created_at FROM users %1 "
                      "ORDER BY created_at DESC LIMIT :limit OFFSET :offset"
                      ).arg(where);

    q.prepare(sql);
    q.bindValue(":limit", pagination.limit());
    q.bindValue(":offset", pagination.offset());

    if (!q.exec()) {
        return Result<QVector<User>>::failure(ErrorInfo::database(q.lastError().text()));
    }

    while (q.next()) {
        users.append(User::fromQuery(q));  // Используем рефлексию!
    }

    return Result<QVector<User>>::success(users);
}*/


