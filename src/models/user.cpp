#include "user.h"
#include <memory>

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

QMap<QString, QString> User::dbColumnMap() {
    return {
        {"login", "login"}, {"passwordHash", "password"},
        {"surname", "name_0"}, {"firstName", "name_1"}, {"patronymic", "name_2"},
        {"snils", "snils"}, {"gender", "gender"}, {"birthDate", "birthday"},
        {"role", "role"}, {"employeeNumber", "tab_num"},
        {"department", "department"}, {"cardId", "card_id"},
        {"dosimetrTldId", "doz_tld_id"}, {"startDoz", "start_doz"},
        {"finishDoz", "finish_doz"}, {"annualDose", "dose_year"},
        {"currentYearDose", "dose_year_now"}, {"currentYearDosePPD", "dose_year_now_ppd"},
        {"lastCellUpdate", "cell_date"}, {"accessCode", "code"},
        {"blockReason", "block"}, {"lastUpdate", "last_update"},
        {"setId", "set_ID"}, {"kasId", "kas_ID"}, {"meshId", "mesh_ID"}
    };
}


void User::bindToQuery(QSqlQuery& query) const
{
    const QMetaObject* meta = this->metaObject();
    const auto& map = dbColumnMap();

    for (int i = meta->propertyOffset(); i < meta->propertyCount(); ++i) {
        QMetaProperty prop = meta->property(i);
        QString dbCol = map.value(prop.name(), prop.name());  // Имя колонки
        query.bindValue(":" + dbCol, prop.read(this));
    }
}
std::unique_ptr<User> User::fromQuery(const QSqlQuery& row)
{
    auto user = std::make_unique<User>(nullptr);  // ← Явный конструктор
    const QMetaObject* meta = user->metaObject();
    const auto& map = dbColumnMap();

    for (int i = meta->propertyOffset(); i < meta->propertyCount(); ++i) {
        QMetaProperty prop = meta->property(i);
        QString dbCol = map.value(prop.name(), prop.name());

        if (row.record().contains(dbCol)) {  // ← record().contains()
            prop.write(user.get(), row.value(dbCol));
        }
    }
    return user;  // ← Авто-move, память удалится сама
}
