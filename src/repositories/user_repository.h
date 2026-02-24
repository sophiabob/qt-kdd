#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

class UserRepository {
public:

};


/*
//на будущее - для большого количества пользователей в системе
struct UserFilter {
    QString loginContains;
    QString emailContains;
    QString role;
    QDateTime createdAfter;
    // ... другие фильтры
};

struct Pagination {
    int page = 1;           // Страница (1-based)
    int pageSize = 50;      // Записей на страницу

    int offset() const { return (page - 1) * pageSize; }
    int limit() const { return pageSize; }
};

class UserRepository {
public:
    // Загрузка с фильтрами и пагинацией
    Result<QVector<User>> findFiltered(
        const UserFilter& filter = {},
        const Pagination& pagination = {});

    // Получить общее количество (для пагинатора в UI)
    Result<int> countFiltered(const UserFilter& filter = {});
};*/


#endif // USER_REPOSITORY_H
