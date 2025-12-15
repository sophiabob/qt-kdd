#include <QSqlQueryModel>
#include <QColor>

class MeshModel : public QSqlQueryModel {
    Q_OBJECT
public:
    explicit MeshModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};
