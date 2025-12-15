#ifndef MESHSTATUSMODEL_H
#define MESHSTATUSMODEL_H

#include <QAbstractTableModel>
#include <QIcon>
#include <QPixmap>

struct MeshItem {
    QString id;
    QString status;
    QString userId;
    QString dozId;

    bool hasUser() const { return !userId.isEmpty() && userId != "0"; }
    bool hasDoz() const { return !dozId.isEmpty() && dozId != "0"; }
};

Q_DECLARE_METATYPE(MeshItem)

class MeshStatusModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    static const int CELL_WIDTH = 120;
    static const int CELL_HEIGHT = 120;
    static const int ICON_SIZE = 32;
    static const int ITEM_PADDING = 4;
    static const int TEXT_AREA_HEIGHT = 40;

    explicit MeshStatusModel(const QString& user_id, const QString& doz_id, int width, QObject *parent = nullptr);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void loadData(const QVector<MeshItem> &items);
    void setWidth(int width);

protected:
    QString detectOS() const;
    void loadIcons();
    QPixmap createCellPixmap(const MeshItem &item) const;
    QColor getStatusColor(const QString& status) const;
    int sourceIndex(int row, int col) const;
    bool isValidIndex(int row, int col) const;
    int totalItems() const { return m_items.size(); }

private:
    int m_width;
    QString m_userId;
    QString m_dozId;
    QVector<MeshItem> m_items;
    QIcon m_humIcon;
    QIcon m_dozIcon;
    bool m_showUserIcon;
    bool m_showDozIcon;

    // Вспомогательные функции для фильтрации
   // QString extractUserIdFromCombo(QComboBox *combo);
    void showAllRows();
};

#endif // MESHSTATUSMODEL_H
