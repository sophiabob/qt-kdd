#include "welcome.h"
#include "ui_welcome.h"

#include "mainwindow.h"
#include <QStringConverter>

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
    db.setConnectOptions("client_encoding=UTF8");
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
QString Welcome::detectOS()
{
    QString osName;

#ifdef Q_OS_WIN
    osName = "Windows";
#elif defined(Q_OS_LINUX)
    osName = "Linux";
#elif defined(Q_OS_UNIX)
    osName = "Unix";
#elif defined(Q_OS_MAC)
    osName = "macOS";
#else
    osName = "Unknown OS";
#endif

    return osName;
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



void Welcome::on_btnSignIn_pressed()
{

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

    QString currentFile = __FILE__;
    QFileInfo fileInfo(currentFile);
    QDir sourceDir = fileInfo.dir();
    sourceDir.cdUp();
    QString gifPath = sourceDir.absolutePath() + "/img/help.gif";
    //QString gifPath = "/home/sds/sh18/kdd_17.11.25/img/help.gif";//"C:/Users/sophia/Documents/work work work/kdd/img/help.gif";

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
    // Отладочный вывод
    qDebug() << "Прочитаны настройки:";
    qDebug() << "  User:" << user;
    qDebug() << "  Password:" << (password.isEmpty() ? "не задан" : "[установлен]");
    qDebug() << "  Database:" << database;
    qDebug() << "  Host:" << host;
    qDebug() << "  Port:" << port;
}

void Welcome::on_pushButton_pressed()
{
    // Закрываем и удаляем все существующие подключения
    QStringList connections = QSqlDatabase::connectionNames();
    for (const QString &connectionName : connections) {
        QSqlDatabase::removeDatabase(connectionName);
    }

    // Получаем параметры подключения как в createConnection()
    QString dbUser, dbPassword, dbName, dbHost;
    int dbPort = 5432;
    QString dataSource = "";

    // 1. Проверяем IP из поля ввода
    QString ipFromUI = ui->lineEdit_ip->text().trimmed();
    if (!ipFromUI.isEmpty()) {
        QHostAddress address;
        if (address.setAddress(ipFromUI)) {
            dbHost = ipFromUI;
            dataSource = "Поле ввода IP";
            qDebug() << "Используется IP из поля ввода:" << dbHost;
        } else {
            qWarning() << "Неверный IP адрес в поле ввода:" << ipFromUI;
        }
    }

    // 2. Если нет IP из UI, читаем из settings.ini
    if (dbHost.isEmpty()) {
        readDatabaseSettings(dbUser, dbPassword, dbName, dbHost, dbPort, false);
        dataSource = "Файл settings.ini";
        qDebug() << "Используются настройки из settings.ini";
    }

    // 3. Если хост все еще пустой, используем OS-специфичные настройки
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
            qDebug() << "Используются OS-специфичные настройки для" << os;
        }
        dataSource = "OS-специфичные настройки";
    }

    qDebug() << "Параметры подключения для создания базы:";
    qDebug() << "Хост:" << dbHost;
    qDebug() << "Порт:" << dbPort;
    qDebug() << "Пользователь:" << dbUser;
    qDebug() << "База:" << dbName;

    // Проверяем подключение к серверу PostgreSQL (к базе postgres)
    QString testConnectionName = "test_connection_" + QString::number(QDateTime::currentMSecsSinceEpoch());
    QSqlDatabase testDb = QSqlDatabase::addDatabase("QPSQL", testConnectionName);

    testDb.setConnectOptions("client_encoding=UTF8");

    testDb.setHostName(dbHost);
    testDb.setPort(dbPort);
    testDb.setUserName(dbUser);
    testDb.setPassword(dbPassword);
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

    // Создаем основное подключение к целевой базе данных
    QString connectionName = QString("main_connection_%1").arg(QDateTime::currentMSecsSinceEpoch());
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", connectionName);

    db.setHostName(dbHost);
    db.setPort(dbPort);
    db.setUserName(dbUser);
    db.setPassword(dbPassword);
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
        time int,
        time_max timestamp,
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

        // Таблица set_kid
        R"(CREATE TABLE set_kid(
        set_id int primary key,
        set_name CHARACTER VARYING(30),
        ip_set inet,
        set_quantity int,
        set_block CHARACTER VARYING(30),
        set_note CHARACTER VARYING(30),
        last_update timestamp
    ))",

        // Таблица kas_kid
        R"(CREATE TABLE kas_kid(
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

        // Таблица mesh_kid
        R"(CREATE TABLE mesh_kid(
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

        // Таблица set_kid_history
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

        // Таблица kas_kid_history
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

        // Таблица mesh_kid_history
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
    // Перед подключением к БД
   // QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    /// или для Windows
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));




    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    QString dbUser, dbPassword, dbName, dbHost;
    int dbPort = 5432;
    QString dataSource = "";

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
        qDebug() << "ПЕРВАЯ попытка: Используются OS-специфичные настройки для" << os;
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


    //query.clear();
    db.close();
    return false;
}

void Welcome::on_pushButton_syncBD_pressed()
{
    checkAndSyncDatabases();
}

// Основные функции управления БД

