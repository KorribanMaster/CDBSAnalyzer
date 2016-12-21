#ifndef CDBHISTTABLE_H
#define CDBHISTTABLE_H

#include <QAbstractTableModel>

#include "histinfo.h"


class HistTable : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit HistTable(QObject *parent = 0);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QStringList getChecked();
private:
    QVector<HistInfo*> mInfoList;
    QVector<bool> mCheckedList;
    int mRows;
    int mColumns;



public slots:
    void updateInfoList(QList<HistInfo *> list);
private:

signals:
};

#endif // CDBHISTTABLE_H
