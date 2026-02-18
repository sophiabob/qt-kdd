#include "meshmodel.h"

MeshModel::MeshModel(QObject *parent) : QSqlQueryModel(parent) {}

QVariant MeshModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();

    // Для роли цвета фона
    if (role == Qt::BackgroundRole) {
        int col = index.column();
        int row = index.row();

        // Получаем статус из столбца mesh_status (предполагаем, что это 2-й столбец)
        if (col == 1) { // Или другой столбец, где у вас mesh_status
            QModelIndex statusIndex = this->index(row, 1); // Индекс ячейки со статусом
            QString status = QSqlQueryModel::data(statusIndex, Qt::DisplayRole).toString();

            // Выбираем цвет в зависимости от статуса
            if (status == "0") return QColor(Qt::red);       // Заблокирована
            else if (status == "1") return QColor(Qt::green); // С дозиметром
            else if (status == "2") return QColor(Qt::white); // Пустая
            else if (status == "3") return QColor(Qt::gray);  // Проблема с датчиком
            else if (status == "4") return QColor(Qt::darkRed); // Сломана
            else if (status == "5") return QColor(Qt::blue);   // Открыта
            else if (status == "6") return QColor(Qt::yellow); // Другое
        }
    }

    // Для всех остальных ролей используем стандартное поведение
    return QSqlQueryModel::data(index, role);
}
