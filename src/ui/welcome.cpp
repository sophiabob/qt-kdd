#include "welcome.h"
#include "ui_welcome.h"

#include "mainwindow.h"

Welcome::Welcome(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Welcome)
{
    ui->setupUi(this);
    //this->showFullScreen(); //на очень полный экран

    qDebug() << "-\nДоступные драйверы для подключения к базе данных: " << QSqlDatabase::drivers() << "\n";

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL"); //QSQLITE - да, QPSQL - нет
    //createConnection();
    //db.close();
    ui->lineEdit_ip->hide();

    QString os = detectOS();
    if (os == "Windows") {
        //this->showFullScreen();
    } else if (os == "Linux") {
        this->showFullScreen();
    } else {
        qDebug() << "ОС не распознана";
    }
}

Welcome::~Welcome()
{
    delete ui;
}


void Welcome::on_btnCencel_pressed()
{
    close();
}



bool Welcome::on_btnSignIn_clicked()
{

    createConnection();
    //inputLogin
    //inputPassword

    QString Login = ui->inputLogin->text();
    QString Password = ui->inputPassword->text();

    // SQL-запрос для добавления данных
    QSqlQuery query;
    //id порядковый номер

    //QMessageBox::information(this, "данные", "Логин: :Login, пароль: :Password .");

    query.prepare("SELECT user_id FROM users WHERE login = :login AND password = :password AND role != :role LIMIT 1");
    query.bindValue(":login", Login);
    query.bindValue(":password", Password);
    query.bindValue(":role", "Пользователь");

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка запроса", query.lastError().text());
        return false;
    }

    if (query.next()) {
        int userId = query.value(0).toInt();
        //QMessageBox::information(this, "Успех", QString("Пользователь найден! ID: %1").arg(userId));
        hide();
        window = new MainWindow(userId, this);
        window->show();
        return true;
    } else {
        QMessageBox::warning(this, "Ошибка", "Неверный логин / пароль или у Вас нет доступа");
        return false;
    }

/*
    hide();
    window = new MainWindow(this);
    window->show();*/

}




void Welcome::on_pushButton_help_pressed()
{
    // Ñîçäàåì overlay êàê îáû÷íûé âèäæåò
    QWidget *overlay = new QWidget(this);
    overlay->setObjectName("helpOverlay");
    overlay->setStyleSheet("background-color: rgba(0, 0, 0, 150);");
    overlay->setGeometry(0, 0, width(), height());
    overlay->setFocusPolicy(Qt::NoFocus);

    // Ñîçäàåì label äëÿ GIF
    QLabel *gifLabel = new QLabel(overlay);
    gifLabel->setFixedSize(600, 400);
    gifLabel->setStyleSheet("background-color: white; border-radius: 10px;");
    gifLabel->setAlignment(Qt::AlignCenter);

    // Çàãðóæàåì GIF
    QMovie *movie = new QMovie(gifLabel);
    QString gifPath = "/home/sds/sh18/kdd_17.11.25/img/help.gif";//"C:/Users/sophia/Documents/work work work/kdd/img/help.gif";

    if (!QFile::exists(gifPath)) {
        QMessageBox::warning(this, "Îøèáêà", "Ôàéë ïîìîùè íå íàéäåí: " + gifPath);
        delete overlay;
        return;
    }

    movie->setFileName(gifPath);
    gifLabel->setMovie(movie);

    // Ðàçìåùàåì ïî öåíòðó
    QVBoxLayout *layout = new QVBoxLayout(overlay);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch();
    layout->addWidget(gifLabel, 0, Qt::AlignCenter);
    layout->addStretch();

    overlay->setAttribute(Qt::WA_DeleteOnClose);
    connect(overlay, SIGNAL(destroyed()), movie, SLOT(deleteLater()));

    overlay->raise();
    overlay->show();
    movie->start();

    overlay->installEventFilter(this);
}

