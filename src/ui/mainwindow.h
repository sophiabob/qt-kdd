#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class UserRepository;
class User;

// ==================== подключаемые библиотеки ====================


#include <QMessageBox>
#include <QCryptographicHash>

#include <QMainWindow>
#include <QWidget>
#include <QObject>

// ядро Qt
#include <QtCore>
#include <QtGlobal>
#include <QTimer>
#include <QDate>
#include <QDateTime>
#include <QVector>
#include <QMap>
#include <QList>
#include <QVariant>
#include <QRegularExpression>

// интерфейс и виджеты
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QInputDialog>
#include <QDateEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QScrollArea>
#include <QStatusBar>
#include <QMovie>

// компоновка
#include <QVBoxLayout>

// работа с базами данных
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRecord>

// файлы, диалоги и системные операции
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QProcess>
#include <QDesktopServices>
#include <QStandardPaths>

// приложение и экран
#include <QApplication>
#include <QScreen>
#include <QMouseEvent>


#include <QCryptographicHash>
#include <QRandomGenerator>


// объявление пространства имён для UI, сгенерированного из .ui файла
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// предварительное объявление класса Welcome (чтобы избежать циклических включений)
class Welcome;

// ==================== структуры данных ====================

// данные о кассетнице
struct KasData {
    int set_id;
    int kas_id;
    QString kas_name;
    double kas_height;
    double kas_width;
    QString kas_block; // статус блокировки
    QString kas_note;  // примечание
    QString last_update;
};

// данные о ячейке
struct MeshData {
    int set_id;
    int kas_id;
    int mesh_id;
    int user_id;        // кто занял ячейку
    QString doz_tld_id; // идентификатор дозиметра
    QString mesh_status;
    QString mesh_note;
    QString last_update;
};

// структуры для поиска (чтобы хранить id и текст для комбобоксов)
struct DutyDataSearch {
    QString id;
    QString displayText; // что показываем пользователю
    QString searchText;  // по чему ищем
};

struct UsersDutyDataSearch {
    QString id;
    QString displayText;
    QString searchText;
};

struct UserDataSearch {
    QString id;
    QString displayText;
    QString searchText;
};

// псевдонимы для удобства
using KasArray = QMap<int, KasData>;
using MeshArray = QMap<int, MeshData>;

// регистрируем наши типы, чтобы их можно было использовать в QVariant
Q_DECLARE_METATYPE(KasData)
Q_DECLARE_METATYPE(MeshData)
Q_DECLARE_METATYPE(DutyDataSearch)
Q_DECLARE_METATYPE(UsersDutyDataSearch)
Q_DECLARE_METATYPE(UserDataSearch)
Q_DECLARE_METATYPE(QList<KasData>)
Q_DECLARE_METATYPE(QList<MeshData>)
Q_DECLARE_METATYPE(QList<DutyDataSearch>)
Q_DECLARE_METATYPE(QList<UsersDutyDataSearch>)
Q_DECLARE_METATYPE(QList<UserDataSearch>)

// ==================== главное окно приложения ====================

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int userId, QWidget *parent = nullptr);
    ~MainWindow();

    // маленькая утилита для определения операционной системы
    QString detectOS() {
#if defined(Q_OS_WIN)
        return "Windows";
#elif defined(Q_OS_LINUX)
        return "Linux";
#elif defined(Q_OS_UNIX)
        return "Unix";
#else
        return "Неизвестная ОС";
#endif
    }

