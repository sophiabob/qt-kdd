#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "welcome.h"
#include "ui_welcome.h"

#include "doubleclickbutton.h"
#include "../models/meshstatusmodel.h"


#include "../repositories/user_repository.h"


#include "../repositories/result.h"
#include "../repositories/user_repository.h"
#include "../models/user.h"




MainWindow::MainWindow(int userId, QWidget *parent)
    : QMainWindow(parent),
      currentUserId(userId)
    , ui(new Ui::MainWindow)
    , clearSearchButton(nullptr)
    , clearUserDutySearchButton(nullptr)
{/*
    // инициализация модели
    MainWindow::MainWindow(UserRepository* repo, QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), m_repo(repo)  // ← Инициализация указателя
    {
        ui->setupUi(this);

        // Проверка, что репозиторий передан
        if (!m_repo) {
            qCritical() << "MainWindow: repository is null!";
            return;
        }
    }*/





    ui->setupUi(this);
    //qDebug() << "Текущий пользователь ID:" << currentUserId;
    //ui->statusbar->showMessage("f,hfrflfmhf");
    //ui->statusbar->setVisible(true);
    //setStatusBar(new QStatusBar(this));  // Восстановление
    setStatusBar(new QStatusBar(this)); // Создаём статусбар программно
    ui->statusbar->setVisible(true);
    ui->statusbar->showMessage("Добро пожаловать в систему!");

    if (ui->statusbar->isVisible()) {
        //qDebug() << "Статусбар veden";
    } else {
        //qDebug() << "Статусбар not viden"; //почему??
    }

    //query.value("max_id").toInt();
   // this->showFullScreen(); //на очень полный экран

    qDebug() << "----\nMainWindow:";
    Welcome welcome;
    bool connected = welcome.createConnection();

    QString os = detectOS();
    if (os == "Windows") {
        //this->showFullScreen();
    } else if (os == "Linux") {
        this->showFullScreen();
    } else {
        qDebug() << "ОС не распознана";
    }


   //MeshStatusModel* m_currentMeshModel = nullptr;  // ← Теперь работает

    //========================================== Начало работы с пользователями ======================================================================

    // Вектор всех полей формы
    const std::vector<FieldRecord> userFormFields = {
        // === ОСНОВНЫЕ ПОЛЯ ===
        {"id",            ui->label_51,         FieldRecord::Label},
        {"surname",       ui->inputName0,       FieldRecord::Text},
        {"firstName",     ui->inputName1,       FieldRecord::Text},
        {"patronymic",    ui->inputName2,       FieldRecord::Text},
        {"login",         ui->inputLogin,       FieldRecord::Text},
        {"password",      ui->inputPassword,    FieldRecord::Text},
        {"accessCode",    ui->inputCode,        FieldRecord::Text},
        {"snils",         ui->inputSnils,       FieldRecord::Text},
        {"blockReason",   ui->selectUserBlock,  FieldRecord::Text},

        // === ЧИСЛОВЫЕ ПОЛЯ (SpinBox) ===
        {"employeeNumber", ui->inputIntTab,     FieldRecord::Int},
        {"cardId",         ui->inputCardId,     FieldRecord::Text},
        {"dosimetrTldId",  ui->inputIntDoz,     FieldRecord::Text},  // Если текст, или Int если число
        {"startDoz",       ui->inputUserStartDoz, FieldRecord::Int},
        {"finishDoz",      ui->inputUserFinishDoz, FieldRecord::Int},

        // === ДОЗИМЕТРИЯ (Float) ===
        {"annualDose",     ui->spinUserDoseYear,    FieldRecord::Float},
        {"currentYearDose", ui->spinUserDoseYearNow, FieldRecord::Float},
        {"currentYearDosePPD", ui->spinUserDoseYearPpd, FieldRecord::Float},

        // === ДАТЫ ===
        {"lastCellUpdate", ui->inputDateCellDate, FieldRecord::Date},
        {"birthDate",      ui->dateUserBirthday,  FieldRecord::Date},
        {"lastUpdate",     ui->dateUserLastUpdate, FieldRecord::Label}, // Если QLabel с текстом
        {"startUsed",      ui->dateUserSrart,     FieldRecord::Date},
        {"finishUsed",     ui->dateUserFinish,    FieldRecord::Date},

        // === КОМБОБОКСЫ ===
        {"role",           ui->selectRole,        FieldRecord::ComboBox},
        {"gender",         ui->selectGender,      FieldRecord::ComboBox},
        {"department",     ui->selectDepartment,  FieldRecord::ComboBox},
    };

    //=========================================== Конец работы с пользователями =====================================================================


    ui->tabWidget_3->setTabVisible(2, false);

    ui->spinUserDoseYear->setValue(5);
    ui->tabWidget_4->setTabVisible(3, false); //скрыть вкладку история комплектов kid

    //скрыть кнопк и выпадающий список
    ui->combobox_createReport->hide();



    //для пользоватлей и их прокрток
    ui->scrollArea_5->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea_5->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollArea_5->setWidgetResizable(true);

    ui->scrollAreaWidgetContents_5->setMinimumWidth(300);

    // Настройка таймера (300ms - время ожидания второго нажатия)
    m_doubleClickTimer.setSingleShot(true);
    m_doubleClickTimer.setInterval(300);
    connect(&m_doubleClickTimer, &QTimer::timeout, this, [this]() {
        m_waitingForSecondClick = false;
        // Выполнить обычное действие при одинарном нажатии
        // Получаем текущий индекс
        int currentIndex = ui->tabWidget_kas->currentIndex();

        // Переходим на следующую вкладку (с проверкой на выход за границы)
        int nextIndex = (currentIndex + 1) % ui->tabWidget_kas->count();
        ui->tabWidget_kas->setCurrentIndex(nextIndex);
    });


    //убрать ограничения на числа

    QList<QSpinBox*> spinBoxes = findChildren<QSpinBox*>();
    foreach(QSpinBox* spinBox, spinBoxes) {
        spinBox->setMinimum(0);
        spinBox->setMaximum(INT_MAX);
    }

    //значение для дробных чисел
    QList<QDoubleSpinBox*> doubleSpinBoxes = findChildren<QDoubleSpinBox*>();
    foreach(QDoubleSpinBox* doubleSpinBox, doubleSpinBoxes) {
        doubleSpinBox->setRange(0, 100.0);
        doubleSpinBox->setDecimals(5);
        doubleSpinBox->setSingleStep(0.01);
        doubleSpinBox->setLocale(QLocale::C);
        doubleSpinBox->setValue(0.3);
        doubleSpinBox->setValue(0.0);
    }

    /*стартовые таблицы*/
    loadSetData();
    //loadFilteredKasData();
    usersHistoryToTable();
    loadDutyUsersHistoryToTable();
    loadDutyHistoryToTable();

    setHistoryToTable();
    kasHistoryToTable();
    meshHistoryToTable();
    comboboxUpdateAll();


    updateListSetForHistory();

    loadDosePpdToTable();
    loadDosePpdHistoryToTable();

    setupSearch();

    //setupSearch();
    setupUserDutySearch();
    setupPpdSearch();


    loadSetData_kid();
    //loadListOfKas_kid(); - ?

    updateSetComboBox(ui->select_kdd_set_1);

    setupSearchUsersDutyComboBox(ui->comboBox_DoseUsersDutyId);


    setupSearchDutyComboBox(ui->select_dutyFilter);
    setupSearchDutyComboBox(ui->duty_id);

    qRegisterMetaType<KasData>("KasData");
    qRegisterMetaType<MeshData>("MeshData");
    qRegisterMetaType<DutyDataSearch>("DutyDataSearch");
    qRegisterMetaType<UsersDutyDataSearch>("UsersDutyDataSearch");
    qRegisterMetaType<UserDataSearch>("UserDataSearsh");
    qRegisterMetaType<QList<KasData>>("QList<KasData>");
    qRegisterMetaType<QList<MeshData>>("QList<MeshData>");
    qRegisterMetaType<QList<DutyDataSearch>>("QList<DutyDataSearch>");
    qRegisterMetaType<QList<UsersDutyDataSearch>>("QList<UsersDutyDataSearch>");
    qRegisterMetaType<QList<UserDataSearch>>("QList<UserDataSearsh>");

    // Вывод пользователей
    QSqlQuery query;

    updateUsersDatabase();


    // Сначала привязываем значение
    query.prepare("SELECT name_0, name_1, name_2 FROM users WHERE user_id = :user_id");
    query.bindValue(":user_id", currentUserId);

    if (query.exec()) {
        if (query.next()) {
            QString name0 = query.value("name_0").toString();
            QString name1 = query.value("name_1").toString();
            QString name2 = query.value("name_2").toString();

            QString userId_text = name0 + " " + name1 + " " + name2;
            ui->label_userId->setText(userId_text);

            checkRole();
        } else {
            //qDebug() << "Пользователь с ID" << currentUserId << "не найден";
            ui->label_userId->setText("Пользователь не найден");
        }
    } else {
        //qDebug() << "Ошибка выполнения запроса1:" << query.lastError().text();
        ui->label_userId->setText("Ошибка загрузки данных currentUserId");
    }

    ui->spinBox_time->setValue(43200);
    ui->spinUserDoseYear->setValue(5);

    ui->doubleSpinBox_dose_a_1->setValue(0.1);
    ui->doubleSpinBox_dose_a_2->setValue(1);

    ui->doubleSpinBox_rate_a_1->setValue(0.1);
    ui->doubleSpinBox_rate_a_2->setValue(1);

    ui->doubleSpinBox_dose_b_1->setValue(0.1);
    ui->doubleSpinBox_dose_b_2->setValue(1);

    ui->doubleSpinBox_rate_b_1->setValue(0.1);
    ui->doubleSpinBox_rate_b_2->setValue(1);

    ui->doubleSpinBox_dose_g_1->setValue(0.1);
    ui->doubleSpinBox_dose_g_2->setValue(1);

    ui->doubleSpinBox_rate_g_1->setValue(0.1);
    ui->doubleSpinBox_rate_g_2->setValue(1);

    ui->doubleSpinBox_dose_n_1->setValue(0.1);
    ui->doubleSpinBox_dose_n_2->setValue(1);

    ui->doubleSpinBox_rate_n_1->setValue(0.1);
    ui->doubleSpinBox_rate_n_2->setValue(1);


//обновление таблицы нарядов
    refresh_duty_table();
    tableUsersDutyUpd();
}/*
void MainWindow::setRepository(UserRepository* repo) {
   // m_repoLocal = repo;  // Локальная переменная или использование сразу

    this->setProperty("userRepository", QVariant::fromValue(repo));
}*/

void MainWindow::on_btnSetChange_pressed(){

    QSqlQuery query;

    QString set_id = ui->textSetId->text();
    int set_quantityStart = 0;
    query.prepare("SELECT set_quantity FROM set WHERE set_id = :set_id");
    query.bindValue(":set_id", set_id);

    if (!query.exec()) {
        qCritical() << "Ошибка выполнения запроса2:" << query.lastError().text();
        return;
    }
    if (query.next()) {
        set_quantityStart = query.value("set_quantity").toInt();
    }

    QString SetName = ui->inputSetName->text();
    QString SetNote = ui->lineSetNote->text();
    QString SetIp = ui->inputSetIp->text();

    QString set_block = ui->comboSetStatus->currentText();


    // Указываем часовой пояс для Москвы (UTC+3)
    QTimeZone moscowTimeZone("Europe/Moscow");
    // Получаем текущее время и дату в UTC
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    // Конвертируем время в московское
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString last_update = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");
    //qDebug() << "Current date and time in Moscow:" << dateTimeString;
    //last_update - Настоящее время



    int SetQuantity = ui->inputIntSetQuantity->value();
    if (SetQuantity > set_quantityStart){
        int newKas = SetQuantity - set_quantityStart;
        //QMessageBox::information(this, "Уведомление", "Будет создано кассетниц: " + QString::number(newKas)); //QString::number(set_quantityStart) + QString::number(SetQuantity) +

        int maxKasNumber = 0;
        query.prepare("SELECT MAX(kas_id) FROM kas WHERE set_id = :set_id");
        query.bindValue(":set_id", set_id);

        if (query.exec() && query.next()) {
            maxKasNumber = query.value(0).toInt();
        } else {
            qWarning() << "Ошибка при получении максимального номера кассеты:" << query.lastError().text();
        }

        for (int i = 1; i <= newKas; ++i) {
            //:set_id, :kas_id, :kas_name, :kas_height, :kas_width, :kas_block, :kas_note, :last_update
            query.prepare("INSERT INTO kas (set_id, kas_id, kas_height, kas_width, kas_block, last_update) VALUES (:set_id, :kas_id, 1, 1, :kas_block, :last_update)");
            query.bindValue(":set_id", set_id);
            query.bindValue(":kas_block", "Разблокирована");
            query.bindValue(":kas_id", maxKasNumber + i);

            if (!query.exec()) {
                qCritical() << "Ошибка при создании кассеты" << i << ":" << query.lastError().text();
            }
            qDebug() << "Новое kas_id " << maxKasNumber + i;

            // Создаем ячейки по строкам
            query.prepare("INSERT INTO mesh (set_id, kas_id, mesh_id, user_id, doz_tld_id, mesh_status, mesh_note, last_update) VALUES (:set_id, :kas_id, :mesh_id, :user_id, :doz_tld_id, :mesh_status, :mesh_note, :last_update)");
            query.bindValue(":set_id", set_id);
            query.bindValue(":kas_id", maxKasNumber + i);
            query.bindValue(":mesh_id", 1); // Порядковый номер ячейки
            query.bindValue(":user_id", QVariant(QVariant::Int)); // NULL значение
            query.bindValue(":doz_tld_id", QVariant(QVariant::Int)); // NULL значение
            query.bindValue(":mesh_status", "0");
            query.bindValue(":mesh_note", "");//, QString("Ячейка %1-%2").arg(row).arg(col));
            query.bindValue(":last_update", QDateTime::currentDateTime());

            if (!query.exec()) {
                qCritical() << "Ошибка при создании ячейки" << i << ":" << query.lastError().text();
                //return; // или break в зависимости от логики
            }

        }

        //qDebug() << "Успешно создано" << newKas << "новых кассет для набора" << set_id;
    }
    else if (SetQuantity < set_quantityStart){
        int delKas = set_quantityStart - SetQuantity;
       // QMessageBox::information(this, "Уведомление", "Будет удалено кассетниц: " + QString::number(delKas));

        query.prepare("DELETE FROM kas WHERE set_id = :set_id AND kas_id > :delKas");
        query.bindValue(":set_id", set_id);
        query.bindValue(":delKas", delKas);
        query.bindValue(":set_id", set_id);
        query.bindValue(":delKas", SetQuantity);

        if (!query.exec()) {
            qCritical() << "Ошибка при удалении последних" << delKas << "кассет:"
                       << query.lastError().text();
            return;
        }
        //qDebug() << "Успешно удалено" << delKas << "последних кассет для набора" << set_id;

        // Удаление из таблицы mesh
        query.prepare("DELETE FROM mesh WHERE set_id = :set_id AND kas_id > :delKas");
        query.bindValue(":set_id", set_id);
        query.bindValue(":delKas", SetQuantity);
        if (!query.exec()) {
            qCritical() << "Ошибка при удалении из mesh:" << query.lastError().text();
            return; // или обработка ошибки
        }
    }



    query.prepare(R"(
        UPDATE set
        SET
            set_name = :set_name,
            ip_set = :ip_set,
            set_quantity = :set_quantity,
            set_block = :set_block,
            set_note = :set_note,
            last_update = :last_update
        WHERE set_id = :set_id;
        )");

    // Привязка параметров
    query.bindValue(":set_id", set_id);
    query.bindValue(":set_name", SetName);
    query.bindValue(":ip_set", SetIp);
    query.bindValue(":set_quantity", SetQuantity);
    query.bindValue(":set_block", set_block);
    query.bindValue(":set_note", SetNote);
    query.bindValue(":last_update", last_update);


    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Данные изменены!");
    } else {
        //QMessageBox::critical(this, "Ошибка2", "Не удалось добавить данные!", query.lastError().text());
        qDebug() << query.lastError().text();
    }

    //обновить таблицу
    loadSetData();
    loadFilteredKasData();
    loadFilteredMeshData("1");





    //в историю
    QString for_user_id = ui->label_userId->text();
    QString changing_user_id;
    QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString name_0 = parts[0];
    QString name_1 = parts[1];
    QString name_2 = parts[2];
    query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
    query.bindValue(":name_0", name_0);
    query.bindValue(":name_1", name_1);
    query.bindValue(":name_2", name_2);
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка02", "Не удалось выполнить запрос: " + query.lastError().text());
        qDebug() << "Ошибка поиска id SQL:" << query.lastQuery() << query.lastError().text();
        return;
    }
    if (query.next()) {
        changing_user_id = query.value("user_id").toString();
    } else {
        QMessageBox::information(this, "Информация", "Пользователь не найден");
    }

    QString type_edit = "change"; // или "update", "delete" в зависимости от операции

    bool success = addSetHistory(
        set_id.toInt(),           // ID комплекта
        SetName,         // Название комплекта
        SetIp,           // IP адрес
        SetQuantity,     // Количество
        set_block,        // Блокировка
        SetNote,         // Примечание
        changing_user_id, // ID пользователя, который внес изменения
        type_edit         // Тип операции
    );

    if (success) {
        qDebug() << "Данные успешно сохранены в таблицу set_history";
        setHistoryToTable(); // Обновляем таблицу истории
    } else {
        qDebug() << "Ошибка при сохранении истории комплекта";
    }
    setHistoryToTable();

    on_btnSetCreateNew_pressed();

    updateSetComboBox(ui->select_kdd_set_1);
    comboboxUpdateAll();

}


void MainWindow::updateDataFromDatabase() {

    // Проверяем подключение к базе данных
    QSqlDatabase db = QSqlDatabase::database(); // Получаем текущее подключение
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT user_id, name_0, name_1, name_2 login FROM users");

    while (query.next()) {
        QString name0 = query.value("name_0").toString();
        QString name1 = query.value("name_1").toString();
        QString name2 = query.value("name_2").toString();
        QString login = query.value("login").toString();
        //QString user_id_str = query.value("user_id").toString();//.toInt();

        QString fullName = QString("%1 %2 %3 - %4").arg(name0, name1, name2, login);
        //qDebug() << fullName;
        QPushButton *button = new QPushButton(fullName, this);
        //кнопки тут
        //verticalUsersList
        ui->verticalUsersList->addWidget(button);
    }

}


void MainWindow::on_btnAddPhoto_pressed() //добавили фоточку
{

    QSqlQuery query;
    int user_id = 1; // Объявляем переменную до условий

    if (ui->label_51->text() == "-") {
        if (query.exec("SELECT MAX(user_id) AS max_id FROM users;")) {
            if (query.next()) {
                // Если в таблице есть записи, берём максимальный ID + 1
                user_id = query.value("max_id").toInt() + 1;
                //qDebug() << "Новый user_id:" << user_id;
            } else {
                //qDebug() << "Таблица пуста, используем user_id = 1";
            }
        } else {
            qDebug() << "Ошибка выполнения запроса3:" << query.lastError().text();
        }
    }
    else {
        user_id = ui->label_51->text().toInt(); // Преобразуем текст в число
    }
    QString filePath;

    QString os = detectOS();
    if (os == "Windows") {
        filePath = QFileDialog::getOpenFileName(this, "Выберите фото", "", "Images (*.png *.jpg *.jpeg)");
    }
    else if (os == "Linux") {
        filePath = "/home/sds/user_id.png";
    }
    else {
        qDebug() << "Неизвестная ОС:" << os;
        return;
    }


    QFile file(filePath);
    if (!file.exists()) {
        qWarning() << "Файл не существует:" << filePath;
        return;
    }

    // Дальнейшая работа с файлом
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Ошибка открытия файла:" << file.errorString();
        return;
    }



    file.open(QIODevice::ReadOnly);
    QByteArray baToSend = file.readAll();

    QString rez = "";
    //query.prepare("insert into user_photo values (8,:img)");
    query.prepare("INSERT INTO user_photo (user_id, user_photo) VALUES (:user_id, :photo_data)");
    query.bindValue(":user_id", user_id);
    query.bindValue(":photo_data", baToSend);

    if (!query.exec()) {
        qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        qDebug() << "Данные успешно добавлены!";
    }

    //ui->labelPhoto->setPixmap(pixmap.scaled(150, 150, Qt::KeepAspectRatio));
    //query.bindValue(":img",baToSend);
    //qDebug() << "exec send query: " << query.exec();

    query.clear();
    //превью фото
    QPixmap pixmap;
    // Используем photo_data из текущего элемента массива, а не необъявленную переменную photo
    if (pixmap.loadFromData(baToSend)) {
        ui->labelPhoto->setPixmap(pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        //qDebug() << QString("Найдено фото для пользователя ID: %1").arg(ar_photoForUser[i].user_id);

        // Прерываем цикл после нахождения совпадения
        //break;
    } else {
        qDebug() << "Не удалось загрузить фото";
    }
    file.close();

    updateUsersDatabase();
}

void MainWindow::on_btnChangePhoto_pressed(){

    //qDebug() << "Будем фото менять";


    QString user_id = ui->label_51->text();
    //query.prepare("insert into user_photo values (8,:img)");
    QSqlQuery query;
    query.prepare("DELETE FROM user_photo WHERE user_id = :user_id");
    query.bindValue(":user_id", user_id);
    if (!query.exec()) {
        qDebug() << "Ошибка удаления старого фото:" << query.lastError().text();
        //return false;
    }



    QString filePath;

    QString os = detectOS();
    if (os == "Windows") {
        filePath = QFileDialog::getOpenFileName(this, "Выберите фото", "", "Images (*.png *.jpg *.jpeg)");
    }
    else if (os == "Linux") {
        filePath = "/home/sds/user_id.png";
    }
    else {
        qDebug() << "Неизвестная ОС:" << os;
        return;
    }


    QFile file(filePath);
    if (!file.exists()) {
        qWarning() << "Файл не существует:" << filePath;
        return;
    }

    // Дальнейшая работа с файлом
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Ошибка открытия файла:" << file.errorString();
        return;
    }

    file.open(QIODevice::ReadOnly);
    QByteArray baToSend = file.readAll();

    query.prepare("INSERT INTO user_photo (user_id, user_photo) VALUES (:user_id, :photo_data)");;
    query.bindValue(":user_id", user_id);
    query.bindValue(":photo_data", baToSend);

    if (!query.exec()) {
        //qDebug() << "Ошибка вставки данных:" << query.lastError().text();
    } else {
        //qDebug() << "Данные успешно добавлены!";
    }

    query.clear();
    //превью фото1
    QPixmap pixmap;
    if (pixmap.loadFromData(baToSend)) {
        ui->labelPhoto->setPixmap(pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        //qDebug() << QString("Найдено фото для пользователя ID: %1").arg(ar_photoForUser[i].user_id);
    } else {
        qDebug() << "Не удалось загрузить фото";
    }
}

#pragma region "Работа с пользователями" //======================== Начало пользователя ====================================================================================================================

// хэширование пароля
QString MainWindow::hashPasswordSecure(const QString& password)
{
    // 1. Генерируем случайную соль
    QByteArray salt(16, 0);
    for (int i = 0; i < salt.size(); ++i) {
        salt[i] = static_cast<char>(QRandomGenerator::system()->generate() % 256);
    }

    // 2. Многократное хэширование (key stretching)
    QByteArray hash = (password + salt).toUtf8();
    for (int i = 0; i < 10000; ++i) {  // 10k итераций = ~10ms
        hash = QCryptographicHash::hash(hash + salt, QCryptographicHash::Sha256);
    }

    // 3. Возвращаем соль + хэш (чтобы потом проверить)
    return salt.toBase64() + ":" + hash.toHex();
}

bool verifyPassword(const QString& password, const QString& storedHash)
{
    // Разделяем соль и хэш
    auto parts = storedHash.split(':');
    if (parts.size() != 2) return false;

    QByteArray salt = QByteArray::fromBase64(parts[0].toUtf8());
    QByteArray expectedHash = QByteArray::fromHex(parts[1].toUtf8());

    // Повторяем ту же процедуру хэширования
    QByteArray hash = (password.toUtf8() + salt);
    for (int i = 0; i < 10000; ++i) {
        hash = QCryptographicHash::hash(hash + salt, QCryptographicHash::Sha256);
    }

    return hash == expectedHash;
}


// проверка валидности и заполнения обязательных полей - доделать
QString checkValidForUser(const User& user)
{
    // === ОБЯЗАТЕЛЬНЫЕ ПОЛЯ ===
    if (user.login().isEmpty()) return "Введите логин";
    if (user.surname().isEmpty()) return "Введите фамилию";
    if (user.passwordHash().isEmpty()) return "Введите пароль";

    // === ФОРМАТ ЛОГИНА ===
    if (user.login().length() < 3)
        return "Логин должен быть не менее 3 символов";
    if (user.login().length() > 20)
        return "Логин не должен превышать 20 символов";
    if (!QRegularExpression("^[a-zA-Z0-9_]+$").match(user.login()).hasMatch())
        return "Логин: только латиница, цифры, подчёркивание";

    // === ПАРОЛЬ (проверяем хэш — длина исходного пароля) ===
    // ⚠️ В модели храним хэш, поэтому проверку длины пароля лучше делать ДО хэширования!
    // Но если очень нужно: мин. длина хэша SHA-256 = 64 символа (hex)
    if (user.passwordHash().length() < 64)
        return "Пароль слишком короткий (минимум 8 символов)";

    // === СНИЛС ===
    if (user.snils() > 0) {  // Если указан
        QString snilsStr = QString::number(user.snils());
        if (snilsStr.length() != 11)
            return "СНИЛС должен содержать 11 цифр";\
    }

    // === ДАТА РОЖДЕНИЯ ===
    if (user.birthDate().isValid()) {
        if (user.birthDate() > QDateTime::currentDateTime())
            return "Дата рождения не может быть в будущем";
        if (user.birthDate() < QDateTime(QDate(1900, 1, 1), QTime(0, 0)))
            return "Неверная дата рождения";
    }

    // === ДОЗИМЕТРИЯ (логические проверки) ===
    if (user.annualDose() < 0)
        return "Годовая доза не может быть отрицательной";
    if (user.currentYearDose() > user.annualDose() && user.annualDose() > 0)
        return "Текущая доза не может превышать годовую";


    // === РОЛЬ ===
    if (!user.role().isEmpty())
        return "Неверная роль пользователя";

    // === ВСЁ ОК ===
    return {};
}


void MainWindow::on_btnCreateNewUser_pressed() //создаём нового пользователя
{

    qDebug() << "Нажата кнопка on_btnCreateNewUser_pressed";

    User* newUser;

    qDebug() << "User* newUser";

    newUser->fillFromMap(readFieldsFromForm(m_userFormFields));  // ← ВСЁ!

    qDebug() << "newUser->fillFromMap";

    // Пароль хэшируем отдельно (безопасность!)
    newUser->setPasswordHash(hashPasswordSecure(readFieldsFromForm(m_userFormFields)["password"].toString()));

    qDebug() << "newUser->setPasswordHash";

    // User → Репозиторий
    auto result = m_repo->createUser(*newUser);

    qDebug() << "m_repo->createUser";

    // Обработка результата
    result.isOk
        ? QMessageBox::information(this, "Успех", "ID: " + QString::number(result.value))
        : QMessageBox::critical(this, "Ошибка", result.errorMessage());



   /* //-------------------------------------
    User newUser;

    formToUser(this); //читаем данные формы

    // чтение данных из формы и запись
    bindUserToQuery(query, m_userFormFields);

    // хэширование пароля
    QString rawPassword = ui->lineEditPassword->text();
    if (rawPassword.length() < 8) {
        QMessageBox::warning(this, "Ошибка", "Пароль должен быть не менее 8 символов");
        return;
    }
    newUser.setPasswordHash(hashPasswordSecure(rawPassword));
    rawPassword.fill('0');

    //создаём через репозиторий
    auto result = m_repo->createUser(newUser);

    if (result.success) {
        QMessageBox::information(this, "Успех", "Пользователь создан!\nID: " + QString::number(result.data));
        clearForm();
    } else {
        QMessageBox::critical(this, "Ошибка", result.errorMessage);
    }
*/
//----------------------------------------------




    //далее история пользователей --------------------------------------------------------------------------------------------------
/*
    QSqlQuery query;

    //ячейки
    query.prepare(R"(
        UPDATE mesh
        SET
            user_id = :user_id,
            doz_tld_id = :doz_tld_id
        WHERE
            set_id = :set_id  AND
            kas_id = :kas_id  AND
            mesh_id = :mesh_id;
        )");
    query.bindValue(":user_id", user_id);
    query.bindValue(":set_id", Set);
    query.bindValue(":kas_id", Kas);
    query.bindValue(":mesh_id", Mesh);
    query.bindValue(":doz_tld_id", IntDoz);
    if (!query.exec()) {
        qDebug() << "Ошибка при вставке в mesh:" << query.lastError().text();
        return;
    }
    // Получаем изображение из QLabel
    QPixmap pixmap = ui->labelPhoto->pixmap(Qt::ReturnByValue);

    if (pixmap.isNull()) {
        qDebug() << "Ошибка: изображение не найдено в QLabel.";
        //return;
    }

    // Преобразуем изображение в массив байтов
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG"); // Сохраняем в формате PNG (можно выбрать другой формат)


    //query.prepare("INSERT INTO user_photo_history (user_id, user_photo) VALUES (:user_id, :user_photo)");
    query.bindValue(":user_photo", imageData);

    if (!query.exec()) {
        qDebug() << "Ошибка при добавлении фото в базу данных:" << query.lastError().text();
    }
    QMessageBox::information(this, "Успех", "Данные успешно дабавлены");

    updateUsersDatabase();
    on_btn_userClear_pressed();

    bool ok;
    int Kas_1 = Kas.toInt(&ok);
    if (!ok) {
        qDebug() << "Ошибка преобразования Kas:" << Kas;
        Kas_1 = 0;
    }

    int Mesh_1 = Mesh.toInt(&ok);
    if (!ok) {
        qDebug() << "Ошибка преобразования Mesh:" << Mesh;
        Mesh_1 = 0;
    }



    QString for_user_id = ui->label_userId->text();
    QString changing_user_id;
    QStringList parts1 = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString name_0 = parts1[0];
    QString name_1 = parts1[1];
    QString name_2 = parts1[2];
    query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
    query.bindValue(":name_0", name_0);
    query.bindValue(":name_1", name_1);
    query.bindValue(":name_2", name_2);
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка08", "Не удалось выполнить запрос: " + query.lastError().text());
        qDebug() << "Ошибка поиска id SQL:" << query.lastQuery() << query.lastError().text();
        return;
    }
    if (query.next()) {
        changing_user_id = query.value("user_id").toString();
    } else {
        QMessageBox::information(this, "Информация", "Пользователь не найден");
    }
    QString type_edit = "create";

    bool success = addUserHistory(
        user_id,
        Login,
        Password,
        name0,
        name1,
        name2,
        Gender,
        UserBirthday,
        Role,
        IntTab,
        Department,
        CardId,
        Set_2,
        Kas_1,
        Mesh_1,
        IntDoz,
        DateCellDate,
        UserDoseYear,
        UserDoseYearNow,
        UserDoseYearPpd,
        Code,
        UserBlock,
        changing_user_id,
        type_edit
    );
    usersHistoryToTable();
    setupAllUserComboBoxes();
}
*/
}

//записываем в историю
// Функция для добавления записи в историю пользователей
bool MainWindow::addUserHistory(
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
    const QString &type_edit)//int changingUserId
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO users_history (
            user_id,
            login,
            password,
            name_0,
            name_1,
            name_2,
            gender,
            birthday,
            role,
            tab_num,
            department,
            card_id,
            set_inf,
            kas_inf,
            mesh_inf,
            doz_tld_id,
            cell_date,
            dose_year,
            dose_year_now,
            dose_year_now_ppd,
            code,
            block,
            last_update,
            user_id_change,
            type_edit
        ) VALUES (
            :user_id,
            :login,
            :password,
            :name_0,
            :name_1,
            :name_2,
            :gender,
            :birthday,
            :role,
            :tab_num,
            :department,
            :card_id,
            :set_ID,
            :kas_ID,
            :mesh_ID,
            :doz_tld_id,
            :cell_date,
            :dose_year,
            :dose_year_now,
            :dose_year_now_ppd,
            :code,
            :block,
            :last_update,
            :user_id_change,
            :type_edit
        )
    )");

    // Привязка значений (каждое на отдельной строке)
    query.bindValue(":user_id", userId);
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    query.bindValue(":name_0", name0);
    query.bindValue(":name_1", name1);
    query.bindValue(":name_2", name2);
    query.bindValue(":gender", gender);
    query.bindValue(":birthday", birthday);
    query.bindValue(":role", role);
    query.bindValue(":tab_num", tabNum);
    query.bindValue(":department", department);
    query.bindValue(":card_id", cardId);
    query.bindValue(":set_ID", setId);
    query.bindValue(":kas_ID", kasId);
    query.bindValue(":mesh_ID", meshId);
    query.bindValue(":doz_tld_id", dozTldId);
    query.bindValue(":cell_date", cellDate);
    query.bindValue(":dose_year", doseYear);
    query.bindValue(":dose_year_now", doseYearNow);
    query.bindValue(":dose_year_now_ppd", doseYearNowPpd);
    query.bindValue(":code", code);
    query.bindValue(":block", block);
    query.bindValue(":last_update", QDateTime::currentDateTime());
    query.bindValue(":user_id_change", changing_user_id);
    query.bindValue(":type_edit", type_edit);

    if (!query.exec()) {
        qDebug() << "Ошибка записи в users_history:" << query.lastError().text();

        QMessageBox::critical(this, "Ошибка09", query.lastError().text());
        return false;
    }
    return true;
    usersHistoryToTable();


}


void MainWindow::usersHistoryToTable()
{
    // Очищаем таблицу перед загрузкой новых данных
    ui->tableWidget_historyUsers->clear();
    ui->tableWidget_historyUsers->setRowCount(0);

    // Устанавливаем количество столбцов
    ui->tableWidget_historyUsers->setColumnCount(25); // По количеству полей в users_history

    // Устанавливаем русские заголовки
    QStringList headers;
    headers << "ID" << "Логин" << "Пароль" << "Фамилия" << "Имя" << "Отчество"
            << "Пол" << "Дата рождения" << "Роль" << "Таб. номер" << "Отдел"
            << "ID карты" << "ID комплекта" << "ID кассетницы" << "ID ячейки"
            << "ID дозиметра" << "Дата закрепления" << "Доза за год" << "Доза текущего года"
            << "Доза PPD" << "Код" << "Причина блокировки" << "Дата обновления" << "ID изменившего" << "Тип изменения";

    ui->tableWidget_historyUsers->setHorizontalHeaderLabels(headers);

    // Выполняем SQL-запрос
    QSqlQuery query;
    query.prepare("SELECT * FROM users_history");// ORDER BY last_update_change DESC

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса5:" << query.lastError().text();
        return;
    }

    // Заполняем таблицу данными
    while (query.next()) {
        int row = ui->tableWidget_historyUsers->rowCount();
        ui->tableWidget_historyUsers->insertRow(row);

        // Заполняем ячейки данными
        for (int col = 0; col < query.record().count(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();

            // Особые обработки для разных типов данных
            if (query.record().fieldName(col) == "birthday" ||
                query.record().fieldName(col) == "cell_date" ||
                query.record().fieldName(col) == "last_update" ||
                query.record().fieldName(col) == "last_update_change") {
                // Форматируем дату в читаемый вид
                item->setText(query.value(col).toDateTime().toString("dd.MM.yyyy hh:mm"));
            }
            else if (query.record().fieldName(col) == "dose_year" ||
                     query.record().fieldName(col) == "dose_year_now" ||
                     query.record().fieldName(col) == "dose_year_now_ppd") {
                // Форматируем числа с плавающей точкой
                item->setText(QString::number(query.value(col).toDouble(), 'f', 2));
            }
            else {
                // Стандартное текстовое представление
                item->setText(query.value(col).toString());
            }

            ui->tableWidget_historyUsers->setItem(row, col, item);
        }
    }

    // Настраиваем внешний вид таблицы
    ui->tableWidget_historyUsers->resizeColumnsToContents();
    ui->tableWidget_historyUsers->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_historyUsers->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_historyUsers->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_historyUsers->setEditTriggers(QAbstractItemView::NoEditTriggers);





    //обновляем список пользователей selectUserFilterFIO_2
    setupSearchUserComboBox(ui->selectUserFilterFIO_2);

    //обновляем список пользователей selectUserFilterFIO_3
    setupSearchUserComboBox(ui->selectUserFilterFIO_3);

    //обновляем список пользователей select_dutyUsersFilterFIO_2
    setupSearchUserComboBox(ui->select_dutyUsersFilterFIO_2);

    //обновляем список пользователей select_dutyUsersFilterFIO_3
    setupSearchUserComboBox(ui->select_dutyUsersFilterFIO_3);

    //обновляем список пользователей select_dutyFilterFIO
    setupSearchUserComboBox(ui->select_dutyFilterFIO);

    //обновляем список пользователей select_SetFilterFIO
    setupSearchUserComboBox(ui->select_SetFilterFIO);

    //обновляем список пользователей select_MeshFilterFIO
    setupSearchUserComboBox(ui->select_MeshFilterFIO);

    //обновляем список пользователей select_KasFilterFIO
    setupSearchUserComboBox(ui->select_KasFilterFIO);


    setupSearchDutyComboBox(ui->select_dutyFilter);
}



void MainWindow::updateUsersDatabase(){
    // Очищаем список пользователей перед обновлением
    QLayoutItem* item;
    while ((item = ui->verticalUsersList->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // Чтение фотографий пользователей
    struct UserPhoto {
        int user_id;
        QByteArray photo_data;
    };
    QList<UserPhoto> ar_photoForUser;
    QSqlQuery query;

    if (query.exec("SELECT user_id, user_photo FROM user_photo")) {
        while (query.next()) {
            UserPhoto item;
            item.user_id = query.value("user_id").toInt();
            item.photo_data = query.value("user_photo").toByteArray();
            ar_photoForUser.append(item);
        }
    } else {
        qDebug() << "Ошибка запроса фотографий:" << query.lastError().text();
    }

    // Чтение данных пользователей
    query.prepare("SELECT set_ID, kas_ID, mesh_ID, user_id, login, password, name_0, name_1, name_2, snils, gender, birthday, tab_num, department, card_id, doz_tld_id, cell_date, dose_year, dose_year_now, dose_year_now_ppd, code, block, last_update, role FROM users ORDER BY name_0, name_1, name_2");
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка018", "Не удалось выполнить вывод пользователей: " + query.lastError().text());
        return;
    }

    // Хэш для отслеживания уникальных пользователей (по ID)
    QSet<QString> uniqueUsers;

    while (query.next()) {
        QString user_id_str = query.value("user_id").toString();
        int user_id_int = query.value("user_id").toInt();

        // Проверка на дублирование
        if (uniqueUsers.contains(user_id_str)) {
            qDebug() << "Пропущен дубликат пользователя ID:" << user_id_str;
            continue;
        }
        uniqueUsers.insert(user_id_str);

        // Получение данных пользователя
        QString name0 = query.value("name_0").toString();
        QString name1 = query.value("name_1").toString();
        QString name2 = query.value("name_2").toString();
        QString login = query.value("login").toString();
        QString password = query.value("password").toString();
        QString snils = query.value("snils").toString();
        QString code = query.value("code").toString();
        int tabNum = query.value("tab_num").toInt();
        QString cardId = query.value("card_id").toString();
        QString dozTldId = query.value("doz_tld_id").toString();
        int startDoz = query.value("start_doz").toInt();
        int finishDoz = query.value("finish_doz").toInt();
        int doseYear = query.value("dose_year").toInt();
        int doseYearNow = query.value("dose_year_now").toInt();
        int doseYearPpd = query.value("dose_year_now_ppd").toInt();
        QDate cellDate = query.value("cell_date").toDate();
        QDate birthday = query.value("birthday").toDate();
        QString lastUpdate = query.value("last_update").toString();
        QDate startUsed = query.value("start_used_date").toDate();
        QDate finishUsed = query.value("finish_used_date").toDate();
        QString role = query.value("role").toString();
        QString gender = query.value("gender").toString();
        QString department = query.value("department").toString();
        QString block = query.value("block").toString();
        QString set = query.value("set_ID").toString();
        QString kas = query.value("kas_ID").toString();
        QString mesh = query.value("mesh_ID").toString();

        // Создание контейнера для кнопки
        QHBoxLayout *userLayout = new QHBoxLayout();
        userLayout->setContentsMargins(0, 2, 0, 2);
        userLayout->setSpacing(5);

        // Создание кнопки пользователя
        QString fullName = QString("%1 %2 %3 - %4").arg(name0, name1, name2, login);
        QPushButton *userButton = new QPushButton(fullName, this);
        userButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        // Настройка стиля для заблокированных пользователей
        if (!block.isEmpty() && block != "0" && block.toLower() != "false") {
            //userButton->setStyleSheet("QPushButton { background-color: #ffcccc; color: #990000; border: 1px solid #ff6666; }");
            userButton->setToolTip("Пользователь заблокирован: " + block);
        }

        // Создание кнопки замка для заблокированных пользователей
        QPushButton *lockButton = nullptr;
        if (!block.isEmpty() && block != "0" && block.toLower() != "false") {
            lockButton = new QPushButton("🔒", this);
            lockButton->setFixedSize(25, 25);
            //lockButton->setStyleSheet("QPushButton { background-color: #ffcccc; border: 1px solid #ff6666; font-size: 12px; }");
            lockButton->setToolTip("Пользователь заблокирован: " + block);
            lockButton->setEnabled(false); // Замок не кликабельный
        }

        // Добавление виджетов в layout
        userLayout->addWidget(userButton);
        if (lockButton) {
            userLayout->addWidget(lockButton);
        }

        // Создание контейнерного виджета
        QWidget *userWidget = new QWidget();
        userWidget->setLayout(userLayout);
        ui->verticalUsersList->addWidget(userWidget);

        // Подключение сигнала для кнопки пользователя
        connect(userButton, &QPushButton::pressed, this, [this, user_id_str, user_id_int, name0, name1, name2, login, password, snils, code, tabNum, cardId, dozTldId, startDoz, finishDoz, doseYear, doseYearNow, doseYearPpd, cellDate, birthday, lastUpdate, startUsed, finishUsed, role, gender, department, block, set, kas, mesh, ar_photoForUser]() {
            // Заполнение полей формы
            ui->labelPhoto->clear();
            ui->label_51->setText(user_id_str);
            ui->inputName0->setText(name0);
            ui->inputName1->setText(name1);
            ui->inputName2->setText(name2);
            ui->inputLogin->setText(login);
            ui->inputPassword->setText(password);
            ui->inputCode->setText(code);
            ui->inputSnils->setText(snils);
            ui->selectUserBlock->setText(block);

            ui->inputIntTab->setValue(tabNum);
            ui->inputCardId->setText(cardId);
            ui->inputIntDoz->setText(dozTldId);
            ui->inputUserStartDoz->setValue(startDoz);
            ui->inputUserFinishDoz->setValue(finishDoz);
            ui->spinUserDoseYear->setValue(doseYear);
            ui->spinUserDoseYearNow->setValue(doseYearNow);
            ui->spinUserDoseYearPpd->setValue(doseYearPpd);

            ui->inputDateCellDate->setDate(cellDate);
            ui->dateUserBirthday->setDate(birthday);
            ui->dateUserLastUpdate->setText(lastUpdate);
            ui->dateUserSrart->setDate(startUsed);
            ui->dateUserFinish->setDate(finishUsed);

            // Установка значений в комбобоксы
            ui->selectRole->setCurrentText(role);
            ui->selectGender->setCurrentText(gender);
            ui->selectDepartment->setCurrentText(department);

            // Установка значений сет/кас/меш с проверкой существования
            auto setComboBoxValue = [](QComboBox* combo, const QString& value) {
                int index = combo->findText(value);
                if (index == -1 && !value.isEmpty()) {
                    combo->addItem(value);
                    index = combo->count() - 1;
                }
                combo->setCurrentIndex(index);
            };

            setComboBoxValue(ui->selectSet, set);
            setComboBoxValue(ui->selectKas, kas);
            setComboBoxValue(ui->selectMesh, mesh);

            // Загрузка фотографии
            for (const auto& photo : ar_photoForUser) {
                if (user_id_int == photo.user_id) {
                    QPixmap pixmap;
                    if (pixmap.loadFromData(photo.photo_data)) {
                        ui->labelPhoto->setPixmap(pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        qDebug() << QString("Найдено фото для пользователя ID: %1").arg(photo.user_id);

                        // Создание кнопки изменения фото
                        QLayoutItem *photoItem;
                        while ((photoItem = ui->horizontalLayout_24->takeAt(0))) {
                            if (photoItem->widget()) {
                                delete photoItem->widget();
                            }
                            delete photoItem;
                        }
                        QPushButton *btnChangePhoto = new QPushButton("Изменить фото", this);
                        ui->horizontalLayout_24->addWidget(btnChangePhoto);
                        connect(btnChangePhoto, &QPushButton::pressed, this, &MainWindow::on_btnChangePhoto_pressed);

                    } else {
                        qDebug() << "Не удалось загрузить фото для пользователя ID:" << photo.user_id;
                        ui->labelPhoto->clear();
                    }
                    break;
                }
            }

            // Создание кнопки сохранения
            QLayoutItem *btnItem;
            while ((btnItem = ui->layoutForBtn->takeAt(0))) {
                if (btnItem->widget()) {
                    delete btnItem->widget();
                }
                delete btnItem;
            }
            QPushButton *btnChangeUser = new QPushButton("Сохранить изменения пользователя", this);
            ui->layoutForBtn->addWidget(btnChangeUser);
            connect(btnChangeUser, &QPushButton::clicked, this, &MainWindow::on_btnChangeUser_pressed);

            checkRole();
        });
    }



    updateDataFromDatabase();
    comboboxUpdateAll();

}

void MainWindow::on_btn_userClear_pressed()
{
    ui->labelPhoto->clear();

    ui->label_51->setText("-");

    ui->inputName0->clear();
    ui->inputName1->clear();
    ui->inputName2->clear();
    ui->inputLogin->clear();
    ui->inputPassword->clear();
    ui->inputCode->clear();
    ui->inputSnils->clear();

    //изменить на нормальную дату
    ui->inputDateCellDate->setDate(QDate(2001, 1, 1));
    ui->dateUserBirthday->setDate(QDate(2001, 1, 1));
    ui->dateUserLastUpdate->setText("-");
    ui->dateUserSrart->setDate(QDate(2001, 1, 1));
    ui->dateUserSrart->setDate(QDate(2001, 1, 1));
    ui->dateUserFinish->setDate(QDate(2001, 1, 1));

    ui->inputIntTab->setValue(0);
    ui->inputCardId->clear();
    ui->inputIntDoz->clear();
    ui->inputUserStartDoz->setValue(0);
    ui->inputUserFinishDoz->setValue(0);
    ui->spinUserDoseYear->setValue(0);
    ui->spinUserDoseYearNow->setValue(0);
    ui->spinUserDoseYearPpd->setValue(0);

    //выпадаюие списки
    ui->selectGender->setCurrentIndex(0);
    ui->selectRole->setCurrentIndex(0);

    ui->selectSet->setCurrentIndex(1);
    ui->selectKas->setCurrentIndex(1);
    ui->selectMesh->setCurrentIndex(1);

    photoData.clear();          // Очищаем данные фото

    ui->selectUserBlock->clear();


    // Создание кнопки сохранения
    QLayoutItem *btnItem;
    while ((btnItem = ui->layoutForBtn->takeAt(0))) {
        if (btnItem->widget()) {
            delete btnItem->widget();
        }
        delete btnItem;
    }
    QPushButton *btnCreateNewUser = new QPushButton("Сохранить нового пользователя", this);
    ui->layoutForBtn->addWidget(btnCreateNewUser);
    connect(btnCreateNewUser, &QPushButton::clicked, this, &MainWindow::on_btnCreateNewUser_pressed);


}

void MainWindow::on_btnChangeUser_pressed(){ //изменения пользователей
    // Читаем текст из QLabel
    QString userId = ui->label_51->text();

    // Получаем данные из полей ввода
    QString name0 = ui->inputName0->text();
    QString name1 = ui->inputName1->text();
    QString name2 = ui->inputName2->text();
    QString Login = ui->inputLogin->text();
    QString Password = ui->inputPassword->text();
    QString Code = ui->inputCode->text();
    QString Snils = ui->inputSnils->text();
    QString UserBlock = ui->selectUserBlock->text();


    int IntTab = ui->inputIntTab->value();
    QString CardId = ui->inputCardId->text();
    QString IntDoz = ui->inputIntDoz->text();
    int UserStartDoz = ui->inputUserStartDoz->value();
    int UserFinishDoz = ui->inputUserFinishDoz->value();

    int UserDoseYear = ui->spinUserDoseYear->value();
    int UserDoseYearNow = ui->spinUserDoseYearNow->value();
    int UserDoseYearPpd = ui->spinUserDoseYearPpd->value();

    QDate DateCellDate = ui->inputDateCellDate->date();
    QDate UserBirthday = ui->dateUserBirthday->date();

    //QDate UserLastUpdate = ui->dateUserLastUpdate->date();
    QTimeZone moscowTimeZone("Europe/Moscow");
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString UserLastUpdate = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");

    QDate UserSrart = ui->dateUserSrart->date();
    QDate UserFinish = ui->dateUserFinish->date();

    QString Role = ui->selectRole->currentText();
    QString Gender = ui->selectGender->currentText();
    QString Department = ui->selectDepartment->currentText();
    QString Set = ui->selectSet->currentText();
    Set.toInt();
    QString Kas = ui->selectKas->currentText();
    Kas.toInt();
    QString Mesh = ui->selectMesh->currentText();
    Mesh.toInt();

    QSqlQuery query;

    query.prepare(R"(
        UPDATE users
        SET
            login = :login,
            password = :password,
            name_0 = :name_0,
            name_1 = :name_1,
            name_2 = :name_2,
            snils = :snils,
            gender = :gender,
            birthday = :birthday,
            role = :role,
            tab_num = :tab_num,
            department = :department,
            card_id = :card_id,
            set_ID = :set_ID,
            kas_ID = :kas_ID,
            mesh_ID = :mesh_ID,
            doz_tld_id = :doz_tld_id,
            cell_date = :cell_date,
            dose_year = :dose_year,
            dose_year_now = :dose_year_now,
            dose_year_now_ppd = :dose_year_now_ppd,
            code = :code,
            block = :block,
            last_update = :last_update
        WHERE user_id = :user_id;
        )");

    // Привязка параметров
    query.bindValue(":user_id", userId);
    query.bindValue(":name_0", name0);
    query.bindValue(":name_1", name1);
    query.bindValue(":name_2", name2);
    query.bindValue(":login", Login);
    query.bindValue(":password", Password);
    query.bindValue(":code", Code);
    query.bindValue(":snils", Snils);

    query.bindValue(":tab_num", IntTab);
    query.bindValue(":card_id", CardId);
    query.bindValue(":doz_tld_id", IntDoz);

    query.bindValue(":dose_year", UserDoseYear);
    query.bindValue(":dose_year_now", UserDoseYearNow);
    query.bindValue(":dose_year_now_ppd", UserDoseYearPpd);

    query.bindValue(":cell_date", DateCellDate);
    query.bindValue(":birthday", UserBirthday);
    query.bindValue(":last_update", UserLastUpdate);

    query.bindValue(":role", Role);
    query.bindValue(":gender", Gender);
    query.bindValue(":department", Department);
    query.bindValue(":set_ID", Set);
    query.bindValue(":kas_ID", Kas);
    query.bindValue(":mesh_ID", Mesh);
    query.bindValue(":block", UserBlock);

    query.bindValue(":start_used_date", UserSrart);
    query.bindValue(":finish_used_date", UserFinish);
    query.bindValue(":start_doz", UserStartDoz);
    query.bindValue(":finish_doz", UserFinishDoz);


    if (!query.exec()) {
        //qDebug() << "Ошибка при вставке в users1:" << query.lastError().text();
        QMessageBox::information(this, "Ошибка при вставке в users1:", query.lastError().text());
        //QSqlDatabase::database().rollback();  // Откатываем транзакцию
        return;
    }

    //для ячеек
    query.prepare(R"(
        UPDATE mesh
        SET
            user_id = :user_id,
            doz_tld_id = :doz_tld_id
        WHERE
            set_id = :set_id  AND
            kas_id = :kas_id  AND
            mesh_id = :mesh_id;
        )");
    query.bindValue(":user_id", userId);
    query.bindValue(":set_id", Set);
    query.bindValue(":kas_id", Kas);
    query.bindValue(":mesh_id", Mesh);
    query.bindValue(":doz_tld_id", IntDoz);
    if (!query.exec()) {
        qDebug() << "Ошибка при вставке в mesh:" << query.lastError().text();
        //QSqlDatabase::database().rollback();  // Откатываем транзакцию
        return;
    }
loadFilteredMeshData("1");



bool ok;
int userId_1 = userId.toInt(&ok);
if (!ok) {
    qDebug() << "Ошибка преобразования userId: " << userId;
    userId_1 = 0;
}


int Set_1 = Set.toInt(&ok);
if (!ok) {
    qDebug() << "Ошибка преобразования Set:" << Set;
    Set_1 = 0;
}

int Kas_1 = Kas.toInt(&ok);
if (!ok) {
    qDebug() << "Ошибка преобразования Kas:" << Kas;
    Kas_1 = 0;
}

int Mesh_1 = Mesh.toInt(&ok);
if (!ok) {
    qDebug() << "Ошибка преобразования Mesh:" << Mesh;
    Mesh_1 = 0;
}


QString for_user_id = ui->label_userId->text();
QString changing_user_id;
QStringList parts1 = for_user_id.split(' ', Qt::SkipEmptyParts);
QString name_0 = parts1[0];
QString name_1 = parts1[1];
QString name_2 = parts1[2];
query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
query.bindValue(":name_0", name_0);
query.bindValue(":name_1", name_1);
query.bindValue(":name_2", name_2);
if (!query.exec()) {
    QMessageBox::critical(this, "Ошибка019", "Не удалось выполнить запрос: " + query.lastError().text());
    qDebug() << "Ошибка поиска id SQL:" << query.lastQuery() << query.lastError().text();
    return;
}
if (query.next()) {
    changing_user_id = query.value("user_id").toString();
} else {
    QMessageBox::information(this, "Информация", "Пользователь не найден");
}
QString type_edit = "change";


bool success = addUserHistory(
    userId_1,
    Login,
    Password,
    name0,
    name1,
    name2,
    Gender,
    UserBirthday,
    Role,
    IntTab,
    Department,
    CardId,
    Set_1,
    Kas_1,
    Mesh_1,
    IntDoz,
    DateCellDate,
    UserDoseYear,
    UserDoseYearNow,
    UserDoseYearPpd,
    Code,
    UserBlock,
    changing_user_id,
    type_edit
);//user_id_change


    // Проверка результата
    if (success) {
        qDebug() << "Данные успешно сохранены в таблицу users_history";
        qDebug() << "Время последнего обновления:" << UserLastUpdate;//.toString("dd.MM.yyyy hh:mm:ss");
    } else {
        qDebug() << "Ошибка при сохранении истории пользователя";
    }


    QMessageBox::information(this, "Успех!", "Данные изменены");
    updateUsersDatabase();
    on_btn_userClear_pressed();

    loadFilteredMeshData("1");
    usersHistoryToTable();
    setupAllUserComboBoxes();

}


void MainWindow::on_btnCreateUser_pressed()
{
    ui->labelPhoto->clear();

    ui->label_51->setText("-");

    ui->inputName0->clear();
    ui->inputName1->clear();
    ui->inputName2->clear();
    ui->inputLogin->clear();
    ui->inputPassword->clear();
    ui->inputCode->clear();
    ui->inputSnils->clear();

    //изменить на нормальную дату
    ui->inputDateCellDate->setDate(QDate(2001, 1, 1));
    ui->dateUserBirthday->setDate(QDate(2001, 1, 1));
    ui->dateUserLastUpdate->setText("-");
    ui->dateUserSrart->setDate(QDate(2001, 1, 1));
    ui->dateUserSrart->setDate(QDate(2001, 1, 1));
    ui->dateUserFinish->setDate(QDate(2001, 1, 1));

    ui->inputIntTab->setValue(0);
    ui->inputCardId->clear();
    ui->inputIntDoz->clear();
    ui->inputUserStartDoz->setValue(0);
    ui->inputUserFinishDoz->setValue(0);
    ui->spinUserDoseYear->setValue(0);
    ui->spinUserDoseYearNow->setValue(0);
    ui->spinUserDoseYearPpd->setValue(0);

    //выпадаюие списки
    ui->selectGender->setCurrentIndex(0);
    ui->selectRole->setCurrentIndex(0);

    ui->selectSet->setCurrentIndex(1);
    ui->selectKas->setCurrentIndex(1);
    ui->selectMesh->setCurrentIndex(1);

    photoData.clear();          // Очищаем данные фото

    //меняем кнопку

    // Удаляем все виджеты из layout
    QLayoutItem *item;
    while ((item = ui->layoutForBtn->takeAt(0))) {
        if (item->widget()) {
            delete item->widget(); // Удаляем виджет
        }
        delete item; // Удаляем элемент layout
    }

    // Создаем новую кнопку
    QPushButton *btnCreateNewUser = new QPushButton("Создать нового пользователя", this);

    // Добавляем кнопку в layout
    ui->layoutForBtn->addWidget(btnCreateNewUser);

    //qDebug() << "Родительский класс кнопки:" << btnCreateNewUser->parent(); //QWidget(0x1f4ae248d90, name = "tab")
    if (btnCreateNewUser){
        connect(btnCreateNewUser, &QPushButton::pressed, this, &MainWindow::on_btnCreateNewUser_pressed);//не забыть по h
    }
}

#pragma endregion //========================================== Конец пользователя ====================================


// === ЗАПИСЬ: User/данные → Виджеты формы ===
void writeFieldsToForm(const std::vector<FieldRecord>& fields, const QMap<QString, QVariant>& data)
{
    for (const auto& field : fields) {
        if (!field.widget || !data.contains(field.fieldName)) continue;

        QVariant value = data[field.fieldName];

        switch (field.type) {
            case FieldRecord::Text:
                if (auto* line = qobject_cast<QLineEdit*>(field.widget))
                    line->setText(value.toString());
                break;

            case FieldRecord::Int:
                if (auto* spin = qobject_cast<QSpinBox*>(field.widget))
                    spin->setValue(value.toInt());
                else if (auto* line = qobject_cast<QLineEdit*>(field.widget))
                    line->setText(QString::number(value.toInt()));
                break;

            case FieldRecord::Float:
                if (auto* dspin = qobject_cast<QDoubleSpinBox*>(field.widget))
                    dspin->setValue(value.toFloat());
                break;

            case FieldRecord::Date:
                if (auto* dateEdit = qobject_cast<QDateEdit*>(field.widget))
                    dateEdit->setDate(value.toDate());
                break;

            case FieldRecord::ComboBox:
                if (auto* combo = qobject_cast<QComboBox*>(field.widget))
                    combo->setCurrentText(value.toString());
                break;

            case FieldRecord::Label:
                if (auto* label = qobject_cast<QLabel*>(field.widget)) {
                    if (value.type() == QVariant::DateTime)
                        label->setText(value.toDateTime().toString("dd.MM.yyyy hh:mm"));
                    else
                        label->setText(value.toString());
                }
                break;
        }
    }
}

// === ЧТЕНИЕ: Виджеты формы → QMap с данными ===
//QMap<QString, QVariant> readFieldsFromForm(const std::vector<FieldRecord>& fields)
QMap<QString, QVariant> MainWindow::readFieldsFromForm(const std::vector<FieldRecord>& fields)
{
    QMap<QString, QVariant> data;

    for (const auto& field : fields) {
        if (!field.widget) continue;

        QVariant value;

        switch (field.type) {
            case FieldRecord::Text:
                if (auto* line = qobject_cast<QLineEdit*>(field.widget))
                    value = line->text();
                break;

            case FieldRecord::Int:
                if (auto* spin = qobject_cast<QSpinBox*>(field.widget))
                    value = spin->value();
                else if (auto* line = qobject_cast<QLineEdit*>(field.widget))
                    value = line->text().toInt();
                break;

            case FieldRecord::Float:
                if (auto* dspin = qobject_cast<QDoubleSpinBox*>(field.widget))
                    value = dspin->value();
                break;

            case FieldRecord::Date:
                if (auto* dateEdit = qobject_cast<QDateEdit*>(field.widget))
                    value = dateEdit->date();
                break;

            case FieldRecord::ComboBox:
                if (auto* combo = qobject_cast<QComboBox*>(field.widget))
                    value = combo->currentText();
                break;

            case FieldRecord::Label:
                if (auto* label = qobject_cast<QLabel*>(field.widget))
                    value = label->text();
                break;
        }

        data[field.fieldName] = value;
    }

    return data;
}


//=========================================== Общие функции закончились =====================================


void MainWindow::on_btnSetSave_pressed() //новый set
{
    //проверить на существование

    QSqlQuery query;

    int set_id = 1; // Значение по умолчанию, если таблица пуста

    if (query.exec("SELECT MAX(set_id) AS max_id FROM set;")) {
        if (query.next()) {
            // Если в таблице есть записи, берём максимальный ID + 1
            set_id = query.value("max_id").toInt() + 1;
            //qDebug() << "Максимальный set_id:" << set_id - 1;
            //qDebug() << "Новый set_id:" << set_id;
        } else {
            //qDebug() << "Таблица пуста, используем set_id = 1";
        }
    } else {
        qDebug() << "Ошибка выполнения запроса7:" << query.lastError().text();
    }

    QString SetName = ui->inputSetName->text();
    int SetQuantity = ui->inputIntSetQuantity->value();
    QString SetNote = ui->lineSetNote->text();
    QString SetIp = ui->inputSetIp->text();

    //QDate SetLastUpdate = ui->dateSetLastUpdate->date();//поменять на настоящее время
    // Указываем часовой пояс для Москвы (UTC+3)
    QTimeZone moscowTimeZone("Europe/Moscow");
    // Получаем текущее время и дату в UTC
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    // Конвертируем время в московское
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString SetLastUpdate = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");
    //qDebug() << "Current date and time in Moscow:" << dateTimeString;

    QString SetStatus = ui->comboSetStatus->currentText();


    // Проверяем, что поля не пустые
    if (SetName.isEmpty()) {
        // для обязательных полей
        QMessageBox::warning(this, "Ошибка020", "Поле 'Имя0' не может быть пустым!");
        return;
    }

    //id порядковый номер
    query.prepare("INSERT INTO set (set_id, set_name, set_quantity, set_block, ip_set, set_note, last_update) VALUES (:set_id, :set_name, :set_quantity, :set_block, :ip_set, :set_note, :last_update)");

    //int set_id = 1;
    query.bindValue(":set_id", set_id);

    query.bindValue(":set_name", SetName);
    query.bindValue(":set_quantity", SetQuantity);
    query.bindValue(":set_block", SetStatus);
    query.bindValue(":ip_set", SetIp);
    query.bindValue(":set_note", SetNote);
    query.bindValue(":last_update", SetLastUpdate);

    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Данные успешно добавлены!");

        ui->inputSetName->clear();
        ui->inputIntSetQuantity->clear();

        ui->inputSetIp->clear();
        ui->lineSetNote->clear();
        ui->dateSetLastUpdate->setText("-");
    } else {
        if (query.lastError().text().contains("неверный синтаксис для типа inet", Qt::CaseInsensitive)) {
            QMessageBox::critical(this, "Ошибка формата IP",
                                QString("Неверный формат IP-адреса: \n"
                                        "Поле 'ip_set' ожидает IP-адрес в формате: 192.168.1.1"));
        } else {
            QMessageBox::critical(this, "Ошибка021", "Не удалось добавить данные!");
            qDebug() << "Не удалось добавить данные в set: " << query.lastError().text();
        }
        return;
    }

    //updateBD_user()();

    //set вывод
    // Подготавливаем запрос

    // Генерируем строки с порядковыми номерами
    for (int i = 1; i <= SetQuantity; ++i) {
        query.prepare("INSERT INTO kas (set_id, kas_id, kas_name, kas_height, kas_width, kas_block, kas_note, last_update) VALUES (:set_id, :kas_id, :kas_name, :kas_height, :kas_width, :kas_block, :kas_note, :last_update)");

        query.bindValue(":set_id", set_id);
        query.bindValue(":kas_id", i);  // Порядковый номер от 1 до set_quantity
        query.bindValue(":kas_name", QString("Кассетница %1").arg(i));
        query.bindValue(":kas_height", 1);
        query.bindValue(":kas_width", 1);
        query.bindValue(":kas_block", "Разблокирована");
        query.bindValue(":kas_note", "");
        query.bindValue(":last_update", QDateTime::currentDateTime());


        if (!query.exec()) {
            qDebug() << "Ошибка при вставке кассетницы" << i << ":" << query.lastError().text();
            break;
        }



        // Подготавливаем запрос для mesh

        int mesh_counter = 1; // Счетчик ячеек
        int all_mesh = 1;


        // Создаем ячейки сетки по строкам и столбцам
        for (int row = 1; row <= all_mesh; ++row) {
            query.prepare("INSERT INTO mesh (set_id, kas_id, mesh_id, user_id, doz_tld_id, mesh_status, mesh_note, last_update) VALUES (:set_id, :kas_id, :mesh_id, :user_id, :doz_tld_id, :mesh_status, :mesh_note, :last_update)");
            query.bindValue(":set_id", set_id);
            query.bindValue(":kas_id", i);
            query.bindValue(":mesh_id", mesh_counter); // Порядковый номер ячейки
            query.bindValue(":user_id", QVariant(QVariant::Int)); // NULL значение
            query.bindValue(":doz_tld_id", QVariant(QVariant::Int)); // NULL значение
            query.bindValue(":mesh_status", "0");
            query.bindValue(":mesh_note", "");//, QString("Ячейка %1-%2").arg(row).arg(col));
            query.bindValue(":last_update", QDateTime::currentDateTime());

            if (!query.exec()) {
                qDebug() << "Ошибка при вставке ячейки" << mesh_counter << ":" << query.lastError().text();
                return; // или break в зависимости от логики
            }
            mesh_counter++;
        }
    }




    //QSqlQuery query;
    loadSetData();

    //в историю
    QString for_user_id = ui->label_userId->text();
    QString changing_user_id;
    QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString name_0 = parts[0];
    QString name_1 = parts[1];
    QString name_2 = parts[2];
    query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
    query.bindValue(":name_0", name_0);
    query.bindValue(":name_1", name_1);
    query.bindValue(":name_2", name_2);
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка022", "Не удалось выполнить запрос: " + query.lastError().text());
        qDebug() << "Ошибка поиска id SQL:" << query.lastQuery() << query.lastError().text();
        return;
    }
    if (query.next()) {
        changing_user_id = query.value("user_id").toString();
    } else {
        QMessageBox::information(this, "Информация", "Пользователь не найден");
    }

    QString type_edit = "create"; // или "update", "delete" в зависимости от операции

    bool success = addSetHistory(
        set_id,           // ID комплекта
        SetName,         // Название комплекта
        SetIp,           // IP адрес
        SetQuantity,     // Количество
        SetStatus,        // Блокировка
        SetNote,         // Примечание
        changing_user_id, // ID пользователя, который внес изменения
        type_edit         // Тип операции
    );

    if (success) {
        qDebug() << "Данные успешно сохранены в таблицу set_history";
        setHistoryToTable(); // Обновляем таблицу истории
    } else {
        qDebug() << "Ошибка при сохранении истории комплекта";
    }
    setHistoryToTable();

    on_btnSetCreateNew_pressed();

    comboboxUpdateAll();
}

bool MainWindow::addSetHistory(
    int setId,
    const QString &setName,
    const QString &ipSet,
    int setQuantity,
    const QString &setBlock,
    const QString &setNote,
    const QString &changing_user_id,
    const QString &type_edit)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO set_history (
            set_id,
            set_name,
            ip_set,
            set_quantity,
            set_block,
            set_note,
            last_update,
            user_id_change,
            last_update_change,
            type_edit
        ) VALUES (
            :set_id,
            :set_name,
            :ip_set,
            :set_quantity,
            :set_block,
            :set_note,
            :last_update,
            :user_id_change,
            :last_update_change,
            :type_edit
        )
    )");

    // Привязка значений
    query.bindValue(":set_id", setId);
    query.bindValue(":set_name", setName);
    query.bindValue(":ip_set", ipSet);
    query.bindValue(":set_quantity", setQuantity);
    query.bindValue(":set_block", setBlock);
    query.bindValue(":set_note", setNote);
    query.bindValue(":last_update", QDateTime::currentDateTime());
    query.bindValue(":user_id_change", changing_user_id);
    query.bindValue(":last_update_change", QDateTime::currentDateTime());
    query.bindValue(":type_edit", type_edit);

    if (!query.exec()) {
        qDebug() << "Ошибка записи в set_history:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка023", query.lastError().text());
        return false;
    }
    return true;
}

void MainWindow::setHistoryToTable()
{
    // Очищаем таблицу перед загрузкой новых данных
    ui->tableWidget_historySet->clear();
    ui->tableWidget_historySet->setRowCount(0);

    // Устанавливаем количество столбцов
    ui->tableWidget_historySet->setColumnCount(10); // По количеству полей в set_history

    // Устанавливаем русские заголовки
    QStringList headers;
    headers << "ID комплекта" << "Название" << "IP адрес" << "Количество"
            << "Блокировка" << "Примечание" << "Дата обновления"
            << "ID изменившего" << "Дата изменения" << "Тип изменения";

    ui->tableWidget_historySet->setHorizontalHeaderLabels(headers);

    // Выполняем SQL-запрос
    QSqlQuery query;
    query.prepare("SELECT * FROM set_history ORDER BY last_update_change DESC");

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса8:" << query.lastError().text();
        return;
    }

    // Заполняем таблицу данными
    while (query.next()) {
        int row = ui->tableWidget_historySet->rowCount();
        ui->tableWidget_historySet->insertRow(row);

        // Заполняем ячейки данными
        for (int col = 0; col < query.record().count(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();

            // Особые обработки для разных типов данных
            if (query.record().fieldName(col) == "last_update" ||
                query.record().fieldName(col) == "last_update_change") {
                // Форматируем дату в читаемый вид
                item->setText(query.value(col).toDateTime().toString("dd.MM.yyyy hh:mm"));
            }
            else {
                // Стандартное текстовое представление
                item->setText(query.value(col).toString());
            }

            ui->tableWidget_historySet->setItem(row, col, item);
        }
    }

    // Настраиваем внешний вид таблицы
    ui->tableWidget_historySet->resizeColumnsToContents();
    ui->tableWidget_historySet->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_historySet->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_historySet->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_historySet->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void MainWindow::on_btnKasSave_pressed()
{
    QSqlQuery query;

    int kas_id = 1; // Значение по умолчанию, если таблица пуста

    if (query.exec("SELECT MAX(kas_id) AS max_id FROM kas;")) {
        if (query.next()) {
            // Если в таблице есть записи, берём максимальный ID + 1
            kas_id = query.value("max_id").toInt() + 1;
            //qDebug() << "Максимальный set_id:" << set_id - 1;
            //qDebug() << "Новый kas_id:" << kas_id;
        } else {
            //qDebug() << "Таблица пуста, используем kas_id = 1";
        }
    } else {
        qDebug() << "Ошибка выполнения запроса9:" << query.lastError().text();
    }
    //last_update
    //int set_id = ui->textSetId->toInt();
    int set_id = ui->textSetId->text().toInt();

    QString KasName = ui->inputKasName->text();
    QString KasNote = ui->inputKasNote->text();

    int KasLenght = ui->inputIntKasLenght->value();
    int KasWidth = ui->inputIntKasWidth->value();

    QString KasStatus = ui->comboKasStatus->currentText();



    //QDate SetLastUpdate = ui->dateSetLastUpdate->date();//поменять на настоящее время
    // Указываем часовой пояс для Москвы (UTC+3)
    QTimeZone moscowTimeZone("Europe/Moscow");
    // Получаем текущее время и дату в UTC
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    // Конвертируем время в московское
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString last_update = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");

    query.prepare("INSERT INTO kas (set_id, kas_id, kas_name, kas_height, kas_width, kas_block, kas_note, last_update) VALUES (:set_id, :kas_id, :kas_name, :kas_height, :kas_width, :kas_block, :kas_note, :last_update)");

    query.bindValue(":set_id", set_id);
    query.bindValue(":kas_id", kas_id);

    query.bindValue(":kas_name", KasName);
    query.bindValue(":kas_note", KasNote);

    query.bindValue(":kas_height", KasLenght);
    query.bindValue(":kas_width", KasWidth);
    //query.bindValue(":KasCount", KasCount);

    query.bindValue(":kas_block", KasStatus);

    //query.bindValue(":KasDelete", KasDelete);
    query.bindValue(":last_update", last_update);


    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Данные успешно добавлены!");
    } else {
        QMessageBox::critical(this, "Ошибка024", "Не удалось добавить данные!", query.lastError().text());
        qDebug() << query.lastError().text();
    }


    query.prepare("UPDATE set SET set_quantity = set_quantity+1 WHERE set_id = :set_id;");//last_update = :last_update
    query.bindValue(":set_id", set_id);

    // Подготавливаем запрос для mesh

    int mesh_counter = 1; // Счетчик ячеек
    int all_mesh = KasLenght*KasWidth;

    // Создаем ячейки сетки по строкам и столбцам
    for (int row = 1; row <= all_mesh; ++row) {

        query.prepare("INSERT INTO mesh (set_id, kas_id, mesh_id, user_id, doz_tld_id, mesh_status, mesh_note, last_update) VALUES (:set_id, :kas_id, :mesh_id, :user_id, :doz_tld_id, :mesh_status, :mesh_note, :last_update)");
        query.bindValue(":set_id", set_id);
        query.bindValue(":kas_id", kas_id);
        query.bindValue(":mesh_id", mesh_counter); // Порядковый номер ячейки
        query.bindValue(":user_id", QVariant(QVariant::Int)); // NULL значение
        query.bindValue(":doz_tld_id", QVariant(QVariant::Int)); // NULL значение
        query.bindValue(":mesh_status", "0");
        query.bindValue(":mesh_note", "");//, QString("Ячейка %1-%2").arg(row).arg(col));
        query.bindValue(":last_update", QDateTime::currentDateTime());

        if (!query.exec()) {
            qDebug() << "Ошибка при вставке ячейки" << mesh_counter << ":" << query.lastError().text();
            return; // или break в зависимости от логики
        }
        mesh_counter++;


        QString for_user_id = ui->label_userId->text();
        QString changing_user_id;
        QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
        QString name_0 = parts[0];
        QString name_1 = parts[1];
        QString name_2 = parts[2];

        query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
        query.bindValue(":name_0", name_0);
        query.bindValue(":name_1", name_1);
        query.bindValue(":name_2", name_2);

        if (!query.exec()) {
            QMessageBox::critical(this, "Ошибка025", "Не удалось выполнить запрос: " + query.lastError().text());
            return;
        }

        if (query.next()) {
            changing_user_id = query.value("user_id").toString();
        } else {
            QMessageBox::information(this, "Информация", "Пользователь не найден");
            return;
        }


        QString type_edit = "craete"; // или "update", "delete" в зависимости от операции

        bool success = addMeshHistory(
            set_id,           // ID комплекта
            kas_id,           // ID кассетницы
            mesh_counter,          // ID ячейки
            0,          // ID пользователя
            "",       // ID дозиметра
            0,      // Статус ячейки
            "",        // Примечание
            changing_user_id, // ID пользователя, который внес изменения
            type_edit         // Тип операции
        );

        if (success) {
            qDebug() << "Данные успешно сохранены в таблицу mesh_history";
            meshHistoryToTable(); // Обновляем таблицу истории
        } else {
            qDebug() << "Ошибка при сохранении истории ячейки";
        }
        meshHistoryToTable();
    }

    //QMessageBox::information(this, "Успех", "ещё работает!");  //неа - проверить+

    query.prepare("UPDATE set SET set_quantity = (SELECT MAX(set_quantity) FROM set) + 1 WHERE set_id = ?");
    query.addBindValue(set_id);
    if (query.exec()) {
        qDebug() << "Количество кассетниц в комплекте увеличено на 1";
    } else {
        qDebug() << "Ошибка при увеличении количества кассетниц в комплекте:" << query.lastError().text();
    }


    loadFilteredKasData();
    loadSetData();
    loadListOfKas();



    // Где-то в коде при сохранении/изменении кассетницы
    QString for_user_id = ui->label_userId->text();
    QString changing_user_id;
    QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString name_0 = parts[0];
    QString name_1 = parts[1];
    QString name_2 = parts[2];

    query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
    query.bindValue(":name_0", name_0);
    query.bindValue(":name_1", name_1);
    query.bindValue(":name_2", name_2);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка026", "Не удалось выполнить запрос: " + query.lastError().text());
        return;
    }

    if (query.next()) {
        changing_user_id = query.value("user_id").toString();
    } else {
        QMessageBox::information(this, "Информация", "Пользователь не найден");
        return;
    }

    QString type_edit = "create"; // или "update", "delete" в зависимости от операции

    bool success = addKasHistory(
        set_id,           // ID комплекта
        kas_id,           // ID кассетницы
        KasName,         // Название кассетницы
        KasLenght,       // Высота
        KasWidth,        // Ширина
        KasStatus,        // Блокировка
        KasNote,         // Примечание
        changing_user_id, // ID пользователя, который внес изменения
        type_edit         // Тип операции
    );

    if (success) {
        qDebug() << "Данные успешно сохранены в таблицу kas_history";
        kasHistoryToTable(); // Обновляем таблицу истории
    } else {
        qDebug() << "Ошибка при сохранении истории кассетницы";
    }
    on_btnKasCreateNew_pressed();

    updateListSetForHistory();
    comboboxUpdateAll();
}
bool MainWindow::addKasHistory(
    int setId,
    int kasId,
    const QString &kasName,
    int kasHeight,
    int kasWidth,
    const QString &kasBlock,
    const QString &kasNote,
    const QString &changing_user_id,
    const QString &type_edit)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO kas_history (
            set_id,
            kas_id,
            kas_name,
            kas_height,
            kas_width,
            kas_block,
            kas_note,
            last_update,
            user_id_change,
            last_update_change,
            type_edit
        ) VALUES (
            :set_id,
            :kas_id,
            :kas_name,
            :kas_height,
            :kas_width,
            :kas_block,
            :kas_note,
            :last_update,
            :user_id_change,
            :last_update_change,
            :type_edit
        )
    )");

    // Привязка значений
    query.bindValue(":set_id", setId);
    query.bindValue(":kas_id", kasId);
    query.bindValue(":kas_name", kasName);
    query.bindValue(":kas_height", kasHeight);
    query.bindValue(":kas_width", kasWidth);
    query.bindValue(":kas_block", kasBlock);
    query.bindValue(":kas_note", kasNote);
    query.bindValue(":last_update", QDateTime::currentDateTime());
    query.bindValue(":user_id_change", changing_user_id);
    query.bindValue(":last_update_change", QDateTime::currentDateTime());
    query.bindValue(":type_edit", type_edit);

    if (!query.exec()) {
        qDebug() << "Ошибка записи в kas_history:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка027", query.lastError().text());
        return false;
    }
    return true;
    kasHistoryToTable();
}

void MainWindow::kasHistoryToTable()
{
    // Очищаем таблицу перед загрузкой новых данных
    ui->tableWidget_historyKas->clear();
    ui->tableWidget_historyKas->setRowCount(0);

    // Устанавливаем количество столбцов
    ui->tableWidget_historyKas->setColumnCount(11); // По количеству полей в kas_history

    // Устанавливаем русские заголовки
    QStringList headers;
    headers << "ID комплекта" << "ID кассетницы" << "Название" << "Высота"
            << "Ширина" << "Блокировка" << "Примечание" << "Дата обновления"
            << "ID изменившего" << "Дата изменения" << "Тип изменения";

    ui->tableWidget_historyKas->setHorizontalHeaderLabels(headers);

    // Выполняем SQL-запрос
    QSqlQuery query;
    query.prepare("SELECT * FROM kas_history ORDER BY last_update_change DESC");

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса0:" << query.lastError().text();
        return;
    }

    // Заполняем таблицу данными
    while (query.next()) {
        int row = ui->tableWidget_historyKas->rowCount();
        ui->tableWidget_historyKas->insertRow(row);

        // Заполняем ячейки данными
        for (int col = 0; col < query.record().count(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();

            // Особые обработки для разных типов данных
            if (query.record().fieldName(col) == "last_update" ||
                query.record().fieldName(col) == "last_update_change") {
                // Форматируем дату в читаемый вид
                item->setText(query.value(col).toDateTime().toString("dd.MM.yyyy hh:mm"));
            }
            else {
                // Стандартное текстовое представление
                item->setText(query.value(col).toString());
            }

            ui->tableWidget_historyKas->setItem(row, col, item);
        }
    }

    // Настраиваем внешний вид таблицы
    ui->tableWidget_historyKas->resizeColumnsToContents();
    ui->tableWidget_historyKas->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_historyKas->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_historyKas->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_historyKas->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void MainWindow::on_btn_saveNewDuty_pressed()
{
    QSqlQuery query;

    //прочитать из формы
    QString blok = ui->lineEdit_blok->text();
    QString room = ui->lineEdit_room->text();
    QString equipment = ui->lineEdit_equipment->text();
    QString name_work = ui->lineEdit_name_work->text();
    QString type_work = ui->lineEdit_type_work->text();
    QString duty_note = ui->lineEdit_duty_note->text();

    int time = ui->spinBox_time->value();

    float dose_a_1 = ui->doubleSpinBox_dose_a_1->value();
    float dose_a_2 = ui->doubleSpinBox_dose_a_2->value();
    if (dose_a_1 > dose_a_2) {
        QMessageBox::warning(this, "Ошибка201", "Неверные значения по дозе Альфа");
        return;}

    float rate_a_1 = ui->doubleSpinBox_rate_a_1->value();
    float rate_a_2 = ui->doubleSpinBox_rate_a_2->value();
    if (rate_a_1 > rate_a_2) {
        QMessageBox::warning(this, "Ошибка201", "Неверные значения по мощности дозы Альфа");
        return;}

    float dose_b_1 = ui->doubleSpinBox_dose_b_1->value();
    float dose_b_2 = ui->doubleSpinBox_dose_b_2->value();
    if (dose_b_1 > dose_b_2) {
        QMessageBox::warning(this, "Ошибка201", "Неверные значения по дозе Бета");
        return;}

    float rate_b_1 = ui->doubleSpinBox_rate_b_1->value();
    float rate_b_2 = ui->doubleSpinBox_rate_b_2->value();
    if (rate_b_1 > rate_b_2) {
        QMessageBox::warning(this, "Ошибка201", "Неверные значения по мощности дозы Бета");
        return;}

    float dose_g_1 = ui->doubleSpinBox_dose_g_1->value();
    float dose_g_2 = ui->doubleSpinBox_dose_g_2->value();
    if (dose_g_1 > dose_g_2) {
        QMessageBox::warning(this, "Ошибка201", "Неверные значения по дозе Гамма");
        return;}
    float rate_g_1 = ui->doubleSpinBox_rate_g_1->value();
    float rate_g_2 = ui->doubleSpinBox_rate_g_2->value();
    if (rate_g_1 > rate_g_2) {
        QMessageBox::warning(this, "Ошибка201", "Неверные значения по мощности дозы Гамма");
        return;}

    float dose_n_1 = ui->doubleSpinBox_dose_n_1->value();
    float dose_n_2 = ui->doubleSpinBox_dose_n_2->value();
    if (dose_n_1 > dose_n_2) {
        QMessageBox::warning(this, "Ошибка201", "Неверные значения по дозе Нейтронны");
        return;}

    float rate_n_1 = ui->doubleSpinBox_rate_n_1->value();
    float rate_n_2 = ui->doubleSpinBox_rate_n_2->value();
    if (rate_n_1 > rate_n_2) {
        QMessageBox::warning(this, "Ошибка201", "Неверные значения по мощности дозы Нейтронны");
        return;}

    QString type_duty_0 = ui->comboBox_type_duty->currentText();
    int type_duty = -1; // Инициализируем значением по умолчанию

    if (type_duty_0 == "Наряд") {type_duty = 1;}
    else if (type_duty_0 == "Распоряжение") {type_duty = 0;}
    else {
        QMessageBox::warning(this, "Ошибка21 Duty", "Ошибка чтения вида работ: неизвестный тип");
        return;
    }


    //duty_id - количество нарядов +1
    int duty_id = 1; // Значение по умолчанию, если таблица пуста

    if (query.exec("SELECT MAX(duty_id) AS max_id FROM duty;")) {
        if (query.next()) {
            // Если в таблице есть записи, берём максимальный ID + 1
            duty_id = query.value("max_id").toInt() + 1;
            //qDebug() << "Максимальный duty_id:" << duty_id - 1;
            //qDebug() << "Новый duty_id:" << duty_id;
        } else {
            //qDebug() << "Таблица пуста, используем duty_id = 1";
        }
    } else {
        qDebug() << "Ошибка выполнения запроса11:" << query.lastError().text();
    }

    //label_last_update - последнее обновление - дд/мм/гг, мм/чч

    // Указываем часовой пояс для Москвы (UTC+3)
    QTimeZone moscowTimeZone("Europe/Moscow");
    // Получаем текущее время и дату в UTC
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    // Конвертируем время в московское
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString last_update = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");
    //qDebug() << "Current date and time in Moscow:" << dateTimeString;


    //выполнить запрос
    query.prepare("INSERT INTO duty (duty_id, type_duty, blok, room, equipment, name_work, type_work, time,dose_a_1, dose_a_2, rate_a_1, rate_a_2, dose_b_1, dose_b_2, rate_b_1, rate_b_2, dose_g_1, dose_g_2, rate_g_1, rate_g_2, dose_n_1, dose_n_2, rate_n_1, rate_n_2, duty_note, last_update) VALUES (:duty_id, :type_duty, :blok, :room, :equipment, :name_work, :type_work, :time, :dose_a_1, :dose_a_2, :rate_a_1, :rate_a_2, :dose_b_1, :dose_b_2, :rate_b_1, :rate_b_2, :dose_g_1, :dose_g_2, :rate_g_1, :rate_g_2, :dose_n_1, :dose_n_2, :rate_n_1, :rate_n_2, :duty_note, :last_update)");

    //привызка переменных
    query.bindValue(":duty_id", duty_id);
    query.bindValue(":type_duty", type_duty);
    query.bindValue(":blok", blok);
    query.bindValue(":room", room);
    query.bindValue(":equipment", equipment);
    query.bindValue(":name_work", name_work);
    query.bindValue(":type_work", type_work);
    query.bindValue(":time", time);
    query.bindValue(":dose_a_1", dose_a_1);
    query.bindValue(":dose_a_2", dose_a_2);
    query.bindValue(":rate_a_1", rate_a_1);
    query.bindValue(":rate_a_2", rate_a_2);
    query.bindValue(":dose_b_1", dose_b_1);
    query.bindValue(":dose_b_2", dose_b_2);
    query.bindValue(":rate_b_1", rate_b_1);
    query.bindValue(":rate_b_2", rate_b_2);
    query.bindValue(":dose_g_1", dose_g_1);
    query.bindValue(":dose_g_2", dose_g_2);
    query.bindValue(":rate_g_1", rate_g_1);
    query.bindValue(":rate_g_2", rate_g_2);
    query.bindValue(":dose_n_1", dose_n_1);
    query.bindValue(":dose_n_2", dose_n_2);
    query.bindValue(":rate_n_1", rate_n_1);
    query.bindValue(":rate_n_2", rate_n_2);
    query.bindValue(":duty_note", duty_note);
    query.bindValue(":last_update", last_update);

    //проверка выполнения запроса и очистка формы
    if (query.exec()) {
        //QMessageBox::information(this, "Успех", "Данные записаны");

        ui->lineEdit_blok->clear();
        ui->lineEdit_room->clear();
        ui->lineEdit_equipment->clear();
        ui->lineEdit_name_work->clear();
        ui->lineEdit_type_work->clear();
        ui->lineEdit_duty_note->clear();

        ui->doubleSpinBox_dose_a_1->setValue(0);
        ui->doubleSpinBox_dose_a_2->setValue(0);
        ui->doubleSpinBox_rate_a_1->setValue(0);
        ui->doubleSpinBox_rate_a_2->setValue(0);
        ui->doubleSpinBox_dose_b_1->setValue(0);
        ui->doubleSpinBox_dose_b_2->setValue(0);
        ui->doubleSpinBox_rate_b_1->setValue(0);
        ui->doubleSpinBox_rate_b_2->setValue(0);
        ui->doubleSpinBox_dose_g_1->setValue(0);
        ui->doubleSpinBox_dose_g_2->setValue(0);
        ui->doubleSpinBox_rate_g_1->setValue(0);
        ui->doubleSpinBox_rate_g_2->setValue(0);
        ui->doubleSpinBox_dose_n_1->setValue(0);
        ui->doubleSpinBox_dose_n_2->setValue(0);
        ui->doubleSpinBox_rate_n_1->setValue(0);
        ui->doubleSpinBox_rate_n_2->setValue(0);

        ui->label_duty_id->setText("-");
        ui->label_last_update->setText("-");

    } else {
        QMessageBox::warning(this, "Ошибка21 Duty, не удалось сохранить наряд", query.lastError().text());
        qDebug() << query.lastError().text();
    }


    //обновить таблицу
    refresh_duty_table();


    QVector<double> doses = {
        dose_a_1, dose_a_2, rate_a_1, rate_a_2,   // Альфа-параметры
        dose_b_1, dose_b_2, rate_b_1, rate_b_2,   // Бета-параметры
        dose_g_1, dose_g_2, rate_g_1, rate_g_2,   // Гамма-параметры
        dose_n_1, dose_n_2, rate_n_1, rate_n_2    // Нейтронные параметры
    };

    QString for_user_id = ui->label_userId->text();
    QString changing_user_id;
    QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString name_0 = parts[0];
    QString name_1 = parts[1];
    QString name_2 = parts[2];
    query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
    query.bindValue(":name_0", name_0);
    query.bindValue(":name_1", name_1);
    query.bindValue(":name_2", name_2);
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка028", "Не удалось выполнить запрос: " + query.lastError().text());
        qDebug() << "Ошибка поиска id SQL:" << query.lastQuery() << query.lastError().text();
        return;
    }
    if (query.next()) {
        changing_user_id = query.value("user_id").toString();
    } else {
        QMessageBox::information(this, "Информация", "Пользователь не найден");
    }
    QString type_edit = "create";

    QDateTime SetLastUpdate_1 = QDateTime::fromString(last_update, "yyyy-MM-dd HH:mm:ss");
    bool success = addDutyHistory(
        QString::number(duty_id),
        type_duty,
        blok,
        room,
        equipment,
        name_work,
        type_work,
        time,
        doses,
        duty_note,
        SetLastUpdate_1,
        changing_user_id,
        type_edit
    );

    if (success) {
        QMessageBox::information(this, "Успех", "Наряд успешно добавлен");
    } else {
        QMessageBox::warning(this, "Ошибка029", "Не удалось добавить наряд");
    }

    setupSearchDutyComboBox(ui->select_dutyFilter);
    setupSearchDutyComboBox(ui->duty_id);
}



bool MainWindow::addDutyHistory(
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
    const QString& user_id_change,
    const QString& type_edit)
{
    // === ПРОВЕРКА ВХОДНЫХ ДАННЫХ ===
    if (doses.size() != 16) {
        qDebug() << "Ошибка: неверный размер doses. Ожидается 16, получено:" << doses.size();
        return false;
    }

    if (duty_id.isEmpty() || user_id_change.isEmpty()) {
        qDebug() << "Ошибка: пустой duty_id или user_id_change";
        return false;
    }

    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO duty_history (
            duty_id, type_duty, blok, room, equipment, name_work, type_work, time,
            dose_a_1, dose_a_2, rate_a_1, rate_a_2,
            dose_b_1, dose_b_2, rate_b_1, rate_b_2,
            dose_g_1, dose_g_2, rate_g_1, rate_g_2,
            dose_n_1, dose_n_2, rate_n_1, rate_n_2,
            duty_note, last_update, user_id_change, type_edit
        ) VALUES (
            :duty_id, :type_duty, :blok, :room, :equipment, :name_work, :type_work, :time,
            :dose_a_1, :dose_a_2, :rate_a_1, :rate_a_2,
            :dose_b_1, :dose_b_2, :rate_b_1, :rate_b_2,
            :dose_g_1, :dose_g_2, :rate_g_1, :rate_g_2,
            :dose_n_1, :dose_n_2, :rate_n_1, :rate_n_2,
            :duty_note, :last_update, :user_id_change, :type_edit
        )
    )");

    try {
        // Основные параметры
        query.bindValue(":duty_id", duty_id);
        query.bindValue(":type_duty", type_duty);
        query.bindValue(":blok", blok);
        query.bindValue(":room", room);
        query.bindValue(":equipment", equipment);
        query.bindValue(":name_work", name_work);
        query.bindValue(":type_work", type_work);
        query.bindValue(":time", time);
        query.bindValue(":last_update", SetLastUpdate.toString("yyyy-MM-dd HH:mm:ss"));
        query.bindValue(":type_edit", type_edit);

        // Дозы и коэффициенты с проверкой индексов
        if (doses.size() >= 16) {
            query.bindValue(":dose_a_1", doses[0]);
            query.bindValue(":dose_a_2", doses[1]);
            query.bindValue(":rate_a_1", doses[2]);
            query.bindValue(":rate_a_2", doses[3]);

            query.bindValue(":dose_b_1", doses[4]);
            query.bindValue(":dose_b_2", doses[5]);
            query.bindValue(":rate_b_1", doses[6]);
            query.bindValue(":rate_b_2", doses[7]);

            query.bindValue(":dose_g_1", doses[8]);
            query.bindValue(":dose_g_2", doses[9]);
            query.bindValue(":rate_g_1", doses[10]);
            query.bindValue(":rate_g_2", doses[11]);

            query.bindValue(":dose_n_1", doses[12]);
            query.bindValue(":dose_n_2", doses[13]);
            query.bindValue(":rate_n_1", doses[14]);
            query.bindValue(":rate_n_2", doses[15]);
        }

        query.bindValue(":duty_note", duty_note);
        query.bindValue(":user_id_change", user_id_change);

        if (!query.exec()) {
            qDebug() << "Ошибка добавления duty_history:" << query.lastError().text();
            qDebug() << "Запрос:" << query.lastQuery();
            return false;
        }

        // Успешное выполнение
        qDebug() << "Запись duty_history успешно добавлена, duty_id:" << duty_id;

        // Обновляем таблицу истории
        QTimer::singleShot(0, this, &MainWindow::loadDutyHistoryToTable);

        return true;

    } catch (const std::exception& e) {
        qDebug() << "Исключение в addDutyHistory:" << e.what();
        return false;
    } catch (...) {
        qDebug() << "Неизвестное исключение в addDutyHistory";
        return false;
    }
}

void MainWindow::loadDutyHistoryToTable()
{
    ui->tableWidget_historyDuty->clear();
    ui->tableWidget_historyDuty->setRowCount(0);

    ui->tableWidget_historyDuty->setColumnCount(29);

    QStringList headers;
    headers << "ID наряда" << "Тип" << "Блок" << "Помещение" << "Оборудование"
            << "Работа" << "Тип работы" << "Время"
            << "α1" << "α2" << "Мα1" << "Мα2"
            << "β1" << "β2" << "Мβ1" << "Мβ2"
            << "γ1" << "γ2" << "Мγ1" << "Мγ2"
            << "Н1" << "Н2" << "МН1" << "МН2"
            << "Примечание" << "Дата обновления" << "ID изменившего" << "Дата изменения" << "Тип изменения";

    ui->tableWidget_historyDuty->setHorizontalHeaderLabels(headers);

    QSqlQuery query;
    QString sql = R"(
        SELECT duty_id, type_duty, blok, room, equipment, name_work, type_work, time,
               dose_a_1, dose_a_2, rate_a_1, rate_a_2, dose_b_1, dose_b_2, rate_b_1, rate_b_2,
               dose_g_1, dose_g_2, rate_g_1, rate_g_2, dose_n_1, dose_n_2, rate_n_1, rate_n_2,
               duty_note, last_update, user_id_change, last_update_change, type_edit
        FROM duty_history
        ORDER BY last_update DESC
        LIMIT 300
    )";

    if (!query.exec(sql)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return;
    }

    //qDebug() << "Количество полей в результате:" << query.record().count();
    for (int i = 0; i < query.record().count(); ++i) {
        //qDebug() << "Поле" << i << ":" << query.record().fieldName(i) << "Значение:" << query.value(i).toString();
    }

    while (query.next()) {
        int row = ui->tableWidget_historyDuty->rowCount();
        ui->tableWidget_historyDuty->insertRow(row);

        for (int col = 0; col < query.record().count(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();
            QVariant value = query.value(col);

            // 0:duty_id, 1:type_duty, 2:blok, 3:room, 4:equipment, 5:name_work, 6:type_work, 7:time,
            // 8-23: дозы и мощности, 24:duty_note, 25:last_update, 26:user_id_change, 27:last_update_change, 28:type_edit

            if (col == 0) { // duty_id
                item->setText(value.toString());
            }
            else if (col == 1) { // type_duty
                item->setText(value.toInt() == 1 ? "Наряд" : "Распор.");
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col == 7) { // time
                item->setText(value.toString());
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col >= 8 && col <= 23) { // Дозы и мощности (8-23)
                if (value.isNull() || value.toString().isEmpty()) {
                    item->setText("0.00");
                } else {
                    bool ok;
                    double numValue = value.toDouble(&ok);
                    item->setText(ok ? QString::number(numValue, 'f', 2) : "0.00");
                }
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col == 24) { // duty_note
                item->setText(value.toString().isEmpty() ? "Нет примечания" : value.toString());
            }
            else if (col == 25) { // last_update
                item->setText(value.toDateTime().toString("dd.MM.yyyy HH:mm"));
            }
            else if (col == 26) { // user_id_change - ID изменившего
                item->setText(value.toString());
                item->setTextAlignment(Qt::AlignCenter);
                //qDebug() << "user_id_change found:" << value.toString(); // Отладочный вывод
            }
            else if (col == 27) { // last_update_change
                item->setText(value.toDateTime().toString("dd.MM.yyyy HH:mm"));
            }
            else if (col == 28) { // type_edit - Тип изменения
                item->setText(value.toString());
                item->setTextAlignment(Qt::AlignCenter);
                //qDebug() << "type_edit found:" << value.toString(); // Отладочный вывод
            }
            else {
                // Все остальные текстовые поля (2-6: blok, room, equipment, name_work, type_work)
                item->setText(value.toString());
            }

            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget_historyDuty->setItem(row, col, item);
        }
    }

    ui->tableWidget_historyDuty->resizeColumnsToContents();
    ui->tableWidget_historyDuty->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_historyDuty->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_historyDuty->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_historyDuty->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //qDebug() << "Загружено строк:" << ui->tableWidget_historyDuty->rowCount();
}

void MainWindow::on_btn_saveNewDutyUser_pressed()
{
    // Получаем данные из формы
    QString duty_id_str = ui->duty_id->currentText();
    QStringList parts = duty_id_str.split(" - ");
    QString duty_id_st = parts[0].trimmed();
    int duty_id = duty_id_st.toInt();

    QString user_id_str = ui->comboBox_user_id->currentText();
    QStringList parts2 = user_id_str.split(" - ");
    QString user_id_st = parts2[0].trimmed();
    int user_id = user_id_st.toInt();

    QString duty_note = ui->lineEdit_users_duty_note->text();
    QDate date_start = ui->dateTimeEdit_date_start->date();
    QDate date_finish = ui->dateTimeEdit_date_finish->date();

    // Получаем московское время
    QTimeZone moscowTimeZone("Europe/Moscow");
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString SetLastUpdate = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");

    // Генерируем новый ID
    int users_duty_id = 1;
    QSqlQuery query;
    if (query.exec("SELECT MAX(users_duty_id) AS max_id FROM users_duty")) {
        if (query.next()) {
            users_duty_id = query.value("max_id").toInt() + 1;
        }
    }

    // Сохраняем в основную таблицу
    query.prepare("INSERT INTO users_duty (users_duty_id, user_id, duty_id, date_start, date_finish, users_duty_note) VALUES (:users_duty_id, :user_id, :duty_id, :date_start, :date_finish, :users_duty_note)");//, :last_update)
    query.bindValue(":users_duty_id", users_duty_id);
    query.bindValue(":user_id", user_id);
    query.bindValue(":duty_id", duty_id);
    query.bindValue(":date_start", date_start);
    query.bindValue(":date_finish", date_finish);
    query.bindValue(":users_duty_note", duty_note);
    //query.bindValue(":last_update", SetLastUpdate);

    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Данные записаны");

        // Очищаем форму
        ui->lineEdit_users_duty_note->clear();
        ui->dateTimeEdit_date_start->setDate(QDate::currentDate());
        ui->dateTimeEdit_date_finish->setDate(QDate::currentDate());
        ui->label_users_duty_id->setText("-");

        // Получаем ID пользователя для истории
        QString for_user_id = ui->label_userId->text();
        QString changing_user_id;
        QStringList parts1 = for_user_id.split(' ', Qt::SkipEmptyParts);
        if (parts1.size() >= 3) {
            QString name_0 = parts1[0];
            QString name_1 = parts1[1];
            QString name_2 = parts1[2];
            QSqlQuery userQuery;
            userQuery.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
            userQuery.bindValue(":name_0", name_0);
            userQuery.bindValue(":name_1", name_1);
            userQuery.bindValue(":name_2", name_2);
            if (userQuery.exec() && userQuery.next()) {
                changing_user_id = userQuery.value("user_id").toString();
            } else {
                changing_user_id = "1";
            }
        } else {
            changing_user_id = "1";
        }

        // Добавляем запись в историю
        QDateTime SetLastUpdate_1 = QDateTime::fromString(SetLastUpdate, "yyyy-MM-dd HH:mm:ss");
        QString type_edit = "create";
        bool success = addUserDutyHistory(users_duty_id, user_id, duty_id, date_start, date_finish, duty_note, SetLastUpdate_1, changing_user_id, type_edit);

        // Обновляем интерфейс
        tableUsersDutyUpd();
        on_btn_userDutyClear_pressed();

    } else {
        QMessageBox::warning(this, "Ошибка030", "Не удалось сохранить наряд: " + query.lastError().text());
    }


    setupSearchUsersDutyComboBox(ui->comboBox_DoseUsersDutyId);
}


void MainWindow::tableUsersDutyUpd(){

    QSqlQuery query;
    // обновление данных таблицы
    ui->line_userDutySearch->clear();

    //QSqlQuery query;
    if (!query.exec("SELECT * FROM users_duty ORDER BY users_duty_id ASC")) {
        QMessageBox::critical(this, "Ошибка031", "Не удалось выполнить запрос: " + query.lastError().text());
        qDebug() << "Ошибка SQL:" << query.lastQuery() << query.lastError().text();
        return;
    }

    // 1. Полностью очищаем таблицу
    ui->table_userDuty->clear();
    ui->table_userDuty->setRowCount(0);
    ui->table_userDuty->setColumnCount(0);

    // 2. Получаем структуру записи из запроса
    QSqlRecord record = query.record();
    int columnCount = record.count();

    // 3. Устанавливаем количество столбцов
    ui->table_userDuty->setColumnCount(columnCount);

    // 4. Устанавливаем заголовки (важно делать это ПОСЛЕ установки количества столбцов)
    QStringList headers;
    headers << "ID"
            << "ID пользователя"
            << "ID наряда"
            << "Дата начала"
            << "Дата завершения"
            << "Комментарий"
            << "Последнее обновление";
    ui->table_userDuty->setHorizontalHeaderLabels(headers);

    // 5. Включаем сортировку (если нужно)
    ui->table_userDuty->setSortingEnabled(false);

    // 6. Заполняем таблицу данными
    while (query.next()) {
        int row = ui->table_userDuty->rowCount();
        ui->table_userDuty->insertRow(row);

        for (int col = 0; col < columnCount; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui->table_userDuty->setItem(row, col, item);
        }
    }

    // 7. Настраиваем отображение (обязательные действия)
    ui->table_userDuty->horizontalHeader()->setVisible(true); // Показываем заголовки
    ui->table_userDuty->horizontalHeader()->setStretchLastSection(true); // Растягиваем последний столбец
    ui->table_userDuty->resizeColumnsToContents(); // Автоподбор ширины


}



// Добавление записи в users_duty_history
bool MainWindow::addUserDutyHistory(
    int users_duty_id,
    int user_id,
    int duty_id,
    const QDate &date_start,
    const QDate &date_finish,
    const QString &duty_note,
    const QDateTime &SetLastUpdate,
    const QString &user_id_change,
    const QString &type_edit
){
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO users_duty_history (
            users_duty_id, user_id, duty_id, date_start, date_finish,
            users_duty_note, last_update_change, user_id_change, type_edit
        ) VALUES (
            :users_duty_id, :user_id, :duty_id, :date_start, :date_finish,
            :users_duty_note, :last_update, :user_id_change, :type_edit
        )
    )");

    query.bindValue(":users_duty_id", users_duty_id);
    query.bindValue(":user_id", user_id);
    query.bindValue(":duty_id", duty_id);
    query.bindValue(":date_start", date_start);
    query.bindValue(":date_finish", date_finish);
    query.bindValue(":users_duty_note", duty_note);
    query.bindValue(":last_update", SetLastUpdate);
    query.bindValue(":user_id_change", user_id_change);
    query.bindValue(":type_edit", type_edit);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления users_duty_history:" << query.lastError().text();
        return false;
    }


    loadDutyUsersHistoryToTable();
}


void MainWindow::loadDutyUsersHistoryToTable()
{
    // Очищаем таблицу перед загрузкой новых данных
    ui->tableWidget_historyDuty_2->clear();
    ui->tableWidget_historyDuty_2->setRowCount(0);

    // Устанавливаем количество столбцов
    ui->tableWidget_historyDuty_2->setColumnCount(11); // По количеству полей в users_history

    // Устанавливаем русские заголовки
    QStringList headers;
    headers << "ID" << "ID пользователя" << "ID наряда"
            << "Дата начала" << "Дата окончания" << "Примечание"
            << "Дата обновления" << "ID изменившего" << "Последнее обновление" << "Тип изменения" << "Последнее изменение";

    ui->tableWidget_historyDuty_2->setHorizontalHeaderLabels(headers);

    // Выполняем SQL-запрос
    QSqlQuery query;
    query.prepare("SELECT * FROM users_duty_history ORDER BY last_update_change DESC");

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса14:" << query.lastError().text();
        return;
    }

    // Заполняем таблицу данными
    while (query.next()) {
        int row = ui->tableWidget_historyDuty_2->rowCount();
        ui->tableWidget_historyDuty_2->insertRow(row);

        // Заполняем ячейки данными
        for (int col = 0; col < query.record().count(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();
            QString fieldName = query.record().fieldName(col);

            // Обработка разных типов данных
            if (fieldName == "date_start" || fieldName == "date_finish") {
                // Форматирование дат в вид "дд.мм.гггг"
                item->setText(query.value(col).toDate().toString("dd.MM.yyyy"));
            }
            else if (fieldName == "last_update_change") {
                // Форматирование временной метки
                item->setText(query.value(col).toDateTime().toString("dd.MM.yyyy HH:mm"));
            }
            else if (fieldName == "users_duty_id" ||
                     fieldName == "user_id" ||
                     fieldName == "duty_id" ||
                     fieldName == "user_id_change") {
                // Целочисленные значения
                item->setText(query.value(col).toString());
                item->setTextAlignment(Qt::AlignCenter); // Выравнивание по центру для чисел
            }
            else if (fieldName == "users_duty_note") {
                // Текстовые примечания
                QString note = query.value(col).toString();
                item->setText(note.isEmpty() ? "Нет примечания" : note);
            }
            else {
                // Все остальные поля (на случай расширения таблицы)
                item->setText(query.value(col).toString());
            }

            // Запрещаем редактирование ячеек
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget_historyDuty_2->setItem(row, col, item);
        }
    }

    // Настраиваем внешний вид таблицы
    ui->tableWidget_historyDuty_2->resizeColumnsToContents();
    ui->tableWidget_historyDuty_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_historyDuty_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_historyDuty_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_historyDuty_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
}



void MainWindow::on_table_userDuty_cellPressed(int row, int column)
{
        QTableWidgetItem *Uduty_id = ui->table_userDuty->item(row, 0);
        if (Uduty_id) {
            ui->label_users_duty_id->setText(Uduty_id->text());
        }

        QTableWidgetItem *users_duty_note = ui->table_userDuty->item(row, 5);
        if (users_duty_note) {
            ui->lineEdit_users_duty_note->setText(users_duty_note->text());
        }

        QTableWidgetItem *date_start = ui->table_userDuty->item(row, 3);
        if (date_start) {
            ui->dateTimeEdit_date_start->setDate(QDate::fromString(date_start->text(), "yyyy-MM-dd"));
        }

        QTableWidgetItem *date_finish = ui->table_userDuty->item(row, 4);
        if (date_finish) {
            ui->dateTimeEdit_date_finish->setDate(QDate::fromString(date_finish->text(), "yyyy-MM-dd"));
        }

        QTableWidgetItem *duty_id = ui->table_userDuty->item(row, 2);
        if (duty_id) {
            QString duty_id_str = duty_id->text();
            QStringList parts = duty_id_str.split(" - ");
            if (!parts.isEmpty()) {
                QString duty_id_st = parts[0].trimmed();

                // Ищем точное совпадение по ID
                int foundIndex = -1;

                // Временно блокируем сигналы, чтобы не вызывать фильтрацию


                ui->duty_id->blockSignals(true);

                // Заполняем список только если он пустой
                if (ui->duty_id->count() == 0) {
                    QSqlQuery query;
                    query.prepare("SELECT duty_id, name_work FROM duty");

                    if (query.exec()) {
                        while (query.next()) {
                            QString id = query.value("duty_id").toString();
                            QString name = query.value("name_work").toString();
                            ui->duty_id->addItem(id + " - " + name, id);
                        }

                        // Настройка фильтрации (только один раз)
                        static bool filterInitialized = false;
                        if (!filterInitialized) {
                            QLineEdit* searchEdit = ui->duty_id->lineEdit();
                            searchEdit->setPlaceholderText("Введите для поиска...");
                            searchEdit->setClearButtonEnabled(true);

                            auto dutyItems = QSharedPointer<QVector<QPair<QString, QString>>>::create();
                            QSqlQuery queryAll;
                            queryAll.prepare("SELECT duty_id, name_work FROM duty");
                            if (queryAll.exec()) {
                                while (queryAll.next()) {
                                    dutyItems->append(qMakePair(
                                        queryAll.value("duty_id").toString(),
                                        queryAll.value("name_work").toString()
                                    ));
                                }
                            }

                            connect(searchEdit, &QLineEdit::textChanged, this, [this, dutyItems]() {
                                static QString lastText;
                                QString currentText = ui->duty_id->lineEdit()->text();
                                if (currentText == lastText) return;
                                lastText = currentText;

                                QSignalBlocker blocker(ui->duty_id);
                                QString savedText = currentText;
                                int cursorPos = ui->duty_id->lineEdit()->cursorPosition();

                                // Сохраняем выбранный элемент
                                int currentIndex = ui->duty_id->currentIndex();
                                QString currentData = currentIndex >= 0 ? ui->duty_id->itemData(currentIndex).toString() : "";

                                // Очищаем и заполняем заново
                                ui->duty_id->clear();
                                ui->duty_id->addItem("Не выбрано");
                                ui->duty_id->addItem("");

                                for (const auto& item : *dutyItems) {
                                    QString displayText = item.first + " - " + item.second;
                                    if (currentText.isEmpty() ||
                                        displayText.contains(currentText, Qt::CaseInsensitive)) {
                                        ui->duty_id->addItem(displayText, item.first);
                                    }
                                }

                                // Восстанавливаем выбор
                                if (!currentData.isEmpty()) {
                                    int idx = ui->duty_id->findData(currentData);
                                    if (idx >= 0) ui->duty_id->setCurrentIndex(idx);
                                }

                                ui->duty_id->lineEdit()->setText(savedText);
                                ui->duty_id->lineEdit()->setCursorPosition(cursorPos);

                                if (!currentText.isEmpty()) {
                                    ui->duty_id->showPopup();
                                }
                            });

                            filterInitialized = true;
                        }
                    }
                }

                // Устанавливаем текущее значение
                for (int i = 0; i < ui->duty_id->count(); ++i) {
                    if (ui->duty_id->itemData(i).toString() == duty_id_st) {
                        foundIndex = i;
                        break;
                    }
                }

                if (foundIndex >= 0) {
                    ui->duty_id->setCurrentIndex(foundIndex);
                } else {
                    ui->duty_id->setCurrentIndex(0);
                }

                // Разблокируем сигналы
                ui->duty_id->blockSignals(false);
            }
        }

        // Очищаем текущую кнопку
        QLayoutItem *item;
        while ((item = ui->horizontalLayout_btnUserDuty->takeAt(0))) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }

        // Создаем новую кнопку
        QPushButton *btn_saveChangeUserDuty = new QPushButton("Сохранить изменения наряда", this);
        ui->horizontalLayout_btnUserDuty->addWidget(btn_saveChangeUserDuty);
        connect(btn_saveChangeUserDuty, &QPushButton::clicked, this, &MainWindow::on_btn_saveChangeUserDuty_pressed);

        checkRole();
    }


void MainWindow::on_btn_saveChangeUserDuty_pressed()
{
    // Получаем данные
    QString users_duty_id = ui->label_users_duty_id->text();

    QString duty_id_str = ui->duty_id->currentText();
    QStringList parts = duty_id_str.split(" - ");
    QString duty_id_st = parts[0].trimmed();
    int duty_id = duty_id_st.toInt();

    QString user_id_str = ui->comboBox_user_id->currentText();
    QStringList parts2 = user_id_str.split(" - ");
    QString user_id_st = parts2[0].trimmed();
    int user_id = user_id_st.toInt();

    QString duty_note = ui->lineEdit_users_duty_note->text();

    QDate date_start = ui->dateTimeEdit_date_start->date();
    QDate date_finish = ui->dateTimeEdit_date_finish->date();

    QTimeZone moscowTimeZone("Europe/Moscow");
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString SetLastUpdate = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");

    QSqlQuery query;

    query.prepare(R"(
        UPDATE users_duty
        SET
            user_id = :user_id,
            duty_id = :duty_id,
            date_start = :date_start,
            date_finish = :date_finish,
            users_duty_note = :users_duty_note
        WHERE users_duty_id = :users_duty_id
    )");
    //last_update = :last_update

    query.bindValue(":user_id", user_id);
    query.bindValue(":duty_id", duty_id);
    query.bindValue(":date_start", date_start);
    query.bindValue(":date_finish", date_finish);
    query.bindValue(":users_duty_note", duty_note);
    //query.bindValue(":last_update", SetLastUpdate);
    query.bindValue(":users_duty_id", users_duty_id);

    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Данные изменены");
    } else {
        QMessageBox::warning(this, "Ошибка032", query.lastError().text());
        qDebug() << "SQL Error:" << query.lastError().text();
        qDebug() << "Executed query:" << query.executedQuery();
        qDebug() << "Bound values:" << query.boundValues();
    }


tableUsersDutyUpd();


    on_btn_userDutyClear_pressed();


    int users_duty_id_1 = users_duty_id.toInt();
    QDateTime SetLastUpdate_1 = QDateTime::fromString(SetLastUpdate, "yyyy-MM-dd HH:mm:ss");
    QString for_user_id = ui->label_userId->text();
    QString changing_user_id;
    QStringList parts1 = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString name_0 = parts1[0];
    QString name_1 = parts1[1];
    QString name_2 = parts1[2];
    query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
    query.bindValue(":name_0", name_0);
    query.bindValue(":name_1", name_1);
    query.bindValue(":name_2", name_2);
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка033", "Не удалось выполнить запрос: " + query.lastError().text());
        qDebug() << "Ошибка поиска id SQL:" << query.lastQuery() << query.lastError().text();
        return;
    }
    if (query.next()) {
        changing_user_id = query.value("user_id").toString();
    } else {
        QMessageBox::information(this, "Информация", "Пользователь не найден");
    }
    QString type_edit = "change";

    bool success = addUserDutyHistory(
        users_duty_id_1,
        user_id,
        duty_id,
        date_start,
        date_finish,
        duty_note,
        SetLastUpdate_1,
        changing_user_id,
        type_edit
    );

    // Проверяем результат
    if (success) {
        qDebug() << "Запись успешно добавлена в users_duty_history";
        // Обновляем интерфейс или выполняем другие действия
    } else {
        qDebug() << "Не удалось добавить запись";
        // Показываем сообщение об ошибке
    }


    on_btn_userDutyClear_pressed();

    setupSearchUsersDutyComboBox(ui->comboBox_DoseUsersDutyId);
}




void MainWindow::on_btn_userDutyClear_pressed() //очистка полей для создания нового
{
    ui->lineEdit_users_duty_note->clear();
    ui->dateTimeEdit_date_start->setDate(QDate::currentDate());
    ui->dateTimeEdit_date_finish->setDate(QDate::currentDate());
    ui->label_users_duty_id->setText("-");


    QString duty_id_str = "0";
    QStringList parts = duty_id_str.split(" - ");
    QString duty_id_st = parts[0].trimmed();
    int duty_id_index = duty_id_st.toInt();
    ui->duty_id->setCurrentIndex(duty_id_index);

    QString user_id_str = "0";
    QStringList parts2 = user_id_str.split(" - ");
    QString user_id_st = parts2[0].trimmed();
    int user_id_index = user_id_st.toInt();
    ui->comboBox_user_id->setCurrentIndex(user_id_index);


    //поменять кнопку
    QLayoutItem *item;
    while ((item = ui->horizontalLayout_btnUserDuty->takeAt(0))) {
        if (item->widget()) {
            delete item->widget(); // Удаляем виджет
        }
        delete item; // Удаляем элемент layout
    }

    // Создаем новую кнопку
    QPushButton *btn_saveNewDutyUser = new QPushButton("Создать новый наряд пользователя", this);
    ui->horizontalLayout_btnUserDuty->addWidget(btn_saveNewDutyUser);
    //qDebug() << "Родительский класс кнопки:" << btn_saveNewDutyUser->parent(); //QWidget(0x1f4ae248d90, name = "tab")
    if (btn_saveNewDutyUser){
        connect(btn_saveNewDutyUser, &QPushButton::pressed, this, &MainWindow::on_btn_saveNewDutyUser_pressed);//не забыть по h
    }


}




void MainWindow::on_tableDuty_cellPressed(int row, int column) //не работает
{
    //данные для отобрадения брать из таблицы
    QTableWidgetItem *duty_id = ui->tableDuty->item(row, 0);
    QString duty_id_str = duty_id->text();
    ui->label_duty_id->setText(duty_id_str);

    QSqlQuery query;
    query.prepare("SELECT type_duty, blok, room, equipment, name_work, type_work, time, dose_a_1, dose_a_2, rate_a_1, rate_a_2, dose_b_1, dose_b_2, rate_b_1, rate_b_2, dose_g_1, dose_g_2, rate_g_1, rate_g_2, dose_n_1, dose_n_2, rate_n_1, rate_n_2, duty_note, last_update FROM duty WHERE duty_id = :duty_id");
    query.bindValue(":duty_id", duty_id_str);

    if (!query.exec() || !query.next()) {
        qDebug() << "Query failed:" << query.lastError().text();
        return;
    }

    // Устанавливаем текстовые значения
    ui->lineEdit_blok->setText(query.value("blok").toString());
    ui->lineEdit_room->setText(query.value("room").toString());
    ui->lineEdit_equipment->setText(query.value("equipment").toString());
    ui->lineEdit_name_work->setText(query.value("name_work").toString());
    ui->lineEdit_type_work->setText(query.value("type_work").toString());
    ui->lineEdit_duty_note->setText(query.value("duty_note").toString());

    // Устанавливаем числовые значения
    ui->spinBox_time->setValue(query.value("time").toInt());

    ui->doubleSpinBox_dose_a_1->setValue(query.value("dose_a_1").toDouble());
    ui->doubleSpinBox_dose_a_2->setValue(query.value("dose_a_2").toDouble());
    ui->doubleSpinBox_rate_a_1->setValue(query.value("rate_a_1").toDouble());
    ui->doubleSpinBox_rate_a_2->setValue(query.value("rate_a_2").toDouble());

    ui->doubleSpinBox_dose_b_1->setValue(query.value("dose_b_1").toDouble());
    ui->doubleSpinBox_dose_b_2->setValue(query.value("dose_b_2").toDouble());
    ui->doubleSpinBox_rate_b_1->setValue(query.value("rate_b_1").toDouble());
    ui->doubleSpinBox_rate_b_2->setValue(query.value("rate_b_2").toDouble());

    ui->doubleSpinBox_dose_g_1->setValue(query.value("dose_g_1").toDouble());
    ui->doubleSpinBox_dose_g_2->setValue(query.value("dose_g_2").toDouble());
    ui->doubleSpinBox_rate_g_1->setValue(query.value("rate_g_1").toDouble());
    ui->doubleSpinBox_rate_g_2->setValue(query.value("rate_g_2").toDouble());

    ui->doubleSpinBox_dose_n_1->setValue(query.value("dose_n_1").toDouble());
    ui->doubleSpinBox_dose_n_2->setValue(query.value("dose_n_2").toDouble());
    ui->doubleSpinBox_rate_n_1->setValue(query.value("rate_n_1").toDouble());
    ui->doubleSpinBox_rate_n_2->setValue(query.value("rate_n_2").toDouble());

    // Обработка даты
    QString last_update = query.value("last_update").toString();
    QStringList parts = last_update.split("T");
    if (!parts.isEmpty()) {
        ui->label_last_update->setText(parts[0].trimmed());
    }



    QLayoutItem *item;
    while ((item = ui->layoutForBtnDuty->takeAt(0))) {
        if (item->widget()) {
            delete item->widget(); // Удаляем виджет
        }
        delete item; // Удаляем элемент layout
    }
    // Создаем новую кнопку
    QPushButton *btn_saveChangeDuty = new QPushButton("Сохранить изменения наряда", this);
    ui->layoutForBtnDuty->addWidget(btn_saveChangeDuty);
    //qDebug() << "Родительский класс кнопки:" << btn_saveChangeDuty->parent(); //QWidget(0x1f4ae248d90, name = "tab")
    if (btn_saveChangeDuty){
        connect(btn_saveChangeDuty, &QPushButton::pressed, this, &MainWindow::on_btn_saveChangeDuty_pressed);//не забыть по h
    }
    checkRole();
}




void MainWindow::on_btn_saveChangeDuty_pressed()
{
    // 1. Получаем данные из формы
    QString duty_id = ui->label_duty_id->text().trimmed();
    QString blok = ui->lineEdit_blok->text().trimmed();
    QString room = ui->lineEdit_room->text().trimmed();
    QString equipment = ui->lineEdit_equipment->text().trimmed();
    QString name_work = ui->lineEdit_name_work->text().trimmed();
    QString type_work = ui->lineEdit_type_work->text().trimmed();
    QString duty_note = ui->lineEdit_duty_note->text().trimmed();
    QString last_update = ui->label_last_update->text().trimmed();
    int time = ui->spinBox_time->value();

    // Проверки значений доз
    auto checkDose = [this](float val1, float val2, const QString& type) {
        if (val1 > val2) {
            QMessageBox::warning(this, "Ошибка034", QString("Неверные значения для %1: %2 > %3")
                                                     .arg(type).arg(val1).arg(val2));
            return false;
        }
        return true;
    };

    if (!checkDose(ui->doubleSpinBox_dose_a_1->value(), ui->doubleSpinBox_dose_a_2->value(), "дозы Альфа") ||
        !checkDose(ui->doubleSpinBox_rate_a_1->value(), ui->doubleSpinBox_rate_a_2->value(), "мощности дозы Альфа") ||
        !checkDose(ui->doubleSpinBox_dose_b_1->value(), ui->doubleSpinBox_dose_b_2->value(), "дозы Бета") ||
        !checkDose(ui->doubleSpinBox_rate_b_1->value(), ui->doubleSpinBox_rate_b_2->value(), "мощности дозы Бета") ||
        !checkDose(ui->doubleSpinBox_dose_g_1->value(), ui->doubleSpinBox_dose_g_2->value(), "дозы Гамма") ||  // Исправлено с dose_a на dose_g
        !checkDose(ui->doubleSpinBox_rate_g_1->value(), ui->doubleSpinBox_rate_g_2->value(), "мощности дозы Гамма") ||
        !checkDose(ui->doubleSpinBox_dose_n_1->value(), ui->doubleSpinBox_dose_n_2->value(), "дозы Нейтроны") ||
        !checkDose(ui->doubleSpinBox_rate_n_1->value(), ui->doubleSpinBox_rate_n_2->value(), "мощности дозы Нейтроны")) {
        return;
    }

    // Определяем тип наряда
    int type_duty = -1;
    QString type_duty_0 = ui->comboBox_type_duty->currentText();
    if (type_duty_0 == "Наряд") {
        type_duty = 1;
    } else if (type_duty_0 == "Распоряжение") {
        type_duty = 0;
    } else {
        QMessageBox::warning(this, "Ошибка035", "Неизвестный тип работ: " + type_duty_0);
        return;
    }

    // 2. Начинаем транзакцию
    QSqlDatabase::database().transaction();

    QSqlQuery query;

    try {
        // 3. Подготавливаем и выполняем запрос UPDATE
        query.prepare(R"(
        UPDATE duty
        SET type_duty = ?, blok = ?, room = ?, equipment = ?,
            name_work = ?, type_work = ?, time = ?,
            dose_a_1 = ?, dose_a_2 = ?, rate_a_1 = ?, rate_a_2 = ?,
            dose_b_1 = ?, dose_b_2 = ?, rate_b_1 = ?, rate_b_2 = ?,
            dose_g_1 = ?, dose_g_2 = ?, rate_g_1 = ?, rate_g_2 = ?,
            dose_n_1 = ?, dose_n_2 = ?, rate_n_1 = ?, rate_n_2 = ?,
            duty_note = ?, last_update = ?
        WHERE duty_id = ?
    )");

        // Привязываем параметры
        query.addBindValue(type_duty);
        query.addBindValue(blok);
        query.addBindValue(room);
        query.addBindValue(equipment);
        query.addBindValue(name_work);
        query.addBindValue(type_work);
        query.addBindValue(time);
        query.addBindValue(ui->doubleSpinBox_dose_a_1->value());
        query.addBindValue(ui->doubleSpinBox_dose_a_2->value());
        query.addBindValue(ui->doubleSpinBox_rate_a_1->value());
        query.addBindValue(ui->doubleSpinBox_rate_a_2->value());
        query.addBindValue(ui->doubleSpinBox_dose_b_1->value());
        query.addBindValue(ui->doubleSpinBox_dose_b_2->value());
        query.addBindValue(ui->doubleSpinBox_rate_b_1->value());
        query.addBindValue(ui->doubleSpinBox_rate_b_2->value());
        query.addBindValue(ui->doubleSpinBox_dose_g_1->value());
        query.addBindValue(ui->doubleSpinBox_dose_g_2->value());
        query.addBindValue(ui->doubleSpinBox_rate_g_1->value());
        query.addBindValue(ui->doubleSpinBox_rate_g_2->value());
        query.addBindValue(ui->doubleSpinBox_dose_n_1->value());
        query.addBindValue(ui->doubleSpinBox_dose_n_2->value());
        query.addBindValue(ui->doubleSpinBox_rate_n_1->value());
        query.addBindValue(ui->doubleSpinBox_rate_n_2->value());
        query.addBindValue(duty_note);
        query.addBindValue(last_update);
        query.addBindValue(duty_id);




        // Указываем часовой пояс для Москвы (UTC+3)
        QTimeZone moscowTimeZone("Europe/Moscow");
        // Получаем текущее время и дату в UTC
        QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
        // Конвертируем время в московское
        QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
        QString last_update = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");
        //qDebug() << "Current date and time in Moscow:" << dateTimeString;

        if (!query.exec()) {
            throw std::runtime_error(query.lastError().text().toStdString());
        }

        if (query.numRowsAffected() <= 0) {
            throw std::runtime_error("Ни одна запись не была обновлена. Проверьте duty_id.");
        }

        // 5. Фиксируем транзакцию
        QSqlDatabase::database().commit();
        //QMessageBox::information(this, "Успех", "Данные успешно обновлены");

    } catch (const std::exception& e) {
        QSqlDatabase::database().rollback();
        QMessageBox::critical(this, "Ошибка036",
                              QString("Ошибка при обновлении данных:\n%1").arg(e.what()));
    }


    refresh_duty_table();


    float dose_a_1 = ui->doubleSpinBox_dose_a_1->value();
    float dose_a_2 = ui->doubleSpinBox_dose_a_2->value();

    float rate_a_1 = ui->doubleSpinBox_rate_a_1->value();
    float rate_a_2 = ui->doubleSpinBox_rate_a_2->value();

    float dose_b_1 = ui->doubleSpinBox_dose_b_1->value();
    float dose_b_2 = ui->doubleSpinBox_dose_b_2->value();

    float rate_b_1 = ui->doubleSpinBox_rate_b_1->value();
    float rate_b_2 = ui->doubleSpinBox_rate_b_2->value();

    float dose_g_1 = ui->doubleSpinBox_dose_g_1->value();
    float dose_g_2 = ui->doubleSpinBox_dose_g_2->value();

    float rate_g_1 = ui->doubleSpinBox_rate_g_1->value();
    float rate_g_2 = ui->doubleSpinBox_rate_g_2->value();

    float dose_n_1 = ui->doubleSpinBox_dose_n_1->value();
    float dose_n_2 = ui->doubleSpinBox_dose_n_2->value();

    float rate_n_1 = ui->doubleSpinBox_rate_n_1->value();
    float rate_n_2 = ui->doubleSpinBox_rate_n_2->value();

    QVector<double> doses = {
        dose_a_1, dose_a_2, rate_a_1, rate_a_2,   // Альфа-параметры
        dose_b_1, dose_b_2, rate_b_1, rate_b_2,   // Бета-параметры
        dose_g_1, dose_g_2, rate_g_1, rate_g_2,   // Гамма-параметры
        dose_n_1, dose_n_2, rate_n_1, rate_n_2    // Нейтронные параметры
    };
    QString for_user_id = ui->label_userId->text();
    QString changing_user_id;
    QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString name_0 = parts[0];
    QString name_1 = parts[1];
    QString name_2 = parts[2];
    query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
    query.bindValue(":name_0", name_0);
    query.bindValue(":name_1", name_1);
    query.bindValue(":name_2", name_2);
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка037", "Не удалось выполнить запрос: " + query.lastError().text());
        qDebug() << "Ошибка поиска id SQL:" << query.lastQuery() << query.lastError().text();
        return;
    }
    if (query.next()) {
        changing_user_id = query.value("user_id").toString();
    } else {
        QMessageBox::information(this, "Информация", "Пользователь не найден");
    }
    QString type_edit = "change";

    QDateTime SetLastUpdate_1 = QDateTime::fromString(last_update, "yyyy-MM-dd HH:mm:ss");
    bool success = addDutyHistory(
        duty_id,
        type_duty,
        blok,
        room,
        equipment,
        name_work,
        type_work,
        time,
        doses,
        duty_note,
        SetLastUpdate_1,
        changing_user_id,
        type_edit
    );

    if (success) {
        QMessageBox::information(this, "Успех", "Наряд успешно изменён");
    } else {
        QMessageBox::warning(this, "Ошибка038", "Не удалось изменён наряд");
    }

    on_btn_createNewDuty_pressed();
    setupSearchDutyComboBox(ui->select_dutyFilter);
    setupSearchDutyComboBox(ui->duty_id);
}

void MainWindow::refresh_duty_table(){
    QSqlQuery query;

    query.prepare(R"(
        SELECT duty_id, type_duty, blok, room, equipment, name_work, type_work, time,
               dose_a_1, dose_a_2, rate_a_1, rate_a_2, dose_b_1, dose_b_2, rate_b_1, rate_b_2,
               dose_g_1, dose_g_2, rate_g_1, rate_g_2, dose_n_1, dose_n_2, rate_n_1, rate_n_2,
               duty_note, last_update
        FROM duty
        ORDER BY duty_id ASC
    )");

    if (!query.exec()) {
        QMessageBox::warning(this, "Ошибка22 duty", "Не удалось выполнить Запрос: " + query.lastError().text());
        qDebug() << query.lastError().text();
        return;
    }

    // Полная очистка таблицы
    ui->tableDuty->clear();
    ui->tableDuty->setRowCount(0);

    // Устанавливаем заголовки
    QStringList headers = {
        "ID", "Вид работ", "Здание", "Помещение", "Оборудование",
        "Название работ", "Выполняемая работа", "Время",
        "α доза 1", "α доза 2", "α мощность 1", "α мощность 2",
        "β доза 1", "β доза 2", "β мощность 1", "β мощность 2",
        "γ доза 1", "γ доза 2", "γ мощность 1", "γ мощность 2",
        "Н доза 1", "Н доза 2", "Н мощность 1", "Н мощность 2",
        "Комментарий", "Последнее обновление"
    };

    // Устанавливаем количество столбцов и заголовки
    ui->tableDuty->setColumnCount(headers.size());
    ui->tableDuty->setHorizontalHeaderLabels(headers);

    // Настройки отображения заголовков
    ui->tableDuty->horizontalHeader()->setVisible(true);
    ui->tableDuty->horizontalHeader()->setHighlightSections(false);
    ui->tableDuty->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableDuty->horizontalHeader()->setStretchLastSection(true);

    // Заполняем таблицу данными
    int row = 0;
    while (query.next()) {
        ui->tableDuty->insertRow(row);
        for (int col = 0; col < headers.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui->tableDuty->setItem(row, col, item);
        }
        row++;
    }

    // Автоподбор ширины столбцов
    ui->tableDuty->resizeColumnsToContents();
}


void MainWindow::on_btn_createNewDuty_pressed()
{

    ui->lineEdit_blok->clear();
    ui->lineEdit_room->clear();
    ui->lineEdit_equipment->clear();
    ui->lineEdit_name_work->clear();
    ui->lineEdit_type_work->clear();
    ui->lineEdit_duty_note->clear();

    ui->spinBox_time->setValue(43200);
    ui->spinUserDoseYear->setValue(5);

    ui->doubleSpinBox_dose_a_1->setValue(0.1);
    ui->doubleSpinBox_dose_a_2->setValue(1);

    ui->doubleSpinBox_rate_a_1->setValue(0.1);
    ui->doubleSpinBox_rate_a_2->setValue(1);

    ui->doubleSpinBox_dose_b_1->setValue(0.1);
    ui->doubleSpinBox_dose_b_2->setValue(1);

    ui->doubleSpinBox_rate_b_1->setValue(0.1);
    ui->doubleSpinBox_rate_b_2->setValue(1);

    ui->doubleSpinBox_dose_g_1->setValue(0.1);
    ui->doubleSpinBox_dose_g_2->setValue(1);

    ui->doubleSpinBox_rate_g_1->setValue(0.1);
    ui->doubleSpinBox_rate_g_2->setValue(1);

    ui->doubleSpinBox_dose_n_1->setValue(0.1);
    ui->doubleSpinBox_dose_n_2->setValue(1);

    ui->doubleSpinBox_rate_n_1->setValue(0.1);
    ui->doubleSpinBox_rate_n_2->setValue(1);

    ui->label_duty_id->setText("-");
    ui->label_last_update->setText("-");
    ui->comboBox_type_duty->setCurrentIndex(0);

    QLayoutItem *item;
    while ((item = ui->layoutForBtnDuty->takeAt(0))) {
        if (item->widget()) {
            delete item->widget(); // Удаляем виджет
        }
        delete item; // Удаляем элемент layout
    }
    // Создаем новую кнопку
    QPushButton *btn_saveNewDuty = new QPushButton("Создать новый наряд", this);
    ui->layoutForBtnDuty->addWidget(btn_saveNewDuty);
    //qDebug() << "Родительский класс кнопки:" << btn_saveNewDuty->parent(); //QWidget(0x1f4ae248d90, name = "tab")
    if (btn_saveNewDuty){
        connect(btn_saveNewDuty, &QPushButton::pressed, this, &MainWindow::on_btn_saveNewDuty_pressed);//не забыть по h
    }


}


void MainWindow::on_pushButton_2_pressed()
{
    QMessageBox::warning(this, "Важная кнопка", "Кнопка желает хорошего дня");
}







MainWindow::~MainWindow()
{
    delete ui;
}



// Cохранить отчет
//
void MainWindow::on_btnKddFilterGo_clicked()
{


}
// завершить эксплуатацию
void MainWindow::on_btn_createReport_pressed(){}


void MainWindow::on_pushButton_3_clicked()
{
    qDebug() << "!243";
}


void MainWindow::setupSearch()
{
    // Создаем кнопку очистки
    clearSearchButton = new QPushButton("×", this);
    clearSearchButton->setFixedSize(20, 20);
    clearSearchButton->setStyleSheet(
        "QPushButton { background: #ccc; border: none; border-radius: 10px; font-weight: bold; color: #666; }"
        "QPushButton:hover { background: #bbb; }"
    );
    clearSearchButton->setCursor(Qt::PointingHandCursor);
    clearSearchButton->setVisible(false);

    // Размещаем кнопку в поле поиска
    QHBoxLayout *layout = new QHBoxLayout(ui->lineSearchUser);
    layout->setContentsMargins(3, 0, 3, 0);
    layout->setAlignment(Qt::AlignRight);
    layout->addWidget(clearSearchButton);

    // Подключаем сигналы
    connect(ui->lineSearchUser, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(clearSearchButton, &QPushButton::clicked, this, [this]() {
        ui->lineSearchUser->clear();
        showAllUsers();
    });
    connect(ui->btnSearchUser, &QPushButton::pressed, this, &MainWindow::on_btnSearchUser_pressed);
}

// Основная функция поиска
void MainWindow::on_btnSearchUser_pressed()
{
    QString searchText = ui->lineSearchUser->text().trimmed();

    // Показываем/скрываем кнопку очистки
    clearSearchButton->setVisible(!searchText.isEmpty());

    if (searchText.isEmpty()) {
        showAllUsers();
        return;
    }

    // Поиск по всем пользователям
    bool found = false;
    for (int i = 0; i < ui->verticalUsersList->count(); ++i) {
        QLayoutItem *item = ui->verticalUsersList->itemAt(i);
        if (item && item->widget()) {
            QWidget *userWidget = item->widget();

            // Ищем все кнопки в виджете пользователя
            bool match = false;
            QList<QPushButton*> buttons = userWidget->findChildren<QPushButton*>();
            for (QPushButton *btn : buttons) {
                if (btn->text().contains(searchText, Qt::CaseInsensitive)) {
                    match = true;
                    break;
                }
            }

            userWidget->setVisible(match);
            if (match) found = true;
        }
    }

    // Показываем сообщение если ничего не найдено
    if (!found) {
        QMessageBox::information(this, "Поиск", "Ничего не найдено");
    }
}

// Вспомогательная функция показа всех пользователей
void MainWindow::showAllUsers()
{
    for (int i = 0; i < ui->verticalUsersList->count(); ++i) {
        QLayoutItem *item = ui->verticalUsersList->itemAt(i);
        if (item && item->widget()) {
            item->widget()->setVisible(true);
        }
    }
}

// Обработчик изменения текста (для скрытия кнопки при очистке)
void MainWindow::onSearchTextChanged(const QString &text)
{
    clearSearchButton->setVisible(!text.isEmpty());
    if (text.isEmpty()) showAllUsers();
}


//
void MainWindow::setupUserDutySearch()
{
    // Создаем кнопку очистки
    clearUserDutySearchButton = new QPushButton("×", this);
    clearUserDutySearchButton->setFixedSize(20, 20);
    clearUserDutySearchButton->setStyleSheet(
        "QPushButton { background: #ccc; border: none; border-radius: 10px; font-weight: bold; color: #666; }"
        "QPushButton:hover { background: #bbb; }"
    );
    clearUserDutySearchButton->setCursor(Qt::PointingHandCursor);
    clearUserDutySearchButton->setVisible(false);

    // Размещаем кнопку в поле поиска
    QHBoxLayout *layout = new QHBoxLayout(ui->line_userDutySearch);
    layout->setContentsMargins(3, 0, 3, 0);
    layout->setAlignment(Qt::AlignRight);
    layout->addWidget(clearUserDutySearchButton);

    // Подключаем сигналы
    connect(ui->line_userDutySearch, &QLineEdit::textChanged, this, &MainWindow::onUserDutySearchTextChanged);
    connect(clearUserDutySearchButton, &QPushButton::clicked, this, [this]() {
        ui->line_userDutySearch->clear();
        showAllUserDuties();
    });
    connect(ui->btn_userDutySearch, &QPushButton::pressed, this, &MainWindow::on_btnUserDutySearch_pressed);
}

// Основная функция поиска по нарядам
void MainWindow::on_btnUserDutySearch_pressed()
{
    QString searchText = ui->line_userDutySearch->text().trimmed();

    // Показываем/скрываем кнопку очистки
    clearUserDutySearchButton->setVisible(!searchText.isEmpty());

    if (searchText.isEmpty()) {
        showAllUserDuties();
        return;
    }

    // Поиск по третьему столбцу таблицы нарядов
    bool found = false;
    for (int row = 0; row < ui->table_userDuty->rowCount(); ++row) {
        // Получаем текст из третьего столбца (индекс 2)
        QTableWidgetItem *item = ui->table_userDuty->item(row, 2);
        bool match = false;

        if (item) {
            QString cellText = item->text();
            if (cellText.contains(searchText, Qt::CaseInsensitive)) {
                match = true;
                found = true;
            }
        }

        // Показываем/скрываем строку в зависимости от результата поиска
        ui->table_userDuty->setRowHidden(row, !match);
    }

    // Показываем сообщение если ничего не найдено
    if (!found) {
        QMessageBox::information(this, "Поиск", "Ничего не найдено");
    }
}

// Вспомогательная функция показа всех нарядов
void MainWindow::showAllUserDuties()
{
    for (int row = 0; row < ui->table_userDuty->rowCount(); ++row) {
        ui->table_userDuty->setRowHidden(row, false);
    }
}

// Обработчик изменения текста (для скрытия кнопки при очистке)
void MainWindow::onUserDutySearchTextChanged(const QString &text)
{
    clearUserDutySearchButton->setVisible(!text.isEmpty());
    if (text.isEmpty()) showAllUserDuties();
}

void MainWindow::on_btnSetCreateNew_pressed()
{

    // Очищаем все поля формы
    ui->textSetId->setText("-");
    ui->inputSetName->clear();
    ui->inputIntSetQuantity->setValue(1);
    //ui->inputIntSetQuantity->setReadOnly(false);  // Разрешаем прямое редактирование
    ui->lineSetNote->clear();
    ui->inputSetIp->clear();
    ui->comboSetStatus->setCurrentIndex(0); // Устанавливаем первый элемент
    ui->dateSetLastUpdate->setText("-"); // Устанавливаем прочерк
    ui->comboSetStatus->setCurrentIndex(0);

    // Опционально: устанавливаем фокус на первое поле
    ui->textSetId->setFocus();


    //меняем кнопку
    // Удаляем все кнопки из layout
    QLayoutItem *item;
    while ((item = ui->gridLayout_18->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // Создаем новую кнопку "Сохранить изменения"
    QPushButton *btnSetSave = new QPushButton("Сохранить новый комплект", this);
    ui->gridLayout_18->addWidget(btnSetSave);

    // Подключаем сигнал для новой кнопки
    connect(btnSetSave, &QPushButton::clicked, this, &MainWindow::on_btnSetSave_pressed);
    on_btnKasCreateNew_pressed();
    //очистить таблицу
    //QSqlQueryModel *model = new QSqlQueryModel(this);
    QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tableView_kas->model());
    if (model) {
        model->clear();
        // model->removeRows(0, model->rowCount());
    }
}


void MainWindow::on_btnKasCreateNew_pressed()
{
    // Очищаем все поля формы
    //ui->textSetId_2->setText("-");
    ui->textKasId->setText("-");
    ui->inputKasName->clear();
    ui->comboKasStatus->setCurrentIndex(0);

    ui->inputIntKasLenght->setValue(1);
    //ui->inputIntKasLenght->setReadOnly(false);  // Разрешаем прямое редактирование

    ui->inputIntKasWidth->setValue(1);
    //ui->inputIntKasWidth->setReadOnly(false);  // Разрешаем прямое редактирование

    ui->inputKasNote->clear();

    // Опционально: устанавливаем фокус на первое поле
    ui->textSetId->setFocus();
    ui->dateKasLastUpdate->setText("-");


    //меняем кнопку
    // Удаляем все кнопки из layout
    QLayoutItem *item;
    while ((item = ui->gridLayout_21->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // Создаем новую кнопку "Сохранить изменения"
    QPushButton *btnKasSave = new QPushButton("Сохранить новую кассетницу", this);
    ui->gridLayout_21->addWidget(btnKasSave);

    // Подключаем сигнал для новой кнопки
    connect(btnKasSave, &QPushButton::clicked, this, &MainWindow::on_btnKasSave_pressed);

}

/**/  //тут всё убрать
void MainWindow::on_btnMeshCreateNew_pressed()
{

    // Очищаем все поля формы
    ui->label_id_set->setText("-");
    ui->label_id_kas->setText("-");
    ui->label_id_mesh->setText("-");
    ui->line_MeshNote->clear();
    ui->lineEdit_MeshDoz->clear();
    //ui->textSetId_2->setText("-");

    ui->comboBox_MeshStatus->setCurrentIndex(0); // Устанавливаем первый элемент
    ui->comboBox_MeshUser->setCurrentIndex(0); // Устанавливаем первый элемент

    ui->label_MeshUpdate->setText("-"); // Устанавливаем прочерк

    // Опционально: устанавливаем фокус на первое поле
    ui->textSetId->setFocus();



    //меняем кнопку
    // Удаляем все кнопки из layout
    QLayoutItem *item;
    while ((item = ui->gridLayout_28->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // Создаем новую кнопку "Сохранить изменения"
    QPushButton *btnMeshSave = new QPushButton("Сохранить новую ячейку", this);
    ui->gridLayout_28->addWidget(btnMeshSave);

    // Подключаем сигнал для новой кнопки
    connect(btnMeshSave, &QPushButton::clicked, this, &MainWindow::on_btnMeshSave_pressed);
}




void MainWindow::on_btnMeshSave_pressed() //выглядит бесполезной тк для неё нет кнопки
{
    QSqlQuery query;

    int id_mesh = 1; // Значение по умолчанию, если таблица пуста

    if (query.exec("SELECT MAX(id_mesh) AS max_id FROM mesh;")) {
        if (query.next()) {
            // Если в таблице есть записи, берём максимальный ID + 1
            id_mesh = query.value("max_id").toInt() + 1;
            //qDebug() << "Максимальный id_mesh:" << id_mesh - 1;
            //qDebug() << "Новый id_mesh:" << id_mesh;
        } else {
            //qDebug() << "Таблица пуста, используем id_mesh = 1";
        }
    } else {
        qDebug() << "Ошибка выполнения запроса15:" << query.lastError().text();
    }

    QString id_set = ui->textSetId->text();//label_id_set
    QString id_kas = ui->textKasId->text();//label_id_kas
    //QString id_mesh = ui->label_id_mesh->text();
    QString MeshNote = ui->line_MeshNote->text();
    QString MeshDoz = ui->lineEdit_MeshDoz->text();

    //QDate SetLastUpdate = ui->dateSetLastUpdate->date();//поменять на настоящее время
    // Указываем часовой пояс для Москвы (UTC+3)
    QTimeZone moscowTimeZone("Europe/Moscow");
    // Получаем текущее время и дату в UTC
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    // Конвертируем время в московское
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString label_MeshUpdate = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");
    //qDebug() << "Current date and time in Moscow:" << dateTimeString;

    QString MeshStatus_str = ui->comboBox_MeshStatus->currentText();
    QStringList parts1 = MeshStatus_str.split(" - ");
    QString MeshStatus_st = parts1[0].trimmed();
    int MeshStatus = MeshStatus_st.toInt();

    QString MeshUser_str = ui->comboBox_MeshUser->currentText();
    QStringList parts2 = MeshUser_str.split(" - ");
    QString MeshUser_st = parts2[0].trimmed();
    int MeshUser = MeshUser_st.toInt();


    // Проверяем, что поля не пустые
    if (id_set.isEmpty() || id_kas.isEmpty()) {
        // для обязательных полей
        QMessageBox::warning(this, "Ошибка039", "Поля 'id_set', 'id_kas' не могут быть пустыми!");
        return;
    }

    //id порядковый номер
    query.prepare("INSERT INTO mesh(set_id, kas_id,  mesh_id, user_id, doz_tld_id, mesh_status, mesh_note, last_update) VALUES (:set_id, :kas_id, :mesh_id, :user_id, :doz_tld_id, :mesh_status, :mesh_note, :last_update)");

    //int set_id = 1;
    query.bindValue(":set_id", id_set);
    query.bindValue(":kas_id", id_kas);
    //query.bindValue("kas_id", id_kas);
    query.bindValue(":mesh_id", id_mesh);

    query.bindValue(":mesh_note", MeshNote);
    query.bindValue(":doz_tld_id", MeshDoz);
    query.bindValue(":last_update", label_MeshUpdate);
    query.bindValue(":mesh_status", MeshStatus);
    query.bindValue(":user_id", MeshUser);
    //QMessageBox::warning(this, "Ошибка", MeshStatus_st);

    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Создана новая ячейка!");
    } else {
        QMessageBox::critical(this, "Ошибка040", "Не удалось создать ячейку!",  query.lastError().text());
        qDebug() << query.lastError().text();
    }
}

void MainWindow::on_btnMeshChange_pressed() {
    // Сохранение изменений ячейки
    QSqlQuery query;

    // Получение данных из UI с минимальным использованием памяти
    const QString set_id = ui->textSetId->text();
    const QString kas_id = ui->label_id_kas->text().split(" - ")[0].trimmed();
    const QString mesh_id = ui->label_id_mesh->text();
    const QString MeshNote = ui->line_MeshNote->text();
    QString MeshDoz = ui->lineEdit_MeshDoz->text().trimmed();

    // Получение текущего времени для Москвы
    const QDateTime moscowDateTime = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("Europe/Moscow"));
    const QString label_MeshUpdate = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");

    // Получение статуса
    const QString MeshStatus_str = ui->comboBox_MeshStatus->currentText();
    const int MeshStatus = MeshStatus_str.split(" - ")[0].trimmed().toInt();

    // Получение пользователя
    const QString MeshUser_str = ui->comboBox_MeshUser->currentText();
    const QStringList userParts = MeshUser_str.split(" - ");
    int MeshUser = userParts.size() > 0 ? userParts[0].trimmed().toInt() : 0;
    const QString userName = userParts.size() > 1 ? userParts[1].trimmed() : "";

    // Проверка обязательных полей
    if (set_id.isEmpty() || kas_id.isEmpty()) {
        QMessageBox::warning(this, "Ошибка042", "Поля 'id_set', 'id_kas' не могут быть пустыми!");
        return;
    }

    // Обработка дозиметра
    if (!MeshDoz.isEmpty()) {
        // Проверяем, где находится дозиметр сейчас
        query.prepare("SELECT set_id, kas_id, mesh_id FROM mesh WHERE doz_tld_id = :doz_tld_id");
        query.bindValue(":doz_tld_id", MeshDoz);

        if (query.exec() && query.next()) {
            const QString oldSetId = query.value("set_id").toString();
            const QString oldKasId = query.value("kas_id").toString();
            const QString oldMeshId = query.value("mesh_id").toString();

            // Если дозиметр не в этой же ячейке - запрашиваем подтверждение
            if (!(oldSetId == set_id && oldKasId == kas_id && oldMeshId == mesh_id)) {
                const QString message = QString("Вы уверены что хотите переместить дозиметр из %1 комплекта %2 кассетницы %3 ячейки?")
                                         .arg(oldSetId, oldKasId, oldMeshId);

                if (QMessageBox::question(this, "Внимание", message, QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
                    MeshDoz.clear();
                } else {
                    // Освобождаем дозиметр из старой ячейки
                    query.prepare("UPDATE mesh SET doz_tld_id = NULL WHERE set_id = :set_id AND kas_id = :kas_id AND mesh_id = :mesh_id");
                    query.bindValue(":set_id", oldSetId);
                    query.bindValue(":kas_id", oldKasId);
                    query.bindValue(":mesh_id", oldMeshId);
                    query.exec();
                }
            }
        }
    }

    // Обработка пользователя
    if (MeshUser > 0) {
        QString oldSetId, oldKasId, oldMeshId, userDosimeter;

        // Получаем текущую привязку пользователя
        query.prepare("SELECT set_id, kas_id, mesh_id FROM mesh WHERE user_id = :user_id");
        query.bindValue(":user_id", MeshUser);

        if (query.exec() && query.next()) {
            oldSetId = query.value("set_id").toString();
            oldKasId = query.value("kas_id").toString();
            oldMeshId = query.value("mesh_id").toString();
        }

        // Получаем дозиметр пользователя
        query.prepare("SELECT doz_tld_id FROM users WHERE user_id = :user_id");
        query.bindValue(":user_id", MeshUser);

        if (query.exec() && query.next()) {
            userDosimeter = query.value("doz_tld_id").toString();
        }

        // Если пользователь уже привязан к другой ячейке - запрашиваем подтверждение
        if (!oldSetId.isEmpty() && (oldSetId != set_id || oldKasId != kas_id || oldMeshId != mesh_id)) {
            const QString message = QString("Вы уверены, что хотите изменить ячейку хранения для %1 из комплекта %2, кассетницы %3, ячейки %4?")
                                     .arg(userName.isEmpty() ? "пользователя" : userName, oldSetId, oldKasId, oldMeshId);

            if (QMessageBox::question(this, "Внимание", message, QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
                MeshUser = 0;
            } else {
                // Освобождаем старую ячейку
                query.prepare("UPDATE mesh SET user_id = NULL WHERE set_id = :set_id AND kas_id = :kas_id AND mesh_id = :mesh_id");
                query.bindValue(":set_id", oldSetId);
                query.bindValue(":kas_id", oldKasId);
                query.bindValue(":mesh_id", oldMeshId);
                query.exec();
            }
        }

        if (MeshUser > 0) {
            // Обработка дозиметра пользователя
            if (!userDosimeter.isEmpty() && MeshDoz != userDosimeter && !MeshDoz.isEmpty()) {
                const QString message = QString("Вы уверены, что хотите изменить дозиметр для пользователя с %1 на %2?")
                                         .arg(userDosimeter, MeshDoz);

                if (QMessageBox::question(this, "Внимание", message, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                    // Обновляем дозиметр пользователя
                    query.prepare("UPDATE users SET doz_tld_id = :doz_tld_id WHERE user_id = :user_id");
                    query.bindValue(":user_id", MeshUser);
                    query.bindValue(":doz_tld_id", MeshDoz);
                    query.exec();
                }
            } else if (MeshDoz.isEmpty() && !userDosimeter.isEmpty()) {
                // Если дозиметр не указан, но у пользователя есть свой - используем его
                MeshDoz = userDosimeter;
            }

            // Привязываем пользователя к новой ячейке
            query.prepare("UPDATE users SET set_id = :set_id, kas_id = :kas_id, mesh_id = :mesh_id WHERE user_id = :user_id");
            query.bindValue(":user_id", MeshUser);
            query.bindValue(":set_id", set_id);
            query.bindValue(":kas_id", kas_id);
            query.bindValue(":mesh_id", mesh_id);
            query.exec();
        }
    }

    // Обновление данных ячейки в базе
    query.prepare(R"(
        UPDATE mesh SET
            user_id = :user_id,
            doz_tld_id = :doz_tld_id,
            mesh_status = :mesh_status,
            mesh_note = :mesh_note,
            last_update = :last_update
        WHERE set_id = :set_id AND kas_id = :kas_id AND mesh_id = :mesh_id
    )");

    query.bindValue(":set_id", set_id);
    query.bindValue(":kas_id", kas_id);
    query.bindValue(":mesh_id", mesh_id);
    query.bindValue(":user_id", MeshUser > 0 ? MeshUser : QVariant());
    query.bindValue(":doz_tld_id", !MeshDoz.isEmpty() ? MeshDoz : QVariant());
    query.bindValue(":mesh_status", MeshStatus);
    query.bindValue(":mesh_note", MeshNote);
    query.bindValue(":last_update", label_MeshUpdate);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка043", "Не удалось изменить ячейку!\n" + query.lastError().text());
        return;
    }

    // Обновление интерфейса
    loadFilteredMeshData(kas_id);
    updateUsersDatabase();

    // Сохранение в историю
    const QString for_user_id = ui->label_userId->text();
    const QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString changing_user_id;

    if (parts.size() >= 3) {
        query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
        query.bindValue(":name_0", parts[0]);
        query.bindValue(":name_1", parts[1]);
        query.bindValue(":name_2", parts[2]);

        if (query.exec() && query.next()) {
            changing_user_id = query.value("user_id").toString();
        }
    }

    if (!changing_user_id.isEmpty()) {
        addMeshHistory(
            set_id.toInt(),
            kas_id.toInt(),
            mesh_id.toInt(),
            MeshUser,
            MeshDoz,
            MeshStatus,
            MeshNote,
            changing_user_id,
            "change"
        );
        meshHistoryToTable();
    }

    QMessageBox::information(this, "Успех", "Ячейка изменена!");
    updateListSetForHistory();
    comboboxUpdateAll();
}

bool MainWindow::addMeshHistory(
    int setId,
    int kasId,
    int meshId,
    int userId,
    const QString &dozTldId,
    int meshStatus,
    const QString &meshNote,
    const QString &changing_user_id,
    const QString &type_edit)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO mesh_history (
            set_id,
            kas_id,
            mesh_id,
            user_id,
            doz_tld_id,
            mesh_status,
            mesh_note,
            last_update,
            user_id_change,
            last_update_change,
            type_edit
        ) VALUES (
            :set_id,
            :kas_id,
            :mesh_id,
            :user_id,
            :doz_tld_id,
            :mesh_status,
            :mesh_note,
            :last_update,
            :user_id_change,
            :last_update_change,
            :type_edit
        )
    )");

    // Привязка значений
    query.bindValue(":set_id", setId);
    query.bindValue(":kas_id", kasId);
    query.bindValue(":mesh_id", meshId);
    query.bindValue(":user_id", userId);
    query.bindValue(":doz_tld_id", dozTldId);
    query.bindValue(":mesh_status", meshStatus);
    query.bindValue(":mesh_note", meshNote);
    query.bindValue(":last_update", QDateTime::currentDateTime());
    query.bindValue(":user_id_change", changing_user_id);
    query.bindValue(":last_update_change", QDateTime::currentDateTime());
    query.bindValue(":type_edit", type_edit);

    if (!query.exec()) {
        qDebug() << "Ошибка записи в mesh_history:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка045", query.lastError().text());
        return false;
    }
    return true;
}

void MainWindow::meshHistoryToTable()
{
    // Очищаем таблицу перед загрузкой новых данных
    ui->tableWidget_historyMesh->clear();
    ui->tableWidget_historyMesh->setRowCount(0);

    // Устанавливаем количество столбцов
    ui->tableWidget_historyMesh->setColumnCount(11); // По количеству полей в mesh_history

    // Устанавливаем русские заголовки
    QStringList headers;
    headers << "ID комплекта" << "ID кассетницы" << "ID ячейки" << "ID пользователя"
            << "ID дозиметра" << "Статус" << "Примечание" << "Дата обновления"
            << "ID изменившего" << "Дата изменения" << "Тип изменения";

    ui->tableWidget_historyMesh->setHorizontalHeaderLabels(headers);

    // Выполняем SQL-запрос
    QSqlQuery query;
    query.prepare("SELECT * FROM mesh_history ORDER BY last_update_change DESC");

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса16:" << query.lastError().text();
        return;
    }

    // Заполняем таблицу данными
    while (query.next()) {
        int row = ui->tableWidget_historyMesh->rowCount();
        ui->tableWidget_historyMesh->insertRow(row);

        // Заполняем ячейки данными
        for (int col = 0; col < query.record().count(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();

            // Особые обработки для разных типов данных
            if (query.record().fieldName(col) == "last_update" ||
                query.record().fieldName(col) == "last_update_change") {
                // Форматируем дату в читаемый вид
                item->setText(query.value(col).toDateTime().toString("dd.MM.yyyy hh:mm"));
            }
            else {
                // Стандартное текстовое представление
                item->setText(query.value(col).toString());
            }

            ui->tableWidget_historyMesh->setItem(row, col, item);
        }
    }

    // Настраиваем внешний вид таблицы
    ui->tableWidget_historyMesh->resizeColumnsToContents();
    ui->tableWidget_historyMesh->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_historyMesh->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_historyMesh->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_historyMesh->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
/*--------------------------------*/

void MainWindow::comboboxUpdateAll(){ //не только для нарядов, обновляет и для систем хранения
    //выпадающий список для пользователей
    setupSearchUserComboBox(ui->comboBox_user_id);

    //настройка списка пользователя в ячейке
    setupSearchUserComboBox(ui->comboBox_MeshUser);

    setupSearchDutyComboBox(ui->duty_id);

    //пользователи
    setupSearchUserComboBox(ui->comboBox_MeshUser_kid);
    setupSearchUserComboBox(ui->select_SetFilterFIO_kid);
    setupSearchUserComboBox(ui->select_KasFilterFIO_kid);
    setupSearchUserComboBox(ui->select_MeshFilterFIO_kid);

    //увд - комплекты в истории
    updateSetComboBox(ui->select_kdd_set_1);
    updateSetComboBox(ui->select_kdd_set_2, ui->select_kss_kas_2);
    updateSetComboBox(ui->select_kdd_set_3, ui->select_kdd_kas_3, ui->select_kdd_mesh_3);

    //увд для пользователей
    updateSetComboBox(ui->selectSet, ui->selectKas, ui->selectMesh);

    //кид
    updateSetKidComboBox(ui->select_kdd_set_1_kid);
    updateSetKidComboBox(ui->select_kdd_set_2_kid, ui->select_kss_kas_2_kid);
    updateSetKidComboBox(ui->select_kdd_set_5_kid, ui->select_kdd_kas_3_kid, ui->select_kdd_mesh_3_kid);
}

void MainWindow::setupSearchDutyComboBox(QComboBox* comboBox)
{
    if (!comboBox) return;

    // Очистка предыдущих соединений
    comboBox->disconnect();
    if (comboBox->lineEdit()) {
        comboBox->lineEdit()->disconnect();
    }

    // Базовая настройка ComboBox
    comboBox->clear();
    comboBox->setEditable(true);
    comboBox->setInsertPolicy(QComboBox::NoInsert);
    comboBox->setDuplicatesEnabled(false);

    // Статические элементы
    comboBox->addItem("Не выбран", QVariant());
    comboBox->addItem("", QVariant());
    comboBox->setCurrentIndex(1);

    // Загрузка данных из БД
    QSqlQuery query;
    if (!query.exec("SELECT duty_id, name_work FROM duty")) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить список нарядов: " + query.lastError().text());
        return;
    }

    // Подсчет количества записей и заполнение данных
    int rowCount = 0;
    QList<DutyDataSearch> dutyData;

    while (query.next()) {
        QString duty_id = query.value("duty_id").toString();
        QString name_work = query.value("name_work").toString();

        DutyDataSearch duty;
        duty.id = duty_id;
        duty.displayText = QString("%1 - %2").arg(duty_id).arg(name_work);
        duty.searchText = QString("%1 %2").arg(duty_id).arg(name_work).toLower();

        dutyData.append(duty);
        comboBox->addItem(duty.displayText, duty.id);

        rowCount++;
    }

    //qDebug() << "Количество нарядов:" << rowCount;

    // Настройка поля поиска
    QLineEdit* searchEdit = comboBox->lineEdit();
    searchEdit->setPlaceholderText("Введите для поиска (ID, название работы...)");
    searchEdit->setClearButtonEnabled(true);

    // Оптимизированная фильтрация
    QTimer* searchTimer = new QTimer(comboBox);
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(300);

    // Сохраняем dutyData в свойство comboBox для доступа
    comboBox->setProperty("dutyData", QVariant::fromValue(dutyData));

    auto filterDuties = [comboBox]() {
        QList<DutyDataSearch> dutyData = comboBox->property("dutyData").value<QList<DutyDataSearch>>();
        QString text = comboBox->lineEdit()->text().toLower();

        QSignalBlocker blocker(comboBox);
        QString currentText = comboBox->lineEdit()->text();
        int currentPos = comboBox->lineEdit()->cursorPosition();

        // Очищаем список (кроме первых 2 элементов)
        while (comboBox->count() > 2) {
            comboBox->removeItem(comboBox->count() - 1);
        }

        // Добавляем отфильтрованные элементы
        if (!text.isEmpty()) {
            for (const auto& duty : dutyData) {
                if (duty.searchText.contains(text)) {
                    comboBox->addItem(duty.displayText, duty.id);
                }
            }
        }

        // Восстанавливаем состояние
        comboBox->lineEdit()->setText(currentText);
        comboBox->lineEdit()->setCursorPosition(currentPos);

        if (!text.isEmpty() && comboBox->count() > 2) {
            comboBox->showPopup();
        }
    };

    connect(searchEdit, &QLineEdit::textChanged, searchTimer, [searchTimer]() {
        searchTimer->start();
    });

    connect(searchTimer, &QTimer::timeout, comboBox, filterDuties);

    // Обработка выбора элемента
    connect(comboBox, QOverload<int>::of(&QComboBox::activated), comboBox, [comboBox](int index) {
        if (index == 0) {
            comboBox->setCurrentIndex(1);
            comboBox->lineEdit()->clear();
        }
        else if (index >= 2) {
            QString selectedText = comboBox->itemText(index);
            comboBox->lineEdit()->setText(selectedText);
            comboBox->hidePopup();

            QString selectedId = comboBox->itemData(index).toString();
            qDebug() << "Выбран ID наряда:" << selectedId;
        }
    });

    // Показ полного списка при фокусе
    connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [comboBox]() {
        QTimer::singleShot(100, [comboBox]() {
            if (comboBox->hasFocus()) {
                comboBox->showPopup();
            }
        });
    });

    // Принудительное обновление через короткую задержку
    QTimer::singleShot(500, [comboBox, dutyData]() {
        QSignalBlocker blocker(comboBox);

        // Очищаем и перезаполняем
        while (comboBox->count() > 2) {
            comboBox->removeItem(comboBox->count() - 1);
        }

        for (const auto& duty : dutyData) {
            comboBox->addItem(duty.displayText, duty.id);
        }

        //qDebug() << "Принудительное обновление comboBox нарядов, элементов:" << comboBox->count();
    });
}

// Функция для загрузки и отображения данных в tableView_set
void MainWindow::loadSetData(){
    ui->inputIntSetQuantity->setValue(1);
    QSqlQueryModel *model = new QSqlQueryModel(this);

    // Выполняем запрос
    QSqlQuery query;
    query.prepare("SELECT set_id, set_name, ip_set, set_quantity, set_block, set_note, last_update FROM set ORDER BY set_id ASC"); //

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка049", "Не удалось загрузить данные комплектов: " + query.lastError().text());
        delete model;
        return;
    }

    // Устанавливаем модель в tableView
    model->setQuery(query);
    ui->tableView_set->setModel(model);

    // Устанавливаем заголовки столбцов
    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Наименование"));
    model->setHeaderData(2, Qt::Horizontal, tr("IP адрес"));
    model->setHeaderData(3, Qt::Horizontal, tr("Количество"));
    model->setHeaderData(4, Qt::Horizontal, tr("Статус"));
    model->setHeaderData(5, Qt::Horizontal, tr("Примечание"));
    model->setHeaderData(6, Qt::Horizontal, tr("Последнее обновление"));

    // Настраиваем внешний вид таблицы
    ui->tableView_set->resizeColumnsToContents();
    ui->tableView_set->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_set->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_set->verticalHeader()->setVisible(false); // Скрываем вертикальные заголовки

    // Подключаем сигнал выбора строки
    connect(ui->tableView_set->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onSetSelectionChanged);

    // Добавляем обработчик двойного клика по таблице
    connect(ui->tableView_set, &QTableView::doubleClicked, this, [this](const QModelIndex &index) {
        Q_UNUSED(index); // Индекс не используется, но требуется для сигнала

        // Переключаем на следующую вкладку
        ui->tabWidget_kas->setCurrentIndex(1);
    });
}


// Обработчик выбора строки в таблице комплектов
void MainWindow::onSetSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{

    Q_UNUSED(deselected);

    if (selected.indexes().isEmpty()) {
        return;
    }

    // Получаем индекс выбранной строки
    QModelIndex index = selected.indexes().first();
    int row = index.row();

    // Получаем модель и извлекаем данные
    QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->tableView_set->model());
    if (!model) return;


    // Извлекаем данные из модели
    QString set_id = model->data(model->index(row, 0)).toString();
    QString set_name = model->data(model->index(row, 1)).toString();
    QString ip_set = model->data(model->index(row, 2)).toString();
    int set_quantity = model->data(model->index(row, 3)).toInt();
    QString set_block = model->data(model->index(row, 4)).toString();
    QString set_note = model->data(model->index(row, 5)).toString();
    QString last_update = model->data(model->index(row, 6)).toString();

    //ui->label_listOfKas->setText("Комплект: "+set_id);
    // Заполняем поля формы
    ui->textSetId->setText(set_id);
    ui->inputSetName->setText(set_name);
    ui->lineSetNote->setText(set_note);
    ui->inputSetIp->setText(ip_set);

    ui->inputIntSetQuantity->setValue(set_quantity); //нельзя редактировать
    //ui->inputIntSetQuantity->setReadOnly(true);

    // Устанавливаем статус
    if (set_block == "0") {
        ui->comboSetStatus->setCurrentText("Разблокирован");
    } else if (set_block == "1") {
        ui->comboSetStatus->setCurrentText("Заблокирован");
    } else {
        //qWarning() << "Неизвестный статус блокировки:" << set_block;
        ui->comboSetStatus->setCurrentIndex(0);
    }

    ui->dateSetLastUpdate->setText(last_update);


    QLayoutItem *item;
    while ((item = ui->gridLayout_18->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    ui->tabWidget_kas->setTabText(0, "Комплект: "+set_name);

    QPushButton *btnSetChange = new QPushButton("Сохранить изменения комплекта", this);
    ui->gridLayout_18->addWidget(btnSetChange);
    connect(btnSetChange, &QPushButton::clicked, this, &MainWindow::on_btnSetChange_pressed);

    loadListOfKas();
    loadFilteredKasData();

    checkRole();
}


void MainWindow::loadListOfKas(){
    ui->comboBox_listOfKas->clear();

    QString set_id = ui->textSetId->text().trimmed();
    ui->label_aboutSet->setText(set_id);

    int kas_id;
    QString kas_name;

    QSqlQuery queryKas;
    queryKas.prepare("SELECT kas_id, kas_name FROM kas WHERE set_id = :set_id ORDER BY set_id ASC, kas_id ASC");
    queryKas.bindValue(":set_id", set_id);

    if (queryKas.exec()) {
        while (queryKas.next()) {
            kas_id = queryKas.value("kas_id").toInt();
            kas_name = queryKas.value("kas_name").toString();

            // Добавляем элемент в формате "ID - Name"
            ui->comboBox_listOfKas->addItem(
                QString("%1 - %2").arg(kas_id).arg(kas_name),
                kas_id  // Сохраняем ID как пользовательские данные
            );
        }

        // Устанавливаем первый элемент по умолчанию (если есть)
        if (ui->comboBox_listOfKas->count() > 0) {
            ui->comboBox_listOfKas->setCurrentIndex(0);
        }
    } else {
        qDebug() << "Ошибка выполнения запроса17:" << queryKas.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить список кассетниц");
    }

    // Добавляем обработчик двойного клика по таблице
    if (ui->tableView_kas) {
        connect(ui->tableView_kas, &QTableView::doubleClicked, this, [this](const QModelIndex &index) {
            // Переключаем на вкладку с индексом 2 (предполагая, что это нужная вкладка)
            ui->tabWidget_kas->setCurrentIndex(2);

            // Получаем модель таблицы
            QAbstractItemModel *model = ui->tableView_kas->model();

            // Получаем строку, по которой кликнули
            int row = index.row();

            // Получаем kas_id из первой колонки (0)
            QModelIndex idIndex = model->index(row, 0); // Замените 0 на индекс колонки с kas_id
            QString kas_id = model->data(idIndex).toString();

            // Получаем kas_name из второй колонки (1)
            QModelIndex nameIndex = model->index(row, 1); // Замените 1 на индекс колонки с kas_name
            QString kas_name = model->data(nameIndex).toString();

            // Формируем текст для поиска в комбобоксе
            QString comboText = kas_id + " - " + kas_name;

            // Ищем текст в комбобоксе
            int comboIndex = ui->comboBox_listOfKas->findText(comboText);

            if (comboIndex >= 0) {
                ui->comboBox_listOfKas->setCurrentIndex(comboIndex);
                qDebug() << "Установлен элемент:" << comboText << "с индексом:" << comboIndex;
            } else {
                qWarning() << "Элемент не найден:" << comboText;
                ui->comboBox_listOfKas->setCurrentIndex(0);
            }
        });
    }
}

void MainWindow::loadFilteredKasData()
{
    // Получаем set_id из текстового поля
    QString set_id = ui->textSetId->text().trimmed();

    if (set_id.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Введите ID комплекта");
        return;
    }

    // Создаем модель для таблицы
    QSqlQueryModel *model = new QSqlQueryModel(this);

    // Подготавливаем запрос с фильтрацией по set_id
    QSqlQuery queryKas;
    queryKas.prepare("SELECT kas_id, kas_name, kas_height, kas_width, kas_block, kas_note, last_update FROM kas WHERE set_id = :set_id ORDER BY set_id ASC, kas_id ASC");
    queryKas.bindValue(":set_id", set_id);

    if (!queryKas.exec()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить данные кассетниц: " + queryKas.lastError().text());
        delete model;
        return;
    }

    // Устанавливаем модель в tableView
    model->setQuery(queryKas);
    ui->tableView_kas->setModel(model);

    // Устанавливаем заголовки столбцов
    QStringList headers = {"ID", "Наименование", "Кол-во модулей хранения", "Кол-во ячеек в модуле хранения", "Статус", "Примечание", "Последнее обновление"};

    for (int i = 0; i < headers.size(); ++i) {
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
    }

    // Настраиваем внешний вид таблицы
    ui->tableView_kas->resizeColumnsToContents();
    ui->tableView_kas->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_kas->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableView_kas->verticalHeader()->setVisible(false); //должен скрыть горизонтальные заголовки

    // Подключаем сигнал выбора строки
    connect(ui->tableView_kas->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onKasSelectionChanged);
}


// Обработчик выбора строки в таблице кассетниц
void MainWindow::onKasSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{

    Q_UNUSED(deselected);

    if (selected.indexes().isEmpty()) {
        return;
    }

    // Получаем индекс выбранной строки
    QModelIndex index = selected.indexes().first();
    int row = index.row();

    // Получаем модель и извлекаем данные
    QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->tableView_kas->model());
    if (!model) return;


    // Извлекаем данные из выбранной строки
    //int set_id = textSetId->value();
    QString set_id = ui->textSetId->text(); //textSetId_2
    //QString set_id = ui->textSetId->text().trimmed();
    int kas_id = model->data(model->index(row, 0)).toInt();
    QString kas_name = model->data(model->index(row, 1)).toString();
    int kas_height = model->data(model->index(row, 2)).toInt();
    int kas_width = model->data(model->index(row, 3)).toInt();
    QString kas_block = model->data(model->index(row, 4)).toString();
    QString kas_note = model->data(model->index(row, 5)).toString();
    QString last_update = model->data(model->index(row, 6)).toString();

    // Заполняем поля формы
    QString setName = " - " + ui->inputSetName->text();
    QString setAllName = set_id + setName;
    ui->textSetId_2->setText(setAllName); //не только цифра
    ui->textKasId->setText(QString::number(kas_id));
    ui->inputKasName->setText(kas_name);
    ui->inputKasNote->setText(kas_note);
    ui->textKasId->setText(QString::number(kas_id));
    ui->inputKasName->setText(kas_name);
    ui->inputKasNote->setText(kas_note);

    ui->inputIntKasLenght->setValue(kas_height);
    //ui->inputIntKasLenght->setReadOnly(true);

    ui->inputIntKasWidth->setValue(kas_width);
    //ui->inputIntKasWidth->setReadOnly(true);

    ui->dateKasLastUpdate->setText(last_update);

    // Устанавливаем статус блокировки
    if (kas_block == "0") {
        ui->comboKasStatus->setCurrentText("Разблокирован");
    }
    else if (kas_block == "1") {
        ui->comboKasStatus->setCurrentText("Заблокирован");
    }
    else {
        //qWarning() << "Неизвестный статус блокировки кассетницы:" << kas_block;
        ui->comboKasStatus->setCurrentIndex(0);
    }


    QLayoutItem *item;
    while ((item = ui->gridLayout_21->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    QPushButton *btnKasChange = new QPushButton("Сохранить изменения кассетницы", this);
    ui->gridLayout_21->addWidget(btnKasChange);
    connect(btnKasChange, &QPushButton::clicked, this, &MainWindow::on_btnKasChange_pressed);

    loadFilteredMeshData(QString::number(kas_id));

    checkRole();
}

void MainWindow::loadFilteredMeshData(QString kas_id)
{
    try {
        const QString set_id = ui->textSetId->text().trimmed();
        if (set_id.isEmpty()) {
            throw std::runtime_error(tr("Не указан ID набора").toStdString());
        }

        // 1. Получаем ширину кассетницы
        int width = 0;
        QSqlQuery widthQuery;
        widthQuery.prepare("SELECT kas_width FROM kas WHERE set_id = :set_id AND kas_id = :kas_id");
        widthQuery.bindValue(":set_id", set_id);
        widthQuery.bindValue(":kas_id", kas_id);

        if (!widthQuery.exec() || !widthQuery.next()) {
            //throw std::runtime_error(tr("Ошибка получения ширины кассетницы").toStdString());
        }
        width = widthQuery.value(0).toInt();
        if (width <= 0) {
            //throw std::runtime_error(tr("Неверная ширина кассетницы").toStdString());
        }

        // 2. Загружаем данные мешей
        QVector<MeshItem> items;
        QString user_id, doz_tld_id;
        QSqlQuery query;
        query.prepare("SELECT mesh_id, mesh_status, user_id, doz_tld_id FROM mesh "
                     "WHERE set_id = :set_id AND kas_id = :kas_id ORDER BY mesh_id");
        query.bindValue(":set_id", set_id);
        query.bindValue(":kas_id", kas_id);

        if (!query.exec()) {
            throw std::runtime_error(query.lastError().text().toStdString());
        }

        while (query.next()) {
            QString current_user_id = query.value(2).toString();
            QString current_doz_id = query.value(3).toString();

            if (!current_user_id.isEmpty() && current_user_id != "0") {
                user_id = current_user_id;
            }
            if (!current_doz_id.isEmpty() && current_doz_id != "0") {
                doz_tld_id = current_doz_id;
            }

            items.append({
                query.value(0).toString(),
                query.value(1).toString(),
                current_user_id,
                current_doz_id
            });
        }

        if (items.isEmpty()) {
            QMessageBox::information(this, tr("Информация"), tr("Данные не найдены"));
            return;
        }

        // 3. Создаем и настраиваем модель
        MeshStatusModel* model = new MeshStatusModel(user_id, doz_tld_id, width, this);
        model->loadData(items);

        // 4. Настройка таблицы
        ui->tableView_mesh->setModel(nullptr);
        if (m_currentMeshModel) {
            delete m_currentMeshModel;
        }
        m_currentMeshModel = model;

        ui->tableView_mesh->setModel(model);

        // Критически важные настройки:
        ui->tableView_mesh->setIconSize(QSize(MeshStatusModel::ICON_SIZE, MeshStatusModel::ICON_SIZE)); // Сохраняем исходный размер иконок

        // Уменьшаем высоту строк, сохраняя размер контента
        int compactRowHeight = MeshStatusModel::ICON_SIZE * 2; // Иконка + текст + иконка с минимальными отступами
        ui->tableView_mesh->verticalHeader()->setDefaultSectionSize(compactRowHeight);

        // Фиксированная ширина столбцов
        for (int i = 0; i < model->columnCount(); ++i) {
            ui->tableView_mesh->setColumnWidth(i, width);
        }

        // Настройки внешнего вида
        ui->tableView_mesh->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        ui->tableView_mesh->verticalHeader()->setVisible(false);
        ui->tableView_mesh->setShowGrid(false);

        // Уменьшаем отступы внутри ячеек
        ui->tableView_mesh->setStyleSheet(
            "QTableView {"
            "   border: none;"
            "}"
            "QTableView::item {"
            "   padding-top: 2px;"       // Уменьшенный верхний отступ
            "   padding-bottom: 2px;"    // Уменьшенный нижний отступ
            "   margin: 0px;"            // Убираем внешние отступы
            "}"
        );

        // Оптимизация производительности
        ui->tableView_mesh->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        ui->tableView_mesh->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

        // Обновляем отображение
        ui->tableView_mesh->viewport()->update();

        // 5. Подключение сигналов
        connect(ui->tableView_mesh->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &MainWindow::onMeshSelectionChanged, Qt::UniqueConnection);

    } catch (const std::exception& e) {
        //QMessageBox::critical(this, tr("Ошибка052"), QString::fromStdString(e.what()));
        qCritical() << "Ошибка в loadFilteredMeshData:" << e.what();
    }
}


//QMessageBox::critical(this, tr("Ошибка"), "выполяемся");
void MainWindow::onMeshSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    // 1. Проверка выбора и модели
    if (selected.indexes().isEmpty()) {
        qDebug() << "Не выбрано ни одной ячейки";
        return;
    }

    QModelIndex selectedIndex = selected.indexes().first();
    MeshStatusModel *model = qobject_cast<MeshStatusModel*>(ui->tableView_mesh->model());
    if (!model) {
        qWarning() << "Ошибка: модель данных недоступна";
        return;
    }

    // 2. Получение mesh_id из выбранной ячейки
    QString mesh_id = model->data(selectedIndex, Qt::DisplayRole).toString();
    qDebug() << "Выбран Mesh ID:" << mesh_id;

    // 3. Получение текущих set_id и kas_id
    QString set_id = ui->textSetId->text().trimmed();
    QString kas_id = ui->comboBox_listOfKas->currentText().section(' ', 0, 0); // Извлекаем только ID из комбобокса

    // 4. Выполнение запроса с проверкой
    QSqlQuery query;
    query.prepare("SELECT user_id, doz_tld_id, mesh_status, mesh_note, last_update FROM mesh WHERE mesh_id = :mesh_id AND set_id = :set_id AND kas_id = :kas_id");
    query.bindValue(":mesh_id", mesh_id);
    query.bindValue(":set_id", set_id);
    query.bindValue(":kas_id", kas_id);

    if (!query.exec()) {
        qCritical() << "Ошибка SQL запроса:" << query.lastError().text();
        return;
    }

    if (!query.next()) {
        qWarning() << "Данные не найдены для Mesh ID:" << mesh_id << "Set ID:" << set_id << "Kas ID:" << kas_id;
        return;
    }

    // 5. Получение данных из запроса
    QString user_id = query.value(0).toString();
    QString doz_tld_id = query.value(1).toString();
    QString mesh_status = query.value(2).toString();
    QString mesh_note = query.value(3).toString();
    QString last_update = query.value(4).toString();

    // 6. Обновление интерфейса
    ui->label_id_set->setText(set_id);
    ui->label_id_kas->setText(kas_id);
    ui->label_id_mesh->setText(mesh_id);
    ui->line_MeshNote->setText(mesh_note);
    ui->lineEdit_MeshDoz->setText(doz_tld_id);
    ui->label_MeshUpdate->setText(last_update);

    // Установка статуса
    if (mesh_status == "0") ui->comboBox_MeshStatus->setCurrentText("0 - заблокирована (красный)");
    else if (mesh_status == "1") ui->comboBox_MeshStatus->setCurrentText("1 - с дозиметром (зеленая)");
    else if (mesh_status == "2") ui->comboBox_MeshStatus->setCurrentText("2 - пустая (белая)");
    else if (mesh_status == "3") ui->comboBox_MeshStatus->setCurrentText("3 - возможно не корректно датчик присутствия (серая)");
    else if (mesh_status == "4") ui->comboBox_MeshStatus->setCurrentText("4 - сломана (красная)");
    else if (mesh_status == "5") ui->comboBox_MeshStatus->setCurrentText("5 - открыта (синяя)");
    else if (mesh_status == "6") ui->comboBox_MeshStatus->setCurrentText("6 - другое (желтая)");


   // QSqlQuery query;
    if (user_id != '0'){
        query.prepare("SELECT name_0, name_1, name_2 FROM users WHERE user_id = :user_id");
        query.bindValue(":user_id", user_id);
        if (!query.exec()) {
            qDebug() << "Ошибка выполнения запроса18:" << query.lastError().text();
            return;
        }
        if (query.next()) {
            QString name0 = query.value("name_0").toString();
            QString name1 = query.value("name_1").toString();
            QString name2 = query.value("name_2").toString();

            QString forUser = QString("%1 - %2 %3 %4")
                             .arg(user_id)
                             .arg(name0)
                             .arg(name1)
                             .arg(name2);

            qDebug() << "Формируемая строка:" << forUser;
            ui->comboBox_MeshUser->setCurrentText(forUser);
        } else {
            ui->comboBox_MeshUser->setCurrentText(user_id + " - Неизвестный пользователь");
        }
    }
    else {
        ui->comboBox_MeshUser->setCurrentIndex(-1);
    }
}


void MainWindow::on_btnKasChange_pressed() {
    // Получение данных из UI
    QString set_id = ui->textSetId->text();
    QString kas_id = ui->textKasId->text();

    // Получение текущих размеров кассетницы
    QSqlQuery query1;
    query1.prepare("SELECT kas_height, kas_width FROM kas WHERE set_id = :set_id AND kas_id = :kas_id");
    query1.bindValue(":set_id", set_id);
    query1.bindValue(":kas_id", kas_id);

    int kas_height = 0, kas_width = 0;
    if (query1.exec()) {
        if (query1.next()) {
            kas_height = query1.value("kas_height").toInt();
            kas_width = query1.value("kas_width").toInt();
        }
    }

    int kasQuantity_old = kas_height * kas_width;

    // Поиск максимального mesh_id
    int maxMeshId = -1;
    QSqlQuery query2;
    query2.prepare("SELECT MAX(mesh_id) AS max_mesh_id FROM mesh WHERE set_id = :set_id AND kas_id = :kas_id");
    query2.bindValue(":set_id", set_id);
    query2.bindValue(":kas_id", kas_id);

    if (query2.exec()) {
        if (query2.next()) {
            if (!query2.value(0).isNull()) {
                maxMeshId = query2.value(0).toInt();
            }
        }
    }

    // Подготовка данных для обновления
    QTimeZone moscowTimeZone("Europe/Moscow");
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString last_update = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");

    QString KasName = ui->inputKasName->text();
    QString KasNote = ui->inputKasNote->text();
    int KasLenght = ui->inputIntKasLenght->value();
    int KasWidth = ui->inputIntKasWidth->value();
    QString KasStatus = ui->comboKasStatus->currentText();

    if (KasName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка053", "Поле 'KasName' не может быть пустым!");
        return;
    }

    // Обновление кассетницы
    QSqlQuery query3;
    query3.prepare("UPDATE kas SET kas_name = :kas_name, kas_height = :kas_height, kas_width = :kas_width, kas_block = :kas_block, kas_note = :kas_note, last_update = :last_update WHERE set_id = :set_id AND kas_id = :kas_id");
    query3.bindValue(":set_id", set_id);
    query3.bindValue(":kas_id", kas_id);
    query3.bindValue(":kas_name", KasName);
    query3.bindValue(":kas_height", KasLenght);
    query3.bindValue(":kas_width", KasWidth);
    query3.bindValue(":kas_block", KasStatus);
    query3.bindValue(":kas_note", KasNote);
    query3.bindValue(":last_update", last_update);

    if (!query3.exec()) {
        QMessageBox::critical(this, "Ошибка054", "Не удалось изменить данные!");
        return;
    }

    // Обработка изменения количества ячеек
    int kasQuantity_new = KasLenght * KasWidth;

    if (kasQuantity_new > kasQuantity_old) {
        int newMesh = kasQuantity_new - kasQuantity_old;
        for (int i = 1; i <= newMesh; ++i) {
            QSqlQuery query4;
            query4.prepare("INSERT INTO mesh (set_id, kas_id, mesh_id, user_id, doz_tld_id, mesh_status, mesh_note, last_update) VALUES (:set_id, :kas_id, :mesh_id, :user_id, :doz_tld_id, :mesh_status, :mesh_note, :last_update)");
            query4.bindValue(":set_id", set_id);
            query4.bindValue(":kas_id", kas_id);
            query4.bindValue(":mesh_id", maxMeshId + i);
            query4.bindValue(":user_id", QVariant());
            query4.bindValue(":doz_tld_id", QVariant());
            query4.bindValue(":mesh_status", 0);
            query4.bindValue(":mesh_note", "");
            query4.bindValue(":last_update", QDateTime::currentDateTime());

            query4.exec();

            // История для каждой новой ячейки
            QString for_user_id = ui->label_userId->text();
            QString changing_user_id;
            QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                QSqlQuery query5;
                query5.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
                query5.bindValue(":name_0", parts[0]);
                query5.bindValue(":name_1", parts[1]);
                query5.bindValue(":name_2", parts[2]);

                if (query5.exec() && query5.next()) {
                    changing_user_id = query5.value("user_id").toString();
                }
            }

            if (!changing_user_id.isEmpty()) {
                addMeshHistory(
                    set_id.toInt(),
                    kas_id.toInt(),
                    maxMeshId + i,
                    0,
                    "",
                    0,
                    "",
                    changing_user_id,
                    "change"
                );
            }
        }
        meshHistoryToTable();
    } else if (kasQuantity_new < kasQuantity_old) {
        int delMesh = kasQuantity_old - kasQuantity_new;
        QSqlQuery query6;
        query6.prepare("DELETE FROM mesh WHERE set_id = :set_id AND kas_id = :kas_id AND mesh_id > :delMesh");
        query6.bindValue(":set_id", set_id);
        query6.bindValue(":kas_id", kas_id);
        query6.bindValue(":delMesh", kasQuantity_new);
        query6.exec();
    }

    // Обновление интерфейса
    loadFilteredKasData();
    loadListOfKas();

    QString comboText = kas_id + " - " + KasName;
    int index = ui->comboBox_listOfKas->findText(comboText);
    if (index >= 0) {
        ui->comboBox_listOfKas->setCurrentIndex(index);
    } else {
        ui->comboBox_listOfKas->setCurrentIndex(0);
    }

    // История для кассетницы
    QString for_user_id2 = ui->label_userId->text();
    QString changing_user_id2;
    QStringList parts2 = for_user_id2.split(' ', Qt::SkipEmptyParts);
    if (parts2.size() >= 3) {
        QSqlQuery query7;
        query7.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
        query7.bindValue(":name_0", parts2[0]);
        query7.bindValue(":name_1", parts2[1]);
        query7.bindValue(":name_2", parts2[2]);

        if (query7.exec() && query7.next()) {
            changing_user_id2 = query7.value("user_id").toString();
        }
    }

    if (!changing_user_id2.isEmpty()) {
        addKasHistory(
            set_id.toInt(),
            kas_id.toInt(),
            KasName,
            KasLenght,
            KasWidth,
            KasStatus,
            KasNote,
            changing_user_id2,
            "change"
        );
        kasHistoryToTable();
    }

    on_btnKasCreateNew_pressed();

    updateListSetForHistory();
    comboboxUpdateAll();
}


void MainWindow::on_comboBox_listOfKas_currentTextChanged(const QString &index)
{
    QString selectedText = ui->comboBox_listOfKas->currentText();

    QString kas_id = selectedText.split("-").first().trimmed();
    if (!kas_id.isEmpty()) {
        loadFilteredMeshData(kas_id);
    } else {
        qDebug() << "Не удалось извлечь ID кассетницы из:" << selectedText;
    }

}

/*--------------------------------------------------------------------------------*/

void MainWindow::on_pushButton_Exit_pressed()
{
    this->close();
}

void MainWindow::on_btn_userDutySearch_pressed()
{
    QString searchText = ui->line_userDutySearch->text().trimmed();

    QSqlQuery query;
    QList<int> foundUserIds;

    query.prepare("SELECT user_id, name_0, name_1, name_2 FROM users");
    if (!query.exec()) {
        qDebug() << "Ошибка запроса к таблице users:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString userIdStr = query.value("user_id").toString();
        QString name0 = query.value("name_0").toString();
        QString name1 = query.value("name_1").toString();
        QString name2 = query.value("name_2").toString();

        // Проверяем совпадение с поисковым запросом
        if (userIdStr.contains(searchText, Qt::CaseInsensitive) ||
            name0.contains(searchText, Qt::CaseInsensitive) ||
            name1.contains(searchText, Qt::CaseInsensitive) ||
            name2.contains(searchText, Qt::CaseInsensitive) ||
            (name0 + " " + name1).contains(searchText, Qt::CaseInsensitive) ||
            (name1 + " " + name2).contains(searchText, Qt::CaseInsensitive) ||
            (name0 + " " + name1 + " " + name2).contains(searchText, Qt::CaseInsensitive))
        {
            foundUserIds << query.value("user_id").toInt();
        }
    }

    if (foundUserIds.isEmpty()) {
        QMessageBox::information(this, "Результат поиска", "Совпадений не найдено");
        // Показываем все строки, если поиск пустой
        if (searchText.isEmpty()) {
            for (int i = 0; i < ui->table_userDuty->rowCount(); ++i) {
                ui->table_userDuty->setRowHidden(i, false);
            }
        }
        return;
    }

    // 2. Фильтрация существующих данных в таблице
    for (int i = 0; i < ui->table_userDuty->rowCount(); ++i) {
        bool match = false;
        QString userId = ui->table_userDuty->item(i, 1)->text(); // Столбец с user_id

        for (int id : foundUserIds) {
            if (userId.toInt() == id) {
                match = true;
                break;
            }
        }

        ui->table_userDuty->setRowHidden(i, !match);
    }
}


void MainWindow::on_btnUserFilterGo_2_pressed()
{
    // Получаем значения из фильтров
    QString dateFilter = ui->selectUserFilterDate_2->currentText();
    QString userFilter = ui->selectUserFilterFIO_2->currentText();
    QString userChangeFilter = ui->selectUserFilterFIO_3->currentText();

    // Функция для извлечения ID
    auto extractId = [](const QString &input) -> QString {
        if (input == "-" || input.isEmpty()) return "-";
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatch match = re.match(input);
        return match.hasMatch() ? match.captured(1) : input;
    };

    QString userId = extractId(userFilter);
    QString userChangeId = extractId(userChangeFilter);

    // Создаем временную таблицу для сортировки
    QList<QList<QString>> visibleRows;

    // Собираем подходящие строки
    for (int row = 0; row < ui->tableWidget_historyUsers->rowCount(); ++row) {
        bool showRow = true;

        // Проверяем фильтр по user_id (столбец 0 - ID пользователя)
        if (userId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyUsers->item(row, 0);
            if (!item || extractId(item->text()) != userId) {
                showRow = false;
            }
        }

        // Проверяем фильтр по user_id_change (столбец 23 - ID изменившего)
        if (showRow && userChangeId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyUsers->item(row, 23);
            if (!item || extractId(item->text()) != userChangeId) {
                showRow = false;
            }
        }

        // Сохраняем данные строки
        if (showRow) {
            QList<QString> rowData;
            for (int col = 0; col < ui->tableWidget_historyUsers->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget_historyUsers->item(row, col);
                rowData.append(item ? item->text() : "");
            }
            visibleRows.append(rowData);
        }
    }

    // Отладочная информация
    qDebug() << "Найдено строк для сортировки:" << visibleRows.size();

    // Сортируем по дате (столбец 22 - last_update)
    if (dateFilter == "Дата (сначала старые)") {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                // Проверяем, что столбец 22 существует
                if (a.size() <= 22 || b.size() <= 22) return false;

                QDateTime dateA = QDateTime::fromString(a[22], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[22], "dd.MM.yyyy HH:mm");

                // Если даты не парсятся, используем строковое сравнение
                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[22] < b[22];
                }

                return dateA < dateB;
            });
    } else {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                // Проверяем, что столбец 22 существует
                if (a.size() <= 22 || b.size() <= 22) return false;

                QDateTime dateA = QDateTime::fromString(a[22], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[22], "dd.MM.yyyy HH:mm");

                // Если даты не парсятся, используем строковое сравнение
                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[22] > b[22];
                }

                return dateA > dateB;
            });
    }

    // Обновляем таблицу
    ui->tableWidget_historyUsers->setRowCount(0);
    for (const QList<QString> &rowData : visibleRows) {
        int newRow = ui->tableWidget_historyUsers->rowCount();
        ui->tableWidget_historyUsers->insertRow(newRow);

        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);

            // Выравнивание для числовых полей
            if (col == 0 || col == 12 || col == 13 || col == 14 || col == 17 ||
                col == 18 || col == 19 || col == 23) {
                item->setTextAlignment(Qt::AlignCenter);
            }

            ui->tableWidget_historyUsers->setItem(newRow, col, item);
        }
    }

}


void MainWindow::on_btnUserFilterStop_2_pressed() //сброс фильтров для пользователей
{
    usersHistoryToTable();
}


void MainWindow::on_pushButton_usersHistoryFile_pressed()
{
    //выгрузить в csv
    // Получаем текущую дату для названия файла
    QString currentDate = QDate::currentDate().toString("dd.MM.yyyy");
    QString fileName = QString("user-%1.csv").arg(currentDate);

    // Запрашиваем путь для сохранения
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить файл",
                                                   QDir::homePath() + "/" + fileName,
                                                   "CSV Files (*.csv)");

    if (filePath.isEmpty()) {
        return; // Пользователь отменил диалог
    }

    // Открываем файл для записи
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка057", "Не удалось создать файл: " + file.errorString());
        return;
    }

    QTextStream out(&file);


    // Установка кодировки в зависимости от версии Qt
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8"); // Qt 5
    #else
    out.setEncoding(QStringConverter::Utf8); // Qt 6
    #endif

    // Записываем заголовки столбцов
    for (int col = 0; col < ui->tableWidget_historyUsers->columnCount(); ++col) {
        QTableWidgetItem *headerItem = ui->tableWidget_historyUsers->horizontalHeaderItem(col);
        if (headerItem) {
            out << "\"" << headerItem->text() << "\"";
        } else {
            out << "\"Column " << col + 1 << "\"";
        }
        if (col < ui->tableWidget_historyUsers->columnCount() - 1) {
            out << ";"; // Разделитель столбцов
        }
    }
    out << "\n";

    // Записываем данные
    for (int row = 0; row < ui->tableWidget_historyUsers->rowCount(); ++row) {
        for (int col = 0; col < ui->tableWidget_historyUsers->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget_historyUsers->item(row, col);
            if (item && !item->text().isEmpty()) {
                // Экранируем кавычки и добавляем текст
                QString text = item->text();
                text.replace("\"", "\"\""); // Экранирование кавычек
                out << "\"" << text << "\"";
            } else {
                out << "\"\""; // Пустая ячейка
            }
            if (col < ui->tableWidget_historyUsers->columnCount() - 1) {
                out << ";";
            }
        }
        out << "\n";
    }

    file.close();
    QMessageBox::information(this, "Успех", QString("Данные сохранены в файл:\n%1").arg(filePath));
}


void MainWindow::on_btn_dutyUsersFilterStop_pressed()
{
    loadDutyUsersHistoryToTable();
}

void MainWindow::on_btn_dutyUsersFilterGo_pressed()
{
    // Получаем значения из фильтров
    QString dateFilter = ui->select_dutyUsersFilterDate_3->currentText();
    QString dutyFilter = ui->select_dutyUsersFilterFIO_2->currentText();
    QString userFilter = ui->select_dutyUsersFilterFIO_3->currentText();

    auto extractId = [](const QString &input) -> QString {
        if (input == "-" || input.isEmpty()) return "-";
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatch match = re.match(input);
        return match.hasMatch() ? match.captured(1) : input;
    };

    QString dutyId = extractId(dutyFilter);
    QString userId = extractId(userFilter);

    QList<QList<QString>> visibleRows;

    // Собираем подходящие строки
    for (int row = 0; row < ui->tableWidget_historyDuty_2->rowCount(); ++row) {
        bool showRow = true;

        // Фильтр по ID пользователя (столбец 1)
        if (dutyId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyDuty_2->item(row, 1);
            if (!item || extractId(item->text()) != dutyId) {
                showRow = false;
            }
        }

        // Фильтр по ID изменившего (столбец 7)
        if (showRow && userId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyDuty_2->item(row, 7);
            if (!item || extractId(item->text()) != userId) {
                showRow = false;
            }
        }

        if (showRow) {
            QList<QString> rowData;
            for (int col = 0; col < ui->tableWidget_historyDuty_2->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget_historyDuty_2->item(row, col);
                rowData.append(item ? item->text() : "");
            }
            visibleRows.append(rowData);

            // Отладочный вывод даты
            if (rowData.size() > 8) {
                qDebug() << "Строка" << row << "дата:" << rowData[8];
            }
        }
    }

    // Сортируем по столбцу 8 (last_update_change)
    if (dateFilter == "Дата (сначала старые)") {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                return a[7] < b[7]; // Простое строковое сравнение
            });
    } else {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                return a[7] > b[7]; // Простое строковое сравнение
            });
    }

    // Обновляем таблицу
    ui->tableWidget_historyDuty_2->setRowCount(0);
    for (const QList<QString> &rowData : visibleRows) {
        int newRow = ui->tableWidget_historyDuty_2->rowCount();
        ui->tableWidget_historyDuty_2->insertRow(newRow);

        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget_historyDuty_2->setItem(newRow, col, item);
        }
    }

    qDebug() << "Сортировка завершена. Показано строк:" << visibleRows.size();
}


void MainWindow::on_btn_dutyFilterStop_pressed()
{
    loadDutyHistoryToTable();
}



void MainWindow::on_btn_dutyFilterGo_pressed()
{
    // Получаем значения из фильтров
    QString dateFilter = ui->selec_dutyFilterDate->currentText();
    QString dutyFilter = ui->select_dutyFilter->currentText();
    QString fioFilter = ui->select_dutyFilterFIO->currentText();

    QStringList parts = fioFilter.split(' ');
    QString targetUserId = parts[0];

    int rowCount = ui->tableWidget_historyDuty->rowCount();

    // Скрываем все строки
    for (int row = 0; row < rowCount; ++row) {
        ui->tableWidget_historyDuty->setRowHidden(row, true);
    }

    // Применяем фильтры и показываем подходящие строки
    for (int row = 0; row < rowCount; ++row) {
        bool showRow = true;

        // Фильтр по duty_id (столбец 0 - ID наряда)
        if (dutyFilter != "-" && !dutyFilter.isEmpty()) {
            QTableWidgetItem *dutyItem = ui->tableWidget_historyDuty->item(row, 0);
            if (dutyItem && dutyItem->text() != dutyFilter) {
                showRow = false;
            }
        }

        // Фильтр по user_id_change (столбец 26 - ID изменившего) с учетом ФИО
        if (showRow && fioFilter != "-" && !fioFilter.isEmpty()) {
            QTableWidgetItem *userItem = ui->tableWidget_historyDuty->item(row, 26);
            if (userItem) {
                bool ok;
                QString rowUserId = userItem->text();
                if (rowUserId != targetUserId) {
                    showRow = false;
                }
            } else {
                showRow = false;
            }
        }

        // Показываем строку если она прошла все фильтры
        if (showRow) {
            ui->tableWidget_historyDuty->setRowHidden(row, false);
        }
    }

    // Применяем сортировку по дате только к видимым строкам
    // Создаем временный список для сортировки видимых строк
    QList<QPair<QDateTime, int>> visibleRows;

    for (int row = 0; row < rowCount; ++row) {
        if (!ui->tableWidget_historyDuty->isRowHidden(row)) {
            QTableWidgetItem *dateItem = ui->tableWidget_historyDuty->item(row, 25); // last_update
            if (dateItem) {
                QDateTime rowDate = QDateTime::fromString(dateItem->text(), "dd.MM.yyyy HH:mm");
                visibleRows.append(qMakePair(rowDate, row));
            }
        }
    }

    // Сортируем видимые строки по дате
    if (dateFilter == "Дата (сначала старые)") {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QPair<QDateTime, int> &a, const QPair<QDateTime, int> &b) {
                return a.first < b.first;
            });
    } else {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QPair<QDateTime, int> &a, const QPair<QDateTime, int> &b) {
                return a.first > b.first;
            });
    }

    // Перемещаем строки в отсортированном порядке
    for (int newIndex = 0; newIndex < visibleRows.size(); ++newIndex) {
        int oldIndex = visibleRows[newIndex].second;
        if (oldIndex != newIndex) {
            // Сохраняем данные строки
            QList<QTableWidgetItem*> rowItems;
            for (int col = 0; col < ui->tableWidget_historyDuty->columnCount(); ++col) {
                rowItems.append(ui->tableWidget_historyDuty->takeItem(oldIndex, col));
            }

            // Вставляем строку в новую позицию
            ui->tableWidget_historyDuty->insertRow(newIndex);
            for (int col = 0; col < rowItems.size(); ++col) {
                if (rowItems[col]) {
                    ui->tableWidget_historyDuty->setItem(newIndex, col, rowItems[col]);
                }
            }

            // Удаляем старую строку
            ui->tableWidget_historyDuty->removeRow(oldIndex + (newIndex < oldIndex ? 1 : 0));
        }
    }

    qDebug() << "Применены фильтры - Дата:" << dateFilter
             << "Наряд:" << dutyFilter
             << "Пользователь:" << fioFilter
             << "Показано строк:" << visibleRows.size();
}


void MainWindow::on_pushButton_dutyUsersFile_2_pressed()
{
    // Получаем текущую дату для названия файла
    QString currentDate = QDate::currentDate().toString("dd.MM.yyyy");
    QString fileName = QString("dutyUser-%1.csv").arg(currentDate);

    // Запрашиваем путь для сохранения
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить файл",
                                                   QDir::homePath() + "/" + fileName,
                                                   "CSV Files (*.csv)");

    if (filePath.isEmpty()) {
        return; // Пользователь отменил диалог
    }

    // Открываем файл для записи
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка058", "Не удалось создать файл: " + file.errorString());
        return;
    }

    QTextStream out(&file);

    // Установка кодировки в зависимости от версии Qt
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8"); // Qt 5
    #else
    out.setEncoding(QStringConverter::Utf8); // Qt 6
    #endif

    // Записываем заголовки столбцов
    for (int col = 0; col < ui->tableWidget_historyDuty_2->columnCount(); ++col) {
        QTableWidgetItem *headerItem = ui->tableWidget_historyDuty_2->horizontalHeaderItem(col);
        if (headerItem) {
            out << "\"" << headerItem->text() << "\"";
        } else {
            out << "\"Column " << col + 1 << "\"";
        }
        if (col < ui->tableWidget_historyDuty_2->columnCount() - 1) {
            out << ";"; // Разделитель столбцов
        }
    }
    out << "\n";

    // Записываем данные
    for (int row = 0; row < ui->tableWidget_historyDuty_2->rowCount(); ++row) {
        for (int col = 0; col < ui->tableWidget_historyDuty_2->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget_historyDuty_2->item(row, col);
            if (item && !item->text().isEmpty()) {
                // Экранируем кавычки и добавляем текст
                QString text = item->text();
                text.replace("\"", "\"\""); // Экранирование кавычек
                out << "\"" << text << "\"";
            } else {
                out << "\"\""; // Пустая ячейка
            }
            if (col < ui->tableWidget_historyDuty_2->columnCount() - 1) {
                out << ";";
            }
        }
        out << "\n";
    }

    file.close();
    QMessageBox::information(this, "Успех", QString("Данные сохранены в файл:\n%1").arg(filePath));
}

void MainWindow::on_pushButton_dutyHistoryFile_pressed()
{
    // Получаем текущую дату для названия файла
    QString currentDate = QDate::currentDate().toString("dd.MM.yyyy");
    QString fileName = QString("duty-%1.csv").arg(currentDate);

    // Запрашиваем путь для сохранения
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить файл",
                                                   QDir::homePath() + "/" + fileName,
                                                   "CSV Files (*.csv)");

    if (filePath.isEmpty()) {
        return; // Пользователь отменил диалог
    }

    // Открываем файл для записи
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка059", "Не удалось создать файл: " + file.errorString());
        return;
    }

    QTextStream out(&file);

    // Установка кодировки в зависимости от версии Qt
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8"); // Qt 5
    #else
    out.setEncoding(QStringConverter::Utf8); // Qt 6
    #endif

    // Записываем заголовки столбцов
    for (int col = 0; col < ui->tableWidget_historyDuty->columnCount(); ++col) {
        QTableWidgetItem *headerItem = ui->tableWidget_historyDuty->horizontalHeaderItem(col);
        if (headerItem) {
            out << "\"" << headerItem->text() << "\"";
        } else {
            out << "\"Column " << col + 1 << "\"";
        }
        if (col < ui->tableWidget_historyDuty->columnCount() - 1) {
            out << ";"; // Разделитель столбцов
        }
    }
    out << "\n";

    // Записываем данные
    for (int row = 0; row < ui->tableWidget_historyDuty->rowCount(); ++row) {
        for (int col = 0; col < ui->tableWidget_historyDuty->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget_historyDuty->item(row, col);
            if (item && !item->text().isEmpty()) {
                // Экранируем кавычки и добавляем текст
                QString text = item->text();
                text.replace("\"", "\"\""); // Экранирование кавычек
                out << "\"" << text << "\"";
            } else {
                out << "\"\""; // Пустая ячейка
            }
            if (col < ui->tableWidget_historyDuty->columnCount() - 1) {
                out << ";";
            }
        }
        out << "\n";
    }

    file.close();
    QMessageBox::information(this, "Успех", QString("Данные сохранены в файл:\n%1").arg(filePath));
}

void MainWindow::on_pushButton_SetHistoryFile_pressed()
{
        // Получаем текущую дату для названия файла
        QString currentDate = QDate::currentDate().toString("dd.MM.yyyy");
        QString fileName = QString("set-%1.csv").arg(currentDate);

        // Запрашиваем путь для сохранения
        QString filePath = QFileDialog::getSaveFileName(this, "Сохранить файл",
                                                       QDir::homePath() + "/" + fileName,
                                                       "CSV Files (*.csv)");

        if (filePath.isEmpty()) {
            return; // Пользователь отменил диалог
        }

        // Открываем файл для записи
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Ошибка060", "Не удалось создать файл: " + file.errorString());
            return;
        }

        QTextStream out(&file);

        // Установка кодировки в зависимости от версии Qt
        #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        out.setCodec("UTF-8"); // Qt 5
        #else
        out.setEncoding(QStringConverter::Utf8); // Qt 6
        #endif

        // Записываем заголовки столбцов
        for (int col = 0; col < ui->tableWidget_historySet->columnCount(); ++col) {
            QTableWidgetItem *headerItem = ui->tableWidget_historySet->horizontalHeaderItem(col);
            if (headerItem) {
                out << "\"" << headerItem->text() << "\"";
            } else {
                out << "\"Column " << col + 1 << "\"";
            }
            if (col < ui->tableWidget_historySet->columnCount() - 1) {
                out << ";"; // Разделитель столбцов
            }
        }
        out << "\n";

        // Записываем данные
        for (int row = 0; row < ui->tableWidget_historySet->rowCount(); ++row) {
            for (int col = 0; col < ui->tableWidget_historySet->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget_historySet->item(row, col);
                if (item && !item->text().isEmpty()) {
                    // Экранируем кавычки и добавляем текст
                    QString text = item->text();
                    text.replace("\"", "\"\""); // Экранирование кавычек
                    out << "\"" << text << "\"";
                } else {
                    out << "\"\""; // Пустая ячейка
                }
                if (col < ui->tableWidget_historySet->columnCount() - 1) {
                    out << ";";
                }
            }
            out << "\n";
        }

        file.close();
        QMessageBox::information(this, "Успех", QString("Данные сохранены в файл:\n%1").arg(filePath));

}


void MainWindow::on_pushButton_KasHistoryFile_pressed()
{
        // Получаем текущую дату для названия файла
        QString currentDate = QDate::currentDate().toString("dd.MM.yyyy");
        QString fileName = QString("kas-%1.csv").arg(currentDate);

        // Запрашиваем путь для сохранения
        QString filePath = QFileDialog::getSaveFileName(this, "Сохранить файл",
                                                       QDir::homePath() + "/" + fileName,
                                                       "CSV Files (*.csv)");

        if (filePath.isEmpty()) {
            return; // Пользователь отменил диалог
        }

        // Открываем файл для записи
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Ошибка061", "Не удалось создать файл: " + file.errorString());
            return;
        }

        QTextStream out(&file);

        // Установка кодировки в зависимости от версии Qt
        #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        out.setCodec("UTF-8"); // Qt 5
        #else
        out.setEncoding(QStringConverter::Utf8); // Qt 6
        #endif

        // Записываем заголовки столбцов
        for (int col = 0; col < ui->tableWidget_historyKas->columnCount(); ++col) {
            QTableWidgetItem *headerItem = ui->tableWidget_historyKas->horizontalHeaderItem(col);
            if (headerItem) {
                out << "\"" << headerItem->text() << "\"";
            } else {
                out << "\"Column " << col + 1 << "\"";
            }
            if (col < ui->tableWidget_historyKas->columnCount() - 1) {
                out << ";"; // Разделитель столбцов
            }
        }
        out << "\n";

        // Записываем данные
        for (int row = 0; row < ui->tableWidget_historyKas->rowCount(); ++row) {
            for (int col = 0; col < ui->tableWidget_historyKas->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget_historyKas->item(row, col);
                if (item && !item->text().isEmpty()) {
                    // Экранируем кавычки и добавляем текст
                    QString text = item->text();
                    text.replace("\"", "\"\""); // Экранирование кавычек
                    out << "\"" << text << "\"";
                } else {
                    out << "\"\""; // Пустая ячейка
                }
                if (col < ui->tableWidget_historyKas->columnCount() - 1) {
                    out << ";";
                }
            }
            out << "\n";
        }

        file.close();
        QMessageBox::information(this, "Успех", QString("Данные сохранены в файл:\n%1").arg(filePath));

}


void MainWindow::on_pushButton_MeshHistoryFile_pressed()
{
        // Получаем текущую дату для названия файла
        QString currentDate = QDate::currentDate().toString("dd.MM.yyyy");
        QString fileName = QString("mesh-%1.csv").arg(currentDate);

        // Запрашиваем путь для сохранения
        QString filePath = QFileDialog::getSaveFileName(this, "Сохранить файл",
                                                       QDir::homePath() + "/" + fileName,
                                                       "CSV Files (*.csv)");

        if (filePath.isEmpty()) {
            return; // Пользователь отменил диалог
        }

        // Открываем файл для записи
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Ошибка062", "Не удалось создать файл: " + file.errorString());
            return;
        }

        QTextStream out(&file);

        // Установка кодировки в зависимости от версии Qt
        #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        out.setCodec("UTF-8"); // Qt 5
        #else
        out.setEncoding(QStringConverter::Utf8); // Qt 6
        #endif

        // Записываем заголовки столбцов
        for (int col = 0; col < ui->tableWidget_historyMesh->columnCount(); ++col) {
            QTableWidgetItem *headerItem = ui->tableWidget_historyMesh->horizontalHeaderItem(col);
            if (headerItem) {
                out << "\"" << headerItem->text() << "\"";
            } else {
                out << "\"Column " << col + 1 << "\"";
            }
            if (col < ui->tableWidget_historyMesh->columnCount() - 1) {
                out << ";"; // Разделитель столбцов
            }
        }
        out << "\n";

        // Записываем данные
        for (int row = 0; row < ui->tableWidget_historyMesh->rowCount(); ++row) {
            for (int col = 0; col < ui->tableWidget_historyMesh->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget_historyMesh->item(row, col);
                if (item && !item->text().isEmpty()) {
                    // Экранируем кавычки и добавляем текст
                    QString text = item->text();
                    text.replace("\"", "\"\""); // Экранирование кавычек
                    out << "\"" << text << "\"";
                } else {
                    out << "\"\""; // Пустая ячейка
                }
                if (col < ui->tableWidget_historyMesh->columnCount() - 1) {
                    out << ";";
                }
            }
            out << "\n";
        }

        file.close();
        QMessageBox::information(this, "Успех", QString("Данные сохранены в файл:\n%1").arg(filePath));

}



void MainWindow::on_btn_SetFilterGo_pressed()
{
    // Получаем значения из фильтров
    QString dateFilter = ui->selec_SetFilterDate->currentText();
    QString setIdFilter = ui->select_kdd_set_1->currentText();
    QString userChangeFilter = ui->select_SetFilterFIO->currentText();

    // Функция для извлечения ID
    auto extractId = [](const QString &input) -> QString {
        if (input == "-" || input.isEmpty()) return "-";
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatch match = re.match(input);
        return match.hasMatch() ? match.captured(1) : input;
    };

    QString setId = extractId(setIdFilter);
    QString userChangeId = extractId(userChangeFilter);

    // Создаем временную таблицу для сортировки
    QList<QList<QString>> visibleRows;

    // Собираем подходящие строки
    for (int row = 0; row < ui->tableWidget_historySet->rowCount(); ++row) {
        bool showRow = true;

        // Проверяем фильтр по set_id (столбец 0 - ID комплекта)
        if (setId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historySet->item(row, 0);
            if (!item || extractId(item->text()) != setId) {
                showRow = false;
            }
        }

        // Проверяем фильтр по user_id_change (столбец 7 - ID изменившего)
        if (showRow && userChangeId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historySet->item(row, 7);
            if (!item || extractId(item->text()) != userChangeId) {
                showRow = false;
            }
        }

        // Сохраняем данные строки
        if (showRow) {
            QList<QString> rowData;
            for (int col = 0; col < ui->tableWidget_historySet->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget_historySet->item(row, col);
                rowData.append(item ? item->text() : "");
            }
            visibleRows.append(rowData);
        }
    }

    // Отладочная информация
    qDebug() << "Найдено строк для сортировки:" << visibleRows.size();

    // Сортируем по дате (столбец 9 - last_update_change)
    if (dateFilter == "Дата (сначала старые)") {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                // Проверяем, что столбец 9 существует
                if (a.size() <= 9 || b.size() <= 9) return false;

                QDateTime dateA = QDateTime::fromString(a[9], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[9], "dd.MM.yyyy HH:mm");

                // Если даты не парсятся, используем строковое сравнение
                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[9] < b[9];
                }

                return dateA < dateB;
            });
    } else {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                // Проверяем, что столбец 9 существует
                if (a.size() <= 9 || b.size() <= 9) return false;

                QDateTime dateA = QDateTime::fromString(a[9], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[9], "dd.MM.yyyy HH:mm");

                // Если даты не парсятся, используем строковое сравнение
                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[9] > b[9];
                }

                return dateA > dateB;
            });
    }

    // Обновляем таблицу
    ui->tableWidget_historySet->setRowCount(0);
    for (const QList<QString> &rowData : visibleRows) {
        int newRow = ui->tableWidget_historySet->rowCount();
        ui->tableWidget_historySet->insertRow(newRow);

        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);

            // Выравнивание для числовых полей и ID
            if (col == 0 || col == 3 || col == 7) { // set_id, set_quantity, user_id_change
                item->setTextAlignment(Qt::AlignCenter);
            }

            ui->tableWidget_historySet->setItem(newRow, col, item);
        }
    }

}


void MainWindow::on_btn_SetFilterStop_pressed()
{
    setHistoryToTable();
}


void MainWindow::on_btn_KasFilterStop_pressed()
{
    kasHistoryToTable();
}


void MainWindow::on_btn_MeshFilterStop_pressed()
{
    meshHistoryToTable();
}


void MainWindow::on_btn_KasFilterGo_pressed()
{
    // Получаем значения из фильтров
    QString dateFilter = ui->selec_KasFilterDate->currentText();
    QString setIdFilter = ui->select_kdd_set_2->currentText();
    QString kasIdFilter = ui->select_kss_kas_2->currentText();
    QString userChangeFilter = ui->select_KasFilterFIO->currentText();

    // Функция для извлечения ID
    auto extractId = [](const QString &input) -> QString {
        if (input == "-" || input.isEmpty()) return "-";
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatch match = re.match(input);
        return match.hasMatch() ? match.captured(1) : input;
    };

    QString setId = extractId(setIdFilter);
    QString kasId = extractId(kasIdFilter);
    QString userChangeId = extractId(userChangeFilter);

    // Создаем временную таблицу для сортировки
    QList<QList<QString>> visibleRows;

    // Собираем подходящие строки
    for (int row = 0; row < ui->tableWidget_historyKas->rowCount(); ++row) {
        bool showRow = true;

        // Проверяем фильтр по set_id (столбец 0 - ID комплекта)
        if (setId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyKas->item(row, 0);
            if (!item || extractId(item->text()) != setId) {
                showRow = false;
            }
        }

        // Проверяем фильтр по kas_id (столбец 1 - ID кассетницы)
        if (showRow && kasId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyKas->item(row, 1);
            if (!item || extractId(item->text()) != kasId) {
                showRow = false;
            }
        }

        // Проверяем фильтр по user_id_change (столбец 8 - ID изменившего)
        if (showRow && userChangeId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyKas->item(row, 8);
            if (!item || extractId(item->text()) != userChangeId) {
                showRow = false;
            }
        }

        // Сохраняем данные строки
        if (showRow) {
            QList<QString> rowData;
            for (int col = 0; col < ui->tableWidget_historyKas->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget_historyKas->item(row, col);
                rowData.append(item ? item->text() : "");
            }
            visibleRows.append(rowData);
        }
    }

    // Отладочная информация
    qDebug() << "Найдено строк для сортировки:" << visibleRows.size();

    // Сортируем по дате (столбец 10 - last_update_change)
    if (dateFilter == "Дата (сначала старые)") {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                // Проверяем, что столбец 10 существует
                if (a.size() <= 10 || b.size() <= 10) return false;

                QDateTime dateA = QDateTime::fromString(a[10], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[10], "dd.MM.yyyy HH:mm");

                // Если даты не парсятся, используем строковое сравнение
                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[10] < b[10];
                }

                return dateA < dateB;
            });
    } else {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                // Проверяем, что столбец 10 существует
                if (a.size() <= 10 || b.size() <= 10) return false;

                QDateTime dateA = QDateTime::fromString(a[10], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[10], "dd.MM.yyyy HH:mm");

                // Если даты не парсятся, используем строковое сравнение
                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[10] > b[10];
                }

                return dateA > dateB;
            });
    }

    // Обновляем таблицу
    ui->tableWidget_historyKas->setRowCount(0);
    for (const QList<QString> &rowData : visibleRows) {
        int newRow = ui->tableWidget_historyKas->rowCount();
        ui->tableWidget_historyKas->insertRow(newRow);

        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);

            // Выравнивание для числовых полей и ID
            if (col == 0 || col == 1 || col == 3 || col == 4 || col == 8) {
                // set_id, kas_id, kas_height, kas_width, user_id_change
                item->setTextAlignment(Qt::AlignCenter);
            }

            ui->tableWidget_historyKas->setItem(newRow, col, item);
        }
    }

}


void MainWindow::on_btn_MeshFilterGo_pressed()
{
    // Получаем значения из фильтров
    QString dateFilter = ui->selec_MeshFilterDate->currentText();
    QString setIdFilter = ui->select_kdd_set_3->currentText();
    QString kasIdFilter = ui->select_kdd_kas_3->currentText();
    QString meshIdFilter = ui->select_kdd_mesh_3->currentText();
    QString userChangeFilter = ui->select_MeshFilterFIO->currentText();

    // Функция для извлечения ID
    auto extractId = [](const QString &input) -> QString {
        if (input == "-" || input.isEmpty()) return "-";
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatch match = re.match(input);
        return match.hasMatch() ? match.captured(1) : input;
    };

    QString setId = extractId(setIdFilter);
    QString kasId = extractId(kasIdFilter);
    QString meshId = extractId(meshIdFilter);
    QString userChangeId = extractId(userChangeFilter);

    // Создаем временную таблицу для сортировки
    QList<QList<QString>> visibleRows;

    // Собираем подходящие строки
    for (int row = 0; row < ui->tableWidget_historyMesh->rowCount(); ++row) {
        bool showRow = true;

        // Проверяем фильтр по set_id (столбец 0 - ID комплекта)
        if (setId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyMesh->item(row, 0);
            if (!item || extractId(item->text()) != setId) {
                showRow = false;
            }
        }

        // Проверяем фильтр по kas_id (столбец 1 - ID кассетницы)
        if (showRow && kasId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyMesh->item(row, 1);
            if (!item || extractId(item->text()) != kasId) {
                showRow = false;
            }
        }

        // Проверяем фильтр по mesh_id (столбец 2 - ID ячейки)
        if (showRow && meshId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyMesh->item(row, 2);
            if (!item || extractId(item->text()) != meshId) {
                showRow = false;
            }
        }

        // Проверяем фильтр по user_id_change (столбец 8 - ID изменившего)
        if (showRow && userChangeId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyMesh->item(row, 8);
            if (!item || extractId(item->text()) != userChangeId) {
                showRow = false;
            }
        }

        // Сохраняем данные строки
        if (showRow) {
            QList<QString> rowData;
            for (int col = 0; col < ui->tableWidget_historyMesh->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget_historyMesh->item(row, col);
                rowData.append(item ? item->text() : "");
            }
            visibleRows.append(rowData);
        }
    }

    // Отладочная информация
    //qDebug() << "Найдено строк для сортировки:" << visibleRows.size();

    // Сортируем по дате (столбец 10 - last_update_change)
    if (dateFilter == "Дата (сначала старые)") {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                // Проверяем, что столбец 10 существует
                if (a.size() <= 10 || b.size() <= 10) return false;

                QDateTime dateA = QDateTime::fromString(a[10], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[10], "dd.MM.yyyy HH:mm");

                // Если даты не парсятся, используем строковое сравнение
                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[10] < b[10];
                }

                return dateA < dateB;
            });
    } else {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                // Проверяем, что столбец 10 существует
                if (a.size() <= 10 || b.size() <= 10) return false;

                QDateTime dateA = QDateTime::fromString(a[10], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[10], "dd.MM.yyyy HH:mm");

                // Если даты не парсятся, используем строковое сравнение
                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[10] > b[10];
                }

                return dateA > dateB;
            });
    }

    // Обновляем таблицу
    ui->tableWidget_historyMesh->setRowCount(0);
    for (const QList<QString> &rowData : visibleRows) {
        int newRow = ui->tableWidget_historyMesh->rowCount();
        ui->tableWidget_historyMesh->insertRow(newRow);

        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);

            // Выравнивание для числовых полей и ID
            if (col == 0 || col == 1 || col == 2 || col == 3 || col == 5 || col == 8) {
                // set_id, kas_id, mesh_id, user_id, mesh_status, user_id_change
                item->setTextAlignment(Qt::AlignCenter);
            }

            ui->tableWidget_historyMesh->setItem(newRow, col, item);
        }
    }

    qDebug() << "Фильтры mesh_history применены. Показано строк:" << visibleRows.size()
             << "Сортировка:" << dateFilter
             << "ID комплекта:" << setId
             << "ID кассетницы:" << kasId
             << "ID ячейки:" << meshId
             << "ID изменившего:" << userChangeId;
}

void MainWindow::on_pushButton_onSet_pressed()
{
    ui->tabWidget_kas->setCurrentIndex(1);
}


void MainWindow::on_pushButton_onKas_pressed()
{
    ui->tabWidget_kas->setCurrentIndex(0);
}



void MainWindow::updateListSetForHistory()
{
    updateSetComboBox(ui->select_kdd_set_1);

    // Обновление для первой группы (set + kas)
    updateComboBox(ui->select_kdd_set_2, "SELECT DISTINCT set_id FROM kas ORDER BY set_id");

    // Обновление для второй группы (set + kas + mesh)
    updateComboBox(ui->select_kdd_set_3, "SELECT DISTINCT set_id FROM mesh ORDER BY set_id");

    // Подключаем сигналы для первой группы
    disconnect(ui->select_kdd_set_2, nullptr, nullptr, nullptr);
    connect(ui->select_kdd_set_2, &QComboBox::currentTextChanged, this, [this]() {
        QString currentSet = ui->select_kdd_set_2->currentText();
        if (!currentSet.isEmpty()) {
            updateComboBox(ui->select_kss_kas_2,
                          "SELECT DISTINCT kas_id FROM kas WHERE set_id = ? ORDER BY kas_id",
                          currentSet);
        } else {
            ui->select_kss_kas_2->clear();
            ui->select_kss_kas_2->addItem("");
        }
    });

    // Подключаем сигналы для второй группы
    disconnect(ui->select_kdd_set_3, nullptr, nullptr, nullptr);
    disconnect(ui->select_kdd_kas_3, nullptr, nullptr, nullptr);

    connect(ui->select_kdd_set_3, &QComboBox::currentTextChanged, this, [this]() {
        QString currentSet = ui->select_kdd_set_3->currentText();
        if (!currentSet.isEmpty()) {
            updateComboBox(ui->select_kdd_kas_3,
                          "SELECT DISTINCT kas_id FROM mesh WHERE set_id = ? ORDER BY kas_id",
                          currentSet);
        } else {
            ui->select_kdd_kas_3->clear();
            ui->select_kdd_kas_3->addItem("");
        }

        // Обновляем mesh только если оба значения не пустые
        QString currentKas = ui->select_kdd_kas_3->currentText();
        if (!currentSet.isEmpty() && !currentKas.isEmpty()) {
            updateComboBox(ui->select_kdd_mesh_3,
                          "SELECT DISTINCT mesh_id FROM mesh WHERE set_id = ? AND kas_id = ? ORDER BY mesh_id",
                          currentSet, currentKas);
        } else {
            ui->select_kdd_mesh_3->clear();
            ui->select_kdd_mesh_3->addItem("");
        }
    });

    connect(ui->select_kdd_kas_3, &QComboBox::currentTextChanged, this, [this]() {
        QString currentSet = ui->select_kdd_set_3->currentText();
        QString currentKas = ui->select_kdd_kas_3->currentText();
        if (!currentSet.isEmpty() && !currentKas.isEmpty()) {
            updateComboBox(ui->select_kdd_mesh_3,
                          "SELECT DISTINCT mesh_id FROM mesh WHERE set_id = ? AND kas_id = ? ORDER BY mesh_id",
                          currentSet, currentKas);
        } else {
            ui->select_kdd_mesh_3->clear();
            ui->select_kdd_mesh_3->addItem("");
        }
    });

    // Первоначальное обновление зависимых списков
    QString currentSet2 = ui->select_kdd_set_2->currentText();
    if (!currentSet2.isEmpty()) {
        updateComboBox(ui->select_kss_kas_2,
                      "SELECT DISTINCT kas_id FROM kas WHERE set_id = ? ORDER BY kas_id",
                      currentSet2);
    }

    QString currentSet3 = ui->select_kdd_set_3->currentText();
    if (!currentSet3.isEmpty()) {
        updateComboBox(ui->select_kdd_kas_3,
                      "SELECT DISTINCT kas_id FROM mesh WHERE set_id = ? ORDER BY kas_id",
                      currentSet3);

        QString currentKas3 = ui->select_kdd_kas_3->currentText();
        if (!currentKas3.isEmpty()) {
            updateComboBox(ui->select_kdd_mesh_3,
                          "SELECT DISTINCT mesh_id FROM mesh WHERE set_id = ? AND kas_id = ? ORDER BY mesh_id",
                          currentSet3, currentKas3);
        }
    }
}

void MainWindow::updateComboBox(QComboBox* combo, const QString& queryText, const QString& bindValue1, const QString& bindValue2)
{
    combo->clear();
    combo->addItem("");

    QSqlQuery query;
    query.prepare(queryText);

    if (!bindValue1.isEmpty()) {
        query.addBindValue(bindValue1);
    }
    if (!bindValue2.isEmpty()) {
        query.addBindValue(bindValue2);
    }

    if (query.exec()) {
        while (query.next()) {
            combo->addItem(query.value(0).toString());
        }
    }
}

void MainWindow::on_pushButton_DoseCleanInput_pressed()
{
    ui->label_DoseId->setText("-");
    ui->label_DoseLastUpdate->setText("-");
    ui->comboBox_DoseUserId->setCurrentIndex(-1);
    ui->comboBox_DoseUsersDutyId->setCurrentIndex(-1);
    ui->comboBox_TypePpd->setCurrentIndex(-1);
    ui->spinBox_DoseNomerPdd->clear();//setValue(0);
    ui->doubleSpinBox_DoseDose->setValue(0.0);
    ui->spinBox_DoseRateMax->setValue(0);
    ui->timeEdit_Time->setTime(QTime(0, 0));
    ui->timeEdit_DoseTimeMax->setTime(QTime(0, 0));
    ui->timeEdit_DoseStartWork->setTime(QTime(0, 0));
    ui->timeEdit_FinishWork->setTime(QTime(0, 0));
    ui->lineEdit_DoseNote->clear();


    //кнопка
    QLayoutItem *item;
    while ((item = ui->verticalLayout_ForDoseBtn->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    QPushButton *pushButton_DoseSaveNew = new QPushButton("Сохранить новое показание", this);
    ui->verticalLayout_ForDoseBtn->addWidget(pushButton_DoseSaveNew);
    if (pushButton_DoseSaveNew){
        connect(pushButton_DoseSaveNew, &QPushButton::pressed, this, &MainWindow::on_pushButton_DoseSaveNew_pressed);//не забыть по h
    }

}


void MainWindow::on_pushButton_DoseSaveNew_pressed()
{
    QSqlQuery query;

    int DoseUserId = extractIdFromComboBox(ui->comboBox_DoseUserId->currentText());
    int DoseUsersDutyId = extractIdFromComboBox(ui->comboBox_DoseUsersDutyId->currentText());
    int TypePpd = extractIdFromComboBox(ui->comboBox_TypePpd->currentText());

    int DoseNomerPdd = ui->spinBox_DoseNomerPdd->text().toInt();
    double DoseDose = ui->doubleSpinBox_DoseDose->value();
    int DoseRateMax = ui->spinBox_DoseRateMax->value();
    QTime Time = ui->timeEdit_Time->time();
    QTime DoseTimeMax = ui->timeEdit_DoseTimeMax->time();
    QTime DoseStartWork = ui->timeEdit_DoseStartWork->time();
    QTime FinishWork = ui->timeEdit_FinishWork->time();
    QString DoseNote = ui->lineEdit_DoseNote->text();

    int dosePpdId = 0;
    query.prepare("SELECT MAX(dose_ppd_id) FROM dose_ppd");

    if (query.exec() && query.next()) {
        dosePpdId = query.value(0).toInt();
    } else {
        qWarning() << "Ошибка при получении maxDoseId: " << query.lastError().text();
    }
    dosePpdId++;

    QTimeZone moscowTimeZone("Europe/Moscow");
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    //QString DoseLastUpdate = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");*/

    QDateTime currentDate = QDateTime::currentDateTime();
    QDateTime timeMaxDateTime = QDateTime(currentDate.date(), DoseTimeMax);
    QDateTime startWorkDateTime = QDateTime(currentDate.date(), DoseStartWork);
    QDateTime finishWorkDateTime = QDateTime(currentDate.date(), FinishWork);

    query.prepare(R"(
        INSERT INTO dose_ppd (
            dose_ppd_id, user_id, users_duty_id, nomer_pdd, time,
            time_max, type_ppd, dose, rate_max, dose_ppd_note,
            start_work, finish_work, last_update
        ) VALUES (
            :dose_ppd_id, :user_id, :users_duty_id, :nomer_pdd, :time,
            :time_max, :type_ppd, :dose, :rate_max, :dose_ppd_note,
            :start_work, :finish_work, :last_update
        )
    )");

    query.bindValue(":dose_ppd_id", dosePpdId);
    query.bindValue(":user_id", DoseUserId);
    query.bindValue(":users_duty_id", DoseUsersDutyId);
    query.bindValue(":nomer_pdd", QString::number(DoseNomerPdd)); //int в string
    query.bindValue(":time", Time.minute() + Time.hour() * 60); // Преобразуем QTime в минуты
    query.bindValue(":time_max", timeMaxDateTime);
    query.bindValue(":type_ppd", TypePpd);
    query.bindValue(":dose", DoseDose);
    query.bindValue(":rate_max", DoseRateMax);
    query.bindValue(":dose_ppd_note", DoseNote);
    query.bindValue(":start_work", startWorkDateTime);
    query.bindValue(":finish_work", finishWorkDateTime);
    query.bindValue(":last_update", moscowDateTime);

    // Выполняем запрос
    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Данные успешно сохранены");//\nID: " + QString::number(dosePpdId));
    } else {
        QMessageBox::critical(this, "Ошибка063", "Не удалось сохранить данные: " + query.lastError().text());
        qDebug() << "Ошибка SQL:" << query.lastError().text();
        qDebug() << "Запрос:" << query.lastQuery();
    }

    //кнопка
    QLayoutItem *item;
    while ((item = ui->verticalLayout_ForDoseBtn->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    QPushButton *pushButton_DoseSaveChange = new QPushButton("Сохранить изменения дозы", this);
    ui->verticalLayout_ForDoseBtn->addWidget(pushButton_DoseSaveChange);
    if (pushButton_DoseSaveChange){
        connect(pushButton_DoseSaveChange, &QPushButton::pressed, this, &MainWindow::on_pushButton_DoseSaveChange_pressed);//не забыть по h
    }

    //история
    QString for_user_id = ui->label_userId->text();
    QString changing_user_id;
    QStringList parts1 = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString name_0 = parts1[0];
    QString name_1 = parts1[1];
    QString name_2 = parts1[2];
    query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
    query.bindValue(":name_0", name_0);
    query.bindValue(":name_1", name_1);
    query.bindValue(":name_2", name_2);
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка064", "Не удалось выполнить запрос: " + query.lastError().text());
        qDebug() << "Ошибка поиска id SQL:" << query.lastQuery() << query.lastError().text();
        return;
    }
    if (query.next()) {
        changing_user_id = query.value("user_id").toString();
    } else {
        QMessageBox::information(this, "Информация", "Пользователь не найден");
    }

    QString type_edit = "create";
    // Теперь добавляем запись в историю
    if (!addDosePpdHistory(
        dosePpdId,
        DoseUserId,
        DoseUsersDutyId,
        QString::number(DoseNomerPdd),
        Time.minute() + Time.hour() * 60,
        timeMaxDateTime,
        TypePpd,
        DoseDose,
        DoseRateMax,
        DoseNote,
        startWorkDateTime,
        finishWorkDateTime,
        moscowDateTime,
        changing_user_id,
        type_edit
    )) {
        throw std::runtime_error("Не удалось добавить запись в историю");
    }

    loadDosePpdToTable();
    loadDosePpdHistoryToTable();

    on_pushButton_DoseCleanInput_pressed();
}


void MainWindow::on_pushButton_DoseSaveChange_pressed()
{
    // Получаем данные из формы
    int dosePpdId = ui->label_DoseId->text().toInt();
    int DoseUserId = extractIdFromComboBox(ui->comboBox_DoseUserId->currentText());
    int DoseUsersDutyId = extractIdFromComboBox(ui->comboBox_DoseUsersDutyId->currentText());
    int TypePpd = extractIdFromComboBox(ui->comboBox_TypePpd->currentText());

    int DoseNomerPdd = ui->spinBox_DoseNomerPdd->text().toInt();
    double DoseDose = ui->doubleSpinBox_DoseDose->value();
    int DoseRateMax = ui->spinBox_DoseRateMax->value();
    QTime Time = ui->timeEdit_Time->time();
    QTime DoseTimeMax = ui->timeEdit_DoseTimeMax->time();
    QTime DoseStartWork = ui->timeEdit_DoseStartWork->time();
    QTime FinishWork = ui->timeEdit_FinishWork->time();
    QString DoseNote = ui->lineEdit_DoseNote->text();

    // Подготовка временных меток
    QDateTime currentDate = QDateTime::currentDateTime();
    int timeInMinutes = Time.hour() * 60 + Time.minute();

    QDateTime timeMaxDateTime = QDateTime(currentDate.date(), DoseTimeMax);
    QDateTime startWorkDateTime = QDateTime(currentDate.date(), DoseStartWork);
    QDateTime finishWorkDateTime = QDateTime(currentDate.date(), FinishWork);

    // Получаем текущее время для Moscow timezone
    QTimeZone moscowTimeZone("Europe/Moscow");
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);

    QSqlQuery query;

    // ОБНОВЛЕНИЕ существующей записи
    query.prepare(R"(
        UPDATE dose_ppd SET
            user_id = :user_id,
            users_duty_id = :users_duty_id,
            nomer_pdd = :nomer_pdd,
            time = :time,
            time_max = :time_max,
            type_ppd = :type_ppd,
            dose = :dose,
            rate_max = :rate_max,
            dose_ppd_note = :dose_ppd_note,
            start_work = :start_work,
            finish_work = :finish_work,
            last_update = :last_update
        WHERE dose_ppd_id = :dose_ppd_id
    )");

    query.bindValue(":dose_ppd_id", dosePpdId);
    query.bindValue(":user_id", DoseUserId);
    query.bindValue(":users_duty_id", DoseUsersDutyId);
    query.bindValue(":nomer_pdd", QString::number(DoseNomerPdd));
    query.bindValue(":time", timeInMinutes);
    query.bindValue(":time_max", timeMaxDateTime);
    query.bindValue(":type_ppd", TypePpd);
    query.bindValue(":dose", DoseDose);
    query.bindValue(":rate_max", DoseRateMax);
    query.bindValue(":dose_ppd_note", DoseNote);
    query.bindValue(":start_work", startWorkDateTime);
    query.bindValue(":finish_work", finishWorkDateTime);
    query.bindValue(":last_update", moscowDateTime);

    if (query.exec()) {
        QMessageBox::information(this, "Информация", "Данные дозиметра успешно изменены");

        // Получаем changing_user_id из ФИО
        QString for_user_id = ui->label_userId->text();
        QString changing_user_id;
        QStringList parts1 = for_user_id.split(' ', Qt::SkipEmptyParts);
        if (parts1.size() >= 3) {
            QString name_0 = parts1[0];
            QString name_1 = parts1[1];
            QString name_2 = parts1[2];

            QSqlQuery userQuery;
            userQuery.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
            userQuery.bindValue(":name_0", name_0);
            userQuery.bindValue(":name_1", name_1);
            userQuery.bindValue(":name_2", name_2);

            if (!userQuery.exec()) {
                QMessageBox::critical(this, "Ошибка065", "Не удалось выполнить запрос: " + userQuery.lastError().text());
                qDebug() << "Ошибка поиска id SQL:" << userQuery.lastQuery() << userQuery.lastError().text();
                return;
            }

            if (userQuery.next()) {
                changing_user_id = userQuery.value("user_id").toString();
            } else {
                QMessageBox::information(this, "Информация", "Пользователь не найден");
                changing_user_id = "1"; // fallback to admin
            }
        } else {
            changing_user_id = "1"; // fallback to admin
        }
        QString type_edit = "change";

        // Добавляем запись в историю
        QSqlQuery historyQuery;
        historyQuery.prepare(R"(
            INSERT INTO dose_ppd_history (
                dose_ppd_id, user_id, users_duty_id, nomer_pdd, time,
                time_max, type_ppd, dose, rate_max, dose_ppd_note,
                start_work, finish_work, last_update, user_id_change, last_update_change, type_edit
            ) VALUES (
                :dose_ppd_id, :user_id, :users_duty_id, :nomer_pdd, :time,
                :time_max, :type_ppd, :dose, :rate_max, :dose_ppd_note,
                :start_work, :finish_work, :last_update, :user_id_change, :last_update_change, :type_edit
            )
        )");

        historyQuery.bindValue(":dose_ppd_id", dosePpdId);
        historyQuery.bindValue(":user_id", DoseUserId);
        historyQuery.bindValue(":users_duty_id", DoseUsersDutyId);
        historyQuery.bindValue(":nomer_pdd", QString::number(DoseNomerPdd));
        historyQuery.bindValue(":time", timeInMinutes);
        historyQuery.bindValue(":time_max", timeMaxDateTime);
        historyQuery.bindValue(":type_ppd", TypePpd);
        historyQuery.bindValue(":dose", DoseDose);
        historyQuery.bindValue(":rate_max", DoseRateMax);
        historyQuery.bindValue(":dose_ppd_note", DoseNote);
        historyQuery.bindValue(":start_work", startWorkDateTime);
        historyQuery.bindValue(":finish_work", finishWorkDateTime);
        historyQuery.bindValue(":last_update", moscowDateTime);
        historyQuery.bindValue(":user_id_change", changing_user_id);
        historyQuery.bindValue(":last_update_change", moscowDateTime);
        historyQuery.bindValue(":type_edit", type_edit);

        if (!historyQuery.exec()) {
            QMessageBox::warning(this, "Предупреждение", "Не удалось добавить запись в историю: " + historyQuery.lastError().text());
        }


        // Обновляем таблицы
        loadDosePpdToTable();
        loadDosePpdHistoryToTable();

        // Обновляем кнопку
        QLayoutItem *item;
        while ((item = ui->verticalLayout_ForDoseBtn->takeAt(0))) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }

        QPushButton *pushButton_DoseSaveNew = new QPushButton("Сохранить новое показание", this);
        ui->verticalLayout_ForDoseBtn->addWidget(pushButton_DoseSaveNew);
        connect(pushButton_DoseSaveNew, &QPushButton::pressed, this, &MainWindow::on_pushButton_DoseSaveNew_pressed);

    } else {
        qDebug() << "Ошибка SQL:" << query.lastError().text();
        qDebug() << "Запрос:" << query.lastQuery();
        QMessageBox::critical(this, "Ошибка066", "Не удалось обновить данные дозиметра: " + query.lastError().text());
    }

    on_pushButton_DoseCleanInput_pressed();
}


bool MainWindow::addDosePpdHistory(
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
    const QString type_edit)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO dose_ppd_history (
            dose_ppd_id, user_id, users_duty_id, nomer_pdd, time,
            time_max, type_ppd, dose, rate_max, dose_ppd_note,
            start_work, finish_work, last_update, user_id_change, last_update_change, type_edit
        ) VALUES (
            :dose_ppd_id, :user_id, :users_duty_id, :nomer_pdd, :time,
            :time_max, :type_ppd, :dose, :rate_max, :dose_ppd_note,
            :start_work, :finish_work, :last_update, :user_id_change, :last_update_change, :type_edit
        )
    )");

    // Привязываем значения
    query.bindValue(":dose_ppd_id", dosePpdId);
    query.bindValue(":user_id", userId);
    query.bindValue(":users_duty_id", usersDutyId);
    query.bindValue(":nomer_pdd", nomerPdd);
    query.bindValue(":time", time);
    query.bindValue(":time_max", timeMax);
    query.bindValue(":type_ppd", typePpd);
    query.bindValue(":dose", dose);
    query.bindValue(":rate_max", rateMax);
    query.bindValue(":dose_ppd_note", dosePpdNote);
    query.bindValue(":start_work", startWork);
    query.bindValue(":finish_work", finishWork);
    query.bindValue(":last_update", lastUpdate);
    query.bindValue(":user_id_change", userIdChange);
    query.bindValue(":last_update_change", QDateTime::currentDateTime());
    query.bindValue(":type_edit", type_edit);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления в dose_ppd_history:" << query.lastError().text();
        return false;
    }

    qDebug() << "Запись добавлена в dose_ppd_history, ID:" << dosePpdId;
    return true;
}

void MainWindow::loadDosePpdToTable()
{
    // Очищаем таблицу перед загрузкой новых данных
    ui->tableWidget_Dose->clear();
    ui->tableWidget_Dose->setRowCount(0);

    // Устанавливаем количество столбцов
    ui->tableWidget_Dose->setColumnCount(13);

    // Устанавливаем заголовки столбцов
    QStringList headers;
    headers << "ID" << "Пользователь" << "ID наряда" << "Номер дозиметра"
            << "Время (мин)" << "Время макс." << "Тип дозиметра" << "Доза (мЗв)"
            << "Мощность макс." << "Примечание" << "Начало работы"
            << "Окончание работы" << "Последнее обновление";

    ui->tableWidget_Dose->setHorizontalHeaderLabels(headers);

    // SQL запрос для получения данных
    QSqlQuery query;
    QString sql = R"(
        SELECT
            dp.dose_ppd_id,
            u.name_0 || ' ' || u.name_1 || ' ' || u.name_2 as user_name,
            dp.users_duty_id,
            dp.nomer_pdd,
            dp.time,
            dp.time_max,
            dp.type_ppd,
            dp.dose,
            dp.rate_max,
            dp.dose_ppd_note,
            dp.start_work,
            dp.finish_work,
            dp.last_update
        FROM dose_ppd dp
        LEFT JOIN users u ON dp.user_id = u.user_id
        ORDER BY dp.last_update DESC
        LIMIT 500
    )";

    if (!query.exec(sql)) {
        qDebug() << "Ошибка выполнения запроса dose_ppd:" << query.lastError().text();
        return;
    }

    // Заполняем таблицу данными
    while (query.next()) {
        int row = ui->tableWidget_Dose->rowCount();
        ui->tableWidget_Dose->insertRow(row);

        // Заполняем ячейки данными
        for (int col = 0; col < query.record().count(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();
            QVariant value = query.value(col);

            // Обработка разных типов данных
            if (col == 0 || col == 2) { // ID поля
                item->setText(value.toString());
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col == 4) { // Время в минутах
                item->setText(value.toString());
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col == 5 || col == 10 || col == 11 || col == 12) { // Дата и время
                QDateTime dateTime = value.toDateTime();
                item->setText(dateTime.toString("dd.MM.yyyy HH:mm"));
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col == 6) { // Тип дозиметра
                QString typeText;
                switch (value.toInt()) {
                    case 0: typeText = "Альфа"; break;
                    case 1: typeText = "Гамма"; break;
                    case 2: typeText = "Нейтроны"; break;
                    case 3: typeText = "Бета"; break;
                    default: typeText = "Неизвестно"; break;
                }
                item->setText(typeText);
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col == 7 || col == 8) { // Числовые значения (доза и мощность)
                if (value.isNull()) {
                    item->setText("0.00");
                } else {
                    item->setText(QString::number(value.toDouble(), 'f', 2));
                }
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col == 9) { // Примечание
                item->setText(value.toString().isEmpty() ? "—" : value.toString());
            }
            else {
                // Все остальные текстовые поля
                item->setText(value.toString());
            }

            // Запрещаем редактирование ячеек
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget_Dose->setItem(row, col, item);
        }
    }

    // Настраиваем внешний вид таблицы
    ui->tableWidget_Dose->resizeColumnsToContents();
    ui->tableWidget_Dose->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_Dose->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_Dose->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_Dose->setEditTriggers(QAbstractItemView::NoEditTriggers);


    //выпадающий список для пользователей
    setupSearchUserComboBox(ui->comboBox_DoseUserId);



    // Заполняем комбобокс только users_duty_ID
    setupSearchUsersDutyComboBox(ui->comboBox_DoseUsersDutyId);

}

void MainWindow::setupSearchUsersDutyComboBox(QComboBox* comboBox)
{
    if (!comboBox) return;

    // Очистка предыдущих соединений
    comboBox->disconnect();
    if (comboBox->lineEdit()) {
        comboBox->lineEdit()->disconnect();
    }

    // Базовая настройка ComboBox
    comboBox->clear();
    comboBox->setEditable(true);
    comboBox->setInsertPolicy(QComboBox::NoInsert);
    comboBox->setDuplicatesEnabled(false);

    // Статические элементы
    comboBox->addItem("Не выбран", QVariant());
    comboBox->addItem("", QVariant());
    comboBox->setCurrentIndex(1);

    // Загрузка данных из БД
    QSqlQuery query;
    if (!query.exec("SELECT users_duty_id FROM users_duty")) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить список users_duty: " + query.lastError().text());
        return;
    }

    // Подсчет количества записей и заполнение данных
    int rowCount = 0;
    QList<UsersDutyDataSearch> usersDutyData;

    while (query.next()) {
        QString users_duty_id = query.value("users_duty_id").toString();

        UsersDutyDataSearch usersDuty;
        usersDuty.id = users_duty_id;
        usersDuty.displayText = users_duty_id;
        usersDuty.searchText = users_duty_id.toLower();

        usersDutyData.append(usersDuty);
        comboBox->addItem(usersDuty.displayText, usersDuty.id);

        rowCount++;
    }

    //qDebug() << "Количество записей users_duty:" << rowCount;

    // Настройка поля поиска
    QLineEdit* searchEdit = comboBox->lineEdit();
    searchEdit->setPlaceholderText("Введите ID users_duty...");
    searchEdit->setClearButtonEnabled(true);

    // Оптимизированная фильтрация
    QTimer* searchTimer = new QTimer(comboBox);
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(300);

    // Сохраняем данные в свойство comboBox для доступа
    comboBox->setProperty("usersDutyData", QVariant::fromValue(usersDutyData));

    auto filterUsersDuty = [comboBox]() {
        QList<UsersDutyDataSearch> usersDutyData = comboBox->property("usersDutyData").value<QList<UsersDutyDataSearch>>();
        QString text = comboBox->lineEdit()->text().toLower();

        QSignalBlocker blocker(comboBox);
        QString currentText = comboBox->lineEdit()->text();
        int currentPos = comboBox->lineEdit()->cursorPosition();

        // Очищаем список (кроме первых 2 элементов)
        while (comboBox->count() > 2) {
            comboBox->removeItem(comboBox->count() - 1);
        }

        // Добавляем отфильтрованные элементы
        if (!text.isEmpty()) {
            for (const auto& usersDuty : usersDutyData) {
                if (usersDuty.searchText.contains(text)) {
                    comboBox->addItem(usersDuty.displayText, usersDuty.id);
                }
            }
        }

        // Восстанавливаем состояние
        comboBox->lineEdit()->setText(currentText);
        comboBox->lineEdit()->setCursorPosition(currentPos);

        if (!text.isEmpty() && comboBox->count() > 2) {
            comboBox->showPopup();
        }
    };

    connect(searchEdit, &QLineEdit::textChanged, searchTimer, [searchTimer]() {
        searchTimer->start();
    });

    connect(searchTimer, &QTimer::timeout, comboBox, filterUsersDuty);

    // Обработка выбора элемента
    connect(comboBox, QOverload<int>::of(&QComboBox::activated), comboBox, [comboBox](int index) {
        if (index == 0) {
            comboBox->setCurrentIndex(1);
            comboBox->lineEdit()->clear();
        }
        else if (index >= 2) {
            QString selectedText = comboBox->itemText(index);
            comboBox->lineEdit()->setText(selectedText);
            comboBox->hidePopup();

            QString selectedId = comboBox->itemData(index).toString();
            qDebug() << "Выбран ID users_duty:" << selectedId;
        }
    });

    // Показ полного списка при фокусе
    connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [comboBox]() {
        QTimer::singleShot(100, [comboBox]() {
            if (comboBox->hasFocus()) {
                comboBox->showPopup();
            }
        });
    });

    // Принудительное обновление через короткую задержку
    QTimer::singleShot(500, [comboBox, usersDutyData]() {
        QSignalBlocker blocker(comboBox);

        // Очищаем и перезаполняем
        while (comboBox->count() > 2) {
            comboBox->removeItem(comboBox->count() - 1);
        }

        for (const auto& usersDuty : usersDutyData) {
            comboBox->addItem(usersDuty.displayText, usersDuty.id);
        }

        //qDebug() << "Принудительное обновление comboBox users_duty, элементов:" << comboBox->count();
    });

}

void MainWindow::loadDosePpdHistoryToTable()
{// Очищаем таблицу перед загрузкой новых данных
    ui->tableWidget_DoseHistory->clear();
    ui->tableWidget_DoseHistory->setRowCount(0);

    // Устанавливаем количество столбцов (15 полей из таблицы)
    ui->tableWidget_DoseHistory->setColumnCount(16);

    // Устанавливаем заголовки столбцов
    QStringList headers;
    headers << "ID дозиметра" << "ID пользователя" << "ID наряда" << "Номер дозиметра"
            << "Время (мин)" << "Время макс." << "Тип дозиметра" << "Доза (мЗв)"
            << "Мощность макс." << "Примечание" << "Начало работы"
            << "Окончание работы" << "Дата обновления" << "ID изменившего"
            << "Дата изменения" << "Тип изменения";

    ui->tableWidget_DoseHistory->setHorizontalHeaderLabels(headers);

    // SQL запрос для получения данных истории
    QSqlQuery query;
    QString sql = R"(
        SELECT
            dh.dose_ppd_id,
            dh.user_id,
            dh.users_duty_id,
            dh.nomer_pdd,
            dh.time,
            dh.time_max,
            dh.type_ppd,
            dh.dose,
            dh.rate_max,
            dh.dose_ppd_note,
            dh.start_work,
            dh.finish_work,
            dh.last_update,
            dh.user_id_change,
            dh.last_update_change,
            dh.type_edit
        FROM dose_ppd_history dh
        ORDER BY dh.last_update_change DESC
        LIMIT 500
    )";

    if (!query.exec(sql)) {
        qDebug() << "Ошибка выполнения запроса dose_ppd_history:" << query.lastError().text();
        return;
    }

    // Заполняем таблицу данными
    while (query.next()) {
        int row = ui->tableWidget_DoseHistory->rowCount();
        ui->tableWidget_DoseHistory->insertRow(row);

        // Заполняем ячейки данными
        for (int col = 0; col < query.record().count(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();
            QVariant value = query.value(col);

            // Обработка разных типов данных
            if (col == 0 || col == 1 || col == 2 || col == 13) { // ID поля
                item->setText(value.toString());
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col == 4) { // Время в минутах
                item->setText(value.toString());
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col == 5 || col == 10 || col == 11 || col == 12 || col == 14) { // Дата и время
                QDateTime dateTime = value.toDateTime();
                item->setText(dateTime.toString("dd.MM.yyyy HH:mm:ss"));
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col == 6) { // Тип дозиметра
                QString typeText;
                switch (value.toInt()) {
                    case 0: typeText = "α - Альфа"; break;
                    case 1: typeText = "γ - Гамма"; break;
                    case 2: typeText = "n - Нейтроны"; break;
                    case 3: typeText = "β - Бета"; break;
                    default: typeText = "Неизвестно (" + value.toString() + ")"; break;
                }
                item->setText(typeText);
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col == 7 || col == 8) { // Числовые значения (доза и мощность)
                if (value.isNull() || value.toDouble() == 0) {
                    item->setText("0.00");
                } else {
                    item->setText(QString::number(value.toDouble(), 'f', 4));
                }
                item->setTextAlignment(Qt::AlignCenter);
            }
            else if (col == 9) { // Примечание
                QString note = value.toString();
                item->setText(note.isEmpty() ? "—" : note);
                if (note.length() > 20) {
                    item->setToolTip(note); // Показываем полный текст при наведении
                }
            }
            else if (col == 3) { // Номер дозиметра
                item->setText(value.toString());
                item->setTextAlignment(Qt::AlignCenter);
            }
            else {
                // Все остальные текстовые поля
                item->setText(value.toString());
            }

            // Запрещаем редактирование ячеек
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget_DoseHistory->setItem(row, col, item);
        }
    }

    // Настраиваем внешний вид таблицы
    ui->tableWidget_DoseHistory->resizeColumnsToContents();
    ui->tableWidget_DoseHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_DoseHistory->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_DoseHistory->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_DoseHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Заполняем комбобокс ID дозиметров
    ui->select_DoseHistoryFilter_1->clear();
    ui->select_DoseHistoryFilter_1->addItem("-");
    // SQL запрос для получения всех ID дозиметров
    sql = "SELECT DISTINCT dose_ppd_id FROM dose_ppd ORDER BY dose_ppd_id";
    if (!query.exec(sql)) {
        qDebug() << "Ошибка получения ID дозиметров:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int doseId = query.value(0).toInt();
        ui->select_DoseHistoryFilter_1->addItem(QString::number(doseId), doseId);
    }


    //выпадающий список для пользователей
    setupSearchUserComboBox(ui->select_DoseHistoryFilter_1);

    // ID дозиметров из dose_ppd
    ui->select_DoseHistoryFilter_2->clear();
    ui->select_DoseHistoryFilter_2->addItem("-");
    sql = "SELECT DISTINCT dose_ppd_id FROM dose_ppd ORDER BY dose_ppd_id DESC";
                if (!query.exec(sql)) {
        qDebug() << "Ошибка получения ID дозиметров для фильтра 2:" << query.lastError().text();
        return;
    }
            while (query.next()) {
        int doseId = query.value(0).toInt();
        ui->select_DoseHistoryFilter_2->addItem(QString::number(doseId), doseId);
    }

}


int MainWindow::extractIdFromComboBox(const QString& text) // Функция для извлечения ID из строки формата "id - текст"
{
    if (text.isEmpty()) return -1;

    // Ищем разделитель " - "
    int separatorPos = text.indexOf(" - ");
    if (separatorPos > 0) {
        // Берем часть до разделителя и преобразуем в int
        QString idStr = text.left(separatorPos).trimmed();
        bool ok;
        int id = idStr.toInt(&ok);
        return ok ? id : -1;
    }

    // Если разделителя нет, пробуем преобразовать всю строку
    bool ok;
    int id = text.toInt(&ok);
    return ok ? id : -1;
}

void MainWindow::on_pushButton_DoseHistoryFile_pressed()
{

    // Получаем текущую дату для названия файла
    QString currentDate = QDate::currentDate().toString("dd.MM.yyyy");
    QString fileName = QString("dose_ppd-%1.csv").arg(currentDate);

    // Запрашиваем путь для сохранения
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить файл",
                                                   QDir::homePath() + "/" + fileName,
                                                   "CSV Files (*.csv)");

    if (filePath.isEmpty()) {
        return; // Пользователь отменил диалог
    }

    // Открываем файл для записи
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка069", "Не удалось создать файл: " + file.errorString());
        return;
    }

    QTextStream out(&file);

    // Установка кодировки в зависимости от версии Qt
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8"); // Qt 5
    #else
    out.setEncoding(QStringConverter::Utf8); // Qt 6
    #endif

    // Записываем заголовки столбцов
    for (int col = 0; col < ui->tableWidget_DoseHistory->columnCount(); ++col) {
        QTableWidgetItem *headerItem = ui->tableWidget_DoseHistory->horizontalHeaderItem(col);
        if (headerItem) {
            out << "\"" << headerItem->text() << "\"";
        } else {
            out << "\"Column " << col + 1 << "\"";
        }
        if (col < ui->tableWidget_DoseHistory->columnCount() - 1) {
            out << ";"; // Разделитель столбцов
        }
    }
    out << "\n";

    // Записываем данные
    for (int row = 0; row < ui->tableWidget_DoseHistory->rowCount(); ++row) {
        for (int col = 0; col < ui->tableWidget_DoseHistory->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget_DoseHistory->item(row, col);
            if (item && !item->text().isEmpty()) {
                // Экранируем кавычки и добавляем текст
                QString text = item->text();
                text.replace("\"", "\"\""); // Экранирование кавычек
                out << "\"" << text << "\"";
            } else {
                out << "\"\""; // Пустая ячейка
            }
            if (col < ui->tableWidget_DoseHistory->columnCount() - 1) {
                out << ";";
            }
        }
        out << "\n";
    }

    file.close();
    QMessageBox::information(this, "Успех", QString("Данные сохранены в файл:\n%1").arg(filePath));
}


void MainWindow::on_btn_DoseHistoryFilterStop_pressed()
{
    loadDosePpdHistoryToTable();
}


void MainWindow::on_btn_DoseHistoryFilterGo_pressed()
{
    // Получаем значения из фильтров
    QString dateFilter = ui->selec_DoseHistoryFilterDate->currentText();
    QString userChangeFilter = ui->select_DoseHistoryFilter_1->currentText();
    QString doseIdFilter = ui->select_DoseHistoryFilter_2->currentText();

    // Функция для извлечения ID
    auto extractId = [](const QString &input) -> QString {
        if (input == "-" || input.isEmpty()) return "-";
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatch match = re.match(input);
        return match.hasMatch() ? match.captured(1) : input;
    };

    QString doseId = extractId(doseIdFilter);
    QString userChangeId = extractId(userChangeFilter);

    // Создаем временную таблицу для сортировки
    QList<QList<QString>> visibleRows;

    // Собираем подходящие строки
    for (int row = 0; row < ui->tableWidget_DoseHistory->rowCount(); ++row) {
        bool showRow = true;

        // Проверяем фильтр по dose_ppd_id (столбец 0 - ID дозиметра)
        if (doseId != "-") {
            QTableWidgetItem *item = ui->tableWidget_DoseHistory->item(row, 0);
            if (!item || extractId(item->text()) != doseId) {
                showRow = false;
            }
        }

        // Проверяем фильтр по user_id_change (столбец 13 - ID изменившего)
        if (showRow && userChangeId != "-") {
            QTableWidgetItem *item = ui->tableWidget_DoseHistory->item(row, 13);
            if (!item || extractId(item->text()) != userChangeId) {
                showRow = false;
            }
        }

        // Сохраняем данные строки
        if (showRow) {
            QList<QString> rowData;
            for (int col = 0; col < ui->tableWidget_DoseHistory->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget_DoseHistory->item(row, col);
                rowData.append(item ? item->text() : "");
            }
            visibleRows.append(rowData);
        }
    }

    // Отладочная информация
    qDebug() << "Найдено строк для сортировки:" << visibleRows.size();

    // Сортируем по дате изменения (столбец 14 - last_update_change)
    if (dateFilter == "Дата (сначала старые)") {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                // Проверяем, что столбец 14 существует
                if (a.size() <= 14 || b.size() <= 14) return false;

                QDateTime dateA = QDateTime::fromString(a[14], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[14], "dd.MM.yyyy HH:mm");

                // Если даты не парсятся, используем строковое сравнение
                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[14] < b[14];
                }

                return dateA < dateB;
            });
    } else {
        // По умолчанию - сначала новые
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                // Проверяем, что столбец 14 существует
                if (a.size() <= 14 || b.size() <= 14) return false;

                QDateTime dateA = QDateTime::fromString(a[14], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[14], "dd.MM.yyyy HH:mm");

                // Если даты не парсятся, используем строковое сравнение
                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[14] > b[14];
                }

                return dateA > dateB;
            });
    }

    // Обновляем таблицу
    ui->tableWidget_DoseHistory->setRowCount(0);
    for (const QList<QString> &rowData : visibleRows) {
        int newRow = ui->tableWidget_DoseHistory->rowCount();
        ui->tableWidget_DoseHistory->insertRow(newRow);

        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);

            // Выравнивание для числовых полей и ID
            if (col == 0 || col == 1 || col == 2 || col == 4 || col == 6 ||
                col == 7 || col == 8 || col == 13) {
                item->setTextAlignment(Qt::AlignCenter);
            }

            ui->tableWidget_DoseHistory->setItem(newRow, col, item);
        }
    }

}

void MainWindow::setComboBoxValue(QComboBox* comboBox, int value)
{
    if (!comboBox) return;

    for (int i = 0; i < comboBox->count(); ++i) {
        // Проверяем по userData (рекомендуемый способ)
        if (comboBox->itemData(i).toInt() == value) {
            comboBox->setCurrentIndex(i);
            return;
        }

        // Дополнительная проверка по тексту (если userData не установлен)
        QString text = comboBox->itemText(i);
        int separatorPos = text.indexOf(" - ");
        if (separatorPos > 0) {
            int id = text.left(separatorPos).toInt();
            if (id == value) {
                comboBox->setCurrentIndex(i);
                return;
            }
        }
    }

    // Если не нашли, сбрасываем выбор
    comboBox->setCurrentIndex(-1);
}


void MainWindow::on_tableWidget_Dose_cellPressed(int row, int column)
{
    Q_UNUSED(column);

    // Получаем данные из строки таблицы
    QTableWidgetItem* idItem = ui->tableWidget_Dose->item(row, 0); // ID
    QTableWidgetItem* userItem = ui->tableWidget_Dose->item(row, 1); // Пользователь
    QTableWidgetItem* dutyIdItem = ui->tableWidget_Dose->item(row, 2); // ID наряда
    QTableWidgetItem* nomerItem = ui->tableWidget_Dose->item(row, 3); // Номер дозиметра
    QTableWidgetItem* timeItem = ui->tableWidget_Dose->item(row, 4); // Время (мин)
    QTableWidgetItem* timeMaxItem = ui->tableWidget_Dose->item(row, 5); // Время макс.
    QTableWidgetItem* typeItem = ui->tableWidget_Dose->item(row, 6); // Тип дозиметра
    QTableWidgetItem* doseItem = ui->tableWidget_Dose->item(row, 7); // Доза
    QTableWidgetItem* rateItem = ui->tableWidget_Dose->item(row, 8); // Мощность макс.
    QTableWidgetItem* noteItem = ui->tableWidget_Dose->item(row, 9); // Примечание
    QTableWidgetItem* startWorkItem = ui->tableWidget_Dose->item(row, 10); // Начало работы
    QTableWidgetItem* finishWorkItem = ui->tableWidget_Dose->item(row, 11); // Окончание работы
    QTableWidgetItem* lastUpdateItem = ui->tableWidget_Dose->item(row, 12); // Последнее обновление

    // Устанавливаем ID
    if (idItem) {
        ui->label_DoseId->setText(idItem->text());
    }

    // Устанавливаем пользователя (извлекаем ID из текста "ID - ФИО")
    //comboBox_DoseUserId
    // Устанавливаем пользователя (принудительно в формате "ID - ФИО")
    if (userItem) {
        QString userText = userItem->text();

        // Если текст уже в формате "ID - ФИО", используем как есть
        int separatorPos = userText.indexOf(" - ");
        if (separatorPos > 0) {
            // Извлекаем ID и устанавливаем комбобокс
            int userId = userText.left(separatorPos).toInt();
            setComboBoxValue(ui->comboBox_DoseUserId, userId);
        } else {
            // Если формат другой, ищем пользователя по ФИО в комбобоксе
            for (int i = 0; i < ui->comboBox_DoseUserId->count(); ++i) {
                QString comboText = ui->comboBox_DoseUserId->itemText(i);
                if (comboText.contains(userText)) {
                    ui->comboBox_DoseUserId->setCurrentIndex(i);
                    break;
                }
            }
        }
    }

    // Устанавливаем наряд (ID из столбца 2)
    if (dutyIdItem) {
        int usersDutyId = dutyIdItem->text().toInt();
        setComboBoxValue(ui->comboBox_DoseUsersDutyId, usersDutyId);
    }

    // Устанавливаем номер дозиметра
    if (nomerItem) {
        ui->spinBox_DoseNomerPdd->setText(nomerItem->text());//setValue(nomerItem->text().toInt());
    }

    // Устанавливаем время (преобразуем минуты в QTime)
    if (timeItem) {
        int timeInMinutes = timeItem->text().toInt();
        QTime time = QTime(timeInMinutes / 60, timeInMinutes % 60);
        ui->timeEdit_Time->setTime(time);
    }

    // Устанавливаем время максимума
    if (timeMaxItem) {
        QDateTime timeMax = QDateTime::fromString(timeMaxItem->text(), "dd.MM.yyyy HH:mm");
        ui->timeEdit_DoseTimeMax->setTime(timeMax.time());
    }

    // Устанавливаем тип дозиметра (извлекаем ID из текста или по индексу)
    if (typeItem) {
        QString typeText = typeItem->text();
        int typePpd = -1;

        if (typeText == "Альфа") typePpd = 0;
        else if (typeText == "Гамма") typePpd = 1;
        else if (typeText == "Нейтроны") typePpd = 2;
        else if (typeText == "Бета") typePpd = 3;

        setComboBoxValue(ui->comboBox_TypePpd, typePpd);
    }

    // Устанавливаем дозу
    if (doseItem) {
        ui->doubleSpinBox_DoseDose->setValue(doseItem->text().toDouble());
    }

    // Устанавливаем максимальную мощность
    if (rateItem) {
        ui->spinBox_DoseRateMax->setValue(rateItem->text().toInt());
    }

    // Устанавливаем примечание
    if (noteItem) {
        ui->lineEdit_DoseNote->setText(noteItem->text());
    }

    // Устанавливаем время начала работы
    if (startWorkItem) {
        QDateTime startWork = QDateTime::fromString(startWorkItem->text(), "dd.MM.yyyy HH:mm");
        ui->timeEdit_DoseStartWork->setTime(startWork.time());
    }

    // Устанавливаем время окончания работы
    if (finishWorkItem) {
        QDateTime finishWork = QDateTime::fromString(finishWorkItem->text(), "dd.MM.yyyy HH:mm");
        ui->timeEdit_FinishWork->setTime(finishWork.time());
    }

    // Устанавливаем дату последнего обновления
    if (lastUpdateItem) {
        ui->label_DoseLastUpdate->setText(lastUpdateItem->text());
    }


    //кнопка
    QLayoutItem *item;
    while ((item = ui->verticalLayout_ForDoseBtn->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    QPushButton *pushButton_DoseSaveChange = new QPushButton("Сохранить изменения дозы", this);
    ui->verticalLayout_ForDoseBtn->addWidget(pushButton_DoseSaveChange);
    if (pushButton_DoseSaveChange){
        connect(pushButton_DoseSaveChange, &QPushButton::pressed, this, &MainWindow::on_pushButton_DoseSaveChange_pressed);//не забыть по h
    }
}

void MainWindow::checkRole(){
    QSqlQuery query;

    QString for_user_id = ui->label_userId->text();
    QStringList parts1 = for_user_id.split(' ', Qt::SkipEmptyParts);
    if (parts1.size() < 3) {
        //qDebug() << "Неверный формат ФИО";
        return;
    }

    query.prepare("SELECT role FROM users WHERE name_0 = ? AND name_1 = ? AND name_2 = ?");
    query.addBindValue(parts1[0]);
    query.addBindValue(parts1[1]);
    query.addBindValue(parts1[2]);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка запроса", query.lastError().text());
        return;
    }

    if (query.next()) {
        QString role = query.value(0).toString();

        if (role == "Оператор") {
            qDebug() << "Оператор - скрываем кнопки";


            ui->btn_userClear->hide();
            for (int i = 0; i < ui->layoutForBtn->count(); ++i) {
                QLayoutItem* item = ui->layoutForBtn->itemAt(i);
                if (item && item->widget()) {
                    item->widget()->hide();
                }
            }

            ui->btn_createNewDuty->hide();
            for (int i = 0; i < ui->layoutForBtnDuty->count(); ++i) {
                QLayoutItem* item = ui->layoutForBtnDuty->itemAt(i);
                if (item && item->widget()) {
                    item->widget()->hide();
                }
            }

            ui->pushButton_DoseCleanInput->hide();
            for (int i = 0; i < ui->verticalLayout_ForDoseBtn->count(); ++i) {
                QLayoutItem* item = ui->verticalLayout_ForDoseBtn->itemAt(i);
                if (item && item->widget()) {
                    item->widget()->hide();
                }
            }

            ui->btnSetCreateNew->hide();
            for (int i = 0; i < ui->gridLayout_18->count(); ++i) {
                QLayoutItem* item = ui->gridLayout_18->itemAt(i);
                if (item && item->widget()) {
                    item->widget()->hide();
                }
            }

            ui->btnKasCreateNew ->hide();
            for (int i = 0; i < ui->gridLayout_21->count(); ++i) {
                QLayoutItem* item = ui->gridLayout_21->itemAt(i);
                if (item && item->widget()) {
                    item->widget()->hide();
                }
            }

            for (int i = 0; i < ui->gridLayout_28->count(); ++i) {
                QLayoutItem* item = ui->gridLayout_28->itemAt(i);
                if (item && item->widget()) {
                    item->widget()->hide();
                }
            }


        } else {
            /*qDebug() << "Администратор - показываем кнопки";
            ui->btnCreateNewUser->show();*/
        }
    } else {
        qDebug() << "Пользователь не найден";
        ui->btnCreateNewUser->hide(); // или show() в зависимости от логики
    }
}

void MainWindow::on_pushButton_Back_pressed()
{
    hide();
    welcomeWindow = new Welcome(this);
    welcomeWindow->show();

}


void MainWindow::on_pushButton_ppdFilterClean_pressed()
{
    loadDosePpdToTable();
}


void MainWindow::setupPpdSearch()
{
    // Создаем кнопку очистки
    clearPpdSearchButton = new QPushButton("×", this);
    clearPpdSearchButton->setFixedSize(20, 20);
    clearPpdSearchButton->setStyleSheet(
        "QPushButton { background: #ccc; border: none; border-radius: 10px; font-weight: bold; color: #666; }"
        "QPushButton:hover { background: #bbb; }"
    );
    clearPpdSearchButton->setCursor(Qt::PointingHandCursor);
    clearPpdSearchButton->setVisible(false);

    // Размещаем кнопку в поле поиска
    QHBoxLayout *layout = new QHBoxLayout(ui->line_ppdSearch);
    layout->setContentsMargins(3, 0, 3, 0);
    layout->setAlignment(Qt::AlignRight);
    layout->addWidget(clearPpdSearchButton);

    // Подключаем сигналы
    connect(ui->line_ppdSearch, &QLineEdit::textChanged, this, &MainWindow::onPpdSearchTextChanged);
    connect(clearPpdSearchButton, &QPushButton::clicked, this, [this]() {
        ui->line_ppdSearch->clear();
        showAllPpdRows();
    });
    connect(ui->btn_ppdSearch, &QPushButton::pressed, this, &MainWindow::on_btn_ppdSearch_pressed);
}

void MainWindow::on_btn_ppdSearch_pressed()
{
    QString searchText = ui->line_ppdSearch->text().trimmed();

    // Показываем/скрываем кнопку очистки
    clearPpdSearchButton->setVisible(!searchText.isEmpty());

    if (searchText.isEmpty()) {
        showAllPpdRows();
        return;
    }

    // Поиск в таблице tableWidget_Dose
    bool found = false;
    int rowCount = ui->tableWidget_Dose->rowCount();
    int columnCount = ui->tableWidget_Dose->columnCount();

    for (int row = 0; row < rowCount; ++row) {
        bool match = false;

        // Проверяем все ячейки в строке
        for (int col = 0; col < columnCount; ++col) {
            QTableWidgetItem *item = ui->tableWidget_Dose->item(row, col);
            if (item && item->text().contains(searchText, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }

        // Показываем/скрываем строку в зависимости от результата поиска
        ui->tableWidget_Dose->setRowHidden(row, !match);
        if (match) found = true;
    }

    // Показываем сообщение если ничего не найдено
    if (!found) {
        QMessageBox::information(this, "Поиск", "Ничего не найдено");
    }
}

void MainWindow::onPpdSearchTextChanged(const QString &text)
{
    // Показываем/скрываем кнопку очистки при изменении текста
    clearPpdSearchButton->setVisible(!text.isEmpty());
}

void MainWindow::showAllPpdRows()
{
    // Показываем все строки в таблице
    int rowCount = ui->tableWidget_Dose->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        ui->tableWidget_Dose->setRowHidden(row, false);
    }
}



void MainWindow::on_pushButton_ppdFilter_pressed()
{
    QDateTime startDateTime = QDateTime(ui->dateEdit_ppdSearchData1->dateTime());
    QDateTime endDateTime = QDateTime(ui->dateEdit_ppdSearchData2->dateTime());

    if (startDateTime > endDateTime) {
        QMessageBox::warning(this, "Ошибка", "Начальная дата не может быть больше конечной");
        return;
    }

    bool found = false;
    int dateColumn = 11;

    for (int row = 0; row < ui->tableWidget_Dose->rowCount(); ++row) {
        QTableWidgetItem *dateItem = ui->tableWidget_Dose->item(row, dateColumn);
        bool match = false;

        if (dateItem && !dateItem->text().isEmpty()) {
            QString dateTimeText = dateItem->text().trimmed();

            // Формат: "dd.MM.yyyy mm:hh" где mm - минуты, hh - часы
            QStringList parts = dateTimeText.split(" ");
            if (parts.size() == 2) {
                QString datePart = parts[0]; // "dd.MM.yyyy"
                QString timePart = parts[1]; // "mm:hh"

                QStringList timeParts = timePart.split(":");
                if (timeParts.size() == 2) {
                    int minutes = timeParts[0].toInt();
                    int hours = timeParts[1].toInt();

                    QDate itemDate = QDate::fromString(datePart, "dd.MM.yyyy");
                    QTime itemTime = QTime(hours, minutes);
                    QDateTime itemDateTime = QDateTime(itemDate, itemTime);

                    if (itemDateTime.isValid()) {
                        if (itemDateTime >= startDateTime && itemDateTime <= endDateTime) {
                            match = true;
                            found = true;
                        }
                    }
                }
            }
        }

        ui->tableWidget_Dose->setRowHidden(row, !match);
    }

    if (!found) {
        QMessageBox::information(this, "Фильтр", "В указанном диапазоне ничего не найдено");
    }
}


void MainWindow::setupSearchUserComboBox(QComboBox* comboBox)
{
    if (!comboBox) return;

    // 0. Очистка предыдущих соединений
    comboBox->disconnect();
    if (comboBox->lineEdit()) {
        comboBox->lineEdit()->disconnect();
    }

    // 1. Базовая настройка ComboBox
    comboBox->clear();
    comboBox->setEditable(true);
    comboBox->setInsertPolicy(QComboBox::NoInsert);
    comboBox->setDuplicatesEnabled(false);

    // 2. Статические элементы
    comboBox->addItem("Не выбран", QVariant());
    comboBox->addItem("", QVariant());
    comboBox->setCurrentIndex(1);

    // 3. Загрузка данных из БД
    QSqlQuery query;
    if (!query.exec("SELECT user_id, name_0, name_1, name_2 FROM users ORDER BY name_0, name_1, name_2")) {
        QMessageBox::critical(this, "Ошибка047", "Не удалось загрузить список пользователей: " + query.lastError().text());
        return;
    }

    // Подсчет количества записей и заполнение данных за один проход
    int rowCount = 0;
    QList<UserDataSearch> usersData;

    while (query.next()) {
        QString user_id = query.value("user_id").toString();
        QString lastName = query.value("name_0").toString();
        QString firstName = query.value("name_1").toString();
        QString middleName = query.value("name_2").toString();

        UserDataSearch user;

        user.id = user_id;
        user.displayText = QString("%1 - %2 %3 %4").arg(user_id).arg(lastName).arg(firstName).arg(middleName);
        user.searchText = QString("%1 %2 %3 %4").arg(user_id).arg(lastName).arg(firstName).arg(middleName).toLower();

        usersData.append(user);
        comboBox->addItem(user.displayText, user.id);

        rowCount++;
    }

    //qDebug() << "Количество пользователей:" << rowCount;

    // 4. Настройка поля поиска
    QLineEdit* searchEdit = comboBox->lineEdit();
    searchEdit->setPlaceholderText("Введите для поиска (ID, фамилия, имя...)");
    searchEdit->setClearButtonEnabled(true);

    // 5. Оптимизированная фильтрация с принудительным обновлением
    QTimer* searchTimer = new QTimer(comboBox);
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(300);

    // Сохраняем usersData в свойство comboBox для доступа
    comboBox->setProperty("usersData", QVariant::fromValue(usersData));

    auto filterUsers = [comboBox]() {
        QList<UserDataSearch> usersData = comboBox->property("usersData").value<QList<UserDataSearch>>();
        QString text = comboBox->lineEdit()->text().toLower();

        QSignalBlocker blocker(comboBox);
        QString currentText = comboBox->lineEdit()->text();
        int currentPos = comboBox->lineEdit()->cursorPosition();

        // Очищаем список (кроме первых 2 элементов)
        while (comboBox->count() > 2) {
            comboBox->removeItem(comboBox->count() - 1);
        }

        // Добавляем отфильтрованные элементы
        if (!text.isEmpty()) {
            for (const auto& user : usersData) {
                if (user.searchText.contains(text)) {
                    comboBox->addItem(user.displayText, user.id);
                }
            }
        }

        // Восстанавливаем состояние
        comboBox->lineEdit()->setText(currentText);
        comboBox->lineEdit()->setCursorPosition(currentPos);

        if (!text.isEmpty() && comboBox->count() > 2) {
            comboBox->showPopup();
        }
    };

    connect(searchEdit, &QLineEdit::textChanged, searchTimer, [searchTimer]() {
        searchTimer->start();
    });

    connect(searchTimer, &QTimer::timeout, comboBox, filterUsers);

    // 6. Обработка выбора элемента
    connect(comboBox, QOverload<int>::of(&QComboBox::activated), comboBox, [comboBox](int index) {
        if (index == 0) {
            comboBox->setCurrentIndex(1);
            comboBox->lineEdit()->clear();
        }
        else if (index >= 2) {
            QString selectedText = comboBox->itemText(index);
            comboBox->lineEdit()->setText(selectedText);
            comboBox->hidePopup();

            QString selectedId = comboBox->itemData(index).toString();
            qDebug() << "Выбран ID пользователя:" << selectedId;
        }
    });

    // 7. Показ полного списка при фокусе
    connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), [comboBox]() {
        QTimer::singleShot(100, [comboBox]() {
            if (comboBox->hasFocus()) {
                comboBox->showPopup();
            }
        });
    });

    // 8. Принудительное обновление через короткую задержку
    QTimer::singleShot(500, [comboBox, usersData]() {
        QSignalBlocker blocker(comboBox);

        // Очищаем и перезаполняем
        while (comboBox->count() > 2) {
            comboBox->removeItem(comboBox->count() - 1);
        }

        for (const auto& user : usersData) {
            comboBox->addItem(user.displayText, user.id);
        }

        //qDebug() << "Принудительное обновление comboBox, элементов:" << comboBox->count();
    });
}

void MainWindow::setupAllUserComboBoxes()
{
    // Список всех комбобоксов для пользователей
    QList<QComboBox*> userComboBoxes = {
        ui->selectUserFilterFIO_2,
        ui->selectUserFilterFIO_3,
        ui->select_dutyUsersFilterFIO_2,
        ui->select_dutyUsersFilterFIO_3,
        ui->select_dutyFilterFIO,
        ui->select_SetFilterFIO,
        ui->select_MeshFilterFIO,
        ui->select_KasFilterFIO,
        ui->comboBox_user_id,
        ui->comboBox_MeshUser,
        ui->comboBox_DoseUserId,
        ui->select_DoseHistoryFilter_1,
    };

    // Настраиваем каждый комбобокс
    for (QComboBox* comboBox : userComboBoxes) {
        if (comboBox) {
            setupSearchUserComboBox(comboBox);
        }
    }
    qDebug() << "Обновление всех списков пользователей";
}

void MainWindow::on_pushButton_help_pressed()
{
    // Создаем полупрозрачный фон
    QWidget *overlay = new QWidget(this);
    overlay->setStyleSheet("background-color: rgba(0, 0, 0, 150);");
    overlay->setGeometry(this->rect());

    // Создаем label для GIF
    QLabel *gifLabel = new QLabel(overlay);
    gifLabel->setFixedSize(600, 400);
    gifLabel->setStyleSheet("background-color: white; border-radius: 10px;");
    gifLabel->setAlignment(Qt::AlignCenter);

    // Загружаем GIF
    QMovie *movie = new QMovie(gifLabel);
    QString gifPath = "/home/sds/sh18/kdd_17.11.25/img/help.gif";//"C:/Users/sophia/Documents/work work work/kdd/img/help.gif";

    if (!QFile::exists(gifPath)) {
        QMessageBox::warning(this, "Ошибка", "Файл помощи не найден: " + gifPath);
        delete overlay;
        return;
    }

    movie->setFileName(gifPath);
    gifLabel->setMovie(movie);

    //по центру
    QVBoxLayout *layout = new QVBoxLayout(overlay);
    layout->addStretch();
    layout->addWidget(gifLabel, 0, Qt::AlignCenter);
    layout->addStretch();

    // Настройки overlay
    overlay->setWindowFlags(Qt::FramelessWindowHint);
    overlay->setAttribute(Qt::WA_DeleteOnClose);

    // Обработчик клика на overlay
    overlay->installEventFilter(this);
    overlay->setProperty("movie", QVariant::fromValue(movie));

    // Показываем и запускаем GIF
    overlay->show();
    movie->start();
}

// Обработчик событий для закрытия по клику
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget *overlay = qobject_cast<QWidget*>(obj);
        if (overlay) {
            QMovie *movie = overlay->property("movie").value<QMovie*>();
            if (movie) {
                movie->stop();
            }
            overlay->close();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::loadSetData_kid(){
    ui->inputIntSetQuantity_kid->setValue(1);
    QSqlQueryModel *model = new QSqlQueryModel(this);

    QSqlQuery query;
    query.prepare("SELECT set_id, set_name, ip_set, set_quantity, set_block, set_note, last_update FROM set_kid ORDER BY set_id ASC");

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка049", "Не удалось загрузить данные комплектов: " + query.lastError().text());
        delete model;
        return;
    }

    model->setQuery(query);
    ui->tableView_set_kid->setModel(model);

    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Наименование"));
    model->setHeaderData(2, Qt::Horizontal, tr("IP адрес"));
    model->setHeaderData(3, Qt::Horizontal, tr("Количество"));
    model->setHeaderData(4, Qt::Horizontal, tr("Статус"));
    model->setHeaderData(5, Qt::Horizontal, tr("Примечание"));
    model->setHeaderData(6, Qt::Horizontal, tr("Последнее обновление"));

    ui->tableView_set_kid->resizeColumnsToContents();
    ui->tableView_set_kid->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_set_kid->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_set_kid->verticalHeader()->setVisible(false);

    connect(ui->tableView_set_kid->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onSetSelectionChanged_kid);

    connect(ui->tableView_set_kid, &QTableView::doubleClicked, this, [this](const QModelIndex &index) {
        Q_UNUSED(index);
        ui->tabWidget_kas_kid->setCurrentIndex(1);
    });
}

void MainWindow::onSetSelectionChanged_kid(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    if (selected.indexes().isEmpty()) {
        return;
    }

    QModelIndex index = selected.indexes().first();
    int row = index.row();

    QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->tableView_set_kid->model());
    if (!model) return;

    QString set_id = model->data(model->index(row, 0)).toString();
    QString set_name = model->data(model->index(row, 1)).toString();
    QString ip_set = model->data(model->index(row, 2)).toString();
    int set_quantity = model->data(model->index(row, 3)).toInt();
    QString set_block = model->data(model->index(row, 4)).toString();
    QString set_note = model->data(model->index(row, 5)).toString();
    QString last_update = model->data(model->index(row, 6)).toString();

    ui->textSetId_kid->setText(set_id);
    ui->inputSetName_kid->setText(set_name);
    ui->lineSetNote_kid->setText(set_note);
    ui->inputSetIp_kid->setText(ip_set);
    ui->inputIntSetQuantity_kid->setValue(set_quantity);

    if (set_block == "0") {
        ui->comboSetStatus_kid->setCurrentText("Разблокирован");
    } else if (set_block == "1") {
        ui->comboSetStatus_kid->setCurrentText("Заблокирован");
    } else {
        ui->comboSetStatus_kid->setCurrentIndex(0);
    }

    ui->dateSetLastUpdate_kid->setText(last_update);

    QLayoutItem *item;
    while ((item = ui->gridLayout_set_kid->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    ui->tabWidget_kas_kid->setTabText(0, "Комплект: "+set_name);

    QPushButton *btnSetChange_kid = new QPushButton("Сохранить изменения комплекта", this);
    ui->gridLayout_set_kid->addWidget(btnSetChange_kid);
    connect(btnSetChange_kid, &QPushButton::clicked, this, &MainWindow::on_btnSetChange_kid_pressed);

    loadListOfKas_kid();
    loadFilteredKasData_kid();

    checkRole();
}

void MainWindow::loadListOfKas_kid(){
    ui->comboBox_listOfKas_kid->clear();

    QString set_id = ui->textSetId_kid->text().trimmed();
    ui->label_aboutSet_kid->setText(set_id);

    int kas_id;
    QString kas_name;

    QSqlQuery queryKas;
    queryKas.prepare("SELECT kas_id, kas_name FROM kas_kid WHERE set_id = :set_id ORDER BY set_id ASC, kas_id ASC");
    queryKas.bindValue(":set_id", set_id);

    if (queryKas.exec()) {
        while (queryKas.next()) {
            kas_id = queryKas.value("kas_id").toInt();
            kas_name = queryKas.value("kas_name").toString();
            ui->comboBox_listOfKas_kid->addItem(
                QString("%1 - %2").arg(kas_id).arg(kas_name),
                kas_id
            );
        }

        if (ui->comboBox_listOfKas_kid->count() > 0) {
            ui->comboBox_listOfKas_kid->setCurrentIndex(0);
        }
    } else {
        qDebug() << "Ошибка выполнения запроса17:" << queryKas.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить список кассетниц");
    }

    if (ui->tableView_kas_kid) {
        connect(ui->tableView_kas_kid, &QTableView::doubleClicked, this, [this](const QModelIndex &index) {
            ui->tabWidget_kas_kid->setCurrentIndex(2);

            QAbstractItemModel *model = ui->tableView_kas_kid->model();
            int row = index.row();

            QModelIndex idIndex = model->index(row, 0);
            QString kas_id = model->data(idIndex).toString();

            QModelIndex nameIndex = model->index(row, 1);
            QString kas_name = model->data(nameIndex).toString();

            QString comboText = kas_id + " - " + kas_name;
            int comboIndex = ui->comboBox_listOfKas_kid->findText(comboText);

            if (comboIndex >= 0) {
                ui->comboBox_listOfKas_kid->setCurrentIndex(comboIndex);
            } else {
                qWarning() << "Элемент не найден:" << comboText;
                ui->comboBox_listOfKas_kid->setCurrentIndex(0);
            }
        });
    }
}

void MainWindow::loadFilteredKasData_kid()
{
    QString set_id = ui->textSetId_kid->text().trimmed();

    if (set_id.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Введите ID комплекта");
        return;
    }

    QSqlQueryModel *model = new QSqlQueryModel(this);

    QSqlQuery queryKas;
    queryKas.prepare("SELECT kas_id, kas_name, kas_height, kas_width, kas_block, kas_note, last_update FROM kas_kid WHERE set_id = :set_id ORDER BY set_id ASC, kas_id ASC");
    queryKas.bindValue(":set_id", set_id);

    if (!queryKas.exec()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить данные кассетниц: " + queryKas.lastError().text());
        delete model;
        return;
    }

    model->setQuery(queryKas);
    ui->tableView_kas_kid->setModel(model);

    QStringList headers = {"ID", "Наименование", "Кол-во модулей хранения", "Кол-во ячеек в модуле хранения", "Статус", "Примечание", "Последнее обновление"};

    for (int i = 0; i < headers.size(); ++i) {
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
    }

    ui->tableView_kas_kid->resizeColumnsToContents();
    ui->tableView_kas_kid->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_kas_kid->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_kas_kid->verticalHeader()->setVisible(false);

    connect(ui->tableView_kas_kid->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onKasSelectionChanged_kid);
}

void MainWindow::onKasSelectionChanged_kid(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    if (selected.indexes().isEmpty()) {
        return;
    }

    QModelIndex index = selected.indexes().first();
    int row = index.row();

    QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->tableView_kas_kid->model());
    if (!model) return;

    QString set_id = ui->textSetId_kid->text();
    int kas_id = model->data(model->index(row, 0)).toInt();
    QString kas_name = model->data(model->index(row, 1)).toString();
    int kas_height = model->data(model->index(row, 2)).toInt();
    int kas_width = model->data(model->index(row, 3)).toInt();
    QString kas_block = model->data(model->index(row, 4)).toString();
    QString kas_note = model->data(model->index(row, 5)).toString();
    QString last_update = model->data(model->index(row, 6)).toString();

    QString setName = " - " + ui->inputSetName_kid->text();
    QString setAllName = set_id + setName;
    ui->textSetId_kid_2->setText(setAllName);
    ui->textKasId_kid->setText(QString::number(kas_id));
    ui->inputKasName_kid->setText(kas_name);
    ui->inputKasNote_kid->setText(kas_note);
    ui->inputIntKasLenght_kid->setValue(kas_height);
    ui->inputIntKasWidth_kid->setValue(kas_width);
    ui->dateKasLastUpdate_kid->setText(last_update);

    if (kas_block == "0") {
        ui->comboKasStatus_kid->setCurrentText("Разблокирован");
    }
    else if (kas_block == "1") {
        ui->comboKasStatus_kid->setCurrentText("Заблокирован");
    }
    else {
        ui->comboKasStatus_kid->setCurrentIndex(0);
    }

    QLayoutItem *item;
    while ((item = ui->gridLayout_kas_kid->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    QPushButton *btnKasChange_kid = new QPushButton("Сохранить изменения кассетницы", this);
    ui->gridLayout_kas_kid->addWidget(btnKasChange_kid);
    connect(btnKasChange_kid, &QPushButton::clicked, this, &MainWindow::on_btnKasChange_kid_pressed);

    loadFilteredMeshData_kid(QString::number(kas_id));

    checkRole();
}

void MainWindow::loadFilteredMeshData_kid(QString kas_id)
{
    try {
        const QString set_id = ui->textSetId_kid->text().trimmed();
        if (set_id.isEmpty()) {
            throw std::runtime_error(tr("Не указан ID набора").toStdString());
        }

        int width = 0;
        QSqlQuery widthQuery;
        widthQuery.prepare("SELECT kas_width FROM kas_kid WHERE set_id = :set_id AND kas_id = :kas_id");
        widthQuery.bindValue(":set_id", set_id);
        widthQuery.bindValue(":kas_id", kas_id);

        if (!widthQuery.exec() || !widthQuery.next()) {
        }
        width = widthQuery.value(0).toInt();
        if (width <= 0) {
        }

        QVector<MeshItem> items;
        QString user_id, doz_tld_id;
        QSqlQuery query;
        query.prepare("SELECT mesh_id, mesh_status, user_id, doz_tld_id FROM mesh_kid "
                     "WHERE set_id = :set_id AND kas_id = :kas_id ORDER BY mesh_id");
        query.bindValue(":set_id", set_id);
        query.bindValue(":kas_id", kas_id);

        if (!query.exec()) {
            throw std::runtime_error(query.lastError().text().toStdString());
        }

        while (query.next()) {
            QString current_user_id = query.value(2).toString();
            QString current_doz_id = query.value(3).toString();

            if (!current_user_id.isEmpty() && current_user_id != "0") {
                user_id = current_user_id;
            }
            if (!current_doz_id.isEmpty() && current_doz_id != "0") {
                doz_tld_id = current_doz_id;
            }

            items.append({
                query.value(0).toString(),
                query.value(1).toString(),
                current_user_id,
                current_doz_id
            });
        }

        if (items.isEmpty()) {
            QMessageBox::information(this, tr("Информация"), tr("Данные не найдены"));
            return;
        }

        MeshStatusModel* model = new MeshStatusModel(user_id, doz_tld_id, width, this);
        model->loadData(items);

        ui->tableView_mesh_kid->setModel(nullptr);
        if (m_currentMeshModel) {
            delete m_currentMeshModel;
        }
        m_currentMeshModel = model;

        ui->tableView_mesh_kid->setModel(model);
        ui->tableView_mesh_kid->setIconSize(QSize(MeshStatusModel::ICON_SIZE, MeshStatusModel::ICON_SIZE));

        int compactRowHeight = MeshStatusModel::ICON_SIZE * 2;
        ui->tableView_mesh_kid->verticalHeader()->setDefaultSectionSize(compactRowHeight);

        for (int i = 0; i < model->columnCount(); ++i) {
            ui->tableView_mesh_kid->setColumnWidth(i, width);
        }

        ui->tableView_mesh_kid->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        ui->tableView_mesh_kid->verticalHeader()->setVisible(false);
        ui->tableView_mesh_kid->setShowGrid(false);

        ui->tableView_mesh_kid->setStyleSheet(
            "QTableView {"
            "   border: none;"
            "}"
            "QTableView::item {"
            "   padding-top: 2px;"
            "   padding-bottom: 2px;"
            "   margin: 0px;"
            "}"
        );

        ui->tableView_mesh_kid->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        ui->tableView_mesh_kid->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

        ui->tableView_mesh_kid->viewport()->update();

        connect(ui->tableView_mesh_kid->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &MainWindow::onMeshSelectionChanged_kid, Qt::UniqueConnection);

    } catch (const std::exception& e) {
        qCritical() << "Ошибка в loadFilteredMeshData_kid:" << e.what();
    }
}

void MainWindow::onMeshSelectionChanged_kid(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    if (selected.indexes().isEmpty()) {
        qDebug() << "Не выбрано ни одной ячейки";
        return;
    }

    QModelIndex selectedIndex = selected.indexes().first();
    MeshStatusModel *model = qobject_cast<MeshStatusModel*>(ui->tableView_mesh_kid->model());
    if (!model) {
        qWarning() << "Ошибка: модель данных недоступна";
        return;
    }

    QString mesh_id = model->data(selectedIndex, Qt::DisplayRole).toString();
    qDebug() << "Выбран Mesh ID:" << mesh_id;

    QString set_id = ui->textSetId_kid->text().trimmed();
    QString kas_id = ui->comboBox_listOfKas_kid->currentText().section(' ', 0, 0);

    QSqlQuery query;
    query.prepare("SELECT user_id, doz_tld_id, mesh_status, mesh_note, last_update FROM mesh_kid WHERE mesh_id = :mesh_id AND set_id = :set_id AND kas_id = :kas_id");
    query.bindValue(":mesh_id", mesh_id);
    query.bindValue(":set_id", set_id);
    query.bindValue(":kas_id", kas_id);

    if (!query.exec()) {
        qCritical() << "Ошибка SQL запроса:" << query.lastError().text();
        return;
    }

    if (!query.next()) {
        qWarning() << "Данные не найдены для Mesh ID:" << mesh_id << "Set ID:" << set_id << "Kas ID:" << kas_id;
        return;
    }

    QString user_id = query.value(0).toString();
    QString doz_tld_id = query.value(1).toString();
    QString mesh_status = query.value(2).toString();
    QString mesh_note = query.value(3).toString();
    QString last_update = query.value(4).toString();

    ui->label_id_set_kid->setText(set_id);
    ui->label_id_kas_kid->setText(kas_id);
    ui->label_id_mesh_kid->setText(mesh_id);
    ui->line_MeshNote_kid->setText(mesh_note);
    ui->lineEdit_MeshDoz_kid->setText(doz_tld_id);
    ui->label_MeshUpdate_kid->setText(last_update);

    if (mesh_status == "0") ui->comboBox_MeshStatus_kid->setCurrentText("0 - заблокирована (красный)");
    else if (mesh_status == "1") ui->comboBox_MeshStatus_kid->setCurrentText("1 - с дозиметром (зеленая)");
    else if (mesh_status == "2") ui->comboBox_MeshStatus_kid->setCurrentText("2 - пустая (белая)");
    else if (mesh_status == "3") ui->comboBox_MeshStatus_kid->setCurrentText("3 - возможно не корректно датчик присутствия (серая)");
    else if (mesh_status == "4") ui->comboBox_MeshStatus_kid->setCurrentText("4 - сломана (красная)");
    else if (mesh_status == "5") ui->comboBox_MeshStatus_kid->setCurrentText("5 - открыта (синяя)");
    else if (mesh_status == "6") ui->comboBox_MeshStatus_kid->setCurrentText("6 - другое (желтая)");

    if (user_id != '0'){
        query.prepare("SELECT name_0, name_1, name_2 FROM users WHERE user_id = :user_id");
        query.bindValue(":user_id", user_id);
        if (!query.exec()) {
            qDebug() << "Ошибка выполнения запроса18:" << query.lastError().text();
            return;
        }
        if (query.next()) {
            QString name0 = query.value("name_0").toString();
            QString name1 = query.value("name_1").toString();
            QString name2 = query.value("name_2").toString();

            QString forUser = QString("%1 - %2 %3 %4")
                             .arg(user_id)
                             .arg(name0)
                             .arg(name1)
                             .arg(name2);

            ui->comboBox_MeshUser_kid->setCurrentText(forUser);
        } else {
            ui->comboBox_MeshUser_kid->setCurrentText(user_id + " - Неизвестный пользователь");
        }
    }
    else {
        ui->comboBox_MeshUser_kid->setCurrentIndex(-1);
    }
}

void MainWindow::on_btnKasChange_kid_pressed() {
    QString set_id = ui->textSetId_kid->text();
    QString kas_id = ui->textKasId_kid->text();

    QSqlQuery query1;
    query1.prepare("SELECT kas_height, kas_width FROM kas_kid WHERE set_id = :set_id AND kas_id = :kas_id");
    query1.bindValue(":set_id", set_id);
    query1.bindValue(":kas_id", kas_id);

    int kas_height = 0, kas_width = 0;
    if (query1.exec()) {
        if (query1.next()) {
            kas_height = query1.value("kas_height").toInt();
            kas_width = query1.value("kas_width").toInt();
        }
    }

    int kasQuantity_old = kas_height * kas_width;

    int maxMeshId = -1;
    QSqlQuery query2;
    query2.prepare("SELECT MAX(mesh_id) AS max_mesh_id FROM mesh_kid WHERE set_id = :set_id AND kas_id = :kas_id");
    query2.bindValue(":set_id", set_id);
    query2.bindValue(":kas_id", kas_id);

    if (query2.exec()) {
        if (query2.next()) {
            if (!query2.value(0).isNull()) {
                maxMeshId = query2.value(0).toInt();
            }
        }
    }

    QTimeZone moscowTimeZone("Europe/Moscow");
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString last_update = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");

    QString KasName = ui->inputKasName_kid->text();
    QString KasNote = ui->inputKasNote_kid->text();
    int KasLenght = ui->inputIntKasLenght_kid->value();
    int KasWidth = ui->inputIntKasWidth_kid->value();
    QString KasStatus = ui->comboKasStatus_kid->currentText();

    if (KasName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка053", "Поле 'KasName' не может быть пустым!");
        return;
    }

    QSqlQuery query3;
    query3.prepare("UPDATE kas_kid SET kas_name = :kas_name, kas_height = :kas_height, kas_width = :kas_width, kas_block = :kas_block, kas_note = :kas_note, last_update = :last_update WHERE set_id = :set_id AND kas_id = :kas_id");
    query3.bindValue(":set_id", set_id);
    query3.bindValue(":kas_id", kas_id);
    query3.bindValue(":kas_name", KasName);
    query3.bindValue(":kas_height", KasLenght);
    query3.bindValue(":kas_width", KasWidth);
    query3.bindValue(":kas_block", KasStatus);
    query3.bindValue(":kas_note", KasNote);
    query3.bindValue(":last_update", last_update);

    if (!query3.exec()) {
        QMessageBox::critical(this, "Ошибка054", "Не удалось изменить данные!");
        return;
    }

    int kasQuantity_new = KasLenght * KasWidth;

    if (kasQuantity_new > kasQuantity_old) {
        int newMesh = kasQuantity_new - kasQuantity_old;
        for (int i = 1; i <= newMesh; ++i) {
            QSqlQuery query4;
            query4.prepare("INSERT INTO mesh_kid (set_id, kas_id, mesh_id, user_id, doz_tld_id, mesh_status, mesh_note, last_update) VALUES (:set_id, :kas_id, :mesh_id, :user_id, :doz_tld_id, :mesh_status, :mesh_note, :last_update)");
            query4.bindValue(":set_id", set_id);
            query4.bindValue(":kas_id", kas_id);
            query4.bindValue(":mesh_id", maxMeshId + i);
            query4.bindValue(":user_id", QVariant());
            query4.bindValue(":doz_tld_id", QVariant());
            query4.bindValue(":mesh_status", 0);
            query4.bindValue(":mesh_note", "");
            query4.bindValue(":last_update", QDateTime::currentDateTime());

            query4.exec();

            QString for_user_id = ui->label_userId->text();
            QString changing_user_id;
            QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                QSqlQuery query5;
                query5.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
                query5.bindValue(":name_0", parts[0]);
                query5.bindValue(":name_1", parts[1]);
                query5.bindValue(":name_2", parts[2]);

                if (query5.exec() && query5.next()) {
                    changing_user_id = query5.value("user_id").toString();
                }
            }

            if (!changing_user_id.isEmpty()) {
                addMeshHistory_kid(
                    set_id.toInt(),
                    kas_id.toInt(),
                    maxMeshId + i,
                    0,
                    "",
                    0,
                    "",
                    changing_user_id,
                    "change"
                );
            }
        }
        meshHistoryToTable_kid();
    } else if (kasQuantity_new < kasQuantity_old) {
        int delMesh = kasQuantity_old - kasQuantity_new;
        QSqlQuery query6;
        query6.prepare("DELETE FROM mesh_kid WHERE set_id = :set_id AND kas_id = :kas_id AND mesh_id > :delMesh");
        query6.bindValue(":set_id", set_id);
        query6.bindValue(":kas_id", kas_id);
        query6.bindValue(":delMesh", kasQuantity_new);
        query6.exec();
    }

    loadFilteredKasData_kid();
    loadListOfKas_kid();

    QString comboText = kas_id + " - " + KasName;
    int index = ui->comboBox_listOfKas_kid->findText(comboText);
    if (index >= 0) {
        ui->comboBox_listOfKas_kid->setCurrentIndex(index);
    } else {
        ui->comboBox_listOfKas_kid->setCurrentIndex(0);
    }

    QString for_user_id2 = ui->label_userId->text();
    QString changing_user_id2;
    QStringList parts2 = for_user_id2.split(' ', Qt::SkipEmptyParts);
    if (parts2.size() >= 3) {
        QSqlQuery query7;
        query7.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
        query7.bindValue(":name_0", parts2[0]);
        query7.bindValue(":name_1", parts2[1]);
        query7.bindValue(":name_2", parts2[2]);

        if (query7.exec() && query7.next()) {
            changing_user_id2 = query7.value("user_id").toString();
        }
    }

    if (!changing_user_id2.isEmpty()) {
        addKasHistory_kid(
            set_id.toInt(),
            kas_id.toInt(),
            KasName,
            KasLenght,
            KasWidth,
            KasStatus,
            KasNote,
            changing_user_id2,
            "change"
        );
        kasHistoryToTable_kid();
    }

    on_btnKasCreateNew_kid_pressed();

    comboboxUpdateAll();
}

void MainWindow::on_comboBox_listOfKas_kid_currentTextChanged(const QString &index)
{
    Q_UNUSED(index);
    QString selectedText = ui->comboBox_listOfKas_kid->currentText();
    QString kas_id = selectedText.split("-").first().trimmed();
    if (!kas_id.isEmpty()) {
        loadFilteredMeshData_kid(kas_id);
    } else {
        qDebug() << "Не удалось извлечь ID кассетницы из:" << selectedText;
    }
}

void MainWindow::on_btnSetChange_kid_pressed(){

    QSqlQuery query;

    QString set_id = ui->textSetId_kid->text();
    int set_quantityStart = 0;
    query.prepare("SELECT set_quantity FROM set_kid WHERE set_id = :set_id");
    query.bindValue(":set_id", set_id);

    if (!query.exec()) {
        qCritical() << "Ошибка выполнения запроса2:" << query.lastError().text();
        return;
    }
    if (query.next()) {
        set_quantityStart = query.value("set_quantity").toInt();
    }

    QString SetName = ui->inputSetName_kid->text();
    QString SetNote = ui->lineSetNote_kid->text();
    QString SetIp = ui->inputSetIp_kid->text();
    QString set_block = ui->comboSetStatus_kid->currentText();

    QTimeZone moscowTimeZone("Europe/Moscow");
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString last_update = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");

    int SetQuantity = ui->inputIntSetQuantity_kid->value();
    if (SetQuantity > set_quantityStart){
        int newKas = SetQuantity - set_quantityStart;
        int maxKasNumber = 0;
        query.prepare("SELECT MAX(kas_id) FROM kas_kid WHERE set_id = :set_id");
        query.bindValue(":set_id", set_id);

        if (query.exec() && query.next()) {
            maxKasNumber = query.value(0).toInt();
        } else {
            qWarning() << "Ошибка при получении максимального номера кассеты:" << query.lastError().text();
        }

        for (int i = 1; i <= newKas; ++i) {
            query.prepare("INSERT INTO kas_kid (set_id, kas_id, kas_height, kas_width, kas_block, last_update) VALUES (:set_id, :kas_id, 1, 1, :kas_block, :last_update)");
            query.bindValue(":set_id", set_id);
            query.bindValue(":kas_block", "Разблокирована");
            query.bindValue(":kas_id", maxKasNumber + i);

            if (!query.exec()) {
                qCritical() << "Ошибка при создании кассеты" << i << ":" << query.lastError().text();
            }

            query.prepare("INSERT INTO mesh_kid (set_id, kas_id, mesh_id, user_id, doz_tld_id, mesh_status, mesh_note, last_update) VALUES (:set_id, :kas_id, :mesh_id, :user_id, :doz_tld_id, :mesh_status, :mesh_note, :last_update)");
            query.bindValue(":set_id", set_id);
            query.bindValue(":kas_id", maxKasNumber + i);
            query.bindValue(":mesh_id", 1);
            query.bindValue(":user_id", QVariant(QVariant::Int));
            query.bindValue(":doz_tld_id", QVariant(QVariant::Int));
            query.bindValue(":mesh_status", "0");
            query.bindValue(":mesh_note", "");
            query.bindValue(":last_update", QDateTime::currentDateTime());

            if (!query.exec()) {
                qCritical() << "Ошибка при создании ячейки" << i << ":" << query.lastError().text();
            }
        }
    }
    else if (SetQuantity < set_quantityStart){
        int delKas = set_quantityStart - SetQuantity;

        query.prepare("DELETE FROM kas_kid WHERE set_id = :set_id AND kas_id > :delKas");
        query.bindValue(":set_id", set_id);
        query.bindValue(":delKas", SetQuantity);

        if (!query.exec()) {
            qCritical() << "Ошибка при удалении последних" << delKas << "кассет:"
                       << query.lastError().text();
            return;
        }

        query.prepare("DELETE FROM mesh_kid WHERE set_id = :set_id AND kas_id > :delKas");
        query.bindValue(":set_id", set_id);
        query.bindValue(":delKas", SetQuantity);
        if (!query.exec()) {
            qCritical() << "Ошибка при удалении из mesh_kid:" << query.lastError().text();
            return;
        }
    }

    query.prepare(R"(
        UPDATE set_kid
        SET
            set_name = :set_name,
            ip_set = :ip_set,
            set_quantity = :set_quantity,
            set_block = :set_block,
            set_note = :set_note,
            last_update = :last_update
        WHERE set_id = :set_id;
        )");

    query.bindValue(":set_id", set_id);
    query.bindValue(":set_name", SetName);
    query.bindValue(":ip_set", SetIp);
    query.bindValue(":set_quantity", SetQuantity);
    query.bindValue(":set_block", set_block);
    query.bindValue(":set_note", SetNote);
    query.bindValue(":last_update", last_update);

    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Данные изменены!");
    } else {
        qDebug() << query.lastError().text();
    }

    loadSetData_kid();
    loadFilteredKasData_kid();
    loadFilteredMeshData_kid("1");

    QString for_user_id = ui->label_userId->text();
    QString changing_user_id;
    QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString name_0 = parts[0];
    QString name_1 = parts[1];
    QString name_2 = parts[2];
    query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
    query.bindValue(":name_0", name_0);
    query.bindValue(":name_1", name_1);
    query.bindValue(":name_2", name_2);
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка02", "Не удалось выполнить запрос: " + query.lastError().text());
        qDebug() << "Ошибка поиска id SQL:" << query.lastQuery() << query.lastError().text();
        return;
    }
    if (query.next()) {
        changing_user_id = query.value("user_id").toString();
    } else {
        QMessageBox::information(this, "Информация", "Пользователь не найден");
    }

    QString type_edit = "change";

    bool success = addSetHistory_kid(
        set_id.toInt(),
        SetName,
        SetIp,
        SetQuantity,
        set_block,
        SetNote,
        changing_user_id,
        type_edit
    );

    if (success) {
        qDebug() << "Данные успешно сохранены в таблицу set_kid_history";
        setHistoryToTable_kid();
    } else {
        qDebug() << "Ошибка при сохранении истории комплекта";
    }

    on_btnSetCreateNew_kid_pressed();
    comboboxUpdateAll();
}

void MainWindow::on_btnSetSave_kid_pressed()
{
    QSqlQuery query;

    int set_id = 1;

    if (query.exec("SELECT MAX(set_id) AS max_id FROM set_kid;")) {
        if (query.next()) {
            set_id = query.value("max_id").toInt() + 1;
        } else {
            qDebug() << "Таблица пуста, используем set_id = 1";
        }
    } else {
        qDebug() << "Ошибка выполнения запроса7:" << query.lastError().text();
    }

    QString SetName = ui->inputSetName_kid->text();
    int SetQuantity = ui->inputIntSetQuantity_kid->value();
    QString SetNote = ui->lineSetNote_kid->text();
    QString SetIp = ui->inputSetIp_kid->text();

    QTimeZone moscowTimeZone("Europe/Moscow");
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString SetLastUpdate = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");

    QString SetStatus = ui->comboSetStatus_kid->currentText();

    if (SetName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка020", "Поле 'Название' не может быть пустым!");
        return;
    }

    query.prepare("INSERT INTO set_kid (set_id, set_name, set_quantity, set_block, ip_set, set_note, last_update) VALUES (:set_id, :set_name, :set_quantity, :set_block, :ip_set, :set_note, :last_update)");

    query.bindValue(":set_id", set_id);
    query.bindValue(":set_name", SetName);
    query.bindValue(":set_quantity", SetQuantity);
    query.bindValue(":set_block", SetStatus);
    query.bindValue(":ip_set", SetIp);
    query.bindValue(":set_note", SetNote);
    query.bindValue(":last_update", SetLastUpdate);

    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Данные успешно добавлены!");

        ui->inputSetName_kid->clear();
        ui->inputIntSetQuantity_kid->clear();
        ui->inputSetIp_kid->clear();
        ui->lineSetNote_kid->clear();
        ui->dateSetLastUpdate_kid->setText("-");
    } else {
        if (query.lastError().text().contains("неверный синтаксис для типа inet", Qt::CaseInsensitive)) {
            QMessageBox::critical(this, "Ошибка формата IP",
                                QString("Неверный формат IP-адреса: \n"
                                        "Поле 'ip_set' ожидает IP-адрес в формате: 192.168.1.1"));
        } else {
            QMessageBox::critical(this, "Ошибка021", "Не удалось добавить данные!");
            qDebug() << "Не удалось добавить данные в set_kid: " << query.lastError().text();
        }
        return;
    }

    for (int i = 1; i <= SetQuantity; ++i) {
        query.prepare("INSERT INTO kas_kid (set_id, kas_id, kas_name, kas_height, kas_width, kas_block, kas_note, last_update) VALUES (:set_id, :kas_id, :kas_name, :kas_height, :kas_width, :kas_block, :kas_note, :last_update)");

        query.bindValue(":set_id", set_id);
        query.bindValue(":kas_id", i);
        query.bindValue(":kas_name", QString("Кассетница %1").arg(i));
        query.bindValue(":kas_height", 1);
        query.bindValue(":kas_width", 1);
        query.bindValue(":kas_block", "Разблокирована");
        query.bindValue(":kas_note", "");//
        query.bindValue(":last_update", QDateTime::currentDateTime());

        if (!query.exec()) {
            qDebug() << "Ошибка при вставке кассетницы" << i << ":" << query.lastError().text();
            break;
        }

        int mesh_counter = 1;
        int all_mesh = 1;

        for (int row = 1; row <= all_mesh; ++row) {
            query.prepare("INSERT INTO mesh_kid (set_id, kas_id, mesh_id, user_id, doz_tld_id, mesh_status, mesh_note, last_update) VALUES (:set_id, :kas_id, :mesh_id, :user_id, :doz_tld_id, :mesh_status, :mesh_note, :last_update)");
            query.bindValue(":set_id", set_id);
            query.bindValue(":kas_id", i);
            query.bindValue(":mesh_id", mesh_counter);
            query.bindValue(":user_id", QVariant(QVariant::Int));
            query.bindValue(":doz_tld_id", QVariant(QVariant::Int));
            query.bindValue(":mesh_status", "0");
            query.bindValue(":mesh_note", "");
            query.bindValue(":last_update", QDateTime::currentDateTime());

            if (!query.exec()) {
                qDebug() << "Ошибка при вставке ячейки" << mesh_counter << ":" << query.lastError().text();
                return;
            }
            mesh_counter++;
        }
    }

    loadSetData_kid();

    QString for_user_id = ui->label_userId->text();
    QString changing_user_id;
    QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString name_0 = parts[0];
    QString name_1 = parts[1];
    QString name_2 = parts[2];
    query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
    query.bindValue(":name_0", name_0);
    query.bindValue(":name_1", name_1);
    query.bindValue(":name_2", name_2);
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка022", "Не удалось выполнить запрос: " + query.lastError().text());
        qDebug() << "Ошибка поиска id SQL:" << query.lastQuery() << query.lastError().text();
        return;
    }
    if (query.next()) {
        changing_user_id = query.value("user_id").toString();
    } else {
        QMessageBox::information(this, "Информация", "Пользователь не найден");
    }

    QString type_edit = "create";

    bool success = addSetHistory_kid(
        set_id,
        SetName,
        SetIp,
        SetQuantity,
        SetStatus,
        SetNote,
        changing_user_id,
        type_edit
    );

    if (success) {
        qDebug() << "Данные успешно сохранены в таблицу set_kid_history";
        setHistoryToTable_kid();
    } else {
        qDebug() << "Ошибка при сохранении истории комплекта";
    }

    on_btnSetCreateNew_kid_pressed();
    comboboxUpdateAll();
}

bool MainWindow::addSetHistory_kid(
    int setId,
    const QString &setName,
    const QString &ipSet,
    int setQuantity,
    const QString &setBlock,
    const QString &setNote,
    const QString &changing_user_id,
    const QString &type_edit)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO set_kid_history (
            set_id,
            set_name,
            ip_set,
            set_quantity,
            set_block,
            set_note,
            last_update,
            user_id_change,
            last_update_change,
            type_edit
        ) VALUES (
            :set_id,
            :set_name,
            :ip_set,
            :set_quantity,
            :set_block,
            :set_note,
            :last_update,
            :user_id_change,
            :last_update_change,
            :type_edit
        )
    )");

    query.bindValue(":set_id", setId);
    query.bindValue(":set_name", setName);
    query.bindValue(":ip_set", ipSet);
    query.bindValue(":set_quantity", setQuantity);
    query.bindValue(":set_block", setBlock);
    query.bindValue(":set_note", setNote);
    query.bindValue(":last_update", QDateTime::currentDateTime());
    query.bindValue(":user_id_change", changing_user_id);
    query.bindValue(":last_update_change", QDateTime::currentDateTime());
    query.bindValue(":type_edit", type_edit);

    if (!query.exec()) {
        qDebug() << "Ошибка записи в set_kid_history:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка023", query.lastError().text());
        return false;
    }
    return true;
}

void MainWindow::setHistoryToTable_kid()
{
    ui->tableWidget_historySet_kid->clear();
    ui->tableWidget_historySet_kid->setRowCount(0);
    ui->tableWidget_historySet_kid->setColumnCount(10);

    QStringList headers;
    headers << "ID комплекта" << "Название" << "IP адрес" << "Количество"
            << "Блокировка" << "Примечание" << "Дата обновления"
            << "ID изменившего" << "Дата изменения" << "Тип изменения";

    ui->tableWidget_historySet_kid->setHorizontalHeaderLabels(headers);

    QSqlQuery query;
    query.prepare("SELECT * FROM set_kid_history ORDER BY last_update_change DESC");

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса8:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int row = ui->tableWidget_historySet_kid->rowCount();
        ui->tableWidget_historySet_kid->insertRow(row);

        for (int col = 0; col < query.record().count(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();

            if (query.record().fieldName(col) == "last_update" ||
                query.record().fieldName(col) == "last_update_change") {
                item->setText(query.value(col).toDateTime().toString("dd.MM.yyyy hh:mm"));
            }
            else {
                item->setText(query.value(col).toString());
            }

            ui->tableWidget_historySet_kid->setItem(row, col, item);
        }
    }

    ui->tableWidget_historySet_kid->resizeColumnsToContents();
    ui->tableWidget_historySet_kid->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_historySet_kid->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_historySet_kid->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_historySet_kid->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::on_btnKasSave_kid_pressed()
{
    QSqlQuery query;

    int kas_id = 1;

    if (query.exec("SELECT MAX(kas_id) AS max_id FROM kas_kid;")) {
        if (query.next()) {
            kas_id = query.value("max_id").toInt() + 1;
        } else {
            qDebug() << "Таблица пуста, используем kas_id = 1";
        }
    } else {
        qDebug() << "Ошибка выполнения запроса9:" << query.lastError().text();
    }

    int set_id = ui->textSetId_kid->text().toInt();

    QString KasName = ui->inputKasName_kid->text();
    QString KasNote = ui->inputKasNote_kid->text();
    int KasLenght = ui->inputIntKasLenght_kid->value();
    int KasWidth = ui->inputIntKasWidth_kid->value();
    QString KasStatus = ui->comboKasStatus_kid->currentText();

    QTimeZone moscowTimeZone("Europe/Moscow");
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString last_update = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");

    query.prepare("INSERT INTO kas_kid (set_id, kas_id, kas_name, kas_height, kas_width, kas_block, kas_note, last_update) VALUES (:set_id, :kas_id, :kas_name, :kas_height, :kas_width, :kas_block, :kas_note, :last_update)");

    query.bindValue(":set_id", set_id);
    query.bindValue(":kas_id", kas_id);
    query.bindValue(":kas_name", KasName);
    query.bindValue(":kas_note", KasNote);
    query.bindValue(":kas_height", KasLenght);
    query.bindValue(":kas_width", KasWidth);
    query.bindValue(":kas_block", KasStatus);
    query.bindValue(":last_update", last_update);

    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Данные успешно добавлены!");
    } else {
        QMessageBox::critical(this, "Ошибка024", "Не удалось добавить данные!", query.lastError().text());
        qDebug() << query.lastError().text();
    }

    query.prepare("UPDATE set_kid SET set_quantity = set_quantity+1 WHERE set_id = :set_id;");
    query.bindValue(":set_id", set_id);

    int mesh_counter = 1;
    int all_mesh = KasLenght*KasWidth;

    for (int row = 1; row <= all_mesh; ++row) {
        query.prepare("INSERT INTO mesh_kid (set_id, kas_id, mesh_id, user_id, doz_tld_id, mesh_status, mesh_note, last_update) VALUES (:set_id, :kas_id, :mesh_id, :user_id, :doz_tld_id, :mesh_status, :mesh_note, :last_update)");
        query.bindValue(":set_id", set_id);
        query.bindValue(":kas_id", kas_id);
        query.bindValue(":mesh_id", mesh_counter);
        query.bindValue(":user_id", QVariant(QVariant::Int));
        query.bindValue(":doz_tld_id", QVariant(QVariant::Int));
        query.bindValue(":mesh_status", "0");
        query.bindValue(":mesh_note", "");
        query.bindValue(":last_update", QDateTime::currentDateTime());

        if (!query.exec()) {
            qDebug() << "Ошибка при вставке ячейки" << mesh_counter << ":" << query.lastError().text();
            return;
        }

        QString for_user_id = ui->label_userId->text();
        QString changing_user_id;
        QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
        QString name_0 = parts[0];
        QString name_1 = parts[1];
        QString name_2 = parts[2];

        query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
        query.bindValue(":name_0", name_0);
        query.bindValue(":name_1", name_1);
        query.bindValue(":name_2", name_2);

        if (!query.exec()) {
            QMessageBox::critical(this, "Ошибка025", "Не удалось выполнить запрос: " + query.lastError().text());
            return;
        }

        if (query.next()) {
            changing_user_id = query.value("user_id").toString();
        } else {
            QMessageBox::information(this, "Информация", "Пользователь не найден");
            return;
        }

        QString type_edit = "create";

        bool success = addMeshHistory_kid(
            set_id,
            kas_id,
            mesh_counter,
            0,
            "",
            0,
            "",
            changing_user_id,
            type_edit
        );

        if (success) {
            qDebug() << "Данные успешно сохранены в таблицу mesh_kid_history";
            meshHistoryToTable_kid();
        }
    }

    query.prepare("UPDATE set_kid SET set_quantity = (SELECT MAX(set_quantity) FROM set_kid) + 1 WHERE set_id = ?");
    query.addBindValue(set_id);
    if (query.exec()) {
        qDebug() << "Количество кассетниц в комплекте увеличено на 1";
    } else {
        qDebug() << "Ошибка при увеличении количества кассетниц в комплекте:" << query.lastError().text();
    }

    loadFilteredKasData_kid();
    loadSetData_kid();
    loadListOfKas_kid();

    QString for_user_id = ui->label_userId->text();
    QString changing_user_id;
    QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString name_0 = parts[0];
    QString name_1 = parts[1];
    QString name_2 = parts[2];

    query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
    query.bindValue(":name_0", name_0);
    query.bindValue(":name_1", name_1);
    query.bindValue(":name_2", name_2);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка026", "Не удалось выполнить запрос: " + query.lastError().text());
        return;
    }

    if (query.next()) {
        changing_user_id = query.value("user_id").toString();
    } else {
        QMessageBox::information(this, "Информация", "Пользователь не найден");
        return;
    }

    QString type_edit = "create";

    bool success = addKasHistory_kid(
        set_id,
        kas_id,
        KasName,
        KasLenght,
        KasWidth,
        KasStatus,
        KasNote,
        changing_user_id,
        type_edit
    );

    if (success) {
        qDebug() << "Данные успешно сохранены в таблицу kas_kid_history";
        kasHistoryToTable_kid();
    } else {
        qDebug() << "Ошибка при сохранении истории кассетницы";
    }
    on_btnKasCreateNew_kid_pressed();
    comboboxUpdateAll();
}

bool MainWindow::addKasHistory_kid(
    int setId,
    int kasId,
    const QString &kasName,
    int kasHeight,
    int kasWidth,
    const QString &kasBlock,
    const QString &kasNote,
    const QString &changing_user_id,
    const QString &type_edit)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO kas_kid_history (
            set_id,
            kas_id,
            kas_name,
            kas_height,
            kas_width,
            kas_block,
            kas_note,
            last_update,
            user_id_change,
            last_update_change,
            type_edit
        ) VALUES (
            :set_id,
            :kas_id,
            :kas_name,
            :kas_height,
            :kas_width,
            :kas_block,
            :kas_note,
            :last_update,
            :user_id_change,
            :last_update_change,
            :type_edit
        )
    )");

    query.bindValue(":set_id", setId);
    query.bindValue(":kas_id", kasId);
    query.bindValue(":kas_name", kasName);
    query.bindValue(":kas_height", kasHeight);
    query.bindValue(":kas_width", kasWidth);
    query.bindValue(":kas_block", kasBlock);
    query.bindValue(":kas_note", kasNote);
    query.bindValue(":last_update", QDateTime::currentDateTime());
    query.bindValue(":user_id_change", changing_user_id);
    query.bindValue(":last_update_change", QDateTime::currentDateTime());
    query.bindValue(":type_edit", type_edit);

    if (!query.exec()) {
        qDebug() << "Ошибка записи в kas_kid_history:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка027", query.lastError().text());
        return false;
    }
    return true;
}

void MainWindow::kasHistoryToTable_kid()
{
    ui->tableWidget_historyKas_kid->clear();
    ui->tableWidget_historyKas_kid->setRowCount(0);
    ui->tableWidget_historyKas_kid->setColumnCount(11);

    QStringList headers;
    headers << "ID комплекта" << "ID кассетницы" << "Название" << "Высота"
            << "Ширина" << "Блокировка" << "Примечание" << "Дата обновления"
            << "ID изменившего" << "Дата изменения" << "Тип изменения";

    ui->tableWidget_historyKas_kid->setHorizontalHeaderLabels(headers);

    QSqlQuery query;
    query.prepare("SELECT * FROM kas_kid_history ORDER BY last_update_change DESC");

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса0:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int row = ui->tableWidget_historyKas_kid->rowCount();
        ui->tableWidget_historyKas_kid->insertRow(row);

        for (int col = 0; col < query.record().count(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();

            if (query.record().fieldName(col) == "last_update" ||
                query.record().fieldName(col) == "last_update_change") {
                item->setText(query.value(col).toDateTime().toString("dd.MM.yyyy hh:mm"));
            }
            else {
                item->setText(query.value(col).toString());
            }

            ui->tableWidget_historyKas_kid->setItem(row, col, item);
        }
    }

    ui->tableWidget_historyKas_kid->resizeColumnsToContents();
    ui->tableWidget_historyKas_kid->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_historyKas_kid->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_historyKas_kid->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_historyKas_kid->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::on_btnSetCreateNew_kid_pressed()
{
    ui->textSetId_kid->setText("-");
    ui->inputSetName_kid->clear();
    ui->inputIntSetQuantity_kid->setValue(1);
    ui->lineSetNote_kid->clear();
    ui->inputSetIp_kid->clear();
    ui->comboSetStatus_kid->setCurrentIndex(0);
    ui->dateSetLastUpdate_kid->setText("-");
    ui->comboSetStatus_kid->setCurrentIndex(0);
    ui->textSetId_kid->setFocus();

    QLayoutItem *item;
    while ((item = ui->gridLayout_set_kid->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    QPushButton *btnSetSave_kid = new QPushButton("Сохранить новый комплект", this);
    ui->gridLayout_set_kid->addWidget(btnSetSave_kid);
    connect(btnSetSave_kid, &QPushButton::clicked, this, &MainWindow::on_btnSetSave_kid_pressed);
    on_btnKasCreateNew_kid_pressed();

    QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tableView_kas_kid->model());
    if (model) {
        model->clear();
    }
}

void MainWindow::on_btnKasCreateNew_kid_pressed()
{
    ui->textKasId_kid->setText("-");
    ui->inputKasName_kid->clear();
    ui->comboKasStatus_kid->setCurrentIndex(0);
    ui->inputIntKasLenght_kid->setValue(1);
    ui->inputIntKasWidth_kid->setValue(1);
    ui->inputKasNote_kid->clear();
    ui->textSetId_kid->setFocus();
    ui->dateKasLastUpdate_kid->setText("-");

    QLayoutItem *item;
    while ((item = ui->gridLayout_kas_kid->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    QPushButton *btnKasSave_kid = new QPushButton("Сохранить новую кассетницу", this);
    ui->gridLayout_kas_kid->addWidget(btnKasSave_kid);
    connect(btnKasSave_kid, &QPushButton::clicked, this, &MainWindow::on_btnKasSave_kid_pressed);
}

void MainWindow::on_btnMeshCreateNew_kid_pressed()
{
    ui->label_id_set_kid->setText("-");
    ui->label_id_kas_kid->setText("-");
    ui->label_id_mesh_kid->setText("-");
    ui->line_MeshNote_kid->clear();
    ui->lineEdit_MeshDoz_kid->clear();
    ui->comboBox_MeshStatus_kid->setCurrentIndex(0);
    ui->comboBox_MeshUser_kid->setCurrentIndex(0);
    ui->label_MeshUpdate_kid->setText("-");
    ui->textSetId_kid->setFocus();

    QLayoutItem *item;
    while ((item = ui->gridLayout_mesh_kid->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    QPushButton *btnMeshSave_kid = new QPushButton("Сохранить новую ячейку", this);
    ui->gridLayout_mesh_kid->addWidget(btnMeshSave_kid);
    connect(btnMeshSave_kid, &QPushButton::clicked, this, &MainWindow::on_btnMeshSave_kid_pressed);
}

void MainWindow::on_btnMeshSave_kid_pressed()
{
    QSqlQuery query;

    int id_mesh = 1;

    if (query.exec("SELECT MAX(mesh_id) AS max_id FROM mesh_kid;")) {
        if (query.next()) {
            id_mesh = query.value("max_id").toInt() + 1;
        } else {
            qDebug() << "Таблица пуста, используем mesh_id = 1";
        }
    } else {
        qDebug() << "Ошибка выполнения запроса15:" << query.lastError().text();
    }

    QString id_set = ui->textSetId_kid->text();
    QString id_kas = ui->textKasId_kid->text();
    QString MeshNote = ui->line_MeshNote_kid->text();
    QString MeshDoz = ui->lineEdit_MeshDoz_kid->text();

    QTimeZone moscowTimeZone("Europe/Moscow");
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.toTimeZone(moscowTimeZone);
    QString label_MeshUpdate = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");

    QString MeshStatus_str = ui->comboBox_MeshStatus_kid->currentText();
    QStringList parts1 = MeshStatus_str.split(" - ");
    QString MeshStatus_st = parts1[0].trimmed();
    int MeshStatus = MeshStatus_st.toInt();

    QString MeshUser_str = ui->comboBox_MeshUser_kid->currentText();
    QStringList parts2 = MeshUser_str.split(" - ");
    QString MeshUser_st = parts2[0].trimmed();
    int MeshUser = MeshUser_st.toInt();

    if (id_set.isEmpty() || id_kas.isEmpty()) {
        QMessageBox::warning(this, "Ошибка039", "Поля 'id_set', 'id_kas' не могут быть пустыми!");
        return;
    }

    query.prepare("INSERT INTO mesh_kid(set_id, kas_id, mesh_id, user_id, doz_tld_id, mesh_status, mesh_note, last_update) VALUES (:set_id, :kas_id, :mesh_id, :user_id, :doz_tld_id, :mesh_status, :mesh_note, :last_update)");

    query.bindValue(":set_id", id_set);
    query.bindValue(":kas_id", id_kas);
    query.bindValue(":mesh_id", id_mesh);
    query.bindValue(":mesh_note", MeshNote);
    query.bindValue(":doz_tld_id", MeshDoz);
    query.bindValue(":last_update", label_MeshUpdate);
    query.bindValue(":mesh_status", MeshStatus);
    query.bindValue(":user_id", MeshUser);

    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Создана новая ячейка!");
    } else {
        QMessageBox::critical(this, "Ошибка040", "Не удалось создать ячейку!",  query.lastError().text());
        qDebug() << query.lastError().text();
    }
}

void MainWindow::on_btnMeshChange_kid_pressed() {
    QSqlQuery query;

    const QString set_id = ui->textSetId_kid->text();
    const QString kas_id = ui->label_id_kas_kid->text().split(" - ")[0].trimmed();
    const QString mesh_id = ui->label_id_mesh_kid->text();
    const QString MeshNote = ui->line_MeshNote_kid->text();
    QString MeshDoz = ui->lineEdit_MeshDoz_kid->text().trimmed();

    const QDateTime moscowDateTime = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("Europe/Moscow"));
    const QString label_MeshUpdate = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");

    const QString MeshStatus_str = ui->comboBox_MeshStatus_kid->currentText();
    const int MeshStatus = MeshStatus_str.split(" - ")[0].trimmed().toInt();

    const QString MeshUser_str = ui->comboBox_MeshUser_kid->currentText();
    const QStringList userParts = MeshUser_str.split(" - ");
    int MeshUser = userParts.size() > 0 ? userParts[0].trimmed().toInt() : 0;
    const QString userName = userParts.size() > 1 ? userParts[1].trimmed() : "";

    if (set_id.isEmpty() || kas_id.isEmpty()) {
        QMessageBox::warning(this, "Ошибка042", "Поля 'id_set', 'id_kas' не могут быть пустыми!");
        return;
    }

    if (!MeshDoz.isEmpty()) {
        query.prepare("SELECT set_id, kas_id, mesh_id FROM mesh_kid WHERE doz_tld_id = :doz_tld_id");
        query.bindValue(":doz_tld_id", MeshDoz);

        if (query.exec() && query.next()) {
            const QString oldSetId = query.value("set_id").toString();
            const QString oldKasId = query.value("kas_id").toString();
            const QString oldMeshId = query.value("mesh_id").toString();

            if (!(oldSetId == set_id && oldKasId == kas_id && oldMeshId == mesh_id)) {
                const QString message = QString("Вы уверены что хотите переместить дозиметр из %1 комплекта %2 кассетницы %3 ячейки?")
                                         .arg(oldSetId, oldKasId, oldMeshId);

                if (QMessageBox::question(this, "Внимание", message, QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
                    MeshDoz.clear();
                } else {
                    query.prepare("UPDATE mesh_kid SET doz_tld_id = NULL WHERE set_id = :set_id AND kas_id = :kas_id AND mesh_id = :mesh_id");
                    query.bindValue(":set_id", oldSetId);
                    query.bindValue(":kas_id", oldKasId);
                    query.bindValue(":mesh_id", oldMeshId);
                    query.exec();
                }
            }
        }
    }

    if (MeshUser > 0) {
        QString oldSetId, oldKasId, oldMeshId, userDosimeter;

        query.prepare("SELECT set_id, kas_id, mesh_id FROM mesh_kid WHERE user_id = :user_id");
        query.bindValue(":user_id", MeshUser);

        if (query.exec() && query.next()) {
            oldSetId = query.value("set_id").toString();
            oldKasId = query.value("kas_id").toString();
            oldMeshId = query.value("mesh_id").toString();
        }

        query.prepare("SELECT doz_tld_id FROM users WHERE user_id = :user_id");
        query.bindValue(":user_id", MeshUser);

        if (query.exec() && query.next()) {
            userDosimeter = query.value("doz_tld_id").toString();
        }

        if (!oldSetId.isEmpty() && (oldSetId != set_id || oldKasId != kas_id || oldMeshId != mesh_id)) {
            const QString message = QString("Вы уверены, что хотите изменить ячейку хранения для %1 из комплекта %2, кассетницы %3, ячейки %4?")
                                     .arg(userName.isEmpty() ? "пользователя" : userName, oldSetId, oldKasId, oldMeshId);

            if (QMessageBox::question(this, "Внимание", message, QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
                MeshUser = 0;
            } else {
                query.prepare("UPDATE mesh_kid SET user_id = NULL WHERE set_id = :set_id AND kas_id = :kas_id AND mesh_id = :mesh_id");
                query.bindValue(":set_id", oldSetId);
                query.bindValue(":kas_id", oldKasId);
                query.bindValue(":mesh_id", oldMeshId);
                query.exec();
            }
        }

        if (MeshUser > 0) {
            if (!userDosimeter.isEmpty() && MeshDoz != userDosimeter && !MeshDoz.isEmpty()) {
                const QString message = QString("Вы уверены, что хотите изменить дозиметр для пользователя с %1 на %2?")
                                         .arg(userDosimeter, MeshDoz);

                if (QMessageBox::question(this, "Внимание", message, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                    query.prepare("UPDATE users SET doz_tld_id = :doz_tld_id WHERE user_id = :user_id");
                    query.bindValue(":user_id", MeshUser);
                    query.bindValue(":doz_tld_id", MeshDoz);
                    query.exec();
                }
            } else if (MeshDoz.isEmpty() && !userDosimeter.isEmpty()) {
                MeshDoz = userDosimeter;
            }

            query.prepare("UPDATE users SET set_id = :set_id, kas_id = :kas_id, mesh_id = :mesh_id WHERE user_id = :user_id");
            query.bindValue(":user_id", MeshUser);
            query.bindValue(":set_id", set_id);
            query.bindValue(":kas_id", kas_id);
            query.bindValue(":mesh_id", mesh_id);
            query.exec();
        }
    }

    query.prepare(R"(
        UPDATE mesh_kid SET
            user_id = :user_id,
            doz_tld_id = :doz_tld_id,
            mesh_status = :mesh_status,
            mesh_note = :mesh_note,
            last_update = :last_update
        WHERE set_id = :set_id AND kas_id = :kas_id AND mesh_id = :mesh_id
    )");

    query.bindValue(":set_id", set_id);
    query.bindValue(":kas_id", kas_id);
    query.bindValue(":mesh_id", mesh_id);
    query.bindValue(":user_id", MeshUser > 0 ? MeshUser : QVariant());
    query.bindValue(":doz_tld_id", !MeshDoz.isEmpty() ? MeshDoz : QVariant());
    query.bindValue(":mesh_status", MeshStatus);
    query.bindValue(":mesh_note", MeshNote);
    query.bindValue(":last_update", label_MeshUpdate);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка043", "Не удалось изменить ячейку!\n" + query.lastError().text());
        return;
    }

    loadFilteredMeshData_kid(kas_id);
    updateUsersDatabase();

    const QString for_user_id = ui->label_userId->text();
    const QStringList parts = for_user_id.split(' ', Qt::SkipEmptyParts);
    QString changing_user_id;

    if (parts.size() >= 3) {
        query.prepare("SELECT user_id FROM users WHERE name_0 = :name_0 AND name_1 = :name_1 AND name_2 = :name_2");
        query.bindValue(":name_0", parts[0]);
        query.bindValue(":name_1", parts[1]);
        query.bindValue(":name_2", parts[2]);

        if (query.exec() && query.next()) {
            changing_user_id = query.value("user_id").toString();
        }
    }

    if (!changing_user_id.isEmpty()) {
        addMeshHistory_kid(
            set_id.toInt(),
            kas_id.toInt(),
            mesh_id.toInt(),
            MeshUser,
            MeshDoz,
            MeshStatus,
            MeshNote,
            changing_user_id,
            "change"
        );
        meshHistoryToTable_kid();
    }

    QMessageBox::information(this, "Успех", "Ячейка изменена!");
    comboboxUpdateAll();
}

bool MainWindow::addMeshHistory_kid(
    int setId,
    int kasId,
    int meshId,
    int userId,
    const QString &dozTldId,
    int meshStatus,
    const QString &meshNote,
    const QString &changing_user_id,
    const QString &type_edit)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO mesh_kid_history (
            set_id,
            kas_id,
            mesh_id,
            user_id,
            doz_tld_id,
            mesh_status,
            mesh_note,
            last_update,
            user_id_change,
            last_update_change,
            type_edit
        ) VALUES (
            :set_id,
            :kas_id,
            :mesh_id,
            :user_id,
            :doz_tld_id,
            :mesh_status,
            :mesh_note,
            :last_update,
            :user_id_change,
            :last_update_change,
            :type_edit
        )
    )");

    query.bindValue(":set_id", setId);
    query.bindValue(":kas_id", kasId);
    query.bindValue(":mesh_id", meshId);
    query.bindValue(":user_id", userId);
    query.bindValue(":doz_tld_id", dozTldId);
    query.bindValue(":mesh_status", meshStatus);
    query.bindValue(":mesh_note", meshNote);
    query.bindValue(":last_update", QDateTime::currentDateTime());
    query.bindValue(":user_id_change", changing_user_id);
    query.bindValue(":last_update_change", QDateTime::currentDateTime());
    query.bindValue(":type_edit", type_edit);

    if (!query.exec()) {
        qDebug() << "Ошибка записи в mesh_kid_history:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка045", query.lastError().text());
        return false;
    }
    return true;
}

void MainWindow::meshHistoryToTable_kid()
{
    ui->tableWidget_historyMesh_kid->clear();
    ui->tableWidget_historyMesh_kid->setRowCount(0);
    ui->tableWidget_historyMesh_kid->setColumnCount(11);

    QStringList headers;
    headers << "ID комплекта" << "ID кассетницы" << "ID ячейки" << "ID пользователя"
            << "ID дозиметра" << "Статус" << "Примечание" << "Дата обновления"
            << "ID изменившего" << "Дата изменения" << "Тип изменения";

    ui->tableWidget_historyMesh_kid->setHorizontalHeaderLabels(headers);

    QSqlQuery query;
    query.prepare("SELECT * FROM mesh_kid_history ORDER BY last_update_change DESC");

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса16:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int row = ui->tableWidget_historyMesh_kid->rowCount();
        ui->tableWidget_historyMesh_kid->insertRow(row);

        for (int col = 0; col < query.record().count(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();

            if (query.record().fieldName(col) == "last_update" ||
                query.record().fieldName(col) == "last_update_change") {
                item->setText(query.value(col).toDateTime().toString("dd.MM.yyyy hh:mm"));
            }
            else {
                item->setText(query.value(col).toString());
            }

            ui->tableWidget_historyMesh_kid->setItem(row, col, item);
        }
    }

    ui->tableWidget_historyMesh_kid->resizeColumnsToContents();
    ui->tableWidget_historyMesh_kid->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_historyMesh_kid->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_historyMesh_kid->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_historyMesh_kid->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::on_pushButton_SetHistoryFile_kid_pressed()
{
    QString currentDate = QDate::currentDate().toString("dd.MM.yyyy");
    QString fileName = QString("set_kid-%1.csv").arg(currentDate);

    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить файл",
                                                   QDir::homePath() + "/" + fileName,
                                                   "CSV Files (*.csv)");

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка060", "Не удалось создать файл: " + file.errorString());
        return;
    }

    QTextStream out(&file);

    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
    #else
    out.setEncoding(QStringConverter::Utf8);
    #endif

    for (int col = 0; col < ui->tableWidget_historySet_kid->columnCount(); ++col) {
        QTableWidgetItem *headerItem = ui->tableWidget_historySet_kid->horizontalHeaderItem(col);
        if (headerItem) {
            out << "\"" << headerItem->text() << "\"";
        } else {
            out << "\"Column " << col + 1 << "\"";
        }
        if (col < ui->tableWidget_historySet_kid->columnCount() - 1) {
            out << ";";
        }
    }
    out << "\n";

    for (int row = 0; row < ui->tableWidget_historySet_kid->rowCount(); ++row) {
        for (int col = 0; col < ui->tableWidget_historySet_kid->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget_historySet_kid->item(row, col);
            if (item && !item->text().isEmpty()) {
                QString text = item->text();
                text.replace("\"", "\"\"");
                out << "\"" << text << "\"";
            } else {
                out << "\"\"";
            }
            if (col < ui->tableWidget_historySet_kid->columnCount() - 1) {
                out << ";";
            }
        }
        out << "\n";
    }

    file.close();
    QMessageBox::information(this, "Успех", QString("Данные сохранены в файл:\n%1").arg(filePath));
}

void MainWindow::on_pushButton_KasHistoryFile_kid_pressed()
{
    QString currentDate = QDate::currentDate().toString("dd.MM.yyyy");
    QString fileName = QString("kas_kid-%1.csv").arg(currentDate);

    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить файл",
                                                   QDir::homePath() + "/" + fileName,
                                                   "CSV Files (*.csv)");

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка061", "Не удалось создать файл: " + file.errorString());
        return;
    }

    QTextStream out(&file);

    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
    #else
    out.setEncoding(QStringConverter::Utf8);
    #endif

    for (int col = 0; col < ui->tableWidget_historyKas_kid->columnCount(); ++col) {
        QTableWidgetItem *headerItem = ui->tableWidget_historyKas_kid->horizontalHeaderItem(col);
        if (headerItem) {
            out << "\"" << headerItem->text() << "\"";
        } else {
            out << "\"Column " << col + 1 << "\"";
        }
        if (col < ui->tableWidget_historyKas_kid->columnCount() - 1) {
            out << ";";
        }
    }
    out << "\n";

    for (int row = 0; row < ui->tableWidget_historyKas_kid->rowCount(); ++row) {
        for (int col = 0; col < ui->tableWidget_historyKas_kid->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget_historyKas_kid->item(row, col);
            if (item && !item->text().isEmpty()) {
                QString text = item->text();
                text.replace("\"", "\"\"");
                out << "\"" << text << "\"";
            } else {
                out << "\"\"";
            }
            if (col < ui->tableWidget_historyKas_kid->columnCount() - 1) {
                out << ";";
            }
        }
        out << "\n";
    }

    file.close();
    QMessageBox::information(this, "Успех", QString("Данные сохранены в файл:\n%1").arg(filePath));
}

void MainWindow::on_pushButton_MeshHistoryFile_kid_pressed()
{
    QString currentDate = QDate::currentDate().toString("dd.MM.yyyy");
    QString fileName = QString("mesh_kid-%1.csv").arg(currentDate);

    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить файл",
                                                   QDir::homePath() + "/" + fileName,
                                                   "CSV Files (*.csv)");

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка062", "Не удалось создать файл: " + file.errorString());
        return;
    }

    QTextStream out(&file);

    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
    #else
    out.setEncoding(QStringConverter::Utf8);
    #endif

    for (int col = 0; col < ui->tableWidget_historyMesh_kid->columnCount(); ++col) {
        QTableWidgetItem *headerItem = ui->tableWidget_historyMesh_kid->horizontalHeaderItem(col);
        if (headerItem) {
            out << "\"" << headerItem->text() << "\"";
        } else {
            out << "\"Column " << col + 1 << "\"";
        }
        if (col < ui->tableWidget_historyMesh_kid->columnCount() - 1) {
            out << ";";
        }
    }
    out << "\n";

    for (int row = 0; row < ui->tableWidget_historyMesh_kid->rowCount(); ++row) {
        for (int col = 0; col < ui->tableWidget_historyMesh_kid->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget_historyMesh_kid->item(row, col);
            if (item && !item->text().isEmpty()) {
                QString text = item->text();
                text.replace("\"", "\"\"");
                out << "\"" << text << "\"";
            } else {
                out << "\"\"";
            }
            if (col < ui->tableWidget_historyMesh_kid->columnCount() - 1) {
                out << ";";
            }
        }
        out << "\n";
    }

    file.close();
    QMessageBox::information(this, "Успех", QString("Данные сохранены в файл:\n%1").arg(filePath));
}

void MainWindow::on_btn_SetFilterGo_kid_pressed()
{
    QString dateFilter = ui->selec_SetFilterDate_kid->currentText();
    QString setIdFilter = ui->select_kdd_set_1_kid->currentText();
    QString userChangeFilter = ui->select_SetFilterFIO_kid->currentText();

    auto extractId = [](const QString &input) -> QString {
        if (input == "-" || input.isEmpty()) return "-";
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatch match = re.match(input);
        return match.hasMatch() ? match.captured(1) : input;
    };

    QString setId = extractId(setIdFilter);
    QString userChangeId = extractId(userChangeFilter);

    QList<QList<QString>> visibleRows;

    for (int row = 0; row < ui->tableWidget_historySet_kid->rowCount(); ++row) {
        bool showRow = true;

        if (setId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historySet_kid->item(row, 0);
            if (!item || extractId(item->text()) != setId) {
                showRow = false;
            }
        }

        if (showRow && userChangeId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historySet_kid->item(row, 7);
            if (!item || extractId(item->text()) != userChangeId) {
                showRow = false;
            }
        }

        if (showRow) {
            QList<QString> rowData;
            for (int col = 0; col < ui->tableWidget_historySet_kid->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget_historySet_kid->item(row, col);
                rowData.append(item ? item->text() : "");
            }
            visibleRows.append(rowData);
        }
    }

    if (dateFilter == "Дата (сначала старые)") {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                if (a.size() <= 9 || b.size() <= 9) return false;

                QDateTime dateA = QDateTime::fromString(a[9], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[9], "dd.MM.yyyy HH:mm");

                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[9] < b[9];
                }

                return dateA < dateB;
            });
    } else {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                if (a.size() <= 9 || b.size() <= 9) return false;

                QDateTime dateA = QDateTime::fromString(a[9], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[9], "dd.MM.yyyy HH:mm");

                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[9] > b[9];
                }

                return dateA > dateB;
            });
    }

    ui->tableWidget_historySet_kid->setRowCount(0);
    for (const QList<QString> &rowData : visibleRows) {
        int newRow = ui->tableWidget_historySet_kid->rowCount();
        ui->tableWidget_historySet_kid->insertRow(newRow);

        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);

            if (col == 0 || col == 3 || col == 7) {
                item->setTextAlignment(Qt::AlignCenter);
            }

            ui->tableWidget_historySet_kid->setItem(newRow, col, item);
        }
    }
}

void MainWindow::on_btn_SetFilterStop_kid_pressed()
{
    setHistoryToTable_kid();
}

void MainWindow::on_btn_KasFilterStop_kid_pressed()
{
    kasHistoryToTable_kid();
}

void MainWindow::on_btn_MeshFilterStop_kid_pressed()
{
    meshHistoryToTable_kid();
}

void MainWindow::on_btn_KasFilterGo_kid_pressed()
{
    QString dateFilter = ui->selec_KasFilterDate_kid->currentText();
    QString setIdFilter = ui->select_kdd_set_2_kid->currentText();
    QString kasIdFilter = ui->select_kss_kas_2_kid->currentText();
    QString userChangeFilter = ui->select_KasFilterFIO_kid->currentText();

    auto extractId = [](const QString &input) -> QString {
        if (input == "-" || input.isEmpty()) return "-";
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatch match = re.match(input);
        return match.hasMatch() ? match.captured(1) : input;
    };

    QString setId = extractId(setIdFilter);
    QString kasId = extractId(kasIdFilter);
    QString userChangeId = extractId(userChangeFilter);

    QList<QList<QString>> visibleRows;

    for (int row = 0; row < ui->tableWidget_historyKas_kid->rowCount(); ++row) {
        bool showRow = true;

        if (setId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyKas_kid->item(row, 0);
            if (!item || extractId(item->text()) != setId) {
                showRow = false;
            }
        }

        if (showRow && kasId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyKas_kid->item(row, 1);
            if (!item || extractId(item->text()) != kasId) {
                showRow = false;
            }
        }

        if (showRow && userChangeId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyKas_kid->item(row, 8);
            if (!item || extractId(item->text()) != userChangeId) {
                showRow = false;
            }
        }

        if (showRow) {
            QList<QString> rowData;
            for (int col = 0; col < ui->tableWidget_historyKas_kid->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget_historyKas_kid->item(row, col);
                rowData.append(item ? item->text() : "");
            }
            visibleRows.append(rowData);
        }
    }

    if (dateFilter == "Дата (сначала старые)") {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                if (a.size() <= 10 || b.size() <= 10) return false;

                QDateTime dateA = QDateTime::fromString(a[10], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[10], "dd.MM.yyyy HH:mm");

                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[10] < b[10];
                }

                return dateA < dateB;
            });
    } else {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                if (a.size() <= 10 || b.size() <= 10) return false;

                QDateTime dateA = QDateTime::fromString(a[10], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[10], "dd.MM.yyyy HH:mm");

                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[10] > b[10];
                }

                return dateA > dateB;
            });
    }

    ui->tableWidget_historyKas_kid->setRowCount(0);
    for (const QList<QString> &rowData : visibleRows) {
        int newRow = ui->tableWidget_historyKas_kid->rowCount();
        ui->tableWidget_historyKas_kid->insertRow(newRow);

        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);

            if (col == 0 || col == 1 || col == 3 || col == 4 || col == 8) {
                item->setTextAlignment(Qt::AlignCenter);
            }

            ui->tableWidget_historyKas_kid->setItem(newRow, col, item);
        }
    }
}

void MainWindow::on_btn_MeshFilterGo_kid_pressed()
{
    QString dateFilter = ui->selec_MeshFilterDate_kid->currentText();
    QString setIdFilter = ui->select_kdd_set_1_kid->currentText();
    QString kasIdFilter = ui->select_kdd_kas_3_kid->currentText();
    QString meshIdFilter = ui->select_kdd_mesh_3_kid->currentText();
    QString userChangeFilter = ui->select_MeshFilterFIO_kid->currentText();

    auto extractId = [](const QString &input) -> QString {
        if (input == "-" || input.isEmpty()) return "-";
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatch match = re.match(input);
        return match.hasMatch() ? match.captured(1) : input;
    };

    QString setId = extractId(setIdFilter);
    QString kasId = extractId(kasIdFilter);
    QString meshId = extractId(meshIdFilter);
    QString userChangeId = extractId(userChangeFilter);

    QList<QList<QString>> visibleRows;

    for (int row = 0; row < ui->tableWidget_historyMesh_kid->rowCount(); ++row) {
        bool showRow = true;

        if (setId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyMesh_kid->item(row, 0);
            if (!item || extractId(item->text()) != setId) {
                showRow = false;
            }
        }

        if (showRow && kasId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyMesh_kid->item(row, 1);
            if (!item || extractId(item->text()) != kasId) {
                showRow = false;
            }
        }

        if (showRow && meshId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyMesh_kid->item(row, 2);
            if (!item || extractId(item->text()) != meshId) {
                showRow = false;
            }
        }

        if (showRow && userChangeId != "-") {
            QTableWidgetItem *item = ui->tableWidget_historyMesh_kid->item(row, 8);
            if (!item || extractId(item->text()) != userChangeId) {
                showRow = false;
            }
        }

        if (showRow) {
            QList<QString> rowData;
            for (int col = 0; col < ui->tableWidget_historyMesh_kid->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget_historyMesh_kid->item(row, col);
                rowData.append(item ? item->text() : "");
            }
            visibleRows.append(rowData);
        }
    }

    if (dateFilter == "Дата (сначала старые)") {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                if (a.size() <= 10 || b.size() <= 10) return false;

                QDateTime dateA = QDateTime::fromString(a[10], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[10], "dd.MM.yyyy HH:mm");

                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[10] < b[10];
                }

                return dateA < dateB;
            });
    } else {
        std::sort(visibleRows.begin(), visibleRows.end(),
            [](const QList<QString> &a, const QList<QString> &b) {
                if (a.size() <= 10 || b.size() <= 10) return false;

                QDateTime dateA = QDateTime::fromString(a[10], "dd.MM.yyyy HH:mm");
                QDateTime dateB = QDateTime::fromString(b[10], "dd.MM.yyyy HH:mm");

                if (!dateA.isValid() || !dateB.isValid()) {
                    return a[10] > b[10];
                }

                return dateA > dateB;
            });
    }

    ui->tableWidget_historyMesh_kid->setRowCount(0);
    for (const QList<QString> &rowData : visibleRows) {
        int newRow = ui->tableWidget_historyMesh_kid->rowCount();
        ui->tableWidget_historyMesh_kid->insertRow(newRow);

        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);

            if (col == 0 || col == 1 || col == 2 || col == 3 || col == 5 || col == 8) {
                item->setTextAlignment(Qt::AlignCenter);
            }

            ui->tableWidget_historyMesh_kid->setItem(newRow, col, item);
        }
    }
}

void MainWindow::updateListSetForHistory_kid()
{
    updateSetComboBox(ui->select_kdd_set_1_kid);

    updateComboBox_kid(ui->select_kdd_set_2_kid, "SELECT DISTINCT set_id FROM kas_kid ORDER BY set_id");

    updateComboBox_kid(ui->select_kdd_set_1_kid, "SELECT DISTINCT set_id FROM mesh_kid ORDER BY set_id");

    disconnect(ui->select_kdd_set_2_kid, nullptr, nullptr, nullptr);
    connect(ui->select_kdd_set_2_kid, &QComboBox::currentTextChanged, this, [this]() {
        QString currentSet = ui->select_kdd_set_2_kid->currentText();
        if (!currentSet.isEmpty()) {
            updateComboBox_kid(ui->select_kss_kas_2_kid,
                          "SELECT DISTINCT kas_id FROM kas_kid WHERE set_id = ? ORDER BY kas_id",
                          currentSet);
        } else {
            ui->select_kss_kas_2_kid->clear();
            ui->select_kss_kas_2_kid->addItem("");
        }
    });

    disconnect(ui->select_kdd_set_1_kid, nullptr, nullptr, nullptr);
    disconnect(ui->select_kdd_kas_3_kid, nullptr, nullptr, nullptr);

    connect(ui->select_kdd_set_1_kid, &QComboBox::currentTextChanged, this, [this]() {
        QString currentSet = ui->select_kdd_set_1_kid->currentText();
        if (!currentSet.isEmpty()) {
            updateComboBox_kid(ui->select_kdd_kas_3_kid,
                          "SELECT DISTINCT kas_id FROM mesh_kid WHERE set_id = ? ORDER BY kas_id",
                          currentSet);
        } else {
            ui->select_kdd_kas_3_kid->clear();
            ui->select_kdd_kas_3_kid->addItem("");
        }

        QString currentKas = ui->select_kdd_kas_3_kid->currentText();
        if (!currentSet.isEmpty() && !currentKas.isEmpty()) {
            updateComboBox_kid(ui->select_kdd_mesh_3_kid,
                          "SELECT DISTINCT mesh_id FROM mesh_kid WHERE set_id = ? AND kas_id = ? ORDER BY mesh_id",
                          currentSet, currentKas);
        } else {
            ui->select_kdd_mesh_3_kid->clear();
            ui->select_kdd_mesh_3_kid->addItem("");
        }
    });

    connect(ui->select_kdd_kas_3_kid, &QComboBox::currentTextChanged, this, [this]() {
        QString currentSet = ui->select_kdd_set_1_kid->currentText();
        QString currentKas = ui->select_kdd_kas_3_kid->currentText();
        if (!currentSet.isEmpty() && !currentKas.isEmpty()) {
            updateComboBox_kid(ui->select_kdd_mesh_3_kid,
                          "SELECT DISTINCT mesh_id FROM mesh_kid WHERE set_id = ? AND kas_id = ? ORDER BY mesh_id",
                          currentSet, currentKas);
        } else {
            ui->select_kdd_mesh_3_kid->clear();
            ui->select_kdd_mesh_3_kid->addItem("");
        }
    });

    QString currentSet2 = ui->select_kdd_set_2_kid->currentText();
    if (!currentSet2.isEmpty()) {
        updateComboBox_kid(ui->select_kss_kas_2_kid,
                      "SELECT DISTINCT kas_id FROM kas_kid WHERE set_id = ? ORDER BY kas_id",
                      currentSet2);
    }

    QString currentSet3 = ui->select_kdd_set_1_kid->currentText();
    if (!currentSet3.isEmpty()) {
        updateComboBox_kid(ui->select_kdd_kas_3_kid,
                      "SELECT DISTINCT kas_id FROM mesh_kid WHERE set_id = ? ORDER BY kas_id",
                      currentSet3);

        QString currentKas3 = ui->select_kdd_kas_3_kid->currentText();
        if (!currentKas3.isEmpty()) {
            updateComboBox_kid(ui->select_kdd_mesh_3_kid,
                          "SELECT DISTINCT mesh_id FROM mesh_kid WHERE set_id = ? AND kas_id = ? ORDER BY mesh_id",
                          currentSet3, currentKas3);
        }
    }
}

void MainWindow::updateComboBox_kid(QComboBox* combo, const QString& queryText, const QString& bindValue1, const QString& bindValue2)
{
    combo->clear();
    combo->addItem("");

    QSqlQuery query;
    query.prepare(queryText);

    if (!bindValue1.isEmpty()) {
        query.addBindValue(bindValue1);
    }
    if (!bindValue2.isEmpty()) {
        query.addBindValue(bindValue2);
    }

    if (query.exec()) {
        while (query.next()) {
            combo->addItem(query.value(0).toString());
        }
    }
}

void MainWindow::updateSetComboBox(QComboBox* comboBox)
{
    if (!comboBox) return;

    QString currentValue = comboBox->currentText();
    comboBox->clear();
    comboBox->addItem("");

    QSqlQuery query("SELECT DISTINCT set_id FROM set ORDER BY set_id");
    while (query.next()) {
        comboBox->addItem(query.value(0).toString());
    }

    // Восстанавливаем предыдущее значение если оно есть
    int index = comboBox->findText(currentValue);
    if (index >= 0) {
        comboBox->setCurrentIndex(index);
    }
}

void MainWindow::updateSetComboBox(QComboBox* comboBox1, QComboBox* comboBox2)
{
    if (!comboBox1 || !comboBox2) return;

    // Сохраняем текущие значения
    QString currentSet = comboBox1->currentText();
    QString currentKas = comboBox2->currentText();

    // Очищаем комбобоксы
    comboBox1->clear();
    comboBox2->clear();
    comboBox1->addItem("");
    comboBox2->addItem("");

    // Кэш для быстрого доступа к kas_id по set_id
    QMap<QString, QStringList> setToKasMap;

    QSqlQuery query("SELECT DISTINCT set_id, kas_id FROM kas ORDER BY set_id, kas_id");
    while (query.next()) {
        QString set_id = query.value("set_id").toString();
        QString kas_id = query.value("kas_id").toString();

        // Добавляем set_id в первый комбобокс (если еще нет)
        if (comboBox1->findText(set_id) == -1) {
            comboBox1->addItem(set_id);
        }

        // Сохраняем kas_id для этого set_id
        setToKasMap[set_id].append(kas_id);
    }

    // Восстанавливаем выбранный set_id
    int setIndex = comboBox1->findText(currentSet);
    if (setIndex >= 0) {
        comboBox1->setCurrentIndex(setIndex);

        // Заполняем второй комбобокс kas_id для выбранного set_id
        QStringList kasList = setToKasMap.value(currentSet);
        kasList.sort();
        for (const QString& kas : kasList) {
            comboBox2->addItem(kas);
        }

        // Восстанавливаем выбранный kas_id
        int kasIndex = comboBox2->findText(currentKas);
        if (kasIndex >= 0) {
            comboBox2->setCurrentIndex(kasIndex);
        }
    }

    // Подключаем сигналы для обновления второго комбобокса при изменении первого
    disconnect(comboBox1, &QComboBox::currentTextChanged, this, &MainWindow::onSetComboBoxChanged);
    connect(comboBox1, &QComboBox::currentTextChanged, this, &MainWindow::onSetComboBoxChanged);

    // Сохраняем ссылки на комбобоксы для использования в слоте
    m_setComboBox = comboBox1;
    m_kasComboBox = comboBox2;
    m_meshComboBox = nullptr; // Третий комбобокс не используется в этой перегрузке
}

void MainWindow::updateSetComboBox(QComboBox* comboBox1, QComboBox* comboBox2, QComboBox* comboBox3)
{
    if (!comboBox1 || !comboBox2 || !comboBox3) return;

    // Сохраняем текущие значения
    QString currentSet = comboBox1->currentText();
    QString currentKas = comboBox2->currentText();
    QString currentMesh = comboBox3->currentText();

    // Очищаем комбобоксы
    comboBox1->clear();
    comboBox2->clear();
    comboBox3->clear();
    comboBox1->addItem("");
    comboBox2->addItem("");
    comboBox3->addItem("");

    // Структуры для кэширования данных
    QMap<QString, QMap<QString, QStringList>> setKasMeshMap; // set_id -> kas_id -> mesh_id[]
    QMap<QString, QStringList> setToKasMap; // set_id -> kas_id[]

    QSqlQuery query("SELECT DISTINCT set_id, kas_id, mesh_id FROM mesh ORDER BY set_id, kas_id, mesh_id");
    while (query.next()) {
        QString set_id = query.value("set_id").toString();
        QString kas_id = query.value("kas_id").toString();
        QString mesh_id = query.value("mesh_id").toString();

        // Добавляем set_id в первый комбобокс (если еще нет)
        if (comboBox1->findText(set_id) == -1) {
            comboBox1->addItem(set_id);
        }

        // Сохраняем связь set_id -> kas_id
        if (!setToKasMap[set_id].contains(kas_id)) {
            setToKasMap[set_id].append(kas_id);
        }

        // Сохраняем полную связь set_id -> kas_id -> mesh_id
        setKasMeshMap[set_id][kas_id].append(mesh_id);
    }

    // Восстанавливаем выбранный set_id
    int setIndex = comboBox1->findText(currentSet);
    if (setIndex >= 0) {
        comboBox1->setCurrentIndex(setIndex);

        // Заполняем второй комбобокс kas_id для выбранного set_id
        QStringList kasList = setToKasMap.value(currentSet);
        kasList.sort();
        for (const QString& kas : kasList) {
            comboBox2->addItem(kas);
        }

        // Восстанавливаем выбранный kas_id
        int kasIndex = comboBox2->findText(currentKas);
        if (kasIndex >= 0) {
            comboBox2->setCurrentIndex(kasIndex);

            // Заполняем третий комбобокс mesh_id для выбранных set_id и kas_id
            QStringList meshList = setKasMeshMap.value(currentSet).value(currentKas);
            meshList.sort();
            for (const QString& mesh : meshList) {
                comboBox3->addItem(mesh);
            }

            // Восстанавливаем выбранный mesh_id
            int meshIndex = comboBox3->findText(currentMesh);
            if (meshIndex >= 0) {
                comboBox3->setCurrentIndex(meshIndex);
            }
        }
    }

    // Подключаем сигналы для обновления зависимых комбобоксов
    disconnect(comboBox1, &QComboBox::currentTextChanged, this, &MainWindow::onSetComboBoxChanged);
    disconnect(comboBox2, &QComboBox::currentTextChanged, this, &MainWindow::onKasComboBoxChanged);

    connect(comboBox1, &QComboBox::currentTextChanged, this, &MainWindow::onSetComboBoxChanged);
    connect(comboBox2, &QComboBox::currentTextChanged, this, &MainWindow::onKasComboBoxChanged);

    // Сохраняем ссылки на комбобоксы для использования в слотах
    m_setComboBox = comboBox1;
    m_kasComboBox = comboBox2;
    m_meshComboBox = comboBox3;
}

// для обновления зависимых списков
void MainWindow::onSetComboBoxChanged()
{
    if (!m_setComboBox || !m_kasComboBox) return;

    QString currentSet = m_setComboBox->currentText();
    QString currentKas = m_kasComboBox->currentText();

    m_kasComboBox->clear();
    m_kasComboBox->addItem("");

    if (m_meshComboBox) {
        m_meshComboBox->clear();
        m_meshComboBox->addItem("");
    }

    if (!currentSet.isEmpty()) {
        QSqlQuery query;
        if (m_meshComboBox) {
            // Для трехкомбобоксного режима (из таблицы mesh)
            query.prepare("SELECT DISTINCT kas_id FROM mesh WHERE set_id = ? ORDER BY kas_id");
        } else {
            // Для двухкомбобоксного режима (из таблицы kas)
            query.prepare("SELECT DISTINCT kas_id FROM kas WHERE set_id = ? ORDER BY kas_id");
        }
        query.addBindValue(currentSet);

        if (query.exec()) {
            while (query.next()) {
                m_kasComboBox->addItem(query.value(0).toString());
            }

            // Восстанавливаем выбранный kas_id если возможно
            int kasIndex = m_kasComboBox->findText(currentKas);
            if (kasIndex >= 0) {
                m_kasComboBox->setCurrentIndex(kasIndex);
            }
        }
    }
}

void MainWindow::onKasComboBoxChanged()
{
    if (!m_setComboBox || !m_kasComboBox || !m_meshComboBox) return;

    QString currentSet = m_setComboBox->currentText();
    QString currentKas = m_kasComboBox->currentText();
    QString currentMesh = m_meshComboBox->currentText();

    m_meshComboBox->clear();
    m_meshComboBox->addItem("");

    if (!currentSet.isEmpty() && !currentKas.isEmpty()) {
        QSqlQuery query;
        query.prepare("SELECT DISTINCT mesh_id FROM mesh WHERE set_id = ? AND kas_id = ? ORDER BY mesh_id");
        query.addBindValue(currentSet);
        query.addBindValue(currentKas);

        if (query.exec()) {
            while (query.next()) {
                m_meshComboBox->addItem(query.value(0).toString());
            }

            // Восстанавливаем выбранный mesh_id если возможно
            int meshIndex = m_meshComboBox->findText(currentMesh);
            if (meshIndex >= 0) {
                m_meshComboBox->setCurrentIndex(meshIndex);
            }
        }
    }
}

void MainWindow::updateSetKidComboBox(QComboBox* comboBox)
{
    if (!comboBox) return;

    QString currentValue = comboBox->currentText();
    comboBox->clear();
    comboBox->addItem("");

    QSqlQuery query("SELECT DISTINCT set_id FROM set_kid_history ORDER BY set_id");
    while (query.next()) {
        comboBox->addItem(query.value(0).toString());
    }

    // Восстанавливаем предыдущее значение если оно есть
    int index = comboBox->findText(currentValue);
    if (index >= 0) {
        comboBox->setCurrentIndex(index);
    }
}

void MainWindow::updateSetKidComboBox(QComboBox* comboBox1, QComboBox* comboBox2)
{
    if (!comboBox1 || !comboBox2) return;

    // Сохраняем текущие значения
    QString currentSet = comboBox1->currentText();
    QString currentKas = comboBox2->currentText();

    // Очищаем комбобоксы
    comboBox1->clear();
    comboBox2->clear();
    comboBox1->addItem("");
    comboBox2->addItem("");

    // Кэш для быстрого доступа к kas_id по set_id
    QMap<QString, QStringList> setToKasMap;

    QSqlQuery query("SELECT DISTINCT set_id, kas_id FROM kas_kid_history ORDER BY set_id, kas_id");
    while (query.next()) {
        QString set_id = query.value("set_id").toString();
        QString kas_id = query.value("kas_id").toString();

        // Добавляем set_id в первый комбобокс (если еще нет)
        if (comboBox1->findText(set_id) == -1) {
            comboBox1->addItem(set_id);
        }

        // Сохраняем kas_id для этого set_id
        setToKasMap[set_id].append(kas_id);
    }

    // Восстанавливаем выбранный set_id
    int setIndex = comboBox1->findText(currentSet);
    if (setIndex >= 0) {
        comboBox1->setCurrentIndex(setIndex);

        // Заполняем второй комбобокс kas_id для выбранного set_id
        QStringList kasList = setToKasMap.value(currentSet);
        kasList.sort();
        for (const QString& kas : kasList) {
            comboBox2->addItem(kas);
        }

        // Восстанавливаем выбранный kas_id
        int kasIndex = comboBox2->findText(currentKas);
        if (kasIndex >= 0) {
            comboBox2->setCurrentIndex(kasIndex);
        }
    }

    // Подключаем сигналы для обновления второго комбобокса при изменении первого
    disconnect(comboBox1, &QComboBox::currentTextChanged, this, &MainWindow::onSetKidComboBoxChanged);
    connect(comboBox1, &QComboBox::currentTextChanged, this, &MainWindow::onSetKidComboBoxChanged);

    // Сохраняем ссылки на комбобоксы для использования в слоте
    m_setKidComboBox = comboBox1;
    m_kasKidComboBox = comboBox2;
    m_meshKidComboBox = nullptr; // Третий комбобокс не используется в этой перегрузке
}

void MainWindow::updateSetKidComboBox(QComboBox* comboBox1, QComboBox* comboBox2, QComboBox* comboBox3)
{
    if (!comboBox1 || !comboBox2 || !comboBox3) return;

    // Сохраняем текущие значения
    QString currentSet = comboBox1->currentText();
    QString currentKas = comboBox2->currentText();
    QString currentMesh = comboBox3->currentText();

    // Очищаем комбобоксы
    comboBox1->clear();
    comboBox2->clear();
    comboBox3->clear();
    comboBox1->addItem("");
    comboBox2->addItem("");
    comboBox3->addItem("");

    // Структуры для кэширования данных
    QMap<QString, QMap<QString, QStringList>> setKasMeshMap; // set_id -> kas_id -> mesh_id[]
    QMap<QString, QStringList> setToKasMap; // set_id -> kas_id[]

    QSqlQuery query("SELECT DISTINCT set_id, kas_id, mesh_id FROM mesh_kid_history ORDER BY set_id, kas_id, mesh_id");
    while (query.next()) {
        QString set_id = query.value("set_id").toString();
        QString kas_id = query.value("kas_id").toString();
        QString mesh_id = query.value("mesh_id").toString();

        // Добавляем set_id в первый комбобокс (если еще нет)
        if (comboBox1->findText(set_id) == -1) {
            comboBox1->addItem(set_id);
        }

        // Сохраняем связь set_id -> kas_id
        if (!setToKasMap[set_id].contains(kas_id)) {
            setToKasMap[set_id].append(kas_id);
        }

        // Сохраняем полную связь set_id -> kas_id -> mesh_id
        setKasMeshMap[set_id][kas_id].append(mesh_id);
    }

    // Восстанавливаем выбранный set_id
    int setIndex = comboBox1->findText(currentSet);
    if (setIndex >= 0) {
        comboBox1->setCurrentIndex(setIndex);

        // Заполняем второй комбобокс kas_id для выбранного set_id
        QStringList kasList = setToKasMap.value(currentSet);
        kasList.sort();
        for (const QString& kas : kasList) {
            comboBox2->addItem(kas);
        }

        // Восстанавливаем выбранный kas_id
        int kasIndex = comboBox2->findText(currentKas);
        if (kasIndex >= 0) {
            comboBox2->setCurrentIndex(kasIndex);

            // Заполняем третий комбобокс mesh_id для выбранных set_id и kas_id
            QStringList meshList = setKasMeshMap.value(currentSet).value(currentKas);
            meshList.sort();
            for (const QString& mesh : meshList) {
                comboBox3->addItem(mesh);
            }

            // Восстанавливаем выбранный mesh_id
            int meshIndex = comboBox3->findText(currentMesh);
            if (meshIndex >= 0) {
                comboBox3->setCurrentIndex(meshIndex);
            }
        }
    }

    // Подключаем сигналы для обновления зависимых комбобоксов
    disconnect(comboBox1, &QComboBox::currentTextChanged, this, &MainWindow::onSetKidComboBoxChanged);
    disconnect(comboBox2, &QComboBox::currentTextChanged, this, &MainWindow::onKasKidComboBoxChanged);

    connect(comboBox1, &QComboBox::currentTextChanged, this, &MainWindow::onSetKidComboBoxChanged);
    connect(comboBox2, &QComboBox::currentTextChanged, this, &MainWindow::onKasKidComboBoxChanged);

    // Сохраняем ссылки на комбобоксы для использования в слотах
    m_setKidComboBox = comboBox1;
    m_kasKidComboBox = comboBox2;
    m_meshKidComboBox = comboBox3;
}

// Вспомогательные слоты для обновления зависимых списков истории КИД
void MainWindow::onSetKidComboBoxChanged()
{
    if (!m_setKidComboBox || !m_kasKidComboBox) return;

    QString currentSet = m_setKidComboBox->currentText();
    QString currentKas = m_kasKidComboBox->currentText();

    m_kasKidComboBox->clear();
    m_kasKidComboBox->addItem("");

    if (m_meshKidComboBox) {
        m_meshKidComboBox->clear();
        m_meshKidComboBox->addItem("");
    }

    if (!currentSet.isEmpty()) {
        QSqlQuery query;
        if (m_meshKidComboBox) {
            // Для трехкомбобоксного режима (из таблицы mesh_kid_history)
            query.prepare("SELECT DISTINCT kas_id FROM mesh_kid_history WHERE set_id = ? ORDER BY kas_id");
        } else {
            // Для двухкомбобоксного режима (из таблицы kas_kid_history)
            query.prepare("SELECT DISTINCT kas_id FROM kas_kid_history WHERE set_id = ? ORDER BY kas_id");
        }
        query.addBindValue(currentSet);

        if (query.exec()) {
            while (query.next()) {
                m_kasKidComboBox->addItem(query.value(0).toString());
            }

            // Восстанавливаем выбранный kas_id если возможно
            int kasIndex = m_kasKidComboBox->findText(currentKas);
            if (kasIndex >= 0) {
                m_kasKidComboBox->setCurrentIndex(kasIndex);
            }
        }
    }
}

void MainWindow::onKasKidComboBoxChanged()
{
    if (!m_setKidComboBox || !m_kasKidComboBox || !m_meshKidComboBox) return;

    QString currentSet = m_setKidComboBox->currentText();
    QString currentKas = m_kasKidComboBox->currentText();
    QString currentMesh = m_meshKidComboBox->currentText();

    m_meshKidComboBox->clear();
    m_meshKidComboBox->addItem("");

    if (!currentSet.isEmpty() && !currentKas.isEmpty()) {
        QSqlQuery query;
        query.prepare("SELECT DISTINCT mesh_id FROM mesh_kid_history WHERE set_id = ? AND kas_id = ? ORDER BY mesh_id");
        query.addBindValue(currentSet);
        query.addBindValue(currentKas);

        if (query.exec()) {
            while (query.next()) {
                m_meshKidComboBox->addItem(query.value(0).toString());
            }

            // Восстанавливаем выбранный mesh_id если возможно
            int meshIndex = m_meshKidComboBox->findText(currentMesh);
            if (meshIndex >= 0) {
                m_meshKidComboBox->setCurrentIndex(meshIndex);
            }
        }
    }
}