bool Welcome::eventFilter(QObject *obj, QEvent *event)
{
    if (obj->objectName() == "helpOverlay" && event->type() == QEvent::MouseButtonPress) {
        QWidget *overlay = qobject_cast<QWidget*>(obj);
        if (overlay) {
            QLabel *gifLabel = overlay->findChild<QLabel*>();
            if (gifLabel) {
                QMovie *movie = gifLabel->movie();
                if (movie) {
                    movie->stop();
                }
            }
            overlay->deleteLater();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event); // Äëÿ Welcome èñïîëüçóåì QWidget
}

// Функция поиска файла настроек
QString Welcome::findSettingsFile()
{
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/settings.ini",
        QDir::currentPath() + "/settings.ini",
        QDir::homePath() + "/.config/" + QCoreApplication::applicationName() + "/settings.ini",
        "./settings.ini"
    };

    for (const QString &path : searchPaths) {
        if (QFileInfo::exists(path)) {
            return path;
        }
    }
         // Создаем файл с настройками по умолчанию
    QString defaultPath = QCoreApplication::applicationDirPath() + "/settings.ini";
    QSettings defaultSettings(defaultPath, QSettings::IniFormat);
    defaultSettings.beginGroup("Database");
    defaultSettings.setValue("UserName", "postgres");
    defaultSettings.setValue("Password", "postgres1");
    defaultSettings.setValue("Name", "newdb");
    defaultSettings.setValue("Host", "localhost");
    defaultSettings.setValue("Port", 5432);
    defaultSettings.endGroup();
    defaultSettings.sync();

    return defaultPath;
}

// Функция для чтения настроек из файла
void Welcome::readDatabaseSettings(QString &user, QString &password,
                                   QString &database, QString &host, int &port,
                                   bool keepHost)
{
    QString settingsPath = findSettingsFile();

    if (settingsPath.isEmpty()) {
        qDebug() << "Файл settings.ini не найден";
        return;
    }

    QSettings settings(settingsPath, QSettings::IniFormat);

    // Проверяем, какие секции есть в файле
    //qDebug() << "Секции в файле:" << settings.childGroups();
    settings.beginGroup("Settings");
    //qDebug() << "Ключи в секции Settings:" << settings.childKeys();
    settings.endGroup();

    // Читаем настройки из секции [Settings]
    if (user.isEmpty()) {
        // Пробуем разные варианты ключей
        user = settings.value("Settings/UserName",
                              settings.value("Settings/Server_UserName",
                                             settings.value("UserName", ""))).toString();
    }

    if (password.isEmpty()) {
        password = settings.value("Settings/Password",
                                  settings.value("Settings/Server_Password",
                                                 settings.value("Password", ""))).toString();
    }

    if (database.isEmpty()) {
        database = settings.value("Settings/DatabaseName",
                                  settings.value("Settings/Server_DatabaseName",
                                                 settings.value("DatabaseName", "newdb"))).toString();
    }

    // Хост читаем только если keepHost = false или host еще не установлен
    if (!keepHost && host.isEmpty()) {
        // Пробуем разные варианты ключей для хоста
        QString hostWithPort = settings.value("Settings/ip_database",
                                              settings.value("Settings/Server_ip_database",
                                                             settings.value("ip_database", ""))).toString();

        // Разделяем хост и порт, если они вместе
        if (hostWithPort.contains(':')) {
            QStringList parts = hostWithPort.split(':');
            host = parts[0];
            bool ok;
            port = (parts.size() > 1) ? parts[1].toInt(&ok) : port;
            if (!ok) port = 5432;
        } else {
            host = hostWithPort;
            // Пробуем прочитать порт отдельно (если есть)
            int savedPort = settings.value("Settings/Port", port).toInt();
            if (savedPort > 0) port = savedPort;
        }
    }

    //qDebug() << "Источник - ini";
   /* // Отладочный вывод
    qDebug() << "Прочитаны настройки:";
    qDebug() << "  User:" << user;
    qDebug() << "  Password:" << (password.isEmpty() ? "не задан" : "[установлен]");
    qDebug() << "  Database:" << database;
    qDebug() << "  Host:" << host;
    qDebug() << "  Port:" << port;*/
}


