#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Кастомные классы приложения
#include "doubleclickbutton.h"
#include "meshstatusmodel.h"
// #include "welcome.h"

// Основные классы Qt
#include <QMainWindow>
#include <QWidget>
#include <QObject>
#include <QVariant>
#include <QProgressDialog>

// Контейнеры и структуры данных Qt
#include <QMap>
#include <QList>
#include <QVector>
#include <QPair>

// Диалоги и элементы ввода
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QFileDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QDateEdit>
#include <QTableWidget>
#include <QTableWidgetItem>

// Работа с базой данных
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRecord>

// Макеты и интерфейс
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QStatusBar>
#include <QLabel>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

// Работа с файлами и данными
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QStandardPaths>
#include <QBuffer>

// Даты и время
#include <QDate>
#include <QDateTime>
#include <QTimer>

// События
#include <QMouseEvent>
#include <QEvent>

// Регулярные выражения
#include <QRegularExpression>

// Мультимедиа
#include <QMovie>

// Процессы и системные функции
#include <QProcess>
#include <QDesktopServices>
#include <QApplication>
#include <QScreen>
#include <QtGlobal>

// Отладка
#include <QDebug>
#include <QLoggingCategory>

#include <QXmlStreamWriter>    // Для создания XML структуры Excel/Word
#include <QByteArray>          // Для работы с бинарными данными
#include <QBuffer>             // Для буферизации данных
#include <QTextDocument>       // Для форматирования текста
#include <QTextCursor>         // Для позиционирования в документе
#include <QTextTable>          // Для создания таблиц в Word

//#include <QAxObject>
//#include <QAxWidget>

#include <QDateTime>
//#include <QDomDocument>        // Для работы с XML (альтернатива QXmlStreamWriter)

// Пространство имен UI
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// Вперед объявление класса Welcome
class Welcome;

// Структуры данных приложения
struct KasData {
    int set_id;
    int kas_id;
    QString kas_name;
    double kas_height;
    double kas_width;
    QString kas_block;
    QString kas_note;
    QString last_update;
};

struct MeshData {
    int set_id;
    int kas_id;
    int mesh_id;
    int user_id;
    QString doz_tld_id;
    QString mesh_status;
    QString mesh_note;
    QString last_update;
};

struct DutyDataSearch {
    QString id;
    QString displayText;
    QString searchText;
};

struct UsersDutyDataSearch {
    QString id;
    QString displayText;
    QString searchText;
};

struct UserDataSearsh {
    QString id;
    QString displayText;
    QString searchText;
};

// Определение типов для массивов
using KasArray = QMap<int, KasData>; // Используем QMap для доступа по kas_id
using MeshArray = QMap<int, MeshData>; // Используем QMap для доступа по kas_id

// Регистрация кастомных типов для QVariant
Q_DECLARE_METATYPE(KasData)
Q_DECLARE_METATYPE(MeshData)
Q_DECLARE_METATYPE(DutyDataSearch)
Q_DECLARE_METATYPE(UsersDutyDataSearch)
Q_DECLARE_METATYPE(UserDataSearsh)
Q_DECLARE_METATYPE(QList<KasData>)
Q_DECLARE_METATYPE(QList<MeshData>)
Q_DECLARE_METATYPE(QList<DutyDataSearch>)
Q_DECLARE_METATYPE(QList<UsersDutyDataSearch>)
Q_DECLARE_METATYPE(QList<UserDataSearsh>)

