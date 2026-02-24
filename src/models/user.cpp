#include "user.h"

User::User(
        int id,
        const QString& firstName,
        const QString& surname,
        const QString& patronymic,
        const QString& login,
        const QString& passwordHash,
        int snils,
        const QString& accessCode,
        int employeeNumber,
        const QString& cardId,
        int dosimetrTldId,
        float annualDose,
        float currentYearDose,
        float currentYearDosePPD,
        const QDateTime& birthDate,
        const QDateTime& lastUpdate,
        const QString& role,
        const QString& gender,
        const QString& department,
        const QString& blockReason,
        int setId,
        int kasId,
        int meshId
    )
    : m_id(id)
    , m_firstName(firstName)
    , m_surname(surname)
    , m_patronymic(patronymic)
    , m_login(login)
    , m_passwordHash(passwordHash)
    , m_snils(snils)
    , m_accessCode(accessCode)
    , m_employeeNumber(employeeNumber)
    , m_cardId(cardId)
    , m_dosimetrTldId(dosimetrTldId)
    , m_annualDose(annualDose)
    , m_currentYearDose(currentYearDose)
    , m_currentYearDosePPD(currentYearDosePPD)
    , m_birthDate(birthDate) //проверить на валидность
    , m_lastUpdate(lastUpdate.isValid() ? lastUpdate : QDateTime::currentDateTime())
    , m_role(role)
    , m_gender(gender)
    , m_department(department)
    , m_blockReason(blockReason)
    , m_setId(setId)
    , m_kasId(kasId)
    , m_meshId(meshId)
{
    // Тело конструктора (если нужна дополнительная логика)
    // Например, автоматическая нормализация логина:
    m_login = m_login.trimmed().toLower();
}

void User::bindToQuery(QSqlQuery& query) const {
    const QMetaObject* meta = this->metaObject();  // 1. Получаем мета-информацию о классе

    // 2. Начинаем с propertyOffset(), чтобы пропустить свойства QObject (objectName, etc.)
    for (int i = meta->propertyOffset(); i < meta->propertyCount(); ++i) {
        QMetaProperty prop = meta->property(i);  // 3. Берём очередное свойство

        // 4. Читаем кастомный атрибут USER (имя колонки в БД)
        const char* dbColumn = prop.userType();

        // 5. Если атрибут задан — привязываем значение к запросу
        if (dbColumn && *dbColumn) {
            // prop.read(this) вызывает геттер и возвращает QVariant
            // query.bindValue(":login", QVariant("admin"))
            query.bindValue(":" + QString(dbColumn), prop.read(this));
        }
    }
}

User User::fromQuery(const QSqlQuery& row) {
    User user;  // 1. Создаём пустой объект
    const QMetaObject* meta = user.metaObject();

    for (int i = meta->propertyOffset(); i < meta->propertyCount(); ++i) {
        QMetaProperty prop = meta->property(i);
        const char* dbColumn = prop.userType();

        // 2. Если есть привязка к БД И в результате запроса есть такая колонка
        if (dbColumn && *dbColumn && row.contains(dbColumn)) {
            // 3. prop.write() вызывает сеттер, передавая значение из БД
            // user.setEmail("a@b.com")
            prop.write(&user, row.value(dbColumn));
        }
    }
    return user;
}
