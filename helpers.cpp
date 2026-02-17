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

void Helpers::playVideoInWindow(QWidget* parent, QString videoPath)
{
    QString Path = "";
    QString currentFile = __FILE__;
    QFileInfo fileInfo(currentFile);
    QDir sourceDir = fileInfo.dir();
    QString os = Welcome::detectOS();
    if (os == "Windows") {
        sourceDir.cdUp();
    }
    Path = sourceDir.absolutePath() +"/video/";
    videoPath = Path + videoPath;




    if (videoPath.isEmpty() || !parent) return;

    // 1. ОВЕРЛЕЙ
    QWidget* overlay = new QWidget(parent);
    overlay->setStyleSheet("background-color: rgba(0, 0, 0, 150);");
    overlay->setGeometry(parent->rect());
    overlay->setWindowFlags(Qt::FramelessWindowHint);
    overlay->setAttribute(Qt::WA_DeleteOnClose);

    // 2. КОНТЕЙНЕР ДЛЯ ВИДЕО И КОНТРОЛОВ
    QWidget* container = new QWidget(overlay);
    container->setFixedSize(640, 480);
    container->setStyleSheet("background-color: black; border-radius: 10px;");

    // Центрируем контейнер
    int containerX = (overlay->width() - container->width()) / 2;
    int containerY = (overlay->height() - container->height()) / 2;
    container->move(containerX, containerY);
    container->show();

    // 3. ВЕРТИКАЛЬНЫЙ LAYOUT ДЛЯ КОНТЕЙНЕРА
    QVBoxLayout* containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    // 4. ВИДЕО ВИДЖЕТ (занимает почти всё место)
    QVideoWidget* videoWidget = new QVideoWidget;
    videoWidget->setStyleSheet("background-color: black;");
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    containerLayout->addWidget(videoWidget, 1); // 1 = растягивается

    // 5. ПАНЕЛЬ УПРАВЛЕНИЯ
    QWidget* controlsPanel = new QWidget;
    controlsPanel->setFixedHeight(40);
    controlsPanel->setStyleSheet("background-color: rgba(30, 30, 30, 200); border-bottom-left-radius: 10px; border-bottom-right-radius: 10px;");
    containerLayout->addWidget(controlsPanel);

    // 6. ГОРИЗОНТАЛЬНЫЙ LAYOUT ДЛЯ КНОПОК
    QHBoxLayout* controlsLayout = new QHBoxLayout(controlsPanel);
    controlsLayout->setContentsMargins(5, 5, 5, 5);
    controlsLayout->setSpacing(5);

    // 7. КНОПКИ
    QPushButton* playPauseBtn = new QPushButton("▶");
    playPauseBtn->setFixedSize(30, 30);
    playPauseBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 100);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 200);"
        "}"
        );

    QPushButton* stopBtn = new QPushButton("⬛");
    stopBtn->setFixedSize(30, 30);
    stopBtn->setStyleSheet(playPauseBtn->styleSheet());

    // 8. СЛАЙДЕР
    QSlider* seekSlider = new QSlider(Qt::Horizontal);
    seekSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "   border: 1px solid #999999;"
        "   height: 4px;"
        "   background: rgba(100, 100, 100, 150);"
        "   margin: 2px 0;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: white;"
        "   border: 1px solid #5c5c5c;"
        "   width: 12px;"
        "   margin: -4px 0;"
        "   border-radius: 6px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "   background: #4CAF50;"
        "}"
        );

    // 9. МЕТКА ВРЕМЕНИ
    QLabel* timeLabel = new QLabel("00:00 / 00:00");
    timeLabel->setStyleSheet("color: white; font-size: 11px;");
    timeLabel->setFixedWidth(80);
    timeLabel->setAlignment(Qt::AlignCenter);

    // 10. ДОБАВЛЯЕМ ВСЁ В КОНТРОЛЫ
    controlsLayout->addWidget(playPauseBtn);
    controlsLayout->addWidget(stopBtn);
    controlsLayout->addWidget(seekSlider, 1); // растягивается
    controlsLayout->addWidget(timeLabel);

    // 11. ПЛЕЕР
    QMediaPlayer* player = new QMediaPlayer(overlay);
    player->setVideoOutput(videoWidget);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    player->setSource(QUrl::fromLocalFile(videoPath));
