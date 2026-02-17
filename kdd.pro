#HEADERS += $$PWD/qsql_psql.h
#SOURCES += $$PWD/qsql_psql.cpp
#
#unix|win32-g++* {
#    LIBS *= $$QT_LFLAGS_PSQL
##    !contains(LIBS, .*pq.*):LIBS += -lpq
##    QMAKE_CXXFLAGS *= $$QT_CFLAGS_PSQL
##} else {
#    !contains(LIBS, .*pq.*):LIBS += -llibpq -lws2_32 -ladvapi32
#}
#include(path/to/QtXlsx/QtXlsx.pri)
# Определяем путь к исходникам
SOURCES_DIR = $$PWD

# Передаем в код как define
DEFINES += SOURCE_CODE_PATH=\"\\\"$$SOURCES_DIR\\\"\"

#QT += core gui sql quick widgets
QT += core gui sql multimedia multimediawidgets

# Проверка подключения QXlsx
exists($$PWD/QXlsx/qxlsx.pri) {
    include($$PWD/QXlsx/qxlsx.pri)
    DEFINES += WITH_QXLSX
    message("QXlsx подключен")
} else:exists($$PWD/../QXlsx/qxlsx.pri) {
    include($$PWD/../QXlsx/qxlsx.pri)
    DEFINES += WITH_QXLSX
    message("QXlsx подключен из родительской папки")
} else {
    message("ВНИМАНИЕ: QXlsx не найден")
}

CONFIG += c++11
CONFIG += utf8_source

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    doubleclickbutton.cpp \
    helpers.cpp \
    main.cpp \
    mainwindow.cpp \
    meshstatusmodel.cpp \
    welcome.cpp

HEADERS += \
    doubleclickbutton.h \
    helpers.h \
    mainwindow.h \
    meshstatusmodel.h \
    welcome.h

FORMS += \
    mainwindow.ui \
    welcome.ui

# PostgreSQL подключение (только для Windows)
win32 {
    PG_PATH = "C:/Program Files/PostgreSQL/18"

    INCLUDEPATH += "$$PG_PATH/include"
    LIBS += -L"$$PG_PATH/lib"
    LIBS += -llibpq
    LIBS += -lws2_32
    LIBS += -lsecur32
    LIBS += -lcrypt32

#RC_FILE = app.rc
}  else:unix:!macx {
# Настройки для Linux
# Проверяем различные возможные пути установки PostgreSQL
exists(/usr/include/postgresql) {
    INCLUDEPATH += /usr/include/postgresql
} else:exists(/usr/include/pgsql) {
    INCLUDEPATH += /usr/include/pgsql
} else:exists(/usr/local/include/postgresql) {
    INCLUDEPATH += /usr/local/include/postgresql
}

# Добавляем библиотеки
LIBS += -lpq
CONFIG += link_pkgconfig
PKGCONFIG += libpq
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#win32 {
#    LIBS += -lkernel32 -luser32 -lshell32
#    QMAKE_LFLAGS += /ENTRY:mainCRTStartup
#}


# Определяем версию Qt
greaterThan(QT_MAJOR_VERSION, 4) {
    # Qt5 или Qt6
    QT += widgets multimedia multimediawidgets
    greaterThan(QT_MINOR_VERSION, 9): QT6 = true
}
DEFINES += QT_DEPRECATED_WARNINGS