private slots:
    // ==================== основные слоты (обработчики нажатий) ====================

    // работа с пользователями
    void on_btnCreateNewUser_pressed();
    void on_btnCreateUser_pressed();
    void on_btnChangeUser_pressed();
    void on_btn_userClear_pressed();
    void on_btnSearchUser_pressed();
    void updateUsersDatabase(); // обновляем список пользователей в интерфейсе
    void checkRole(); // проверяем права текущего пользователя

    // фотографии пользователей
    void on_btnAddPhoto_pressed();
    void on_btnChangePhoto_pressed();

    // управление сетами, кассетницами и ячейками (основное)
    void on_btnSetSave_pressed();
    void on_btnSetChange_pressed();
    void on_btnSetCreateNew_pressed();

    void on_btnKasSave_pressed();
    void on_btnKasChange_pressed();
    void on_btnKasCreateNew_pressed();

    void on_btnMeshSave_pressed();
    void on_btnMeshChange_pressed();
    void on_btnMeshCreateNew_pressed();

    // управление сетами, кассетницами и ячейками (КИД - история изменений)
    void on_btnSetSave_kid_pressed();
    void on_btnSetChange_kid_pressed();
    void on_btnSetCreateNew_kid_pressed();

    void on_btnKasSave_kid_pressed();
    void on_btnKasChange_kid_pressed();
    void on_btnKasCreateNew_kid_pressed();

    void on_btnMeshSave_kid_pressed();
    void on_btnMeshChange_kid_pressed();
    void on_btnMeshCreateNew_kid_pressed();

    // дозы и PPD (персональные дозиметры)
    void on_pushButton_DoseCleanInput_pressed();
    void on_pushButton_DoseSaveNew_pressed();
    void on_pushButton_DoseSaveChange_pressed();
    void on_pushButton_ppdFilterClean_pressed();
    void on_btn_ppdSearch_pressed();
    void on_pushButton_ppdFilter_pressed();

    // дежурства и назначения
    void on_btn_saveNewDuty_pressed();
    void on_btn_saveNewDutyUser_pressed();
    void on_btn_createNewDuty_pressed();
    void on_btn_saveChangeDuty_pressed();
    void on_btn_saveChangeUserDuty_pressed();
    void refresh_duty_table(); // обновляем таблицу дежурств
    void tableUsersDutyUpd(); // обновляем таблицу назначений пользователей на дежурства

    // загрузка истории в таблицы
    void setHistoryToTable();
    void kasHistoryToTable();
    void meshHistoryToTable();
    void usersHistoryToTable();
    void loadDutyHistoryToTable();
    void loadDutyUsersHistoryToTable();
    void loadDosePpdHistoryToTable();

    // история для КИД
    void setHistoryToTable_kid();
    void kasHistoryToTable_kid();
    void meshHistoryToTable_kid();

    // кнопки фильтрации истории (включить/выключить)
    void on_btn_SetFilterGo_pressed();
    void on_btn_SetFilterStop_pressed();
    void on_btn_KasFilterGo_pressed();
    void on_btn_KasFilterStop_pressed();
    void on_btn_MeshFilterGo_pressed();
    void on_btn_MeshFilterStop_pressed();
    void on_btn_DoseHistoryFilterGo_pressed();
    void on_btn_DoseHistoryFilterStop_pressed();
    void on_btn_dutyFilterGo_pressed();
    void on_btn_dutyFilterStop_pressed();
    void on_btn_dutyUsersFilterGo_pressed();
    void on_btn_dutyUsersFilterStop_pressed();
    void on_btnUserFilterGo_2_pressed();
    void on_btnUserFilterStop_2_pressed();

    // фильтрация для истории КИД
    void on_btn_SetFilterGo_kid_pressed();
    void on_btn_SetFilterStop_kid_pressed();
    void on_btn_KasFilterGo_kid_pressed();
    void on_btn_KasFilterStop_kid_pressed();
    void on_btn_MeshFilterGo_kid_pressed();
    void on_btn_MeshFilterStop_kid_pressed();

    // экспорт истории в файлы (например, CSV)
    void on_pushButton_SetHistoryFile_pressed();
    void on_pushButton_KasHistoryFile_pressed();
    void on_pushButton_MeshHistoryFile_pressed();
    void on_pushButton_dutyHistoryFile_pressed();
    void on_pushButton_dutyUsersFile_2_pressed();
    void on_pushButton_usersHistoryFile_pressed();
    void on_pushButton_DoseHistoryFile_pressed();

    // экспорт для КИД
    void on_pushButton_SetHistoryFile_kid_pressed();
    void on_pushButton_KasHistoryFile_kid_pressed();
    void on_pushButton_MeshHistoryFile_kid_pressed();

    // навигация и общие действия
    void on_pushButton_Exit_pressed();
    void on_pushButton_Back_pressed();
    void on_pushButton_help_pressed();
    void on_pushButton_onSet_pressed();
    void on_pushButton_onKas_pressed();
    void on_pushButton_2_pressed();
    void on_pushButton_3_clicked();

    // отчёты
    void on_btn_createReport_pressed();

    // загрузка и отображение данных
    void loadSetData();
    void loadSetData_kid();
    void loadListOfKas();
    void loadListOfKas_kid();
    void loadFilteredKasData();
    void loadFilteredKasData_kid();
    void loadFilteredMeshData(QString kas_id);
    void loadFilteredMeshData_kid(QString kas_id);
    void loadDosePpdToTable();
    void updateDataFromDatabase(); // обновляем различные данные из БД

    // обработка выбора строк в таблицах
    void onSetSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onKasSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onMeshSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void on_tableWidget_Dose_cellPressed(int row, int column);
    void on_tableDuty_cellPressed(int row, int column);
    void on_table_userDuty_cellPressed(int row, int column);

    // обработка выбора для КИД
    void onSetSelectionChanged_kid(const QItemSelection &selected, const QItemSelection &deselected);
    void onKasSelectionChanged_kid(const QItemSelection &selected, const QItemSelection &deselected);
    void onMeshSelectionChanged_kid(const QItemSelection &selected, const QItemSelection &deselected);

    // изменение выбора в комбобоксах
    void on_comboBox_listOfKas_currentTextChanged(const QString &arg1);
    void on_comboBox_listOfKas_kid_currentTextChanged(const QString &index);

    // поиск и фильтрация данных
    void on_btnKddFilterGo_clicked();
    void on_btn_userDutySearch_pressed();
    void on_btn_userDutyClear_pressed();
    void onSearchTextChanged(const QString &text);
    void onUserDutySearchTextChanged(const QString &text);
    void onPpdSearchTextChanged(const QString &text);

    // ==================== добавление записей в историю ====================

    // добавить запись об изменении пользователя
    bool addUserHistory(
        int userId,
        const QString &login,
        const QString &password,
        const QString &name0, // фамилия
        const QString &name1, // имя
        const QString &name2, // отчество
        const QString &gender,
        const QDate &birthday,
        const QString &role,
        int tabNum, // табельный номер
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
        const QString &changing_user_id, // кто внёс изменение
        const QString &type_edit // тип изменения: create, update, delete
        );

    // добавить запись об изменении назначения на дежурство
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

    // добавить запись об изменении дежурства
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

    // добавить запись об изменении сета
    bool addSetHistory(int setId, const QString &setName, const QString &ipSet,
                       int setQuantity, const QString &setBlock, const QString &setNote,
                       const QString &changing_user_id, const QString &type_edit);

    // добавить запись об изменении кассетницы
    bool addKasHistory(int setId, int kasId, const QString &kasName, int kasHeight,
                       int kasWidth, const QString &kasBlock, const QString &kasNote,
                       const QString &changing_user_id, const QString &type_edit);

    // добавить запись об изменении ячейки
    bool addMeshHistory(int setId, int kasId, int meshId, int userId,
                        const QString &dozTldId, int meshStatus, const QString &meshNote,
                        const QString &changing_user_id, const QString &type_edit);

    // аналогичные методы для истории КИД
    bool addSetHistory_kid(int setId, const QString &setName, const QString &ipSet,
                           int setQuantity, const QString &setBlock, const QString &setNote,
                           const QString &changing_user_id, const QString &type_edit);

    bool addKasHistory_kid(int setId, int kasId, const QString &kasName, int kasHeight,
                           int kasWidth, const QString &kasBlock, const QString &kasNote,
                           const QString &changing_user_id, const QString &type_edit);

    bool addMeshHistory_kid(int setId, int kasId, int meshId, int userId,
                            const QString &dozTldId, int meshStatus, const QString &meshNote,
                            const QString &changing_user_id, const QString &type_edit);

    // добавить запись об изменении дозы PPD
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
        const QString type_edit
        );

    // ==================== работа с комбобоксами ====================

    // обновление комбобоксов для сетов (основное)
    void updateSetComboBox(QComboBox* comboBox);
    void updateSetComboBox(QComboBox* comboBox1, QComboBox* comboBox2);
    void updateSetComboBox(QComboBox* comboBox1, QComboBox* comboBox2, QComboBox* comboBox3);
    void onSetComboBoxChanged();
    void onKasComboBoxChanged();

    // обновление комбобоксов для сетов (КИД)
    void updateSetKidComboBox(QComboBox* comboBox);
    void updateSetKidComboBox(QComboBox* comboBox1, QComboBox* comboBox2);
    void updateSetKidComboBox(QComboBox* comboBox1, QComboBox* comboBox2, QComboBox* comboBox3);
    void onSetKidComboBoxChanged();
    void onKasKidComboBoxChanged();

    // вспомогательные методы для работы с комбобоксами
    void comboboxUpdateAll(); // обновить все комбобоксы разом
    void updateListSetForHistory(); // обновить список сетов для истории
    void updateListSetForHistory_kid(); // то же для КИД
    void setComboBoxValue(QComboBox* comboBox, int value); // установить значение в комбобокс
    int extractIdFromComboBox(const QString& text); // извлечь ID из текста комбобокса

    // общие методы обновления комбобоксов
    void updateComboBox(QComboBox* combo, const QString& queryText,
                        const QString& bindValue1 = QString(),
                        const QString& bindValue2 = QString());

    void updateComboBox_kid(QComboBox* combo, const QString& queryText,
                            const QString& bindValue1 = QString(),
                            const QString& bindValue2 = QString());

    // настройка комбобоксов с поиском
    void setupSearch();
    void setupUserDutySearch();
    void setupPpdSearch();
    void setupSearchUserComboBox(QComboBox* comboBox);
    void setupSearchDutyComboBox(QComboBox* comboBox);
    void setupSearchUsersDutyComboBox(QComboBox* comboBox);
    void setupAllUserComboBoxes();

    // вспомогательные методы для поиска
    void showAllUsers();
    void showAllUserDuties();
    void showAllPpdRows();

