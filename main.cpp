#include "mainwindow.h"
#include "welcome.h"


#include <QLocale>
#include <QTranslator>
#include <QLabel>

#include <QApplication>
#include <QApplication>    // Основной класс приложения
#include <QFont>
#include <QLocale>

int main(int argc, char *argv[])
{
    // Установите переменную окружения перед созданием QApplication
    qputenv("QT_LOGGING_RULES", "qt.core.qmetaobject.connectslotsbyname=false");
    qputenv("QT_LOGGING_RULES", "qt.sql.*=false");
    qputenv("QT_QPA_PLATFORM", "windows:fontengine=freetype"); // Для лучшей поддержки шрифтов



    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::Russia));
    QApplication a(argc, argv);

    // Настройка шрифта с поддержкой кириллицы
    QFont font("Arial", 10);
    font.setStyleStrategy(QFont::PreferAntialias);
    a.setFont(font);

    Welcome w;
    w.show();

    return a.exec();
}