void Welcome::checkAndSyncDatabases()
{
    // Подключаемся к базам данных
    QList<DatabaseInfo> databases = connectToDatabases();

    // Проверяем успешные подключения
    QList<DatabaseInfo> connectedDBs;
    for (DatabaseInfo &dbInfo : databases) {
        if (dbInfo.isConnected) {
            connectedDBs.append(dbInfo);
        }
    }

    if (connectedDBs.isEmpty()) {
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось подключиться ни к одной базе данных.");
        return;
    }

    QString fullReport = "ОТЧЕТ О ПРОВЕРКЕ И СИНХРОНИЗАЦИИ БАЗ ДАННЫХ\n";
    fullReport += "==============================================\n\n";

    // 1. Проверяем структуру каждой базы
    fullReport += "1. ПРОВЕРКА СТРУКТУРЫ БАЗ ДАННЫХ:\n";
    fullReport += "================================\n";

    for (DatabaseInfo &dbInfo : connectedDBs) {
        QString structureReport = checkAndRepairDatabaseStructure(dbInfo.db, dbInfo.name);
        fullReport += structureReport + "\n";
    }

    // 2. Синхронизируем данные между базами (если их две)
    if (connectedDBs.size() >= 2) {
        fullReport += "\n2. СИНХРОНИЗАЦИЯ ДАННЫХ:\n";
        fullReport += "=======================\n";

        // Двусторонняя синхронизация
        QString syncReport1 = compareAndSyncData(connectedDBs[0].db, connectedDBs[1].db,
                                                 connectedDBs[0].name, connectedDBs[1].name);
        fullReport += syncReport1 + "\n";

        QString syncReport2 = compareAndSyncData(connectedDBs[1].db, connectedDBs[0].db,
                                                 connectedDBs[1].name, connectedDBs[0].name);
        fullReport += syncReport2 + "\n";
    } else if (connectedDBs.size() == 1) {
        fullReport += "\n2. СИНХРОНИЗАЦИЯ ДАННЫХ:\n";
        fullReport += "=======================\n";
        fullReport += "✓ Подключена только одна база данных.\n";
        fullReport += "✓ Синхронизация с другими базами невозможна.\n";
    }

    // Выводим полный отчет
    QMessageBox::information(this, "Полный отчет", fullReport);

    // Закрываем соединения
    for (DatabaseInfo &dbInfo : connectedDBs) {
        if (dbInfo.db.isOpen()) {
            dbInfo.db.close();
        }
    }
}

QList<DatabaseInfo> Welcome::connectToDatabases()
{
    QList<DatabaseInfo> databases;

    // 1. Локальная база данных
    DatabaseInfo localDB;
    QString os = detectOS();

    if (os == "Windows") {
        this->showNormal();
        localDB.port = 5432;
        localDB.user = "postgres";
        localDB.password = "123";
        localDB.database = "postgres";
        localDB.host = "localhost";
        localDB.name = "Локальная БД (Windows)";
    } else if (os == "Linux") {
        this->showFullScreen();
        localDB.port = 5432;
        localDB.user = "postgres";
        localDB.password = "postgres1";
        localDB.database = "kdd";
        localDB.host = "localhost";
        localDB.name = "Локальная БД (Linux)";
    } else {
        qDebug() << "OS не опознана" << os;
    }

    databases.append(localDB);

    // 2. Удаленная база данных (из настроек)
    DatabaseInfo remoteDB;
    remoteDB.name = "Удаленная база данных";
    readDatabaseSettings(remoteDB.user, remoteDB.password,
                         remoteDB.database, remoteDB.host, remoteDB.port, false);

    // Если настройки не найдены, используем значения по умолчанию
    if (remoteDB.host.isEmpty()) {
        qDebug() << "Настройки из ini не найдены";
    }

    databases.append(remoteDB);

    // Подключаемся к каждой базе
    for (int i = 0; i < databases.size(); ++i) {
        DatabaseInfo &dbInfo = databases[i];

        QString connectionName = QString("db_connection_%1").arg(i);
        dbInfo.db = QSqlDatabase::addDatabase("QPSQL", connectionName);

        dbInfo.db.setConnectOptions("client_encoding=UTF8");

        dbInfo.db.setHostName(dbInfo.host);
        dbInfo.db.setPort(dbInfo.port);
        dbInfo.db.setDatabaseName(dbInfo.database);
        dbInfo.db.setUserName(dbInfo.user);
        dbInfo.db.setPassword(dbInfo.password);

        // Устанавливаем таймауты
        dbInfo.db.setConnectOptions("connect_timeout=5");

        dbInfo.isConnected = dbInfo.db.open();

        if (!dbInfo.isConnected) {
            dbInfo.errorMessage = dbInfo.db.lastError().text();
        }
    }

    // Выводим информационное окно
    QString message = "Результаты подключения к базам данных:\n";
    message += "=============================================\n\n";

    for (const DatabaseInfo &dbInfo : databases) {
        message += QString("%1:\n").arg(dbInfo.name);
        message += QString("  Хост: %1:%2\n").arg(dbInfo.host).arg(dbInfo.port);
        message += QString("  База данных: %1\n").arg(dbInfo.database);
        message += QString("  Пользователь: %1\n").arg(dbInfo.user);
        message += QString("  Статус: %1\n\n").arg(dbInfo.isConnected ?
                                                             "✓ Успешно подключено" :
                                                             "✗ Ошибка подключения");
        if (!dbInfo.isConnected) {
            message += QString("  Ошибка: %1\n\n").arg(dbInfo.errorMessage);
        }
    }

    QMessageBox::information(this, "Результаты подключения", message);

    return databases;
}

QString Welcome::checkAndRepairDatabaseStructure(QSqlDatabase &database, const QString &dbName)
{
    QString resultMessage;
    int tablesModified = 0;
    int columnsAdded = 0;

    QStringList tables = getTableList();

    for (const QString &tableName : tables) {
        // Проверяем существование таблицы
        QSqlQuery checkTableQuery(database);
        checkTableQuery.prepare(
            "SELECT EXISTS ("
            "SELECT FROM information_schema.tables "
            "WHERE table_schema = 'public' "
            "AND table_name = :tableName)"
            );
        checkTableQuery.bindValue(":tableName", tableName);

        if (!checkTableQuery.exec() || !checkTableQuery.next()) {
            resultMessage += QString("\nОшибка проверки таблицы %1: %2")
                                 .arg(tableName)
                                 .arg(checkTableQuery.lastError().text());
            continue;
        }

        bool tableExists = checkTableQuery.value(0).toBool();

        if (!tableExists) {
            // Таблица не существует - создаем
            resultMessage += QString("\nТаблица %1 не существует. Создание...")
                                 .arg(tableName);
            createMissingTable(database, tableName);
            tablesModified++;
        } else {
            // Таблица существует - проверяем структуру
            QList<ColumnInfo> expectedColumns = getExpectedTableStructure(tableName);
            QString tableCheckResult = checkTableStructure(database, tableName, expectedColumns);

            if (!tableCheckResult.isEmpty()) {
                resultMessage += tableCheckResult;
                columnsAdded += tableCheckResult.count("Добавлен столбец");
                tablesModified++;
            }
        }
    }

    // Формируем итоговое сообщение
    QString finalMessage = QString("\n\n=== ИТОГ ДЛЯ %1 ===\n").arg(dbName);

    if (tablesModified > 0 || columnsAdded > 0) {
        finalMessage += QString("Изменено таблиц: %1\n").arg(tablesModified);
        finalMessage += QString("Добавлено столбцов: %1\n").arg(columnsAdded);
        finalMessage += "Были внесены следующие изменения:\n";
        finalMessage += resultMessage;
    } else {
        finalMessage += "✓ Структура базы данных в полном порядке.\n";
        finalMessage += "✓ Все таблицы присутствуют.\n";
        finalMessage += "✓ Все столбцы соответствуют ожидаемой структуре.\n";
        finalMessage += "✓ Изменений не требуется.";
    }

    return finalMessage;
}