private:
    UserRepository* m_repo;
    // ==================== члены класса ====================

    // интерфейс и окна
    Ui::MainWindow *ui; // указатель на сгенерированный UI
    Welcome *welcomeWindow = nullptr; // окно приветствия (может быть nullptr)

    // данные текущего пользователя
    int currentUserId; // ID пользователя, который вошёл в систему
    QByteArray photoData; // фотография пользователя в бинарном виде

    // подключение к базе данных
    QSqlDatabase db;

    // модели данных для отображения
    MeshStatusModel* m_currentMeshModel = nullptr;

    // данные для поиска (кешируем, чтобы не ходить в БД каждый раз)
    struct UserData {
        QString id;
        QString displayText; // что показываем в комбобоксе
        QString searchText;  // по чему фильтруем
    };
    QVector<UserData> m_usersData;

    struct DutyData {
        QString id;
        QString displayText;
        QString searchText;
    };
    QVector<DutyData> m_dutiesData;

    // конфигурация подключения к БД (возможно для работы с ячейками)
    QString G_ip_database_cell;
    QString G_DatabaseName_cell;
    QString G_UserName_cell;
    QString G_Password_cell;

    // указатели на комбобоксы для связи между ними
    QComboBox* m_setComboBox = nullptr;
    QComboBox* m_kasComboBox = nullptr;
    QComboBox* m_meshComboBox = nullptr;

    QComboBox* m_setKidComboBox = nullptr;
    QComboBox* m_kasKidComboBox = nullptr;
    QComboBox* m_meshKidComboBox = nullptr;

    // кнопки очистки поиска
    QPushButton *clearSearchButton = nullptr;
    QPushButton *clearUserDutySearchButton = nullptr;
    QPushButton *clearPpdSearchButton = nullptr;

    // для обработки двойного клика
    QTimer m_doubleClickTimer;
    bool m_waitingForSecondClick = false;



    // Хэширование с солью и key stretching (защита от перебора)
    QString hashPasswordSecure(const QString& password);

    // Проверка пароля против сохранённого хэша
    bool verifyPasswordSecure(const QString& password, const QString& storedHash);





    // структура привязки для виджетов
    struct FieldMap {
        QString name;
        QWidget* widget;
        enum Type { Text, Int, Float, DateTime, Bool } type;
        // Лямбда: ВИДЖЕТ → User (чтение из формы)
        std::function<void(User&, const QVariant&)> readToUser;
        // Лямбда: User → ВИДЖЕТ (запись в форму)
        std::function<void(QWidget*, const QVariant&)> writeFromUser;
    };
    QVector<FieldMap> m_fieldMaps;

    void initFieldMapsUser();

    User formToUser() const;           // Считать все поля из формы в User
    void userToForm(const User& user); // Записать все поля из User в форму

    // работа с одним полем (если нужно точечно)
    QVariant readField(const QString& fieldName) const;
    void writeField(const QString& fieldName, const QVariant& value);



protected:
    // фильтр событий (например, для обработки специальных кликов)
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // MAINWINDOW_H
