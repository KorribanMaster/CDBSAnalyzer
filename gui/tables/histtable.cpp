#include "histtable.h"

HistTable::HistTable(QObject *parent)
    : QAbstractTableModel(parent)
{
    mRows =0;
    mColumns = 7;
//    HistInfo *info;
//    mInfoList.append(info);
    mCheckedList.resize(1024);
    mInfoList.reserve(1024);
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
                case 5:
                    return QString("Size");
                case 6:
                    return QString("Ref Name");
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
    if (mRows==0){
        return QVariant();
    }
    switch(role){
    case Qt::DisplayRole:
        switch (col) {
        case 0:
            return mInfoList.at(row).name();
        case 1:
            return mInfoList.at(row).roiWidth();
        case 2:
            return mInfoList.at(row).roiLength();
        case 3:
            return mInfoList.at(row).binWidth();
        case 4:
            return mInfoList.at(row).counts();
        case 5:
            return mInfoList.at(row).size();
        case 6:
            return mInfoList.at(row).ref();
        }
    case Qt::CheckStateRole:
        if(col==0){
            if(mCheckedList.at(row)==1){
                return Qt::Checked;
            }
            else{
                return Qt::Unchecked;
            }

        }


    }
    return QVariant::Invalid;

}

void HistTable::updateInfoList(QList<HistInfo> list){
    int tmp =mRows;
    mRows = list.size();
    beginInsertRows(QModelIndex(), tmp,mRows-tmp);
    //mInfoList = list;
    for(int i=tmp;i<mRows;i++){
        mInfoList.append(list.at(i));
    }
    //mCheckedList.resize(mRows);
    QModelIndex tl= createIndex(tmp-1,0);
    QModelIndex rb= createIndex(mRows-1,mColumns-1);
    //qDebug() << data(tl);
    emit dataChanged(tl,rb);
    endInsertRows();


}

Qt::ItemFlags HistTable::flags(const QModelIndex &index) const
{
    if(index.column()==0){
        return Qt::ItemIsUserCheckable | QAbstractTableModel::flags(index);
    }
    else{
        return QAbstractTableModel::flags(index);
    }
}

bool HistTable::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::CheckStateRole)
    {
        mCheckedList.replace(index.row(),value.toBool());

    }
    emit dataChanged(index,index);
    return true;
}

QStringList HistTable::getChecked(){
    QStringList list;
    for (int i=0; i<mCheckedList.size();i++) {
       if (mCheckedList.at(i)){
           list.append(mInfoList.at(i).name());
       }
    }
    return list;
}
