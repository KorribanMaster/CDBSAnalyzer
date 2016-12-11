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

    QList<HistInfo> mInfoList;
    int mRows;
    int mColumns;



public slots:
    void updateInfoList(QList<HistInfo> list);
private:

signals:
};

#endif // CDBHISTTABLE_H
