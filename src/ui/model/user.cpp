#include "user.h"
// Конструктор
User(
        int id,
        const QString& login,
        QDateTime birthDate
    ):
    m_id(id),
    m_login(login),
    m_birthDate(birthDate),
    m_lastModified(QDateTime::currentDateTime())  // автоматически при создании
{}



