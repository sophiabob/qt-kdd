#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "doubleclickbutton.h"
#include "meshstatusmodel.h"

#include <QMainWindow>
#include <QtCore>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QMap>
#include <QFileDialog>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QList>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QMessageBox>
#include <QComboBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QFile>
#include <QDate>
#include <QDateEdit>
#include <QTextStream>
#include <QVBoxLayout>
#include <QByteArray>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QVector>
#include <QRegularExpression>
#include <QApplication>
#include <QScreen>
#include <QtGlobal>
#include <QStatusBar>
#include <QComboBox>
#include <QApplication>
#include <QScreen>
#include <QVariant>
#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QDateTime>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QLabel>
#include <QMovie>
#include <QFile>
#include <QProcess>
#include <QDesktopServices>

//#include "welcome.h"

class Welcome;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

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

// Îáúÿâëÿåì òèï äëÿ ìàññèâà
using KasArray = QMap<int, KasData>; // Èñïîëüçóåì QMap äëÿ äîñòóïà ïî kas_id
using MeshArray = QMap<int, MeshData>; // Èñïîëüçóåì QMap äëÿ äîñòóïà ïî kas_id

// Ðåãèñòðàöèÿ êàñòîìíûõ òèïîâ äëÿ QVariant
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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int userId, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //void onConnectButtonClicked();

    void on_btnCreateNewUser_pressed();
    void on_btnAddPhoto_pressed();
    void on_btnSetSave_pressed();
    void on_btnKasSave_pressed();
    void on_btnMeshSave_pressed();
    void on_btnCreateUser_pressed();
    void on_btnChangeUser_pressed();
    void updateDataFromDatabase();
    void on_btn_saveNewDuty_pressed();
    void on_btn_saveNewDutyUser_pressed();
    void on_table_userDuty_cellPressed(int row, int column);
    void on_tableDuty_cellPressed(int row, int column);
    void on_btn_createNewDuty_pressed();
    void on_btn_saveChangeDuty_pressed();
    void on_btn_saveChangeUserDuty_pressed();
    void on_btnKddFilterGo_clicked();
    //void on_tabWidget_currentChanged(int index);
    void on_pushButton_2_pressed();
    void on_btn_createReport_pressed();
    void on_btnSetChange_pressed();
    //void on_btn_createReport_clicked();
    //void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void updateUsersDatabase();
    void on_btn_userClear_pressed();
    void on_btnSearchUser_pressed();
    void on_btnChangePhoto_pressed();
    void on_btnKasChange_pressed();
    void on_btnSetCreateNew_pressed();
    void on_btnKasCreateNew_pressed();
    void on_btnMeshCreateNew_pressed();
    void on_btnMeshChange_pressed();
    void loadSetData();
    void onSetSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void loadFilteredKasData();
    void onKasSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void loadFilteredMeshData(QString kas_id);
    void onMeshSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void loadListOfKas();
    void on_comboBox_listOfKas_currentTextChanged(const QString &arg1);

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

    void on_pushButton_Exit_pressed();
    void on_btn_userDutySearch_pressed();
    void on_btn_userDutyClear_pressed();
    void usersHistoryToTable();


    //void onSetChanged(const QString& text);
    //void onKasChanged(const QString& text);
    //void updateUsersDatabase();

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
        const QDateTime &SetLastUpdate,
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

    void setHistoryToTable();

    bool addKasHistory(int setId, int kasId, const QString &kasName, int kasHeight,
                       int kasWidth, const QString &kasBlock, const QString &kasNote,
                       const QString &changing_user_id, const QString &type_edit);
    void kasHistoryToTable();

    bool addMeshHistory(int setId, int kasId, int meshId, int userId,
                        const QString &dozTldId, int meshStatus, const QString &meshNote,
                        const QString &changing_user_id, const QString &type_edit);
    void meshHistoryToTable();

    void on_btnUserFilterGo_2_pressed();
    void loadDutyUsersHistoryToTable();
    void loadDutyHistoryToTable();
    void on_btnUserFilterStop_2_pressed();
    void on_pushButton_usersHistoryFile_pressed();
    void on_btn_dutyUsersFilterStop_pressed();
    void on_btn_dutyUsersFilterGo_pressed();
    void on_btn_dutyFilterStop_pressed();
    void on_btn_dutyFilterGo_pressed();
    void on_pushButton_dutyUsersFile_2_pressed();
    void on_pushButton_dutyHistoryFile_pressed();
    void on_pushButton_SetHistoryFile_pressed();
    void on_pushButton_KasHistoryFile_pressed();
    void on_pushButton_MeshHistoryFile_pressed();
    void on_btn_SetFilterGo_pressed();
    void on_btn_SetFilterStop_pressed();
    void on_btn_KasFilterStop_pressed();
    void on_btn_MeshFilterStop_pressed();
    void on_btn_KasFilterGo_pressed();
    void on_btn_MeshFilterGo_pressed();
    void on_pushButton_onSet_pressed();
    void on_pushButton_onKas_pressed();
    void refresh_duty_table();
    void tableUsersDutyUpd();
    void updateListSetForHistory();
    void on_pushButton_DoseCleanInput_pressed();
    void on_pushButton_DoseSaveNew_pressed();
    int extractIdFromComboBox(const QString& text);
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
        const QDateTime &lastUpdate,
        const QString userIdChange,
        const QString type_edit);
    void loadDosePpdToTable();
    void loadDosePpdHistoryToTable();
    void on_pushButton_DoseSaveChange_pressed();
    void on_btn_DoseHistoryFilterStop_pressed();
    void on_pushButton_DoseHistoryFile_pressed();
    void on_btn_DoseHistoryFilterGo_pressed();
    void setComboBoxValue(QComboBox* comboBox, int value);
    void on_tableWidget_Dose_cellPressed(int row, int column);
    void onSearchTextChanged(const QString &text);
    void checkRole();
    void on_pushButton_Back_pressed();
    void setupUserDutySearch();
    void on_btnUserDutySearch_pressed();
    void onUserDutySearchTextChanged(const QString &text);
    void showAllUserDuties();
    void on_pushButton_ppdFilterClean_pressed();
    void on_btn_ppdSearch_pressed();
    void setupPpdSearch();
    void onPpdSearchTextChanged(const QString &text);
    void showAllPpdRows();
    void on_pushButton_ppdFilter_pressed();
    void setupSearchUserComboBox(QComboBox* comboBox);
    void setupAllUserComboBoxes();
    void setupSearchDutyComboBox(QComboBox* comboBox);
    void setupSearchUsersDutyComboBox(QComboBox* comboBox);
    //void updateSetComboBox(QComboBox* comboBox);
    void on_pushButton_help_pressed();


    /**/
    void on_comboBox_listOfKas_kid_currentTextChanged(const QString &index);


    /*set*/
    void on_btnSetSave_kid_pressed();
    void on_btnSetChange_kid_pressed();
    void on_btnSetCreateNew_kid_pressed();
    void loadSetData_kid();
    void onSetSelectionChanged_kid(const QItemSelection &selected, const QItemSelection &deselected);
    bool addSetHistory_kid(int setId, const QString &setName, const QString &ipSet,
                           int setQuantity, const QString &setBlock, const QString &setNote,
                           const QString &changing_user_id, const QString &type_edit);

    /*kas*/
    void on_btnKasSave_kid_pressed();
    void on_btnKasChange_kid_pressed();
    void on_btnKasCreateNew_kid_pressed();
    void loadListOfKas_kid();
    void loadFilteredKasData_kid();
    void onKasSelectionChanged_kid(const QItemSelection &selected, const QItemSelection &deselected);
    bool addKasHistory_kid(int setId, int kasId, const QString &kasName, int kasHeight,
                           int kasWidth, const QString &kasBlock, const QString &kasNote,
                           const QString &changing_user_id, const QString &type_edit);

    /*mesh*/
    void on_btnMeshSave_kid_pressed();
    void on_btnMeshChange_kid_pressed();
    void on_btnMeshCreateNew_kid_pressed();
    //список ячеек - нет
    void loadFilteredMeshData_kid(QString kas_id);
    void onMeshSelectionChanged_kid(const QItemSelection &selected, const QItemSelection &deselected);
    bool addMeshHistory_kid(int setId, int kasId, int meshId, int userId,
                            const QString &dozTldId, int meshStatus, const QString &meshNote,
                            const QString &changing_user_id, const QString &type_edit);


    void setHistoryToTable_kid();
    void kasHistoryToTable_kid();
    void meshHistoryToTable_kid();

    void on_pushButton_SetHistoryFile_kid_pressed();
    void on_pushButton_KasHistoryFile_kid_pressed();
    void on_pushButton_MeshHistoryFile_kid_pressed();

    void on_btn_SetFilterGo_kid_pressed();
    void on_btn_SetFilterStop_kid_pressed();
    void on_btn_KasFilterGo_kid_pressed();
    void on_btn_KasFilterStop_kid_pressed();
    void on_btn_MeshFilterGo_kid_pressed();
    void on_btn_MeshFilterStop_kid_pressed();

    void updateListSetForHistory_kid();
    void updateComboBox_kid(QComboBox* combo, const QString& queryText,
                            const QString& bindValue1 = QString(),
                            const QString& bindValue2 = QString());

    void updateSetComboBox(QComboBox* comboBox);
    void updateSetComboBox(QComboBox* comboBox1, QComboBox* comboBox2);
    void updateSetComboBox(QComboBox* comboBox1, QComboBox* comboBox2, QComboBox* comboBox3);
    void onSetComboBoxChanged();
    void onKasComboBoxChanged();

    void comboboxUpdateAll();

    void updateSetKidComboBox(QComboBox* comboBox);
    void updateSetKidComboBox(QComboBox* comboBox1, QComboBox* comboBox2);
    void updateSetKidComboBox(QComboBox* comboBox1, QComboBox* comboBox2, QComboBox* comboBox3);
    void onSetKidComboBoxChanged();
    void onKasKidComboBoxChanged();

    void on_pushButton_userUpdate_pressed();

private:
    Welcome *welcomeWindow;
    //Ui::Welcome *ui;
    Ui::MainWindow *ui;
    int currentUserId;
    QSqlDatabase db;
    QByteArray photoData;
    QTimer m_doubleClickTimer;
    bool m_waitingForSecondClick = false;

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

    MeshStatusModel* m_currentMeshModel = nullptr;
    QString G_ip_database_cell;
    QString G_DatabaseName_cell;
    QString G_UserName_cell;
    QString G_Password_cell;

    void setupSearch();
    void showAllUsers();
    QPushButton *clearSearchButton;
    void updateComboBox(QComboBox* combo, const QString& queryText,
                        const QString& bindValue1 = "", const QString& bindValue2 = "");
    QPushButton *clearUserDutySearchButton;
    QPushButton *clearPpdSearchButton;


    QComboBox* m_setComboBox = nullptr;
    QComboBox* m_kasComboBox = nullptr;
    QComboBox* m_meshComboBox = nullptr;

    QComboBox* m_setKidComboBox = nullptr;
    QComboBox* m_kasKidComboBox = nullptr;
    QComboBox* m_meshKidComboBox = nullptr;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // MAINWINDOW_H
