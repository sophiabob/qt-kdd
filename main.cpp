#include "mainwindow.h"
#include "welcome.h"


#include <QLocale>
#include <QTranslator>
#include <QLabel>

#include <QApplication>

int main(int argc, char *argv[])
{
    /*// убирает уведомления в выводе о слотах
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext&, const QString& msg) {
        if (type == QtWarningMsg && msg.contains("wildcard call disconnects"))
            return;  // Игнорируем это предупреждение
        fprintf(stderr, "%s\n", qPrintable(msg));  // Остальные — как обычно
    });
*/


    qputenv("QT_LOGGING_RULES", "qt.core.qmetaobject.connectslotsbyname=false"); // Отключаем предупреждения об автоподключении слотов
    qputenv("QT_LOGGING_RULES", "qt.sql.*=false");  // Отключить все SQL-логи
    QLoggingCategory::setFilterRules("qt.core.qmetaobject.connectslotsbyname=false");
    QApplication a(argc, argv);
    //MainWindow w;////
    Welcome w;
    w.show();
    //w.showFullScreen();
    return a.exec();
}
