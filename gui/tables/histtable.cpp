#include "histtable.h"

HistTable::HistTable(QObject *parent)
    : QAbstractTableModel(parent)
{
    mRows =1;
    mColumns = 5;
    const HistInfo info;
    mInfoList.append(info);

}

QVariant HistTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
        {
            if (orientation == Qt::Horizontal) {
                switch (section)
                {
                case 0:
                    return QString("Name");
                case 1:
                    return QString("Roi Width");
                case 2:
                    return QString("Roi Length");
                case 3:
                    return QString("Bin Width");
                case 4:
                    return QString("Counts");
                }
            }
    }
    return QVariant::Invalid;
}

int HistTable::rowCount(const QModelIndex &parent) const
{
    //if (parent.isValid())
        return mRows;

}

int HistTable::columnCount(const QModelIndex &parent) const
{
    //if (parent.isValid())
        return mColumns;
}

QVariant HistTable::data(const QModelIndex &index, int role) const
{
    //if (!index.isValid())
    //    return QVariant();

    int row = index.row();
    int col = index.column();
    const HistInfo *info;
    switch(role){
    case Qt::DisplayRole:
        switch (col) {
        case 0:
            info = &mInfoList.at(row);
            return info->name();
        case 1:
            info = &mInfoList.at(row);
            return info->roiWidth();
        case 2:
            info = &mInfoList.at(row);
            return info->roiLength();
        case 3:
            info = &mInfoList.at(row);
            return info->binWidth();
        case 4:
            info = &mInfoList.at(row);
            return info->counts();
        }

    }
    return QVariant::Invalid;

}

void HistTable::updateInfoList(QList<HistInfo> list){
    mColumns = list.size();
    mInfoList = list;
    QModelIndex tl= createIndex(0,0);
    QModelIndex rb= createIndex(mRows-1,mColumns-1);
    qDebug() << data(tl);
    emit dataChanged(tl,rb);
}
