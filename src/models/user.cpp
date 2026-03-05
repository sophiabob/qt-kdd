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
        int meshId,
        int startDoz,
        int finishDoz
    )
    : QObject(nullptr)
    , m_id(id)
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

    , m_startDoz(startDoz)
    , m_finishDoz(finishDoz)
{
    // Тело конструктора (если нужна дополнительная логика)
    // Например, автоматическая нормализация логина:
    m_login = m_login.trimmed().toLower();

}

// В начале user.cpp:
User::User(QObject *parent) : QObject(parent)
{
    // Инициализация полей, если нужна
}

QMap<QString, QString> User::dbColumnMap()
{
    static const QMap<QString, QString> map {
        {"id", "user_id"}, {"login", "login"}, {"passwordHash", "password"},
        {"firstName", "name_0"}, {"surname", "name_1"}, {"patronymic", "name_2"},
        {"gender", "gender"}, {"birthDate", "birthday"}, {"snils", "snils"},
        {"employeeNumber", "tab_num"}, {"role", "role"}, {"department", "department"},
        {"cardId", "card_id"}, {"accessCode", "code"}, {"blockReason", "block"},
        {"setId", "set_id"}, {"kasId", "kas_id"}, {"meshId", "mesh_id"},
        {"dosimetrTldId", "doz_tld_id"}, {"lastCellUpdate", "cell_date"},
        {"annualDose", "dose_year"}, {"currentYearDose", "dose_year_now"},
        {"currentYearDosePPD", "dose_year_now_ppd"}, {"startDoz", "start_doz"},
        {"finishDoz", "finish_doz"}, {"lastUpdate", "last_update"}
    };
    return map;
}

inline QString User::clamp(const QString& str, int maxLen) const {
    return str.length() > maxLen ? str.left(maxLen) : str;
}

void User::bindToQueryUser(QSqlQuery& query) const
{
   // query.bindValue(":user_id", m_id); //убрать
    query.bindValue(":login", clamp(m_login, 30));
    query.bindValue(":password", clamp(m_passwordHash, 30));
    query.bindValue(":name_0", clamp(m_surname, 30));
    query.bindValue(":name_1", clamp(m_firstName, 30));
    query.bindValue(":name_2", clamp(m_patronymic, 30));
    query.bindValue(":snils", m_snils);  // Числа не обрезаем
    query.bindValue(":gender", clamp(m_gender, 3));
    query.bindValue(":birthday", m_birthDate);  // QDate не обрезаем
    query.bindValue(":role", clamp(m_role, 30));
    query.bindValue(":tab_num", m_employeeNumber);
    query.bindValue(":department", clamp(m_department, 30));
    query.bindValue(":card_id", clamp(m_cardId, 30));
    query.bindValue(":set_ID", m_setId);
    query.bindValue(":kas_ID", m_kasId);
    query.bindValue(":mesh_ID", m_meshId);
    query.bindValue(":doz_tld_id", m_dosimetrTldId);
    query.bindValue(":cell_date", m_lastCellUpdate);
    query.bindValue(":dose_year", m_annualDose);
    query.bindValue(":dose_year_now", m_currentYearDose);
    query.bindValue(":dose_year_now_ppd", m_currentYearDosePPD);
    query.bindValue(":code", clamp(m_accessCode, 30));
    query.bindValue(":block", clamp(m_blockReason, 30));
    query.bindValue(":last_update", m_lastUpdate);
}
std::unique_ptr<User> User::fromQuery(const QSqlQuery& row)
{
    auto user = std::make_unique<User>();
    const auto& map = dbColumnMap();

    for (auto it = map.begin(); it != map.end(); ++it) {
        const QString& fieldName = it.key();   // "login", "surname"...
        const QString& dbCol = it.value();     // "login", "name_1"...

        // Пропускаем, если колонки нет в результате запроса
        if (!row.record().contains(dbCol)) continue;

        QVariant value = row.value(dbCol);

        // === Dispatch по имени поля (типобезопасно) ===
        if (fieldName == "id") {
            user->m_id = value.toInt();
        }
        else if (fieldName == "login") {
            user->setLogin(value.toString());
        }
        else if (fieldName == "passwordHash") {
            user->setPasswordHash(value.toString());  // ⚠️ Хэш, не пароль!
        }
        else if (fieldName == "firstName") {
            user->setFirstName(value.toString());
        }
        else if (fieldName == "surname") {
            user->setSurname(value.toString());
        }
        else if (fieldName == "patronymic") {
            user->setPatronymic(value.toString());
        }
        else if (fieldName == "gender") {
            user->setGender(value.toString());
        }
        else if (fieldName == "birthDate") {
            user->setBirthDate(value.toDateTime());
        }
        else if (fieldName == "snils") {
            user->setSnils(value.toInt());
        }
        else if (fieldName == "employeeNumber") {
            user->setEmployeeNumber(value.toInt());
        }
        else if (fieldName == "role") {
            user->setRole(value.toString());
        }
        else if (fieldName == "department") {
            user->setDepartment(value.toString());
        }
        else if (fieldName == "cardId") {
            user->setCardId(value.toString());
        }
        else if (fieldName == "accessCode") {
            user->setAccessCode(value.toString());
        }
        else if (fieldName == "blockReason") {
            user->setBlockReason(value.toString());
        }
        else if (fieldName == "setId") {
            user->setSetId(value.toInt());
        }
        else if (fieldName == "kasId") {
            user->setKasId(value.toInt());
        }
        else if (fieldName == "meshId") {
            user->setMeshId(value.toInt());
        }
        else if (fieldName == "dosimetrTldId") {
            user->setDosimetrTldId(value.toInt());
        }
        else if (fieldName == "lastCellUpdate") {
            user->setLastCellUpdate(value.toDateTime());
        }
        else if (fieldName == "annualDose") {
            user->setAnnualDose(value.toFloat());
        }
        else if (fieldName == "currentYearDose") {
            user->setCurrentYearDose(value.toFloat());
        }
        else if (fieldName == "currentYearDosePPD") {
            user->setCurrentYearDosePPD(value.toFloat());
        }
        else if (fieldName == "startDoz") {
            user->setStartDoz(value.toInt());
        }
        else if (fieldName == "finishDoz") {
            user->setFinishDoz(value.toInt());
        }
        else if (fieldName == "lastUpdate") {
            user->setLastUpdate(value.toDateTime());
        }
        // else: неизвестное поле — игнорируем
    }

    return user;
}