void Welcome::on_pushButton_pressed()
{
    //QString dbName = "newdb";

    // Закрываем и удаляем все существующие подключения
    QStringList connections = QSqlDatabase::connectionNames();
    for (const QString &connectionName : connections) {
        QSqlDatabase::removeDatabase(connectionName);
    }

    // Получаем параметры подключения
    QString host, user, password;
    int port = 5432;

    // Используем ту же логику, что и в createConnection()
    QString dbName, dbUser, dbPassword, dbHost;
    int dbPort = 5432;

    // 1. Сначала проверяем, есть ли IP в lineEdit_ip
    QString ipFromUI = ui->lineEdit_ip->text().trimmed();

    if (!ipFromUI.isEmpty()) {
        // Проверяем валидность IP
        QHostAddress address;
        if (address.setAddress(ipFromUI)) {
            // Используем IP из поля ввода
            dbHost = ipFromUI;
            qDebug() << "Используется IP из поля ввода:" << dbHost;
        } else {
            qWarning() << "Неверный IP адрес в поле ввода:" << ipFromUI;
        }
    }

    // 2. Если IP из поля ввода не задан или невалиден, читаем из settings.ini
    if (dbHost.isEmpty()) {
        readDatabaseSettings(dbUser, dbPassword, dbName, dbHost, dbPort, false);
    }

    // 3. Если после чтения настроек хост все еще пустой, используем OS-специфичные настройки
    if (dbHost.isEmpty()) {
        QString os = detectOS();
        if (os == "Windows") {
            this->showNormal();
            dbUser = "postgres";
            dbPassword = "postgres";
            dbHost = "localhost";
        } else if (os == "Linux") {
            this->showFullScreen();
            dbUser = "postgres";
            dbPassword = "postgres1";
            dbHost = "localhost";
        } else {
            qDebug() << "Используются OS-специфичные настройки для" << os;
        }
        qDebug() << "Источник - вручную";
    }

    // Используем полученные настройки
    host = dbHost;
    port = dbPort;
    user = dbUser;
    password = dbPassword;

    qDebug() << "Параметры подключения для создания базы:";
    qDebug() << "Хост:" << host;
    qDebug() << "Порт:" << port;
    qDebug() << "Пользователь:" << user;
    qDebug() << "Пароль:" << "[скрыто]";

    // Проверяем подключение к серверу PostgreSQL (к базе postgres)
    {
        QString testConnectionName = "test_connection_" + QString::number(QDateTime::currentMSecsSinceEpoch());
        QSqlDatabase testDb = QSqlDatabase::addDatabase("QPSQL", testConnectionName);

        // Настраиваем подключение к системной базе postgres
        testDb.setHostName(host);
        testDb.setPort(port);
        testDb.setUserName(user);
        testDb.setPassword(password);
        testDb.setDatabaseName("postgres");

        if (!testDb.open()) {
            QMessageBox::critical(this, "Ошибка",
                                  "Не удалось подключиться к серверу PostgreSQL: " + testDb.lastError().text());
            QSqlDatabase::removeDatabase(testConnectionName);
            return;
        }

        // Проверяем существование базы данных
        QSqlQuery checkQuery(testDb);
        if (!checkQuery.exec("SELECT 1 FROM pg_database WHERE datname = '" + dbName + "'") || !checkQuery.next()) {
            // Базы не существует, создаем её
            if (!checkQuery.exec("CREATE DATABASE " + dbName)) {
                QMessageBox::critical(this, "Ошибка",
                                      "Не удалось создать базу данных: " + checkQuery.lastError().text());
                testDb.close();
                QSqlDatabase::removeDatabase(testConnectionName);
                return;
            }
            QMessageBox::information(this, "Информация", "База данных " + dbName + " создана успешно");
        } else {
            qDebug() << "База данных" << dbName << "уже существует";
        }

        // Закрываем тестовое соединение
        testDb.close();
        QSqlDatabase::removeDatabase(testConnectionName);
    }

    // Создаем основное подключение к целевой базе данных
    QString connectionName = QString("main_connection_%1").arg(QDateTime::currentMSecsSinceEpoch());
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", connectionName);

    // Настраиваем подключение к нашей базе
    db.setHostName(host);
    db.setPort(port);
    db.setUserName(user);
    db.setPassword(password);
    db.setDatabaseName(dbName);

    if (!db.open()) {
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось подключиться к базе данных: " + db.lastError().text());
        QSqlDatabase::removeDatabase(connectionName);
        return;
    }

    qDebug() << "Успешное подключение к базе данных!";

    // Запрос подтверждения действия
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение",
                                  "Вы уверены, что хотите удалить все таблицы и создать заново?\nЭто действие нельзя отменить!",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
        return;
    }

    // Используем одно соединение для всех операций
    QSqlQuery query(db);

    // Отключаем проверку внешних ключей для безопасного удаления
    if (!query.exec("SET session_replication_role = 'replica'")) {
        QMessageBox::warning(this, "Предупреждение",
                             "Не удалось отключить проверку внешних ключей: " + query.lastError().text());
    }

    // Получаем список всех таблиц для удаления
    QStringList tablesToDrop;
    if (query.exec("SELECT table_name FROM information_schema.tables WHERE table_schema = 'public' ORDER BY table_name")) {
        while (query.next()) {
            tablesToDrop << query.value(0).toString();
        }
        qDebug() << "Найдено таблиц для удаления:" << tablesToDrop.size();
    } else {
        QMessageBox::critical(this, "Ошибка", "Ошибка получения списка таблиц: " + query.lastError().text());
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
        return;
    }

    // Удаляем таблицы если они существуют
    if (!tablesToDrop.isEmpty()) {
        // Удаляем все таблицы по одной для лучшего контроля
        for (const QString &tableName : tablesToDrop) {
            if (!query.exec(QString("DROP TABLE IF EXISTS %1 CASCADE").arg(tableName))) {
                QMessageBox::warning(this, "Предупреждение",
                                     QString("Не удалось удалить таблицу %1: %2").arg(tableName).arg(query.lastError().text()));
            }
        }
    } else {
        qDebug() << "Нет таблиц для удаления, продолжаем создание новых таблиц";
    }

    // Включаем проверку внешних ключей обратно
    query.exec("SET session_replication_role = 'origin'");

    // [ОСТАВЛЯЕМ ВЕСЬ КОД СОЗДАНИЯ ТАБЛИЦ БЕЗ ИЗМЕНЕНИЙ]
    // ... (ваш существующий код создания таблиц остается без изменений)

    // Создаем таблицы заново
    QStringList sqlCommands = {
        // Таблица users
        R"(CREATE TABLE users(
        user_id int primary key,
        login CHARACTER VARYING(30) UNIQUE,
        password CHARACTER VARYING(30),
        name_0 CHARACTER VARYING(30),
        name_1 CHARACTER VARYING(30),
        name_2 CHARACTER VARYING(30),
        gender CHARACTER VARYING(3),
        birthday date,
        role CHARACTER VARYING(30),
        snils CHARACTER VARYING(30),
        tab_num int UNIQUE,
        department CHARACTER VARYING(30),
        card_id CHARACTER VARYING(30) UNIQUE,
        set_ID int NOT NULL,
        kas_ID int NOT NULL,
        mesh_ID int NOT NULL,
        doz_tld_id CHARACTER VARYING(30),
        cell_date date,
        dose_year float8,
        dose_year_now float8,
        dose_year_now_ppd float8,
        code CHARACTER VARYING(50) UNIQUE,
        block CHARACTER VARYING(30),
        last_update timestamp
    ))",

        // Таблица user_photo
        R"(CREATE TABLE user_photo(
        user_id bigint,
        user_photo bytea
    ))",

        // Таблица duty
        R"(CREATE TABLE duty(
        duty_id SERIAL PRIMARY KEY,
        type_duty INTEGER NOT NULL,
        blok VARCHAR(100),
        room VARCHAR(100),
        equipment VARCHAR(100),
        name_work VARCHAR(200),
        type_work VARCHAR(200),
        time INTEGER,
        dose_a_1 NUMERIC(10,4) DEFAULT 0.1,
        dose_a_2 NUMERIC(10,4) DEFAULT 1.0,
        rate_a_1 NUMERIC(10,4) DEFAULT 0.1,
        rate_a_2 NUMERIC(10,4) DEFAULT 1.0,
        dose_b_1 NUMERIC(10,4) DEFAULT 0.1,
        dose_b_2 NUMERIC(10,4) DEFAULT 1.0,
        rate_b_1 NUMERIC(10,4) DEFAULT 0.1,
        rate_b_2 NUMERIC(10,4) DEFAULT 1.0,
        dose_g_1 NUMERIC(10,4) DEFAULT 0.1,
        dose_g_2 NUMERIC(10,4) DEFAULT 1.0,
        rate_g_1 NUMERIC(10,4) DEFAULT 0.1,
        rate_g_2 NUMERIC(10,4) DEFAULT 1.0,
        dose_n_1 NUMERIC(10,4) DEFAULT 0.01,
        dose_n_2 NUMERIC(10,4) DEFAULT 0.1,
        rate_n_1 NUMERIC(10,4) DEFAULT 0.01,
        rate_n_2 NUMERIC(10,4) DEFAULT 0.1,
        duty_note TEXT,
        last_update TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    ))",

        // Таблица users_duty
        R"(CREATE TABLE users_duty(
        users_duty_id int primary key,
        user_id int,
        duty_id int,
        date_start date,
        date_finish date,
        users_duty_note CHARACTER VARYING(30)
    ))",

        // Таблица set
        R"(CREATE TABLE set(
        set_id int primary key,
        set_name CHARACTER VARYING(30),
        ip_set inet,
        set_quantity int,
        set_block CHARACTER VARYING(30),
        set_note CHARACTER VARYING(30),
        last_update timestamp
    ))",

        // Таблица kas
        R"(CREATE TABLE kas(
        set_id int,
        kas_id int,
        kas_name CHARACTER VARYING(30),
        kas_height int,
        kas_width int,
        kas_block CHARACTER VARYING(30),
        kas_note CHARACTER VARYING(30),
        last_update timestamp,
        PRIMARY KEY (kas_id, set_id)
    ))",

        // Таблица mesh
        R"(CREATE TABLE mesh(
        set_id int,
        kas_id int,
        mesh_id int,
        user_id int,
        doz_tld_id CHARACTER VARYING(30),
        mesh_status int,
        mesh_note CHARACTER VARYING(30),
        last_update timestamp,
        PRIMARY KEY (kas_id, set_id, mesh_id)
    ))",

        // Таблица users_history
        R"(CREATE TABLE users_history (
        user_id INTEGER NOT NULL,
        login VARCHAR(50) NOT NULL,
        password VARCHAR(100) NOT NULL,
        name_0 VARCHAR(50) NOT NULL,
        name_1 VARCHAR(50) NOT NULL,
        name_2 VARCHAR(50),
        gender VARCHAR(10),
        birthday DATE,
        role VARCHAR(30),
        tab_num VARCHAR(20) NOT NULL,
        department VARCHAR(100),
        card_id VARCHAR(30) NOT NULL,
        set_inf INTEGER NOT NULL DEFAULT 1,
        kas_inf INTEGER NOT NULL DEFAULT 1,
        mesh_inf INTEGER NOT NULL DEFAULT 1,
        doz_tld_id VARCHAR(30) NOT NULL,
        cell_date DATE,
        dose_year DECIMAL(10,4),
        dose_year_now DECIMAL(10,4),
        dose_year_now_ppd DECIMAL(10,4),
        code VARCHAR(30),
        block VARCHAR(30),
        last_update TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        user_id_change INTEGER,
        type_edit VARCHAR(30) DEFAULT 'create'
    ))",

        // Таблица duty_history
        R"(CREATE TABLE duty_history (
        duty_id VARCHAR(100),
        type_duty INTEGER,
        blok VARCHAR(100),
        room VARCHAR(100),
        equipment VARCHAR(100),
        name_work VARCHAR(200),
        type_work VARCHAR(200),
        time INTEGER,
        dose_a_1 NUMERIC(10,4),
        dose_a_2 NUMERIC(10,4),
        rate_a_1 NUMERIC(10,4),
        rate_a_2 NUMERIC(10,4),
        dose_b_1 NUMERIC(10,4),
        dose_b_2 NUMERIC(10,4),
        rate_b_1 NUMERIC(10,4),
        rate_b_2 NUMERIC(10,4),
        dose_g_1 NUMERIC(10,4),
        dose_g_2 NUMERIC(10,4),
        rate_g_1 NUMERIC(10,4),
        rate_g_2 NUMERIC(10,4),
        dose_n_1 NUMERIC(10,4),
        dose_n_2 NUMERIC(10,4),
        rate_n_1 NUMERIC(10,4),
        rate_n_2 NUMERIC(10,4),
        duty_note TEXT,
        last_update TIMESTAMP,
        user_id_change INTEGER NOT NULL,
        last_update_change TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        type_edit VARCHAR(30) DEFAULT 'create'
    ))",

        // Таблица users_duty_history
        R"(CREATE TABLE users_duty_history (
        users_duty_id INTEGER,
        user_id INTEGER,
        duty_id INTEGER,
        date_start DATE,
        date_finish DATE,
        users_duty_note TEXT,
        last_update_change TIMESTAMP,
        user_id_change INTEGER NOT NULL,
        last_update TIMESTAMP,
        type_edit VARCHAR(30) DEFAULT 'create',
        change_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    ))",

        // Таблица set_history
        R"(CREATE TABLE set_history (
        set_id INTEGER NOT NULL,
        set_name VARCHAR(30),
        ip_set INET,
        set_quantity INTEGER,
        set_block VARCHAR(30),
        set_note VARCHAR(30),
        last_update TIMESTAMP,
        user_id_change INTEGER,
        last_update_change TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        type_edit VARCHAR(30) DEFAULT 'create'
    ))",

        // Таблица kas_history
        R"(CREATE TABLE kas_history (
        set_id INTEGER NOT NULL,
        kas_id INTEGER NOT NULL,
        kas_name VARCHAR(30),
        kas_height INTEGER,
        kas_width INTEGER,
        kas_block VARCHAR(30),
        kas_note VARCHAR(30),
        last_update TIMESTAMP,
        user_id_change INTEGER NOT NULL,
        last_update_change TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        type_edit VARCHAR(30) DEFAULT 'create'
    ))",

        // Таблица mesh_history
        R"(CREATE TABLE mesh_history (
        set_id INTEGER NOT NULL,
        kas_id INTEGER NOT NULL,
        mesh_id INTEGER NOT NULL,
        user_id INTEGER,
        doz_tld_id VARCHAR(30),
        mesh_status INTEGER DEFAULT 0,
        mesh_note VARCHAR(30),
        last_update TIMESTAMP,
        user_id_change INTEGER NOT NULL,
        last_update_change TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        type_edit VARCHAR(30) DEFAULT 'create'
    ))",

        // Таблица dose_ppd_history
        //R"(CREATE TABLE dose_ppd_history(dose_ppd_id int, user_id int, users_duty_id int, nomer_pdd CHARACTER VARYING(30), time int, time_max timestamp, type_ppd smallint, dose float8 DEFAULT 0, rate_max float8, dose_ppd_note CHARACTER VARYING(30), start_work timestamp, finish_work timestamp, last_update timestamp, user_id_change int, last_update_change timestamp))",

        // Таблица dose_ppd
        R"(CREATE TABLE dose_ppd(
        dose_ppd_id int primary key,
        user_id int,
        users_duty_id int,
        nomer_pdd CHARACTER VARYING(30),
        time int,
        time_max timestamp,
        type_ppd smallint,
        dose float8 DEFAULT 0,
        rate_max float8,
        dose_ppd_note CHARACTER VARYING(30),
        start_work timestamp,
        finish_work timestamp,
        last_update timestamp
    ))",

        // Таблица dose_ppd_history
        R"(CREATE TABLE dose_ppd_history(
        dose_ppd_id int,
        user_id int,
        users_duty_id int,
        nomer_pdd CHARACTER VARYING(30),
        time int, time_max timestamp,
        type_ppd smallint,
        dose float8 DEFAULT 0,
        rate_max float8,
        dose_ppd_note CHARACTER VARYING(30),
        start_work timestamp,
        finish_work timestamp,
        last_update timestamp,
        user_id_change int,
        last_update_change timestamp,
        type_edit VARCHAR(30) DEFAULT 'create'
    ))",

        // Таблица dose_ppd_history
        R"(CREATE TABLE set_kid(set_id int primary key, set_name CHARACTER VARYING(30), ip_set inet, set_quantity int, set_block CHARACTER VARYING(30), set_note CHARACTER VARYING(30), last_update timestamp);)",

        // Таблица dose_ppd_history
        R"(CREATE TABLE kas_kid(set_id int, kas_id int, kas_name CHARACTER VARYING(30) , kas_height int, kas_width int, kas_block CHARACTER VARYING(30), kas_note CHARACTER VARYING(30), last_update timestamp, PRIMARY KEY (kas_id, set_id));)",

        // Таблица dose_ppd_history
        R"(CREATE TABLE mesh_kid(set_id int, kas_id int,  mesh_id int, user_id int, doz_tld_id CHARACTER VARYING(30), mesh_status int, mesh_note CHARACTER VARYING(30), last_update timestamp, PRIMARY KEY (kas_id, set_id, mesh_id));)",

        R"(CREATE TABLE set_kid_history (
        set_id INTEGER NOT NULL,
        set_name VARCHAR(30),
        ip_set INET,
        set_quantity INTEGER,
        set_block VARCHAR(30),
        set_note VARCHAR(30),
        last_update TIMESTAMP,
        user_id_change INTEGER,
        last_update_change TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        type_edit VARCHAR(30) DEFAULT 'create'
    ))",

        // Таблица kas_kid_history для записи истории изменений кассетниц КИД
        R"(CREATE TABLE kas_kid_history (
        set_id INTEGER NOT NULL,
        kas_id INTEGER NOT NULL,
        kas_name VARCHAR(30),
        kas_height INTEGER,
        kas_width INTEGER,
        kas_block VARCHAR(30),
        kas_note VARCHAR(30),
        last_update TIMESTAMP,
        user_id_change INTEGER NOT NULL,
        last_update_change TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        type_edit VARCHAR(30) DEFAULT 'create'
    ))",

        // Таблица mesh_kid_history для записи истории изменений ячеек КИД
        R"(CREATE TABLE mesh_kid_history (
        set_id INTEGER NOT NULL,
        kas_id INTEGER NOT NULL,
        mesh_id INTEGER NOT NULL,
        user_id INTEGER,
        doz_tld_id VARCHAR(30),
        mesh_status INTEGER DEFAULT 0,
        mesh_note VARCHAR(30),
        last_update TIMESTAMP,
        user_id_change INTEGER NOT NULL,
        last_update_change TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        type_edit VARCHAR(30) DEFAULT 'create'
    ))"
    };
    // Создаем таблицы
    int createdCount = 0;
    int errorCount = 0;

    for (const QString &sql : sqlCommands) {
        if (query.exec(sql)) {
            createdCount++;
        } else {
            errorCount++;
            qDebug() << "Ошибка создания таблицы:" << query.lastError().text();
        }
    }

    // Создаем администратора
    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO users (user_id, login, password, name_0, name_1, name_2, role, tab_num, card_id, doz_tld_id, set_id, kas_id, mesh_id) "
                        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    insertQuery.addBindValue("1");
    insertQuery.addBindValue("admin");
    insertQuery.addBindValue("admin");
    insertQuery.addBindValue("Администратор");
    insertQuery.addBindValue("Администратор");
    insertQuery.addBindValue("Администратор");
    insertQuery.addBindValue("Администратор");
    insertQuery.addBindValue(1);
    insertQuery.addBindValue("admin_1");
    insertQuery.addBindValue("admin_1");
    insertQuery.addBindValue("1");
    insertQuery.addBindValue("1");
    insertQuery.addBindValue("1");

    QString message;
    if (insertQuery.exec()) {
        message = QString("База данных пересоздана успешно!\n\n"
                          "Создано таблиц: %1\n"
                          "Ошибок: %2\n\n"
                          "Создан пользователь Администратор:\n"
                          "Логин: admin\nПароль: admin\n"
                          "ФИО: Администратор Администратор Администратор\nРоль: admin \n\n"
                          "Перезапустите приложение")
                      .arg(createdCount).arg(errorCount);
        QMessageBox::information(this, "Готово", message);
    } else {
        message = QString("Таблицы созданы (успешно: %1, ошибок: %2), но не удалось создать пользователя admin: %3")
                      .arg(createdCount).arg(errorCount).arg(insertQuery.lastError().text());
        QMessageBox::warning(this, "Ошибка", message);
    }

    db.close();
    QSqlDatabase::removeDatabase(connectionName);
}

