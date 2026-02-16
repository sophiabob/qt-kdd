#ifndef WELCOME_H
#define WELCOME_H

#include <QDialog>
#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <QSettings>
#include <QDateTime>
#include <QHostAddress>
#include <QTcpSocket>
#include <QTimer>
#include <QRegularExpression>
#include <QGroupBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QProgressDialog>
#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QList>

#include "mainwindow.h"

namespace Ui {
class Welcome;
}

// Вынесенная структура DatabaseInfo (вне класса Welcome)
struct DatabaseInfo {
    QString name;
    QString host;
    QString user;
    QString database;
    QString password;
    int port;
    QSqlDatabase db;
    bool isConnected;
    QString errorMessage;
};

// Структура ColumnInfo тоже может быть вынесена или оставлена внутри класса
struct ColumnInfo {
    QString name;
    QString type;
    bool notNull;
    QString defaultValue;
    bool isPrimaryKey;
};

class Welcome : public QDialog
{
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome();

    QString findSettingsFile();
    void readDatabaseSettings(QString &user, QString &password,
                              QString &database, QString &host, int &port,
                              bool keepHost = false);
    bool createConnection();
    void checkAndSyncDatabases();
    // Функция определения операционной системы
    static QString detectOS();

private slots:
    void on_btnCencel_pressed();
    bool on_btnSignIn_clicked();
    void on_pushButton_pressed();
    void on_btnSignIn_pressed();
    void on_pushButton_help_pressed();
    void on_pushButton_syncBD_pressed();

    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::Welcome *ui;
    MainWindow *window;


    // Основные функции управления БД
    QList<DatabaseInfo> connectToDatabases();
    QString checkAndRepairDatabaseStructure(QSqlDatabase &database, const QString &dbName);
    QString compareAndSyncData(QSqlDatabase &sourceDB, QSqlDatabase &targetDB,
                               const QString &dbNameSource, const QString &dbNameTarget);

    // Функции для работы с таблицами
    QStringList getTableList();
    QStringList getMainTables();
    QStringList getHistoryTables();
    QStringList getKidTables();

    // Функция для получения структуры таблиц
    QList<ColumnInfo> getExpectedTableStructure(const QString &tableName);
    QString getCreateTableSQL(const QString &tableName);

    // Функции для работы с конкретными таблицами
    QString checkTableStructure(QSqlDatabase &db, const QString &tableName,
                                const QList<ColumnInfo> &expectedColumns);
    void createMissingTable(QSqlDatabase &db, const QString &tableName);
    void addMissingColumn(QSqlDatabase &db, const QString &tableName,
                          const ColumnInfo &column);

    // SQL функции для создания таблиц
    QString getUsersSQL();
    QString getUserPhotoSQL();
    QString getDutySQL();
    QString getUsersDutySQL();
    QString getSetSQL();
    QString getKasSQL();
    QString getMeshSQL();
    QString getUsersHistorySQL();
    QString getDutyHistorySQL();
    QString getUsersDutyHistorySQL();
    QString getSetHistorySQL();
    QString getKasHistorySQL();
    QString getMeshHistorySQL();
    QString getDosePpdSQL();
    QString getDosePpdHistorySQL();
    QString getSetKidSQL();
    QString getKasKidSQL();
    QString getMeshKidSQL();
    QString getSetKidHistorySQL();
    QString getKasKidHistorySQL();
    QString getMeshKidHistorySQL();

    QString fixEncoding(const QString &text);
};

#endif // WELCOME_H
