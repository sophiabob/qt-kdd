#ifndef WELCOME_H
#define WELCOME_H

#include <QDialog>
#include <QtSql/QSqlDatabase>

#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>


#include <QMainWindow>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>

#include <QSqlDatabase>

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStringList>

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStringList>
#include <QDateTime>


#include <QHostAddress>
#include <QTcpSocket>
#include <QTimer>
#include <QMessageBox>
#include <QRegularExpression>

#include "mainwindow.h"

namespace Ui {
class Welcome;
}

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

private slots:
    void on_btnCencel_pressed();

    //void on_btnSignIn_clicked();

    bool on_btnSignIn_clicked();
    QString detectOS() {
        QString osName;

    #if defined(Q_OS_WIN)
            osName = "Windows";

    #elif defined(Q_OS_LINUX)
            osName = "Linux";

    #elif defined(Q_OS_UNIX)
            osName = "Unix";

    #else
            osName = "Unknown OS";
    #endif

        return osName;
    }

    void on_pushButton_pressed();


    void on_btnSignIn_pressed();

    void on_pushButton_help_pressed();

    bool eventFilter(QObject *obj, QEvent *event);


    /*QString findSettingsFile();

    void readDatabaseSettings(QString &user, QString &password,
                              QString &database, QString &host, int &port,
                              bool keepHost = false);

    bool createConnection();*/

private:
    Ui::Welcome *ui;

    MainWindow *window;

};

#endif // WELCOME_H
