#pragma once

#include <QAbstractListModel>

struct ReadoutRow
{
    QString label;
    QString value;
    int status = 0;
};

class ReadoutsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    // Qt's naming fields
    enum Roles
    {
        LabelRole = Qt::UserRole + 1,
        ValueRole,
        StatusRole
    };

    explicit ReadoutsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

    void SetRows(const QVector<ReadoutRow> &rows);
    void UpdateRow(int row, const QString &value, int status);

private:
    QVector<ReadoutRow> rows_;
};