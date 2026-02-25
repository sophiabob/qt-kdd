#ifndef USER_H
#define USER_H //#pragma once - современная альтернатива


#include <QString>
#include <QDateTime>
#include <QMetaType> //регистрация типа
#include <QSqlQuery>
#include <QObject>

class User
    : public QObject {
    Q_OBJECT //Qt Meta-Object System (MOC)
    // Макрос: тип, имя, геттер, сеттер, имя колонки в БД (user_data)
    Q_PROPERTY(QString login READ login WRITE setLogin USER "login")
    Q_PROPERTY(QString passwordHash READ passwordHash WRITE setPasswordHash USER "password")
    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName USER "name_0")
    Q_PROPERTY(QString surname READ surname WRITE setSurname USER "name_1")
    Q_PROPERTY(QString patronymic READ patronymic WRITE setPatronymic USER "name_2")
    Q_PROPERTY(QString gender READ gender WRITE setGender USER "gender")
    Q_PROPERTY(QDateTime birthDate READ birthDate WRITE setBirthDate USER "birthday")
    Q_PROPERTY(QString role READ role WRITE setRole USER "role")
    Q_PROPERTY(int snils READ snils WRITE setSnils USER "snils")
    Q_PROPERTY(int employeeNumber READ employeeNumber WRITE setEmployeeNumber USER "tab_num")
    Q_PROPERTY(QString department READ department WRITE setDepartment USER "department")
    Q_PROPERTY(QString cardId READ cardId WRITE setCardId USER "card_id")
    Q_PROPERTY(int setId READ setId WRITE setSetId USER "set_id")
    Q_PROPERTY(int kasId READ kasId WRITE setKasId USER "kas_id")
    Q_PROPERTY(int  meshId READ meshId WRITE setMeshId USER "mesh_id")
    Q_PROPERTY(QString dosimetrTldId READ dosimetrTldId WRITE dosimetrTldId USER "doz_tld_id")
    Q_PROPERTY(QDateTime lastCellUpdate READ lastCellUpdate WRITE setLastCellUpdate USER "cell_date")
    Q_PROPERTY(float annualDose READ annualDose WRITE setAnnualDose USER "dose_year")
    Q_PROPERTY(float currentYearDose READ currentYearDose WRITE setCurrentYearDose USER "dose_year_now")
    Q_PROPERTY(float currentYearDosePPD READ currentYearDosePPD WRITE setCurrentYearDosePPD USER "dose_year_now_ppd")
    Q_PROPERTY(QString accessCode READ accessCode WRITE setAccessCode USER "code")
    Q_PROPERTY(QString blockReason READ blockReason WRITE setBlockReason USER "block")
    Q_PROPERTY(QDateTime lastUpdate READ lastUpdate WRITE setLastUpdate USER "last_update")


//проверить
    friend class UserRepository;

public:
    User() = default;
    User(
        int id = -1,
        const QString& firstName = QString(),
        const QString& surname = QString(),
        const QString& patronymic = QString(),
        const QString& login = QString(), //
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
    QDateTime lastCellUpdate() const { return m_lastCellUpdate; }

    float annualDose() const { return m_annualDose; }
    float currentYearDose() const { return m_currentYearDose; }
    float currentYearDosePPD() const { return m_currentYearDosePPD; }

    QDateTime lastUpdate() const { return m_lastUpdate; }



    //сеттеры - БД

    void setLogin(const QString& login) { m_login = login; }
    void setPasswordHash(const QString& v) { m_passwordHash = v; }

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
    void setLastCellUpdate(const QDateTime& lastCellUpdate) { m_lastCellUpdate = lastCellUpdate; }

    void setAnnualDose(float annualDose) { m_annualDose = annualDose; }
    void setCurrentYearDose(float currentYearDose) { m_currentYearDose = currentYearDose; }
    void setCurrentYearDosePPD(float currentYearDosePPD) { m_currentYearDosePPD = currentYearDosePPD; }

    void setLastUpdate(const QDateTime& lastUpdate) { m_lastUpdate = lastUpdate; }


    //вспомогательные методы

    //привязка переменных к полям sql
    void bindToQuery(QSqlQuery& query) const;
    static User fromQuery(const QSqlQuery& row);


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
    QDateTime m_lastCellUpdate;  // Дата последнего обновления ячейки (было cell_date)

    // --- Дозиметрия (Radiation Dose) ---
    float m_annualDose;          // Доза за год (было dose_year)
    float m_currentYearDose;     // Доза за текущий год (было dose_year_now)
    float m_currentYearDosePPD;  // Доза за текущий год (ППД)

    // --- Время ---
    QDateTime m_lastUpdate;      // Время последнего обновления (было timestamp)

};

Q_DECLARE_METATYPE(User)  //Signal/slot (между потоками, queued) — ТРЕБУЕТ,   QVariant — ТРЕБУЕТ
#endif // USER_H