QString Welcome::compareAndSyncData(QSqlDatabase &sourceDB, QSqlDatabase &targetDB,
                                    const QString &dbNameSource, const QString &dbNameTarget)
{
    QString resultMessage;
    int totalRowsAdded = 0;

    QStringList tables = getMainTables(); // Синхронизируем только основные таблицы

    for (const QString &tableName : tables) {
        // Определяем первичный ключ
        QString primaryKey;
        if (tableName == "users_duty") {
            primaryKey = "users_duty_id";
        } else if (tableName == "dose_ppd") {
            primaryKey = "dose_ppd_id";
        } else if (tableName.contains("_")) {
            QString firstPart = tableName.split("_").first();
            primaryKey = firstPart + "_id";
        } else {
            primaryKey = tableName + "_id";
        }

        // Проверяем, существует ли таблица в целевой базе
        QSqlQuery checkTargetTable(targetDB);
        checkTargetTable.prepare(
            "SELECT EXISTS ("
            "SELECT FROM information_schema.tables "
            "WHERE table_schema = 'public' "
            "AND table_name = :tableName)"
            );
        checkTargetTable.bindValue(":tableName", tableName);

        if (!checkTargetTable.exec() || !checkTargetTable.next() || !checkTargetTable.value(0).toBool()) {
            resultMessage += QString("\nТаблица %1 отсутствует в целевой базе %2")
                                 .arg(tableName).arg(dbNameTarget);
            continue;
        }

        // Проверяем, существует ли таблица в исходной базе
        QSqlQuery checkSourceTable(sourceDB);
        checkSourceTable.prepare(
            "SELECT EXISTS ("
            "SELECT FROM information_schema.tables "
            "WHERE table_schema = 'public' "
            "AND table_name = :tableName)"
            );
        checkSourceTable.bindValue(":tableName", tableName);

        if (!checkSourceTable.exec() || !checkSourceTable.next() || !checkSourceTable.value(0).toBool()) {
            resultMessage += QString("\nТаблица %1 отсутствует в исходной базе %2")
                                 .arg(tableName).arg(dbNameSource);
            continue;
        }

        // Получаем ID из исходной базы
        QSqlQuery sourceQuery(sourceDB);
        QString sourceSql = QString("SELECT %1 FROM %2 ORDER BY %1")
                                .arg(primaryKey).arg(tableName);

        if (!sourceQuery.exec(sourceSql)) {
            resultMessage += QString("\nОшибка получения ID из %1.%2: %3")
                                 .arg(dbNameSource).arg(tableName).arg(sourceQuery.lastError().text());
            continue;
        }

        QSet<int> sourceIds;
        while (sourceQuery.next()) {
            bool ok;
            int id = sourceQuery.value(0).toInt(&ok);
            if (ok) {
                sourceIds.insert(id);
            }
        }

        // Получаем ID из целевой базы
        QSqlQuery targetQuery(targetDB);
        QString targetSql = QString("SELECT %1 FROM %2 ORDER BY %1")
                                .arg(primaryKey).arg(tableName);

        if (!targetQuery.exec(targetSql)) {
            resultMessage += QString("\nОшибка получения ID из %1.%2: %3")
                                 .arg(dbNameTarget).arg(tableName).arg(targetQuery.lastError().text());
            continue;
        }

        QSet<int> targetIds;
        while (targetQuery.next()) {
            bool ok;
            int id = targetQuery.value(0).toInt(&ok);
            if (ok) {
                targetIds.insert(id);
            }
        }

        // Находим отсутствующие ID
        QSet<int> missingIds;
        for (int id : sourceIds) {
            if (!targetIds.contains(id)) {
                missingIds.insert(id);
            }
        }

        if (!missingIds.isEmpty()) {
            // Получаем структуру таблицы
            QSqlQuery columnsQuery(sourceDB);
            columnsQuery.prepare(
                "SELECT column_name "
                "FROM information_schema.columns "
                "WHERE table_schema = 'public' AND table_name = :tableName "
                "ORDER BY ordinal_position"
                );
            columnsQuery.bindValue(":tableName", tableName);

            if (!columnsQuery.exec()) {
                resultMessage += QString("\nОшибка получения структуры таблицы %1: %2")
                                     .arg(tableName).arg(columnsQuery.lastError().text());
                continue;
            }

            QStringList columnNames;
            while (columnsQuery.next()) {
                columnNames.append(columnsQuery.value(0).toString());
            }

            // Вставляем недостающие строки
            int rowsAdded = 0;
            for (int id : missingIds) {
                // Получаем данные строки
                QSqlQuery dataQuery(sourceDB);
                QString dataSql = QString("SELECT * FROM %1 WHERE %2 = :id")
                                      .arg(tableName).arg(primaryKey);
                dataQuery.prepare(dataSql);
                dataQuery.bindValue(":id", id);

                if (!dataQuery.exec() || !dataQuery.next()) {
                    resultMessage += QString("\nОшибка получения данных для ID %1 из таблицы %2")
                                         .arg(id).arg(tableName);
                    continue;
                }

                // Формируем запрос INSERT
                QString insertSql = QString("INSERT INTO %1 (").arg(tableName);
                QString placeholders;

                for (int i = 0; i < columnNames.size(); ++i) {
                    insertSql += columnNames[i];
                    placeholders += ":" + columnNames[i];

                    if (i < columnNames.size() - 1) {
                        insertSql += ", ";
                        placeholders += ", ";
                    }
                }
                insertSql += ") VALUES (" + placeholders + ")";

                // Выполняем вставку
                QSqlQuery insertQuery(targetDB);
                insertQuery.prepare(insertSql);

                for (int i = 0; i < columnNames.size(); ++i) {
                    QVariant value = dataQuery.value(i);
                    // Для NULL значений используем QVariant()
                    if (value.isNull()) {
                        insertQuery.bindValue(":" + columnNames[i], QVariant());
                    } else {
                        insertQuery.bindValue(":" + columnNames[i], value);
                    }
                }

                if (!insertQuery.exec()) {
                    resultMessage += QString("\nОшибка вставки ID %1 в %2.%3: %4")
                                         .arg(id)
                                         .arg(dbNameTarget)
                                         .arg(tableName)
                                         .arg(insertQuery.lastError().text());
                } else {
                    rowsAdded++;
                }
            }

            if (rowsAdded > 0) {
                resultMessage += QString("\n✓ Таблица %1: добавлено %2 строк из %3 в %4")
                                     .arg(tableName)
                                     .arg(rowsAdded)
                                     .arg(dbNameSource)
                                     .arg(dbNameTarget);
                totalRowsAdded += rowsAdded;
            }
        }
    }

    // Формируем итоговое сообщение
    QString finalMessage = QString("\n\n=== СИНХРОНИЗАЦИЯ ИЗ %1 В %2 ===\n")
                               .arg(dbNameSource).arg(dbNameTarget);

    if (totalRowsAdded > 0) {
        finalMessage += QString("✓ Всего добавлено строк: %1\n").arg(totalRowsAdded);
        finalMessage += "Подробности синхронизации:\n";
        finalMessage += resultMessage;
    } else {
        finalMessage += "✓ Базы данных полностью синхронизированы\n";
        finalMessage += "✓ Добавление строк не требуется\n";
        finalMessage += "✓ Все данные совпадают";
    }

    return finalMessage;
}

