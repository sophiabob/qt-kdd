// helpers.h
#ifndef HELPERS_H
#define HELPERS_H

#include <QString>
#include <QDateTime>
#include <QTimeZone>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QLayoutItem>
#include <QTableWidget>
#include <stdexcept>
#include <memory>
#include <QString>
#include <QTableWidget>
#include <QVector>
#include <QPair>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <functional>

#include <QTimer>
#include <QApplication>
#include <QEvent>

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
       // void startInactivityTimer(int seconds, std::function<void()> callback);

        static void startInactivityTimer(int seconds, std::function<void()> callback, QObject *parent = nullptr);



    private:
        QTimer* inactivityTimer = nullptr;
        bool eventFilter(QObject *obj, QEvent *event);
        void stopInactivityTimer();
};

#endif // HELPERS_H