// Основной класс MainWindow
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int userId, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Слоты для пользователей
    void on_btnCreateNewUser_pressed();
    void on_btnAddPhoto_pressed();
    void on_btnCreateUser_pressed();
    void on_btnChangeUser_pressed();
    void on_btn_userClear_pressed();
    void on_btnSearchUser_pressed();
    void on_btnChangePhoto_pressed();
    void on_pushButton_userUpdate_pressed();
    void updateUsersDatabase();
    void updateCurrentUserFromDatabase(int userId);
    void checkRole();

    // Слоты для наборов (Set)
    void on_btnSetSave_pressed();
    void on_btnSetChange_pressed();
    void on_btnSetCreateNew_pressed();
    void loadSetData();
    void onSetSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    // Слоты для наборов (Set) в KID
    void on_btnSetSave_kid_pressed();
    void on_btnSetChange_kid_pressed();
    void on_btnSetCreateNew_kid_pressed();
    void loadSetData_kid();
    void onSetSelectionChanged_kid(const QItemSelection &selected, const QItemSelection &deselected);

    // Слоты для кассетниц (Kas)
    void on_btnKasSave_pressed();
    void on_btnKasChange_pressed();
    void on_btnKasCreateNew_pressed();
    void loadListOfKas();
    void loadFilteredKasData();
    void onKasSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void on_comboBox_listOfKas_currentTextChanged(const QString &arg1);

    // Слоты для кассетниц (Kas) в KID
    void on_btnKasSave_kid_pressed();
    void on_btnKasChange_kid_pressed();
    void on_btnKasCreateNew_kid_pressed();
    void loadListOfKas_kid();
    void loadFilteredKasData_kid();
    void onKasSelectionChanged_kid(const QItemSelection &selected, const QItemSelection &deselected);
    void on_comboBox_listOfKas_kid_currentTextChanged(const QString &index);

    // Слоты для ячеек (Mesh)
    void on_btnMeshSave_pressed();
    void on_btnMeshChange_pressed();
    void on_btnMeshCreateNew_pressed();
    void loadFilteredMeshData(QString kas_id);
    void onMeshSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    // Слоты для ячеек (Mesh) в KID
    void on_btnMeshSave_kid_pressed();
    void on_btnMeshChange_kid_pressed();
    void on_btnMeshCreateNew_kid_pressed();
    void loadFilteredMeshData_kid(QString kas_id);
    void onMeshSelectionChanged_kid(const QItemSelection &selected, const QItemSelection &deselected);

    // Слоты для дежурств (Duty)
    void on_btn_saveNewDuty_pressed();
    void on_btn_saveNewDutyUser_pressed();
    void on_btn_saveChangeDuty_pressed();
    void on_btn_saveChangeUserDuty_pressed();
    void on_btn_createNewDuty_pressed();
    void on_table_userDuty_cellPressed(int row, int column);
    void on_tableDuty_cellPressed(int row, int column);
    void refresh_duty_table();
    void tableUsersDutyUpd();

    // Слоты для пользовательских дежурств
    void on_btn_userDutySearch_pressed();
    void on_btn_userDutyClear_pressed();
    void on_btnUserDutySearch_pressed();
    void onUserDutySearchTextChanged(const QString &text);
    void showAllUserDuties();
    void setupUserDutySearch();

    // Слоты для истории
    void usersHistoryToTable();
    void setHistoryToTable();
    void kasHistoryToTable();
    void meshHistoryToTable();
    void loadDutyUsersHistoryToTable();
    void loadDutyHistoryToTable();

    // Слоты для истории в KID
    void setHistoryToTable_kid();
    void kasHistoryToTable_kid();
    void meshHistoryToTable_kid();

    // Слоты для доз (Dose)
    void on_pushButton_DoseCleanInput_pressed();
    void on_pushButton_DoseSaveNew_pressed();
    void on_pushButton_DoseSaveChange_pressed();
    void loadDosePpdToTable();
    void loadDosePpdHistoryToTable();
    void on_tableWidget_Dose_cellPressed(int row, int column);

    // Слоты для поиска PPD
    void on_pushButton_ppdFilterClean_pressed();
    void on_btn_ppdSearch_pressed();
    void on_pushButton_ppdFilter_pressed();
    void setupPpdSearch();
    void onPpdSearchTextChanged(const QString &text);
    void showAllPpdRows();

    // Слоты для фильтров истории
    void on_btnUserFilterGo_2_pressed();
    void on_btnUserFilterStop_2_pressed();
    void on_btn_dutyUsersFilterGo_pressed();
    void on_btn_dutyUsersFilterStop_pressed();
    void on_btn_dutyFilterGo_pressed();
    void on_btn_dutyFilterStop_pressed();
    void on_btn_SetFilterGo_pressed();
    void on_btn_SetFilterStop_pressed();
    void on_btn_KasFilterGo_pressed();
    void on_btn_KasFilterStop_pressed();
    void on_btn_MeshFilterGo_pressed();
    void on_btn_MeshFilterStop_pressed();
    void on_btn_DoseHistoryFilterGo_pressed();
    void on_btn_DoseHistoryFilterStop_pressed();

    // Слоты для фильтров истории в KID
    void on_btn_SetFilterGo_kid_pressed();
    void on_btn_SetFilterStop_kid_pressed();
    void on_btn_KasFilterGo_kid_pressed();
    void on_btn_KasFilterStop_kid_pressed();
    void on_btn_MeshFilterGo_kid_pressed();
    void on_btn_MeshFilterStop_kid_pressed();

    // Слоты для экспорта в файл
    void on_pushButton_usersHistoryFile_pressed();
    void on_pushButton_dutyUsersFile_2_pressed();
    void on_pushButton_dutyHistoryFile_pressed();
    void on_pushButton_SetHistoryFile_pressed();
    void on_pushButton_KasHistoryFile_pressed();
    void on_pushButton_MeshHistoryFile_pressed();
    void on_pushButton_DoseHistoryFile_pressed();

    // Слоты для экспорта в файл в KID
    void on_pushButton_SetHistoryFile_kid_pressed();
    void on_pushButton_KasHistoryFile_kid_pressed();
    void on_pushButton_MeshHistoryFile_kid_pressed();

    // Слоты для фильтров KDD
    void on_btnKddFilterGo_clicked();

    // Слоты для отчетов
    void on_btn_createReport_pressed();

    // Слоты для навигации и управления
    void on_pushButton_2_pressed();
    void on_pushButton_3_clicked();
    void on_pushButton_Exit_pressed();
    void on_pushButton_Back_pressed();
    void on_pushButton_help_pressed();
    void on_pushButton_onSet_pressed();
    void on_pushButton_onKas_pressed();

    // Слоты для поиска пользователей
    void onSearchTextChanged(const QString &text);
    void showAllUsers();
    void setupSearch();

    // Слоты для обновления данных
    void updateDataFromDatabase();
    void comboboxUpdateAll();

    // Слоты для обновления комбобоксов
    void updateSetComboBox(QComboBox* comboBox);
    void updateSetComboBox(QComboBox* comboBox1, QComboBox* comboBox2);
    void updateSetComboBox(QComboBox* comboBox1, QComboBox* comboBox2, QComboBox* comboBox3);
    void updateSetKidComboBox(QComboBox* comboBox);
    void updateSetKidComboBox(QComboBox* comboBox1, QComboBox* comboBox2);
    void updateSetKidComboBox(QComboBox* comboBox1, QComboBox* comboBox2, QComboBox* comboBox3);
    void onSetComboBoxChanged();
    void onKasComboBoxChanged();
    void onSetKidComboBoxChanged();
    void onKasKidComboBoxChanged();

    // Вспомогательные слоты
    void updateListSetForHistory();
    void updateListSetForHistory_kid();

    // Методы экспорта
    bool saveTableToCSV(QTableWidget *table, const QString &filePath);
    bool saveTableToExcel(QTableWidget *table, const QString &filePath);
    bool saveTableToWord(QTableWidget *table, const QString &filePath);


    // Методы для добавления истории
    bool addUserHistory(
        int userId,
        const QString &login,
        const QString &password,
        const QString &name0,
        const QString &name1,
        const QString &name2,
        const QString &gender,
        const QDate &birthday,
        const QString &role,
        int tabNum,
        const QString &department,
        const QString &cardId,
        int setId,
        int kasId,
        int meshId,
        const QString &dozTldId,
        const QDate &cellDate,
        double doseYear,
        double doseYearNow,
        double doseYearNowPpd,
        const QString &code,
        const QString &block,
        const QString &changing_user_id,
        const QString &type_edit
        );

    bool addUserDutyHistory(
        int users_duty_id,
        int user_id,
        int duty_id,
        const QDate &date_start,
        const QDate &date_finish,
        const QString &duty_note,
        const QString &SetLastUpdate,
        const QString &user_id_change,
        const QString &type_edit
        );

    bool addDutyHistory(
        const QString& duty_id,
        int type_duty,
        const QString& blok,
        const QString& room,
        const QString& equipment,
        const QString& name_work,
        const QString& type_work,
        int time,
        const QVector<double>& doses,
        const QString& duty_note,
        const QDateTime &SetLastUpdate,
        const QString &user_id_change,
        const QString &type_edit
        );

    bool addSetHistory(int setId, const QString &setName, const QString &ipSet,
                       int setQuantity, const QString &setBlock, const QString &setNote,
                       const QString &changing_user_id, const QString &type_edit);

    bool addKasHistory(int setId, int kasId, const QString &kasName, int kasHeight,
                       int kasWidth, const QString &kasBlock, const QString &kasNote,
                       const QString &changing_user_id, const QString &type_edit);

    bool addMeshHistory(int setId, int kasId, int meshId, int userId,
                        const QString &dozTldId, int meshStatus, const QString &meshNote,
                        const QString &changing_user_id, const QString &type_edit);

    bool addSetHistory_kid(int setId, const QString &setName, const QString &ipSet,
                           int setQuantity, const QString &setBlock, const QString &setNote,
                           const QString &changing_user_id, const QString &type_edit);

    bool addKasHistory_kid(int setId, int kasId, const QString &kasName, int kasHeight,
                           int kasWidth, const QString &kasBlock, const QString &kasNote,
                           const QString &changing_user_id, const QString &type_edit);

    bool addMeshHistory_kid(int setId, int kasId, int meshId, int userId,
                            const QString &dozTldId, int meshStatus, const QString &meshNote,
                            const QString &changing_user_id, const QString &type_edit);

    bool addDosePpdHistory(
        int dosePpdId,
        int userId,
        int usersDutyId,
        const QString &nomerPdd,
        int time,
        const QDateTime &timeMax,
        int typePpd,
        double dose,
        double rateMax,
        const QString &dosePpdNote,
        const QDateTime &startWork,
        const QDateTime &finishWork,
        const QString &lastUpdate,
        const QString userIdChange,
        const QString type_edit);

    // Вспомогательные методы
    int extractIdFromComboBox(const QString& text);
    void setComboBoxValue(QComboBox* comboBox, int value);
    void updateComboBox(QComboBox* combo, const QString& queryText,
                        const QString& bindValue1 = "", const QString& bindValue2 = "");
    void updateComboBox_kid(QComboBox* combo, const QString& queryText,
                            const QString& bindValue1 = QString(),
                            const QString& bindValue2 = QString());
    void setupAllUserComboBoxes();
    void setupSearchUserComboBox(QComboBox* comboBox);
    void setupSearchDutyComboBox(QComboBox* comboBox);
    void setupSearchUsersDutyComboBox(QComboBox* comboBox);

    //эксплуатация
    void comboboxExploitation();
    void on_btn_createReport_clicked();

    // Метод определения ОС
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

