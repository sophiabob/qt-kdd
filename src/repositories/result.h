#pragma once
#ifndef RESULT_H
#define RESULT_H

#include <QString>

struct ErrorInfo {
    enum Type { None = 0, Validation, Database, NotFound, Unknown };
    Type type = None;
    QString message;
    QString field;

    static ErrorInfo validation(const QString& fld, const QString& msg) { return {Validation, msg, fld}; }
    static ErrorInfo database(const QString& msg) { return {Database, msg, {}}; }
    static ErrorInfo notFound(const QString& msg = "Not found") { return {NotFound, msg, {}}; }
};

template<typename T>
struct Result {
    bool isOk;
    T value;
    ErrorInfo error;

    Result() : isOk(true), value{}, error{} {}
    Result(bool ok, T val, ErrorInfo err) : isOk(ok), value(val), error(err) {}

    static Result<T> ok(T val) { return Result<T>(true, val, ErrorInfo{}); }
    static Result<T> err(ErrorInfo err) { return Result<T>(false, T{}, err); }

    bool hasError() const { return !isOk; }
    QString errorMessage() const { return isOk ? QString() : error.message; }
};

#endif // RESULT_H
