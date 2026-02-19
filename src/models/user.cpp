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