bool Welcome::createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    QString dbUser, dbPassword, dbName, dbHost;
    int dbPort = 5432;
    QString dataSource = "";

    // ШАГ 1: ПЕРВАЯ ПОПЫТКА ПОДКЛЮЧЕНИЯ

    // 1.1. Проверяем IP из поля ввода
    QString ipFromUI = ui->lineEdit_ip->text().trimmed();
    if (!ipFromUI.isEmpty()) {
        QHostAddress address;
        if (address.setAddress(ipFromUI)) {
            dbHost = ipFromUI;
            dataSource = "Поле ввода IP";
            qDebug() << "ПЕРВАЯ попытка: Используется IP из поля ввода:" << dbHost;
        } else {
            qWarning() << "Неверный IP адрес в поле ввода:" << ipFromUI;
        }
    }

    // 1.2. Если нет IP из UI, читаем из settings.ini
    if (dbHost.isEmpty()) {
        readDatabaseSettings(dbUser, dbPassword, dbName, dbHost, dbPort, false);
        dataSource = "Файл settings.ini";
        qDebug() << "ПЕРВАЯ попытка: Используются настройки из settings.ini";
    }

    // 1.3. Если хост все еще пустой, используем OS-специфичные настройки
    if (dbHost.isEmpty()) {
        QString os = detectOS();
        if (os == "Windows") {
            this->showNormal();
            dbUser = "postgres";
            dbPassword = "0";
            dbName = "postgres";
            dbHost = "localhost";
        } else if (os == "Linux") {
            this->showFullScreen();
            dbUser = "postgres";
            dbPassword = "postgres1";
            dbName = "newdb";
            dbHost = "localhost";
        } else {
            qDebug() << "OS не опознана" << os;
        }
        dataSource = "OS-специфичные настройки";
        qDebug() << "ВТОРАЯ попытка: Используются OS-специфичные настройки для" << os;
    }

    // 1.4. Пытаемся подключиться с первыми параметрами
    db.setHostName(dbHost);
    db.setPort(dbPort);
    db.setUserName(dbUser);
    db.setPassword(dbPassword);
    db.setDatabaseName(dbName);

    //qDebug() << "Источник:" << dataSource;
    qDebug() << "Хост:" << dbHost << "Порт:" << dbPort
             << "Пользователь:" << dbUser << "База:" << dbName;

    if (db.open()) {
        qDebug() << "Успешное подключение (первая попытка) \n-";
        return true;
    }

    // Если первая попытка не удалась
    qDebug() << "Ошибка подключения (первая попытка):" << db.lastError().text();

    // ШАГ 2: ВТОРАЯ ПОПЫТКА ПОДКЛЮЧЕНИЯ (локальные настройки)

    // Закрываем предыдущее подключение если оно открыто
    if (db.isOpen()) {
        db.close();
    } else {
        QMessageBox::critical(this, "Ошибка подключения 1",
                              QString("Не удалось подключиться к базе данных после двух попыток:\n\n"
                                      "ПЕРВАЯ попытка (%1):\n"
                                      "Хост: %2\nПорт: %3\nПользователь: %4\nБаза: %5\n"
                                      "Ошибка: %6\n\n"
                                      "ВТОРАЯ попытка (%7):\n"
                                      "Хост: %8\nПорт: %9\nПользователь: %10\nБаза: %11\n"
                                      "Ошибка: %12")
                                  .arg(dataSource)
                                  .arg(dbHost).arg(dbPort).arg(dbUser).arg(dbName)
                                  .arg(db.lastError().text())
                                  //.arg(newDataSource)
                                  .arg(dbHost).arg(dbPort).arg(dbUser).arg(dbName)
                                  .arg(db.lastError().text()));
    }

    // 2.1. Получаем локальные настройки для второй попытки
    QString os = detectOS();
    QString newDataSource = "ВТОРАЯ попытка: локальные настройки из кода";

    if (os == "Windows") {
        dbUser = "postgres_local";
        dbPassword = "postgres";
        dbName = "postgres"; // Или ваша база по умолчанию
        dbHost = "localhost";
        dbPort = 5432;
    } else if (os == "Linux") {
        dbUser = "postgres";
        dbPassword = "postgres1";
        dbName = "postgres"; // Или ваша база по умолчанию
        dbHost = "localhost";
        dbPort = 5432;
    } else {
        qDebug() << "ОС не распознана";
    }

    // 2.2. Устанавливаем новые параметры
    db.setHostName(dbHost);
    db.setPort(dbPort);
    db.setUserName(dbUser);
    db.setPassword(dbPassword);
    db.setDatabaseName(dbName);

    //qDebug() << "\nВТОРАЯ попытка подключения с параметрами:";
    //qDebug() << "Источник:" << newDataSource;
    qDebug() << "Хост:" << dbHost << "Порт:" << dbPort << "Пользователь:" << dbUser << "База:" << dbName;

    // 2.3. Пытаемся подключиться второй раз
    if (db.open()) {
        qDebug() << "Успешное подключение (вторая попытка) \n-";
        //qDebug() << "Итоговый источник данных:" << newDataSource;
        return true;
    }

    // Если обе попытки не удались
    qDebug() << "Ошибка подключения (вторая попытка):" << db.lastError().text();

    // Выводим подробную информацию об ошибке второго подключения
    QMessageBox::critical(this, "Ошибка подключения 2",
                          QString("Не удалось подключиться к базе данных после двух попыток:\n\n"
                                  "ПЕРВАЯ попытка (%1):\n"
                                  "Хост: %2\nПорт: %3\nПользователь: %4\nБаза: %5\n"
                                  "Ошибка: %6\n\n"
                                  "ВТОРАЯ попытка (%7):\n"
                                  "Хост: %8\nПорт: %9\nПользователь: %10\nБаза: %11\n"
                                  "Ошибка: %12")
                              .arg(dataSource)
                              .arg(dbHost).arg(dbPort).arg(dbUser).arg(dbName)
                              .arg(db.lastError().text())
                              .arg(newDataSource)
                              .arg(dbHost).arg(dbPort).arg(dbUser).arg(dbName)
                              .arg(db.lastError().text()));

    return false;
}
