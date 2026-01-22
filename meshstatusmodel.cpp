#include "meshstatusmodel.h"
#include <QPainter>
#include <QDebug>
#include <QDir>

MeshStatusModel::MeshStatusModel(const QString& user_id, const QString& doz_id, int width, QObject *parent)
    : QAbstractTableModel(parent),
    m_width(qMax(1, width)),
    m_userId(user_id),
    m_dozId(doz_id),
    m_showUserIcon(!user_id.isEmpty() && user_id != "0"),
    m_showDozIcon(!doz_id.isEmpty() && doz_id != "0")
{
    qRegisterMetaType<MeshItem>("MeshItem");
    loadIcons();
}

QString MeshStatusModel::detectOS() const
{
#ifdef Q_OS_WIN
    return "Windows";
#elif defined(Q_OS_LINUX)
    return "Linux";
#else
    return "Unknown";
#endif
}

void MeshStatusModel::loadIcons()
{

    QString currentFile = __FILE__;
    QFileInfo fileInfo(currentFile);
    QDir sourceDir = fileInfo.dir();
    sourceDir.cdUp();
    QString iconPath = sourceDir.absolutePath() + "/img/hum.png";
    QString iconDoz = sourceDir.absolutePath() + "/img/doz.png";

    /*QString os = detectOS();
    if (os == "Windows") {
        iconPath = "C:/Users/sophi/Documents/kdd-folder/kdd_nice/img/hum.png";
        iconDoz = "C:/Users/sophi/Documents/kdd-folder/kdd_nice/img/doz.png";
        iconPath = "C:/Users/sophia/Documents/work work work/kdd/img/hum.png";
        iconDoz = "C:/Users/sophia/Documents/work work work/kdd/img/doz.png";
    } else if (os == "Linux") {
        iconPath = "/home/sds/share (19)/kdd_nice_02.07.25/img/hum.png";
        iconDoz = "/home/sds/share (19)/kdd_nice_02.07.25/img/doz.png";
    } else {
        iconPath = ":/default/img/hum.png";
        iconDoz = ":/default/img/doz.png";
    }*/

    if (m_showUserIcon) {
        m_humIcon = QIcon(iconPath);
        if (m_humIcon.isNull()) {
            m_humIcon = QIcon::fromTheme("user-identity", QIcon(":/icons/default-user.png"));
        }
    }

    if (m_showDozIcon) {
        m_dozIcon = QIcon(iconDoz);
        if (m_dozIcon.isNull()) {
            m_dozIcon = QIcon::fromTheme("device", QIcon(":/icons/default-doz.png"));
        }
    }
}

QPixmap MeshStatusModel::createCellPixmap(const MeshItem &item) const
{
    QPixmap pixmap(CELL_WIDTH, CELL_HEIGHT);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    int contentHeight = TEXT_AREA_HEIGHT;
    int yPos = ITEM_PADDING;

    // 1. Верхняя иконка пользователя (если нужно показывать и есть у элемента)
    if (m_showUserIcon && item.hasUser()) {
        painter.drawPixmap((CELL_WIDTH - ICON_SIZE)/2, yPos,
                           m_humIcon.pixmap(ICON_SIZE, ICON_SIZE));
        yPos += ICON_SIZE + ITEM_PADDING;
        contentHeight += ICON_SIZE + ITEM_PADDING;
    }

    // 2. Текст ID по центру
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(32);

    QFontMetrics fm(font);
    QString elidedText = fm.elidedText(item.id, Qt::ElideRight, CELL_WIDTH - 2*ITEM_PADDING);

    painter.setFont(font);
    painter.drawText(QRect(ITEM_PADDING, yPos,
                           CELL_WIDTH - 2*ITEM_PADDING, TEXT_AREA_HEIGHT),
                     Qt::AlignCenter, elidedText);
    yPos += TEXT_AREA_HEIGHT + ITEM_PADDING;

    // 3. Нижняя иконка DOZ (если нужно показывать и есть у элемента)
    if (m_showDozIcon && item.hasDoz()) {
        painter.drawPixmap((CELL_WIDTH - ICON_SIZE)/2, yPos,
                           m_dozIcon.pixmap(ICON_SIZE, ICON_SIZE));
        contentHeight += ICON_SIZE + ITEM_PADDING;
    }

    return pixmap;
}

int MeshStatusModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : (totalItems() + m_width - 1) / m_width;
}

int MeshStatusModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_width;
}

QVariant MeshStatusModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !isValidIndex(index.row(), index.column()))
        return QVariant();

    const MeshItem &item = m_items.at(sourceIndex(index.row(), index.column()));

    switch (role) {
    case Qt::DisplayRole:
        return item.id;

    case Qt::DecorationRole:
        return QIcon(createCellPixmap(item));

    case Qt::SizeHintRole:
        return QSize(CELL_WIDTH, CELL_HEIGHT / 2);

    case Qt::UserRole:
        return QVariant::fromValue(item);

    case Qt::ToolTipRole: {
        QString tooltip = QString("ID: %1\nStatus: %2").arg(item.id).arg(item.status);
        if (item.hasUser()) tooltip += QString("\nUser: %1").arg(item.userId);
        if (item.hasDoz()) tooltip += QString("\nDOZ: %1").arg(item.dozId);
        return tooltip;
    }

    case Qt::BackgroundRole:
        return getStatusColor(item.status);

    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;

    default:
        return QVariant();
    }
}

QColor MeshStatusModel::getStatusColor(const QString& status) const
{
    static const QColor colors[] = {
        QColor(255, 200, 200), // 0
        QColor(200, 255, 200), // 1
        Qt::white,             // 2
        QColor(220, 220, 220), // 3
        QColor(255, 150, 150), // 4
        QColor(200, 200, 255), // 5
        QColor(255, 255, 150)  // 6
    };
    int s = status.toInt();
    return (s >= 0 && s <= 6) ? colors[s] : Qt::white;
}

Qt::ItemFlags MeshStatusModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void MeshStatusModel::loadData(const QVector<MeshItem> &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}

void MeshStatusModel::setWidth(int width)
{
    width = qMax(1, width);
    if (m_width != width) {
        beginResetModel();
        m_width = width;
        endResetModel();
    }
}

int MeshStatusModel::sourceIndex(int row, int col) const
{
    return row * m_width + col;
}

bool MeshStatusModel::isValidIndex(int row, int col) const
{
    const int idx = sourceIndex(row, col);
    return idx >= 0 && idx < totalItems();
}
