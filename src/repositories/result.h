#pragma once
#ifndef RESULT_H
#define RESULT_H

#include <QString>

// === Информация об ошибке ===
struct ErrorInfo {
    enum Type { None = 0, Validation, Database, NotFound, Unknown };

    Type type = None;
    QString message;
    QString field;  // Имя поля для подсветки в UI

    static ErrorInfo validation(const QString& fld, const QString& msg) {
        return {Validation, msg, fld};
    }
    static ErrorInfo database(const QString& msg) {
        return {Database, msg, {}};
    }
    static ErrorInfo notFound(const QString& msg = "Not found") {
        return {NotFound, msg, {}};
    }
};

// === Шаблон результата операции ===
template<typename T>
struct Result {
    bool isOk;              // true = успех, false = ошибка
    T value;                // Данные (валидно если isOk=true)
    ErrorInfo error;        // Ошибка (валидно если isOk=false)

    // Конструкторы
    Result() : isOk(true), value{}, error{} {}
    Result(bool ok, T val, ErrorInfo err) : isOk(ok), value(val), error(err) {}

    // === ФАБРИЧНЫЕ МЕТОДЫ ===
    static Result<T> ok(T val) { return Result<T>(true, val, ErrorInfo{}); }
    static Result<T> err(ErrorInfo err) { return Result<T>(false, T{}, err); }

    // === ПОМОЩНИКИ ===
    bool hasError() const { return !isOk; }
    bool hasError(ErrorInfo::Type type) const { return !isOk && error.type == type; }
    QString errorMessage() const { return isOk ? QString() : error.message; }
};

#endif // RESULT_H