#else
    player->setMedia(QUrl::fromLocalFile(videoPath));
#endif

    // 12. КНОПКА ЗАКРЫТИЯ
    QPushButton* closeBtn = new QPushButton("×", overlay);
    closeBtn->setGeometry(overlay->width() - 50, 10, 40, 40);
    closeBtn->setStyleSheet(
        "QPushButton {"
        "   font-size: 24px;"
        "   background: rgba(255, 0, 0, 150);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 20px;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(255, 0, 0, 255);"
        "}"
        );
    closeBtn->raise(); // поверх всего
    connect(closeBtn, &QPushButton::clicked, overlay, &QWidget::close);

    // 13. ПОДКЛЮЧЕНИЕ СИГНАЛОВ (УПРАВЛЕНИЕ)

    // Play/Pause
    QObject::connect(playPauseBtn, &QPushButton::clicked, [player, playPauseBtn]() {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        // Qt6
        if (player->playbackState() == QMediaPlayer::PlayingState) {
#else
            // Qt5
        if (player->state() == QMediaPlayer::PlayingState) {
#endif
            player->pause();
            playPauseBtn->setText("▶");
        } else {
            player->play();
            playPauseBtn->setText("⏸");
        }
    });

    // Stop
    QObject::connect(stopBtn, &QPushButton::clicked, [player, playPauseBtn]() {
        player->stop();
        playPauseBtn->setText("▶");
    });

    // Duration (длительность видео)
    QObject::connect(player, &QMediaPlayer::durationChanged, [seekSlider, timeLabel](qint64 duration) {
        seekSlider->setRange(0, static_cast<int>(duration));
        int seconds = static_cast<int>(duration / 1000);
        timeLabel->setText(QString("00:00 / %1:%2")
                               .arg(seconds / 60, 2, 10, QChar('0'))
                               .arg(seconds % 60, 2, 10, QChar('0')));
    });

    // Position (текущая позиция)
    QObject::connect(player, &QMediaPlayer::positionChanged, [seekSlider, timeLabel, player](qint64 position) {
        if (!seekSlider->isSliderDown()) {
            seekSlider->setValue(static_cast<int>(position));
        }

        qint64 duration = player->duration();
        if (duration > 0) {
            int currentSec = static_cast<int>(position / 1000);
            int totalSec = static_cast<int>(duration / 1000);
            timeLabel->setText(QString("%1:%2 / %3:%4")
                                   .arg(currentSec / 60, 2, 10, QChar('0'))
                                   .arg(currentSec % 60, 2, 10, QChar('0'))
                                   .arg(totalSec / 60, 2, 10, QChar('0'))
                                   .arg(totalSec % 60, 2, 10, QChar('0')));
        }
    });

    // Перемотка
    overlay->setProperty("wasPlayingBeforeSeek", false);

    QObject::connect(seekSlider, &QSlider::sliderPressed, [player, overlay]() {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        bool wasPlaying = (player->playbackState() == QMediaPlayer::PlayingState);
#else
        bool wasPlaying = (player->state() == QMediaPlayer::PlayingState);
#endif
        overlay->setProperty("wasPlayingBeforeSeek", wasPlaying);
        player->pause();
    });
    QObject::connect(seekSlider, &QSlider::sliderReleased, [player, seekSlider, overlay]() {
        player->setPosition(seekSlider->value());
        bool wasPlaying = overlay->property("wasPlayingBeforeSeek").toBool();
        if (wasPlaying) {
            player->play();
        }
    });
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QObject::connect(seekSlider, &QSlider::sliderMoved, [player](int position) {
        player->setPosition(position);  // Обновляем позицию при движении
    });
#endif

    // Окончание видео
    QObject::connect(player, &QMediaPlayer::mediaStatusChanged, [overlay, playPauseBtn](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            overlay->close();
        }
    });

    // Ошибки
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    // Qt6
    QObject::connect(player, &QMediaPlayer::errorOccurred, [](QMediaPlayer::Error error, const QString& errorString) {
        qDebug() << "Ошибка видео (Qt6):" << error << errorString;
    });
