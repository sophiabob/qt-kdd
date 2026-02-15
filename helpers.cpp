#include "helpers.h"


QString Helpers::timeDateMoscow()
{
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.addSecs(3 * 3600);
    return moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");
}

bool Helpers::searchInLayout(QVBoxLayout* layout, const QString& searchText, QPushButton* clearButton) {

    if (!layout) {
        qWarning() << "Область поиска", layout->objectName(), " пуста";
        return false;
    }

    if (clearButton) {// Показываем/скрываем кнопку очистки
        clearButton->setVisible(!searchText.isEmpty());
    }

    if (searchText.isEmpty()) { // Показываем все элементы
        for (int i = 0; i < layout->count(); ++i) {
            QLayoutItem* item = layout->itemAt(i);
            if (item && item->widget()) {
                item->widget()->setVisible(true);
            }
        }
        return true;
    }

    // Поиск по всем элементам
    bool found = false;
    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem* item = layout->itemAt(i);
        if (!item || !item->widget()) continue;

        QWidget* userWidget = item->widget();
        bool match = false;

        // Ищем все кнопки в виджете
        QList<QPushButton*> buttons = userWidget->findChildren<QPushButton*>();
        for (QPushButton* btn : buttons) {
            if (btn->text().contains(searchText, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }

        userWidget->setVisible(match);
        if (match) found = true;
    }

    return found;
}

bool Helpers::searchInTable(QTableWidget* table, const QString& searchText, QPushButton* clearButton, const TableSearchConfig& config) {

    // Проверка входных параметров
    if (!table) {
        throw std::invalid_argument("TableWidget не может быть nullptr");
    }

    // Управление кнопкой очистки
    if (clearButton) {
        clearButton->setVisible(!searchText.isEmpty());
    }

    // Если поиск пустой - показываем все строки
    if (searchText.isEmpty()) {
        for (int row = 0; row < table->rowCount(); ++row) {
            table->setRowHidden(row, false);
        }
        return true;
    }

    int foundCount = 0;
    int rowCount = table->rowCount();
    int columnCount = table->columnCount();

    Qt::CaseSensitivity caseSensitivity = config.caseSensitive ?
        Qt::CaseSensitive : Qt::CaseInsensitive;

    // Определяем столбцы для поиска
    QList<int> columnsToCheck;
    if (config.searchInAllColumns || config.columnsToSearch.isEmpty()) {
        for (int col = 0; col < columnCount; ++col) {
            columnsToCheck.append(col);
        }
    } else {
        columnsToCheck = config.columnsToSearch;
    }

    // Поиск по таблице
    for (int row = 0; row < rowCount; ++row) {
        bool match = false;

        // Поиск в указанных столбцах
        for (int col : columnsToCheck) {
            QTableWidgetItem* item = table->item(row, col);
            if (item && item->text().contains(searchText, caseSensitivity)) {
                match = true;
                break;
            }
        }

        // Скрываем/показываем строку
        table->setRowHidden(row, !match);
        if (match) {
            foundCount++;
        }
    }

    // Автоматическое изменение размера столбцов если что-то найдено
    if (foundCount > 0) {
        table->resizeColumnsToContents();
    }

    return foundCount > 0;
}

static QTimer *globalTimer = nullptr;
void Helpers::startInactivityTimer(int seconds, std::function<void()> callback, QObject *parent)
{
    // Останавливаем и удаляем предыдущий таймер
    if (globalTimer) {
        if (globalTimer->isActive()) {
            globalTimer->stop();
        }
        globalTimer->deleteLater();
        globalTimer = nullptr;
    }

    // Создаем новый таймер
    globalTimer = new QTimer(parent ? parent : qApp);
    globalTimer->setSingleShot(true);
    globalTimer->setInterval(seconds * 1000);

    // Подключаем callback
    QObject::connect(globalTimer, &QTimer::timeout, [callback]() {
        callback();
    });

    globalTimer->start();
}

void Helpers::stopInactivityTimer()
{
    if (inactivityTimer) {
        inactivityTimer->stop();
        inactivityTimer->deleteLater();
        inactivityTimer = nullptr;
    }
}

bool Helpers::eventFilter(QObject*, QEvent* event)
{
    if (inactivityTimer && inactivityTimer->isActive() &&
        (event->type() == QEvent::MouseMove ||
         event->type() == QEvent::KeyPress)) {
        inactivityTimer->start();
    }
    return false;
}

void Helpers::readMP4(QString mp4)
{
    if (mp4) {
        // Создаем виджет, который будет главным окном
        QWidget window;
        QVBoxLayout *layout = new QVBoxLayout(&window);

        // 1. Создаем плеер
        QMediaPlayer *player = new QMediaPlayer(&window);

        // 2. Создаем виджет для видео
        QVideoWidget *videoWidget = new QVideoWidget(&window);
        layout->addWidget(videoWidget); // Добавляем виджет в окно

        // 3. Связываем плеер и виджет (самое главное!)
        player->setVideoOutput(videoWidget);

        // 4. Указываем источник (файл или URL)
        player->setSource(QUrl::fromLocalFile("/path/to/your/video.mp4"));

        // 5. Запускаем воспроизведение
        player->play();

        window.resize(800, 600);
        window.show();

    }
}

