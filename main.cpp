#include "mainwindow.h"
#include "welcome.h"
#include "db_config.h"


#include <QLocale>
#include <QTranslator>
#include <QLabel>
#include <QScopedValueRollback>

#include <QApplication>
#include <QApplication>    // Основной класс приложения
#include <QFont>
#include <QLocale>

int main(int argc, char *argv[])
{
<<<<<<< HEAD
#ifdef Q_OS_LINUX
    qputenv("QT_QPA_PLATFORM", "xcb");
#endif

    QApplication a(argc, argv);

    // Проверка
    qDebug() << "Platform plugin:" << qgetenv("QT_QPA_PLATFORM");


    // Установите переменную окружения перед созданием QApplication
    qputenv("QT_LOGGING_RULES", "qt.core.qmetaobject.connectslotsbyname=false");
    qputenv("QT_LOGGING_RULES", "qt.sql.*=false");
    qputenv("QT_QPA_PLATFORM", "windows:fontengine=freetype"); // Для лучшей поддержки шрифтов



    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::Russia));
    //QApplication a(argc, argv);

    // Настройка шрифта с поддержкой кириллицы
    QFont font("Arial", 10);
    font.setStyleStrategy(QFont::PreferAntialias);
    a.setFont(font);
=======
    /*// убирает уведомления в выводе о слотах
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext&, const QString& msg) {
        if (type == QtWarningMsg && msg.contains("wildcard call disconnects"))
            return;  // Игнорируем это предупреждение
        fprintf(stderr, "%s\n", qPrintable(msg));  // Остальные — как обычно
    });*/

    //qputenv("QT_LOGGING_RULES", "qt.core.qmetaobject.connectslotsbyname=false"); // Отключаем предупреждения об автоподключении слотов
    //qputenv("QT_LOGGING_RULES", "qt.sql.*=false");  // Отключить все SQL-логи

    qputenv("QT_LOGGING_RULES", "qt.core.qmetaobject.connectslotsbyname=false;qt.sql.*=false");
    QLoggingCategory::setFilterRules("qt.core.qmetaobject.connectslotsbyname=false");


#ifdef QT_DEBUG
    qDebug() << "[DEBUG] Memory tracking enabled";
#endif
    QApplication a(argc, argv);

    DbConfig::loadFromIni();
    qDebug() << "[MAIN] Config loaded:" << DbConfig::isLoaded() << "\n";


>>>>>>> new-architecture

    Welcome w;
    w.show();

<<<<<<< HEAD
    return a.exec();
=======
    if (qApp->quitOnLastWindowClosed()){ //выаолненеие команд после выхода - потом разберёмся
        qDebug() << "qApp->quitOnLastWindowClosed(): " << "true";
    } else {
        qDebug() << "qApp->quitOnLastWindowClosed(): " << "false";
    }

    int result = a.exec();
    qDebug() << "[EXIT] Config still loaded:" << DbConfig::isLoaded();
    //w.showFullScreen();
    DbConfig::unload();
    return result;
>>>>>>> new-architecture
}