#else
    // Qt5 - используем старый синтаксис
    QObject::connect(player, SIGNAL(error(QMediaPlayer::Error)), player, SLOT(/* нужен слот */));
    // Проще использовать лямбду через старый синтаксис нельзя, поэтому добавим отдельный слот
    // Временно просто выведем через обработчик
    QObject::connect(player, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
                     [player](QMediaPlayer::Error /*error*/) {
                         qDebug() << "Ошибка видео (Qt5):" << player->errorString();
                     });
#endif

    // 14. ЗАПУСК
    player->play();
    playPauseBtn->setText("⏸");

    // 15. ПОКАЗЫВАЕМ ОВЕРЛЕЙ
    overlay->show();

    // 16. ОБРАБОТКА ИЗМЕНЕНИЯ РАЗМЕРА ОКНА
    // Сохраняем указатели для обработчика
    overlay->setProperty("container", QVariant::fromValue(container));
    overlay->setProperty("closeBtn", QVariant::fromValue(closeBtn));

    // Создаём фильтр событий для обработки resize
    class ResizeFilter : public QObject {
    public:
        ResizeFilter(QObject* parent = nullptr) : QObject(parent) {}

    protected:
        bool eventFilter(QObject* obj, QEvent* event) override {
            if (event->type() == QEvent::Resize) {
                QWidget* overlay = qobject_cast<QWidget*>(obj);
                if (overlay) {
                    QWidget* container = overlay->property("container").value<QWidget*>();
                    QPushButton* closeBtn = overlay->property("closeBtn").value<QPushButton*>();

                    if (container) {
                        int x = (overlay->width() - container->width()) / 2;
                        int y = (overlay->height() - container->height()) / 2;
                        container->move(x, y);
                    }

                    if (closeBtn) {
                        closeBtn->move(overlay->width() - 50, 10);
                    }
                }
            }
            return false;
        }
    };

    overlay->installEventFilter(new ResizeFilter(overlay));
}

bool Helpers::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::Resize) {
        QWidget* overlay = qobject_cast<QWidget*>(obj);
        if (overlay) {
            QWidget* container = overlay->property("videoContainer").value<QWidget*>();
            QPushButton* closeBtn = overlay->property("closeBtn").value<QPushButton*>();

            if (container) {
                int x = (overlay->width() - container->width()) / 2;
                int y = (overlay->height() - container->height()) / 2;
                container->move(x, y);
            }

            if (closeBtn) {
                closeBtn->move(overlay->width() - 50, 10);
            }
        }
    }
    return QObject::eventFilter(obj, event);
}


QString Helpers::dialogWithVariatives(QWidget* parent, QString forDialogTitle, QString forDialogText, const std::vector<FileRecord>& options)
{
    QDialog formatDialog(parent);
    formatDialog.setWindowTitle(forDialogTitle);
    formatDialog.resize(300, 150);

    QVBoxLayout layout(&formatDialog);

    QLabel label(forDialogText, &formatDialog);
    layout.addWidget(&label);

    QComboBox formatComboBox(&formatDialog);

    for (const auto& element : options) {
        if (!element.description.isEmpty()) {
            formatComboBox.addItem(element.description, element.path);
        }
    }

    layout.addWidget(&formatComboBox);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &formatDialog);
    layout.addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &formatDialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &formatDialog, &QDialog::reject);

    if (formatDialog.exec() != QDialog::Accepted) {
        return QString();
    }

    QString selectedText = formatComboBox.currentText();
    QString selectedOption = formatComboBox.currentData().toString();

    qDebug() << "Выбран текст:" << selectedText;
    qDebug() << "Option:" << selectedOption;

    if (selectedOption.isEmpty()) {
        return QString();
    }

    return selectedOption;
}

