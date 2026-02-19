#ifndef USER_H //#pragma once - современная альтернатива
#define USER_H

#include <QMainWindow>


#include <QString>
#include <QDateTime>
#include <QMetaType> //регистрация типа

class User
{
public:
    // Конструкторы
    User() = default;
    User(
        int id,
        const QString& login,
        const QString& email
    );


    // Геттеры (const методы) - даём пользователю
    int id() const { return m_id; }
    const QString& login() const { return m_login; }
    const QString& email() const { return m_email; }
    QDateTime createdAt() const { return m_createdAt; } // без сеттера тк только при создании
    // Геттер для даты рождения (не меняется после создания)
    QDateTime birthDate() const { return m_birthDate; }
    // Геттер для last modified (может меняться)
    QDateTime lastModified() const { return m_lastModified; }



    // Сеттеры - читаем и даём БД
    void setId(int id) { m_id = id; } // ID меняется только при создании → нет setId()
    void setLogin(const QString& login) { m_login = login; } // Login можно менять → есть setLogin()
    void setEmail(const QString& email) { m_email = email; }


    //void touch() { m_lastModified = QDateTime::currentDateTime(); } // Сеттер ТОЛЬКО для lastModified тк изменяемое
    void updateLastModified() {  // простой метод обновления
        m_lastModified = QDateTime::currentDateTime();
    }



    // Вспомогательные методы
    bool isValid() const { return m_id > 0 && !m_login.isEmpty(); }


    //---МОЁ---

    User() = default;
    User(
        int id = -1;
        QString& name1;
        QString& name0;
        QString& name2;
        QString& login;
        QString& password;
        QString& snils;
        QString& code;
        int tabNum;
        QString& cardId;
        QString& dozTldId;
        int startDoz;
        int finishDoz;
        int doseYear;
        int doseYearNow;
        int doseYearPpd;
        QDate cellDate;
        QDate birthday;
        QString& lastUpdate;
        QDate startUsed;
        QDate finishUsed;
        QString& role;
        QString& gender;
        QString& department;
        QString& block;
        QString& set;
        QString& kas;
        QString& mesh;
    );






private:
    int m_id = -1;
    QString m_login;
    QString m_email;
    QDateTime m_createdAt;

    QDateTime m_birthDate;      // не меняется после создания → нет сеттера
    QDateTime m_lastModified;   // меняется → есть метод touch()
};

// Для использования в сигналах/слотах Qt
Q_DECLARE_METATYPE(User)  //Signal/slot (между потоками, queued) — ТРЕБУЕТ,   QVariant — ТРЕБУЕТ



/*
User u(1, "admin", QDate(1990, 1, 1));
// u.setBirthDate(...);  // ❌ Нет такого метода — дата рождения неизменна

u.touch();  // ✅ Обновляем lastModified при любом изменении
*/
