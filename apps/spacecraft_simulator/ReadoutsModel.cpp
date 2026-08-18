#include "ReadoutsModel.h"

ReadoutsModel::ReadoutsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ReadoutsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(rows_.size());
}

QVariant ReadoutsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rows_.size())
        return {};

    const ReadoutRow &row = rows_[index.row()];

    switch (role)
    {
    case LabelRole:
        return row.label;
    case ValueRole:
        return row.value;
    case StatusRole:
        return row.status;
    default:
        return {};
    }
}

QHash<int, QByteArray> ReadoutsModel::roleNames() const
{
    return {
        {LabelRole, "label"},
        {ValueRole, "value"},
        {StatusRole, "status"}};
}

void ReadoutsModel::SetRows(const QVector<ReadoutRow> &rows)
{
    beginResetModel();
    rows_ = rows;
    endResetModel();
}

void ReadoutsModel::UpdateRow(int row, const QString &value, int status)
{
    if (row < 0 || row >= rows_.size())
        return;

    ReadoutRow &stored = rows_[row];

    QVector<int> changedRoles;

    if (stored.value != value)
    {
        stored.value = value;
        changedRoles.append(ValueRole);
    }

    if(stored.status != status)
    {
        stored.status = status;
        changedRoles.append(StatusRole);
    }

    if(!changedRoles.isEmpty())
        emit dataChanged(index(row), index(row), changedRoles);
}