// Функции для работы с таблицами

QStringList Welcome::getTableList()
{
    return {
        "users", "user_photo", "duty", "users_duty", "set", "kas", "mesh",
        "users_history", "duty_history", "users_duty_history", "set_history",
        "kas_history", "mesh_history", "dose_ppd", "dose_ppd_history",
        "set_kid", "kas_kid", "mesh_kid", "set_kid_history", "kas_kid_history",
        "mesh_kid_history"
    };
}

QStringList Welcome::getMainTables()
{
    return {
        "users", "user_photo", "duty", "users_duty", "set", "kas", "mesh",
        "dose_ppd", "set_kid", "kas_kid", "mesh_kid"
    };
}

QStringList Welcome::getHistoryTables()
{
    return {
        "users_history", "duty_history", "users_duty_history", "set_history",
        "kas_history", "mesh_history", "dose_ppd_history",
        "set_kid_history", "kas_kid_history", "mesh_kid_history"
    };
}

QStringList Welcome::getKidTables()
{
    return {
        "set_kid", "kas_kid", "mesh_kid", "set_kid_history",
        "kas_kid_history", "mesh_kid_history"
    };
}

QString Welcome::checkTableStructure(QSqlDatabase &db, const QString &tableName,
                                     const QList<ColumnInfo> &expectedColumns)
{
    QString result;

    // Получаем существующие столбцы
    QSqlQuery query(db);
    query.prepare(
        "SELECT column_name, data_type, is_nullable, column_default "
        "FROM information_schema.columns "
        "WHERE table_schema = 'public' AND table_name = :tableName "
        "ORDER BY ordinal_position"
        );
    query.bindValue(":tableName", tableName);

    if (!query.exec()) {
        return QString("Ошибка получения столбцов таблицы %1: %2")
            .arg(tableName).arg(query.lastError().text());
    }

    QMap<QString, ColumnInfo> existingColumns;
    while (query.next()) {
        ColumnInfo col;
        col.name = query.value("column_name").toString();
        col.type = query.value("data_type").toString();
        col.notNull = (query.value("is_nullable").toString() == "NO");
        col.defaultValue = query.value("column_default").toString();
        existingColumns[col.name] = col;
    }

    // Проверяем каждый ожидаемый столбец
    for (const ColumnInfo &expectedCol : expectedColumns) {
        if (!existingColumns.contains(expectedCol.name)) {
            // Столбец отсутствует - добавляем
            addMissingColumn(db, tableName, expectedCol);
            result += QString("\n  Добавлен столбец %1 (%2)").arg(expectedCol.name).arg(expectedCol.type);
        }
    }

    return result;
}

void Welcome::createMissingTable(QSqlDatabase &db, const QString &tableName)
{
    QString createSQL = getCreateTableSQL(tableName);
    if (!createSQL.isEmpty()) {
        QSqlQuery query(db);
        if (!query.exec(createSQL)) {
            qDebug() << "Ошибка создания таблицы" << tableName << ":" << query.lastError().text();
        }
    }
}

void Welcome::addMissingColumn(QSqlDatabase &db, const QString &tableName,
                               const ColumnInfo &column)
{
    QString sql = QString("ALTER TABLE %1 ADD COLUMN %2 %3")
    .arg(tableName)
        .arg(column.name)
        .arg(column.type);

    if (column.notNull) {
        sql += " NOT NULL";
    }
    if (!column.defaultValue.isEmpty()) {
        sql += QString(" DEFAULT %1").arg(column.defaultValue);
    }

    QSqlQuery query(db);
    if (!query.exec(sql)) {
        qDebug() << "Ошибка добавления столбца" << column.name << "в таблицу" << tableName
                 << ":" << query.lastError().text();
    }
}

// SQL функции для создания таблиц

