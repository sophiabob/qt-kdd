#ifndef RESULT_H
#define RESULT_H

/*class Result
{
public:
    Result();
};
*/

// repositories/result.h (универсальный шаблон)
//#pragma once
#include <QVector>
#include <QString>

struct ErrorInfo {
    enum Type { Validation, Database, NotFound, Unknown };
    Type type;
    QString message;
    QString field;  // Для валидации: "login", "email"

    static ErrorInfo validation(const QString& field, const QString& msg) {
        return {Validation, msg, field};
    }
    static ErrorInfo database(const QString& msg) {
        return {Database, msg, ""};
    }
};

template<typename T>
struct Result {
    bool ok;
    T value;                    // Если ok=true
    QVector<ErrorInfo> errors;  // Если ok=false

    static Result<T> success(T val) { return {true, val, {}}; }
    static Result<T> failure(QVector<ErrorInfo> errs) { return {false, T{}, errs}; }
    static Result<T> failure(ErrorInfo err) { return failure({err}); }

    // Удобные методы
    bool hasError(ErrorInfo::Type type) const {
        return !ok && std::any_of(errors.begin(), errors.end(),
                                  [type](const ErrorInfo& e) { return e.type == type; });
    }
};

#endif // RESULT_H
