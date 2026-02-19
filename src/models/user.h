#ifndef USER_H //#pragma once - современная альтернатива
#define USER_H

#include <QMainWindow>

#include <QString>
#include <QDateTime>
#include <QMetaType> //регистрация типа

class User
{
public:
    User() = default;
    User(
        int id = -1,
        const QString& firstName = QString(),
        const QString& surname = QString(),
        const QString& patronymic = QString(),
        const QString& login = QString(),
        const QString& passwordHash = QString(),
        int snils = 0,
        const QString& accessCode = QString(),
        int employeeNumber = 0,
        const QString& cardId = QString(),
        int dosimetrTldId = -1,
        float annualDose = 0.0f,
        float currentYearDose = 0.0f,
        float currentYearDosePPD = 0.0f,
        const QDateTime& birthDate = QDateTime(),
        const QDateTime& lastUpdate = QDateTime::currentDateTime(),
        const QString& role = QString(),
        const QString& gender = QString(),
        const QString& department = QString(),
        const QString& blockReason = QString(),
        int setId = -1,
        int kasId = -1,
        int meshId = -1
    );



    //геттеры - интерфейс

    int id() const { return m_id; }

    const QString& login() const { return m_login; }
    const QString& email() const { return m_email; }

    const QString& firstName() const { return m_firstName; }
    const QString& surname() const { return m_surname; }
    const QString& patronymic() const { return m_patronymic; }

    const QString& gender() const { return m_gender; }
    QDateTime birthDate() const { return m_birthDate; }
    int snils() const { return m_snils; }
    int employeeNumber() const { return m_employeeNumber; }

    const QString& role() const { return m_role; }
    const QString& department() const { return m_department; }
    const QString& cardId() const { return m_cardId; }
    const QString& accessCode() const { return m_accessCode; }
    const QString& blockReason() const { return m_blockReason; }

    int setId() const { return m_setId; }
    int kasId() const { return m_kasId; }
    int meshId() const { return m_meshId; }
    int dosimetrTldId() const { return m_dosimetrTldId; }

    float annualDose() const { return m_annualDose; }
    float currentYearDose() const { return m_currentYearDose; }
    float currentYearDosePPD() const { return m_currentYearDosePPD; }

    QDateTime lastUpdate() const { return m_lastUpdate; }



    //сеттеры - БД

    void setLogin(const QString& login) { m_login = login; }
    void setEmail(const QString& email) { m_email = email; }

    void setFirstName(const QString& firstName) { m_firstName = firstName; }
    void setSurname(const QString& surname) { m_surname = surname; }
    void setPatronymic(const QString& patronymic) { m_patronymic = patronymic; }

    void setGender(const QString& gender) { m_gender = gender; }
    void setBirthDate(const QDateTime& birthDate) { m_birthDate = birthDate; }
    void setSnils(int snils) { m_snils = snils; }
    void setEmployeeNumber(int employeeNumber) { m_employeeNumber = employeeNumber; }

    void setRole(const QString& role) { m_role = role; }
    void setDepartment(const QString& department) { m_department = department; }
    void setCardId(const QString& cardId) { m_cardId = cardId; }
    void setAccessCode(const QString& accessCode) { m_accessCode = accessCode; }
    void setBlockReason(const QString& blockReason) { m_blockReason = blockReason; }

    void setSetId(int setId) { m_setId = setId; }
    void setKasId(int kasId) { m_kasId = kasId; }
    void setMeshId(int meshId) { m_meshId = meshId; }
    void setDosimetrTldId(int dosimetrTldId) { m_dosimetrTldId = dosimetrTldId; }

    void setAnnualDose(float annualDose) { m_annualDose = annualDose; }
    void setCurrentYearDose(float currentYearDose) { m_currentYearDose = currentYearDose; }
    void setCurrentYearDosePPD(float currentYearDosePPD) { m_currentYearDosePPD = currentYearDosePPD; }



    //вспомогательные методы

    void touch() { m_lastUpdate = QDateTime::currentDateTime(); }

    bool isValid() const {
        return m_id > 0 && !m_login.isEmpty();
    }

    QString fullName() const { // полное ФИО одной строкой, если не заполнено - логин
        if (m_surname.isEmpty() && m_firstName.isEmpty() && m_patronymic.isEmpty()) return m_login;
        QString result = m_surname;
        if (!m_firstName.isEmpty()) result += " " + m_firstName;
        if (!m_patronymic.isEmpty()) result += " " + m_patronymic;
        return result.trimmed();
    }

    QString shortName() const { // короткое ФИО: "Иванов И.И." (логин)
        if (m_surname.isEmpty()) return m_login;
        QString initials;
        if (!m_firstName.isEmpty()) initials += m_firstName[0] + ".";
        if (!m_patronymic.isEmpty()) initials += m_patronymic[0] + ".";
        return m_surname + (initials.isEmpty() ? "" : " " + initials);
    }



private:
    Q_DISABLE_COPY(User) //защита от случайного копирования

    // --- Основная информация ---
    int m_id;                    // Уникальный ID записи
    QString m_login;             // Логин для входа
    QString m_passwordHash;      // Хэш пароля (не храните пароли в открытом виде!)

    // --- ФИО (FullName) ---
    QString m_surname;           // Фамилия (было name_0)
    QString m_firstName;         // Имя (было name_1)
    QString m_patronymic;        // Отчество (было name_2)

    // --- Личные данные ---
    QString m_gender;            // Пол
    QDateTime m_birthDate;       // Дата рождения
    int m_snils;                 // СНИЛС
    int m_employeeNumber;        // Табельный номер (было tab_num)

    // --- Работа и доступы ---
    QString m_role;              // Роль (Admin, User, etc.)
    QString m_department;        // Отдел/Подразделение
    QString m_cardId;            // ID карты доступа
    QString m_accessCode;        // Код доступа (было code)
    QString m_blockReason;       // причина болкировки

    // --- Технические ID (внешние системы) ---
    int m_setId;                 // ID набора данных
    int m_kasId;                 // ID системы КАС
    int m_meshId;                // ID сетки
    int m_dosimetrTldId;         // ID дозиметра TLD (было doz_tld_id)

    // --- Дозиметрия (Radiation Dose) ---
    float m_annualDose;          // Доза за год (было dose_year)
    float m_currentYearDose;     // Доза за текущий год (было dose_year_now)
    float m_currentYearDosePPD;  // Доза за текущий год (ППД)

    // --- Время ---
    QDateTime m_lastUpdate;      // Время последнего обновления (было timestamp)
};

Q_DECLARE_METATYPE(User)  //Signal/slot (между потоками, queued) — ТРЕБУЕТ,   QVariant — ТРЕБУЕТ