QString Welcome::getUsersSQL()
{
    return R"(CREATE TABLE users(
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
    ))";
}

QString Welcome::getUserPhotoSQL()
{
    return R"(CREATE TABLE user_photo(
        user_id bigint,
        user_photo bytea
    ))";
}

QString Welcome::getDutySQL()
{
    return R"(CREATE TABLE duty(
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
    ))";
}

QString Welcome::getUsersDutySQL()
{
    return R"(CREATE TABLE users_duty(
        users_duty_id int primary key,
        user_id int,
        duty_id int,
        date_start date,
        date_finish date,
        users_duty_note CHARACTER VARYING(30)
    ))";
}

QString Welcome::getSetSQL()
{
    return R"(CREATE TABLE set(
        set_id int primary key,
        set_name CHARACTER VARYING(30),
        ip_set inet,
        set_quantity int,
        set_block CHARACTER VARYING(30),
        set_note CHARACTER VARYING(30),
        last_update timestamp
    ))";
}

QString Welcome::getKasSQL()
{
    return R"(CREATE TABLE kas(
        set_id int,
        kas_id int,
        kas_name CHARACTER VARYING(30),
        kas_height int,
        kas_width int,
        kas_block CHARACTER VARYING(30),
        kas_note CHARACTER VARYING(30),
        last_update timestamp,
        PRIMARY KEY (kas_id, set_id)
    ))";
}

QString Welcome::getMeshSQL()
{
    return R"(CREATE TABLE mesh(
        set_id int,
        kas_id int,
        mesh_id int,
        user_id int,
        doz_tld_id CHARACTER VARYING(30),
        mesh_status int,
        mesh_note CHARACTER VARYING(30),
        last_update timestamp,
        PRIMARY KEY (kas_id, set_id, mesh_id)
    ))";
}

QString Welcome::getUsersHistorySQL()
{
    return R"(CREATE TABLE users_history (
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
    ))";
}

QString Welcome::getDutyHistorySQL()
{
    return R"(CREATE TABLE duty_history (
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
    ))";
}

QString Welcome::getUsersDutyHistorySQL()
{
    return R"(CREATE TABLE users_duty_history (
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
    ))";
}

QString Welcome::getSetHistorySQL()
{
    return R"(CREATE TABLE set_history (
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
    ))";
}

QString Welcome::getKasHistorySQL()
{
    return R"(CREATE TABLE kas_history (
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
    ))";
}

QString Welcome::getMeshHistorySQL()
{
    return R"(CREATE TABLE mesh_history (
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
    ))";
}

QString Welcome::getDosePpdSQL()
{
    return R"(CREATE TABLE dose_ppd(
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
    ))";
}

QString Welcome::getDosePpdHistorySQL()
{
    return R"(CREATE TABLE dose_ppd_history(
        dose_ppd_id int,
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
        last_update timestamp,
        user_id_change int,
        last_update_change timestamp,
        type_edit VARCHAR(30) DEFAULT 'create'
    ))";
}

QString Welcome::getSetKidSQL()
{
    return R"(CREATE TABLE set_kid(
        set_id int primary key,
        set_name CHARACTER VARYING(30),
        ip_set inet,
        set_quantity int,
        set_block CHARACTER VARYING(30),
        set_note CHARACTER VARYING(30),
        last_update timestamp
    ))";
}

QString Welcome::getKasKidSQL()
{
    return R"(CREATE TABLE kas_kid(
        set_id int,
        kas_id int,
        kas_name CHARACTER VARYING(30),
        kas_height int,
        kas_width int,
        kas_block CHARACTER VARYING(30),
        kas_note CHARACTER VARYING(30),
        last_update timestamp,
        PRIMARY KEY (kas_id, set_id)
    ))";
}

QString Welcome::getMeshKidSQL()
{
    return R"(CREATE TABLE mesh_kid(
        set_id int,
        kas_id int,
        mesh_id int,
        user_id int,
        doz_tld_id CHARACTER VARYING(30),
        mesh_status int,
        mesh_note CHARACTER VARYING(30),
        last_update timestamp,
        PRIMARY KEY (kas_id, set_id, mesh_id)
    ))";
}

QString Welcome::getSetKidHistorySQL()
{
    return R"(CREATE TABLE set_kid_history (
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
    ))";
}

QString Welcome::getKasKidHistorySQL()
{
    return R"(CREATE TABLE kas_kid_history (
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
    ))";
}

QString Welcome::getMeshKidHistorySQL()
{
    return R"(CREATE TABLE mesh_kid_history (
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
    ))";
}

QString Welcome::getCreateTableSQL(const QString &tableName)
{
    if (tableName == "users") return getUsersSQL();
    if (tableName == "user_photo") return getUserPhotoSQL();
    if (tableName == "duty") return getDutySQL();
    if (tableName == "users_duty") return getUsersDutySQL();
    if (tableName == "set") return getSetSQL();
    if (tableName == "kas") return getKasSQL();
    if (tableName == "mesh") return getMeshSQL();
    if (tableName == "users_history") return getUsersHistorySQL();
    if (tableName == "duty_history") return getDutyHistorySQL();
    if (tableName == "users_duty_history") return getUsersDutyHistorySQL();
    if (tableName == "set_history") return getSetHistorySQL();
    if (tableName == "kas_history") return getKasHistorySQL();
    if (tableName == "mesh_history") return getMeshHistorySQL();
    if (tableName == "dose_ppd") return getDosePpdSQL();
    if (tableName == "dose_ppd_history") return getDosePpdHistorySQL();
    if (tableName == "set_kid") return getSetKidSQL();
    if (tableName == "kas_kid") return getKasKidSQL();
    if (tableName == "mesh_kid") return getMeshKidSQL();
    if (tableName == "set_kid_history") return getSetKidHistorySQL();
    if (tableName == "kas_kid_history") return getKasKidHistorySQL();
    if (tableName == "mesh_kid_history") return getMeshKidHistorySQL();

    return "";
}
/*
// Вспомогательные функции
QString Welcome::detectOS()
{
#ifdef Q_OS_WIN
    return "Windows";
#elif defined(Q_OS_LINUX)
    return "Linux";
#elif defined(Q_OS_MAC)
    return "macOS";
#else
    return "Unknown";
#endif
}

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

    // Читаем настройки из секции [Settings]
    if (user.isEmpty()) {
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
}*/