private:
    // UI и окна
    Welcome *welcomeWindow;
    Ui::MainWindow *ui;

    // Данные пользователя
    int currentUserId;
    QByteArray photoData;

    // База данных
    QSqlDatabase db;

    // Таймеры и флаги
    QTimer m_doubleClickTimer;
    bool m_waitingForSecondClick = false;

    // Структуры данных
    struct UserData {
        QString id;
        QString displayText;
        QString searchText;
    };
    QVector<UserData> m_usersData;

    struct DutyData {
        QString id;
        QString displayText;
        QString searchText;
    };
    QVector<DutyData> m_dutiesData;

    // Модели и данные
    MeshStatusModel* m_currentMeshModel = nullptr;

    // Настройки подключения к БД
    QString G_ip_database_cell;
    QString G_DatabaseName_cell;
    QString G_UserName_cell;
    QString G_Password_cell;

    // Элементы поиска
    QPushButton *clearSearchButton;
    QPushButton *clearUserDutySearchButton;
    QPushButton *clearPpdSearchButton;

    // Комбобоксы для выбора
    QComboBox* m_setComboBox = nullptr;
    QComboBox* m_kasComboBox = nullptr;
    QComboBox* m_meshComboBox = nullptr;

    QComboBox* m_setKidComboBox = nullptr;
    QComboBox* m_kasKidComboBox = nullptr;
    QComboBox* m_meshKidComboBox = nullptr;



    // Диалоговые окна (указатели для управления)
    QDialog *exportFormatDialog;
    QComboBox *formatComboBox;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // MAINWINDOW_H
