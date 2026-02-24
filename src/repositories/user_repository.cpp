#include "user_repository.h"

#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QSqlError>
#include <QString>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>


// вывод пользователя по логину

// авторизация пользователя
// список в формате фио - логин
// список в формате id - фио
// создания нового пользователя
// изменение старого пользователя
// проверка валидности переменных

UserRepository::UserRepository() {}

//обработчик ошибок БД - разобрать
void UserRepository::logDatabaseError(const QString& context, const QSqlError& error)
{

#ifdef QT_DEBUG
    qDebug() << "[DB ERROR]" << context << ":" << error.text();
#endif

    /*// 2. Пишем в файл логов (для продакшена)
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                      + "/logs/app.log";
    QFile logFile(logPath);
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << QDateTime::currentDateTime().toString(Qt::ISODate)
            << " [ERROR] " << context << ": " << error.text() << "\n";
        logFile.close();
    }*/
}


//обновление времени на текущее для любой операции
bool UserRepository::updateDateTime()
{
    lastUpdate = QDateTime::currentDateTime();
}

//создать пользователя - подправить, но в целом должно работать
bool UserRepository::createUser(const User& user)
{
    QSqlQuery query;

    int user_id = 1; // Значение по умолчанию, если таблица пуста
    if (query.exec("SELECT MAX(user_id) AS max_id FROM users;")) {
        if (query.next()) {
            user_id = query.value("max_id").toInt() + 1;
        }
    } else {
        qDebug() << "Ошибка выполнения запроса: не удаётся user_id++ для записи в БД \n" << query.lastError().text();
    }

    updateDateTime();


    QSqlQuery q(m_db);
    query.prepare("INSERT INTO users(user_id, login, password, name_0, name_1, name_2, snils, gender, birthday, role, tab_num, department, card_id, set_ID, kas_ID, mesh_ID, doz_tld_id, cell_date, dose_year, dose_year_now, dose_year_now_ppd, code, block, last_update) VALUES (:user_id, :login, :password, :name_0, :name_1, :name_2, :snils, :gender, :birthday, :role, :tab_num, :department, :card_id, :set_ID, :kas_ID, :mesh_ID, :doz_tld_id, :cell_date, :dose_year, :dose_year_now, :dose_year_now_ppd, :code, :block, :last_update)");
    user.bindToQuery(q); //query.bindValue(":login", QVariant("admin")) и тд


    if (!q.exec()) {
        logDatabaseError("UserRepository::createUser", q.lastError());

        return false;
    }


    //закрыть бд
    query.close();
    delete(query);
    return q.exec();
}


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
    query.close();
    delete(query);
    return q.exec();
}




//==================================================================================

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