QList<ColumnInfo> Welcome::getExpectedTableStructure(const QString &tableName)
{
    QList<ColumnInfo> columns;

    if (tableName == "users") {
        columns = {
            {"user_id", "int", true, "", true},
            {"login", "CHARACTER VARYING(30)", false, "", false},
            {"password", "CHARACTER VARYING(30)", false, "", false},
            {"name_0", "CHARACTER VARYING(30)", false, "", false},
            {"name_1", "CHARACTER VARYING(30)", false, "", false},
            {"name_2", "CHARACTER VARYING(30)", false, "", false},
            {"gender", "CHARACTER VARYING(3)", false, "", false},
            {"birthday", "date", false, "", false},
            {"role", "CHARACTER VARYING(30)", false, "", false},
            {"snils", "CHARACTER VARYING(30)", false, "", false},
            {"tab_num", "int", false, "", false},
            {"department", "CHARACTER VARYING(30)", false, "", false},
            {"card_id", "CHARACTER VARYING(30)", false, "", false},
            {"set_ID", "int", true, "", false},
            {"kas_ID", "int", true, "", false},
            {"mesh_ID", "int", true, "", false},
            {"doz_tld_id", "CHARACTER VARYING(30)", false, "", false},
            {"cell_date", "date", false, "", false},
            {"dose_year", "float8", false, "", false},
            {"dose_year_now", "float8", false, "", false},
            {"dose_year_now_ppd", "float8", false, "", false},
            {"code", "CHARACTER VARYING(50)", false, "", false},
            {"block", "CHARACTER VARYING(30)", false, "", false},
            {"last_update", "timestamp", false, "", false}
        };
    }
    else if (tableName == "user_photo") {
        columns = {
            {"user_id", "bigint", false, "", false},
            {"user_photo", "bytea", false, "", false}
        };
    }
    else if (tableName == "duty") {
        columns = {
            {"duty_id", "SERIAL", true, "", true},
            {"type_duty", "INTEGER", true, "", false},
            {"blok", "VARCHAR(100)", false, "", false},
            {"room", "VARCHAR(100)", false, "", false},
            {"equipment", "VARCHAR(100)", false, "", false},
            {"name_work", "VARCHAR(200)", false, "", false},
            {"type_work", "VARCHAR(200)", false, "", false},
            {"time", "INTEGER", false, "", false},
            {"dose_a_1", "NUMERIC(10,4)", false, "0.1", false},
            {"dose_a_2", "NUMERIC(10,4)", false, "1.0", false},
            {"rate_a_1", "NUMERIC(10,4)", false, "0.1", false},
            {"rate_a_2", "NUMERIC(10,4)", false, "1.0", false},
            {"dose_b_1", "NUMERIC(10,4)", false, "0.1", false},
            {"dose_b_2", "NUMERIC(10,4)", false, "1.0", false},
            {"rate_b_1", "NUMERIC(10,4)", false, "0.1", false},
            {"rate_b_2", "NUMERIC(10,4)", false, "1.0", false},
            {"dose_g_1", "NUMERIC(10,4)", false, "0.1", false},
            {"dose_g_2", "NUMERIC(10,4)", false, "1.0", false},
            {"rate_g_1", "NUMERIC(10,4)", false, "0.1", false},
            {"rate_g_2", "NUMERIC(10,4)", false, "1.0", false},
            {"dose_n_1", "NUMERIC(10,4)", false, "0.01", false},
            {"dose_n_2", "NUMERIC(10,4)", false, "0.1", false},
            {"rate_n_1", "NUMERIC(10,4)", false, "0.01", false},
            {"rate_n_2", "NUMERIC(10,4)", false, "0.1", false},
            {"duty_note", "TEXT", false, "", false},
            {"last_update", "TIMESTAMP", false, "CURRENT_TIMESTAMP", false}
        };
    }
    else if (tableName == "users_duty") {
        columns = {
            {"users_duty_id", "int", true, "", true},
            {"user_id", "int", false, "", false},
            {"duty_id", "int", false, "", false},
            {"date_start", "date", false, "", false},
            {"date_finish", "date", false, "", false},
            {"users_duty_note", "CHARACTER VARYING(30)", false, "", false}
        };
    }
    else if (tableName == "set") {
        columns = {
            {"set_id", "int", true, "", true},
            {"set_name", "CHARACTER VARYING(30)", false, "", false},
            {"ip_set", "inet", false, "", false},
            {"set_quantity", "int", false, "", false},
            {"set_block", "CHARACTER VARYING(30)", false, "", false},
            {"set_note", "CHARACTER VARYING(30)", false, "", false},
            {"last_update", "timestamp", false, "", false}
        };
    }
    else if (tableName == "kas") {
        columns = {
            {"set_id", "int", false, "", false},
            {"kas_id", "int", false, "", false},
            {"kas_name", "CHARACTER VARYING(30)", false, "", false},
            {"kas_height", "int", false, "", false},
            {"kas_width", "int", false, "", false},
            {"kas_block", "CHARACTER VARYING(30)", false, "", false},
            {"kas_note", "CHARACTER VARYING(30)", false, "", false},
            {"last_update", "timestamp", false, "", false}
        };
    }
    else if (tableName == "mesh") {
        columns = {
            {"set_id", "int", false, "", false},
            {"kas_id", "int", false, "", false},
            {"mesh_id", "int", false, "", false},
            {"user_id", "int", false, "", false},
            {"doz_tld_id", "CHARACTER VARYING(30)", false, "", false},
            {"mesh_status", "int", false, "", false},
            {"mesh_note", "CHARACTER VARYING(30)", false, "", false},
            {"last_update", "timestamp", false, "", false}
        };
    }
    else if (tableName == "users_history") {
        columns = {
            {"user_id", "INTEGER", true, "", false},
            {"login", "VARCHAR(50)", true, "", false},
            {"password", "VARCHAR(100)", true, "", false},
            {"name_0", "VARCHAR(50)", true, "", false},
            {"name_1", "VARCHAR(50)", true, "", false},
            {"name_2", "VARCHAR(50)", false, "", false},
            {"gender", "VARCHAR(10)", false, "", false},
            {"birthday", "DATE", false, "", false},
            {"role", "VARCHAR(30)", false, "", false},
            {"tab_num", "VARCHAR(20)", true, "", false},
            {"department", "VARCHAR(100)", false, "", false},
            {"card_id", "VARCHAR(30)", true, "", false},
            {"set_inf", "INTEGER", true, "1", false},
            {"kas_inf", "INTEGER", true, "1", false},
            {"mesh_inf", "INTEGER", true, "1", false},
            {"doz_tld_id", "VARCHAR(30)", true, "", false},
            {"cell_date", "DATE", false, "", false},
            {"dose_year", "DECIMAL(10,4)", false, "", false},
            {"dose_year_now", "DECIMAL(10,4)", false, "", false},
            {"dose_year_now_ppd", "DECIMAL(10,4)", false, "", false},
            {"code", "VARCHAR(30)", false, "", false},
            {"block", "VARCHAR(30)", false, "", false},
            {"last_update", "TIMESTAMP", false, "CURRENT_TIMESTAMP", false},
            {"user_id_change", "INTEGER", false, "", false},
            {"type_edit", "VARCHAR(30)", false, "'create'", false}
        };
    }
    else if (tableName == "duty_history") {
        columns = {
            {"duty_id", "VARCHAR(100)", false, "", false},
            {"type_duty", "INTEGER", false, "", false},
            {"blok", "VARCHAR(100)", false, "", false},
            {"room", "VARCHAR(100)", false, "", false},
            {"equipment", "VARCHAR(100)", false, "", false},
            {"name_work", "VARCHAR(200)", false, "", false},
            {"type_work", "VARCHAR(200)", false, "", false},
            {"time", "INTEGER", false, "", false},
            {"dose_a_1", "NUMERIC(10,4)", false, "", false},
            {"dose_a_2", "NUMERIC(10,4)", false, "", false},
            {"rate_a_1", "NUMERIC(10,4)", false, "", false},
            {"rate_a_2", "NUMERIC(10,4)", false, "", false},
            {"dose_b_1", "NUMERIC(10,4)", false, "", false},
            {"dose_b_2", "NUMERIC(10,4)", false, "", false},
            {"rate_b_1", "NUMERIC(10,4)", false, "", false},
            {"rate_b_2", "NUMERIC(10,4)", false, "", false},
            {"dose_g_1", "NUMERIC(10,4)", false, "", false},
            {"dose_g_2", "NUMERIC(10,4)", false, "", false},
            {"rate_g_1", "NUMERIC(10,4)", false, "", false},
            {"rate_g_2", "NUMERIC(10,4)", false, "", false},
            {"dose_n_1", "NUMERIC(10,4)", false, "", false},
            {"dose_n_2", "NUMERIC(10,4)", false, "", false},
            {"rate_n_1", "NUMERIC(10,4)", false, "", false},
            {"rate_n_2", "NUMERIC(10,4)", false, "", false},
            {"duty_note", "TEXT", false, "", false},
            {"last_update", "TIMESTAMP", false, "", false},
            {"user_id_change", "INTEGER", true, "", false},
            {"last_update_change", "TIMESTAMP", false, "CURRENT_TIMESTAMP", false},
            {"type_edit", "VARCHAR(30)", false, "'create'", false}
        };
    }
    else if (tableName == "users_duty_history") {
        columns = {
            {"users_duty_id", "INTEGER", false, "", false},
            {"user_id", "INTEGER", false, "", false},
            {"duty_id", "INTEGER", false, "", false},
            {"date_start", "DATE", false, "", false},
            {"date_finish", "DATE", false, "", false},
            {"users_duty_note", "TEXT", false, "", false},
            {"last_update_change", "TIMESTAMP", false, "", false},
            {"user_id_change", "INTEGER", true, "", false},
            {"last_update", "TIMESTAMP", false, "", false},
            {"type_edit", "VARCHAR(30)", false, "'create'", false},
            {"change_time", "TIMESTAMP", false, "CURRENT_TIMESTAMP", false}
        };
    }
    else if (tableName == "set_history") {
        columns = {
            {"set_id", "INTEGER", true, "", false},
            {"set_name", "VARCHAR(30)", false, "", false},
            {"ip_set", "INET", false, "", false},
            {"set_quantity", "INTEGER", false, "", false},
            {"set_block", "VARCHAR(30)", false, "", false},
            {"set_note", "VARCHAR(30)", false, "", false},
            {"last_update", "TIMESTAMP", false, "", false},
            {"user_id_change", "INTEGER", false, "", false},
            {"last_update_change", "TIMESTAMP", false, "CURRENT_TIMESTAMP", false},
            {"type_edit", "VARCHAR(30)", false, "'create'", false}
        };
    }
    else if (tableName == "kas_history") {
        columns = {
            {"set_id", "INTEGER", true, "", false},
            {"kas_id", "INTEGER", true, "", false},
            {"kas_name", "VARCHAR(30)", false, "", false},
            {"kas_height", "INTEGER", false, "", false},
            {"kas_width", "INTEGER", false, "", false},
            {"kas_block", "VARCHAR(30)", false, "", false},
            {"kas_note", "VARCHAR(30)", false, "", false},
            {"last_update", "TIMESTAMP", false, "", false},
            {"user_id_change", "INTEGER", true, "", false},
            {"last_update_change", "TIMESTAMP", false, "CURRENT_TIMESTAMP", false},
            {"type_edit", "VARCHAR(30)", false, "'create'", false}
        };
    }
    else if (tableName == "mesh_history") {
        columns = {
            {"set_id", "INTEGER", true, "", false},
            {"kas_id", "INTEGER", true, "", false},
            {"mesh_id", "INTEGER", true, "", false},
            {"user_id", "INTEGER", false, "", false},
            {"doz_tld_id", "VARCHAR(30)", false, "", false},
            {"mesh_status", "INTEGER", false, "0", false},
            {"mesh_note", "VARCHAR(30)", false, "", false},
            {"last_update", "TIMESTAMP", false, "", false},
            {"user_id_change", "INTEGER", true, "", false},
            {"last_update_change", "TIMESTAMP", false, "CURRENT_TIMESTAMP", false},
            {"type_edit", "VARCHAR(30)", false, "'create'", false}
        };
    }
    else if (tableName == "dose_ppd") {
        columns = {
            {"dose_ppd_id", "int", true, "", true},
            {"user_id", "int", false, "", false},
            {"users_duty_id", "int", false, "", false},
            {"nomer_pdd", "CHARACTER VARYING(30)", false, "", false},
            {"time", "int", false, "", false},
            {"time_max", "timestamp", false, "", false},
            {"type_ppd", "smallint", false, "", false},
            {"dose", "float8", false, "0", false},
            {"rate_max", "float8", false, "", false},
            {"dose_ppd_note", "CHARACTER VARYING(30)", false, "", false},
            {"start_work", "timestamp", false, "", false},
            {"finish_work", "timestamp", false, "", false},
            {"last_update", "timestamp", false, "", false}
        };
    }
    else if (tableName == "dose_ppd_history") {
        columns = {
            {"dose_ppd_id", "int", false, "", false},
            {"user_id", "int", false, "", false},
            {"users_duty_id", "int", false, "", false},
            {"nomer_pdd", "CHARACTER VARYING(30)", false, "", false},
            {"time", "int", false, "", false},
            {"time_max", "timestamp", false, "", false},
            {"type_ppd", "smallint", false, "", false},
            {"dose", "float8", false, "0", false},
            {"rate_max", "float8", false, "", false},
            {"dose_ppd_note", "CHARACTER VARYING(30)", false, "", false},
            {"start_work", "timestamp", false, "", false},
            {"finish_work", "timestamp", false, "", false},
            {"last_update", "timestamp", false, "", false},
            {"user_id_change", "int", false, "", false},
            {"last_update_change", "timestamp", false, "", false},
            {"type_edit", "VARCHAR(30)", false, "'create'", false}
        };
    }
    else if (tableName == "set_kid") {
        columns = {
            {"set_id", "int", true, "", true},
            {"set_name", "CHARACTER VARYING(30)", false, "", false},
            {"ip_set", "inet", false, "", false},
            {"set_quantity", "int", false, "", false},
            {"set_block", "CHARACTER VARYING(30)", false, "", false},
            {"set_note", "CHARACTER VARYING(30)", false, "", false},
            {"last_update", "timestamp", false, "", false}
        };
    }
    else if (tableName == "kas_kid") {
        columns = {
            {"set_id", "int", false, "", false},
            {"kas_id", "int", false, "", false},
            {"kas_name", "CHARACTER VARYING(30)", false, "", false},
            {"kas_height", "int", false, "", false},
            {"kas_width", "int", false, "", false},
            {"kas_block", "CHARACTER VARYING(30)", false, "", false},
            {"kas_note", "CHARACTER VARYING(30)", false, "", false},
            {"last_update", "timestamp", false, "", false}
        };
    }
    else if (tableName == "mesh_kid") {
        columns = {
            {"set_id", "int", false, "", false},
            {"kas_id", "int", false, "", false},
            {"mesh_id", "int", false, "", false},
            {"user_id", "int", false, "", false},
            {"doz_tld_id", "CHARACTER VARYING(30)", false, "", false},
            {"mesh_status", "int", false, "", false},
            {"mesh_note", "CHARACTER VARYING(30)", false, "", false},
            {"last_update", "timestamp", false, "", false}
        };
    }
    else if (tableName == "set_kid_history") {
        columns = {
            {"set_id", "INTEGER", true, "", false},
            {"set_name", "VARCHAR(30)", false, "", false},
            {"ip_set", "INET", false, "", false},
            {"set_quantity", "INTEGER", false, "", false},
            {"set_block", "VARCHAR(30)", false, "", false},
            {"set_note", "VARCHAR(30)", false, "", false},
            {"last_update", "TIMESTAMP", false, "", false},
            {"user_id_change", "INTEGER", false, "", false},
            {"last_update_change", "TIMESTAMP", false, "CURRENT_TIMESTAMP", false},
            {"type_edit", "VARCHAR(30)", false, "'create'", false}
        };
    }
    else if (tableName == "kas_kid_history") {
        columns = {
            {"set_id", "INTEGER", true, "", false},
            {"kas_id", "INTEGER", true, "", false},
            {"kas_name", "VARCHAR(30)", false, "", false},
            {"kas_height", "INTEGER", false, "", false},
            {"kas_width", "INTEGER", false, "", false},
            {"kas_block", "VARCHAR(30)", false, "", false},
            {"kas_note", "VARCHAR(30)", false, "", false},
            {"last_update", "TIMESTAMP", false, "", false},
            {"user_id_change", "INTEGER", true, "", false},
            {"last_update_change", "TIMESTAMP", false, "CURRENT_TIMESTAMP", false},
            {"type_edit", "VARCHAR(30)", false, "'create'", false}
        };
    }
    else if (tableName == "mesh_kid_history") {
        columns = {
            {"set_id", "INTEGER", true, "", false},
            {"kas_id", "INTEGER", true, "", false},
            {"mesh_id", "INTEGER", true, "", false},
            {"user_id", "INTEGER", false, "", false},
            {"doz_tld_id", "VARCHAR(30)", false, "", false},
            {"mesh_status", "INTEGER", false, "0", false},
            {"mesh_note", "VARCHAR(30)", false, "", false},
            {"last_update", "TIMESTAMP", false, "", false},
            {"user_id_change", "INTEGER", true, "", false},
            {"last_update_change", "TIMESTAMP", false, "CURRENT_TIMESTAMP", false},
            {"type_edit", "VARCHAR(30)", false, "'create'", false}
        };
    }

    return columns;
}
