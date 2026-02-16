#ifndef HELPERS_H
#define HELPERS_H
#include "welcome.h"

// Qt Core
#include <QString>
#include <QDateTime>
#include <QTimeZone>
#include <QDebug>
#include <QTimer>
#include <QEvent>
#include <functional>
// Qt GUI
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLayoutItem>
#include <QVBoxLayout>
// Qt Multimedia
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QUrl>
// Qt Dialogs
#include <QMessageBox>
// Qt Tables
#include <QTableWidget>
// Qt File
#include <QFile>
#include <QTextStream>
// STL
#include <stdexcept>
#include <memory>
#include <vector>

struct TableSearchConfig {
    bool caseSensitive = false;        // чувствительность к регистру
    bool searchInAllColumns = true;    // искать во всех столбцах
    QList<int> columnsToSearch;        // или только в указанных
    bool showNotFoundMessage = true;   // показывать сообщение "не найдено"
    QString notFoundMessage = "Ничего не найдено"; // кастомное сообщение
};

namespace ExportHelper {
    struct TableData { //Чтение данных из таблицы
        QVector<QString> headers;
        QVector<QVector<QString>> rows;
    };
    TableData readTableData(QTableWidget* table);
    QString convertToCSV(const TableData& data);
    QString convertToExcelHTML(const TableData& data);
    QString convertToWordHTML(const TableData& data);
}

class Helpers : public QObject {
    Q_OBJECT

    public:
        static QString timeDateMoscow(); //присваивает строке текущее ("yyyy-MM-dd HH:mm:ss")
        static bool searchInLayout(QVBoxLayout* layout, const QString& searchText, QPushButton* clearButton = nullptr); //поиск в области
        static bool searchInTable(QTableWidget* table, const QString& searchText, QPushButton* clearButton = nullptr, const TableSearchConfig& config = TableSearchConfig());

        static void startInactivityTimer(int seconds, std::function<void()> callback, QObject *parent = nullptr);
        static void playVideoInWindow(QWidget* parent, QString videoPath);
        static QString dialogWithVariatives(QWidget* parent, QString forDialogTitle, QString forDialogText, const std::vector<FileRecord>& options);



    private:
        QTimer* inactivityTimer = nullptr;
        bool eventFilter(QObject *obj, QEvent *event);
        void stopInactivityTimer();
};

#endif // HELPERS_H
