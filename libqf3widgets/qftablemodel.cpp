#include "qftablemodel.h"
#include <QDebug>

QFTableModel::QFTableModel(QObject * parent):
    QAbstractTableModel(parent)
{
    readonly=true;
    readonlyButStillCheckable=false;
    doEmitSignals=true;
    defaultEditValue=QVariant();
    verticalHeaderShowRowNumbers=false;
    rows=1;
    columns=1;
    //quint32 a=xyAdressToUInt32(5, 5);
    //std::cout<<"adress test: "<<a<<" => row="<<UInt32ToRow(a)<<", column="<<UInt32ToColumn(a)<<"\n";
}

QFTableModel::~QFTableModel()
{
    //dtor
}

int QFTableModel::rowCount(const QModelIndex &parent) const {
    //std::cout<<"rowCount: "<<rows<<"\n";
    return rows;
};
int QFTableModel::columnCount(const QModelIndex &parent) const {
    //std::cout<<"columnCount: "<<columns<<"\n";
    return columns;
};


QVariant QFTableModel::data(const QModelIndex &index, int role) const {
    if (index.isValid()) {
        quint32 a=xyAdressToUInt32(index.row(), index.column());
        //std::cout<<"QFTableModel::data("<<a<<")"<<std::endl;
        if (role == Qt::DisplayRole) {
            if (dataMap.contains(a)) return dataMap[a];
        } else if (role == Qt::EditRole) {
            QVariant v=QVariant();
            if (dataEditMap.contains(a)) v=dataEditMap.value(a, v);
            if (dataMap.contains(a)) v=dataMap.value(a, v);
            if (!v.isValid()) v=defaultEditValue;
            return v;
        } else if (role == Qt::CheckStateRole) {
            if (dataCheckedMap.contains(a)) return dataCheckedMap[a];
        } else if (role == Qt::BackgroundRole) {
            if (dataBackgroundMap.contains(a)) return dataBackgroundMap[a];
        } else if (role == Qt::ToolTipRole) {
            if (dataMap.contains(a)) return tr("row: <i>%2</i><br>column: <i>%3 (\"%4\")</i><br>datatype: <i>%1</i><br>data: <b>%5</b>").arg(getQVariantType(dataMap[a])).arg(index.row()).arg(index.column()).arg(columnNames.value(index.column(), "")).arg(getQVariantData(index.data()));
        } else if (role >= Qt::UserRole) {
            if (moreDataMap.contains(a)) {
                return moreDataMap[a].value(role, QVariant());
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags QFTableModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return 0;
    if (!readonly) return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
    if (readonlyButStillCheckable) return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
         if (role==Qt::DisplayRole) {
            if (section<columnNames.size()) return columnNames[section];
         } else if (headerDataMap.contains(section)) {
             return headerDataMap[section].value(role, QVariant());
         }
     } else if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
         if (verticalHeaderShowRowNumbers) return QString::number(section+1);
     }

     return QVariant();
}

bool QFTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;

    //qDebug()<<index<<"("<<role<<" chk="<<Qt::CheckStateRole<<")"<<value;
    if (index.isValid() && role == Qt::EditRole && !readonly) {
        quint32 a=xyAdressToUInt32(index.row(), index.column());
        QVariant old=QVariant();
        if (dataMap.contains(a)) old=dataMap[a];
        dataMap[a]=value;
        //dataEditMap.remove(a);
        hasDataChanged=hasDataChanged | (old!=value);
        doEmitSignals=oldEmit;
        if (doEmitSignals) emit dataChanged(index, index);
        return true;
    } else if (index.isValid() && role == Qt::CheckStateRole && (!readonly || readonlyButStillCheckable)) {
        quint32 a=xyAdressToUInt32(index.row(), index.column());
        QVariant old=QVariant();
        if (dataCheckedMap.contains(a)) old=dataCheckedMap[a];
        dataCheckedMap[a]=value;
        hasDataChanged=hasDataChanged | (old!=value);
        doEmitSignals=oldEmit;
        if (doEmitSignals) emit dataChanged(index, index);
        return true;
    } else if (index.isValid() && role >= Qt::UserRole && !readonly) {
        quint32 a=xyAdressToUInt32(index.row(), index.column());
        QVariant old=QVariant();
        if (moreDataMap.contains(a)) old=moreDataMap[a].value(role, QVariant());
        moreDataMap[a].insert(role, value);
        hasDataChanged=hasDataChanged | (old!=value);
        doEmitSignals=oldEmit;
        if (doEmitSignals) emit dataChanged(index, index);
        return true;
    }
    doEmitSignals=oldEmit;
    return false;
}

void QFTableModel::swapCells(quint16 row1, quint16 column1, quint16 row2, quint16 column2) {
    if (readonly) return;
    quint32 adr1=xyAdressToUInt32(row1, column1);
    quint32 adr2=xyAdressToUInt32(row2, column2);
    QVariant d1, d2;

    d1=dataMap.value(adr1, QVariant());
    d2=dataMap.value(adr2, QVariant());
    dataMap.remove(adr1);
    dataMap.remove(adr2);
    if (d2.isValid()) dataMap[adr1]=d2;
    if (d1.isValid()) dataMap[adr2]=d1;

    d1=dataEditMap.value(adr1, QVariant());
    d2=dataEditMap.value(adr2, QVariant());
    dataEditMap.remove(adr1);
    dataEditMap.remove(adr2);
    if (d2.isValid()) dataEditMap[adr1]=d2;
    if (d1.isValid()) dataEditMap[adr2]=d1;

    d1=dataBackgroundMap.value(adr1, QVariant());
    d2=dataBackgroundMap.value(adr2, QVariant());
    dataBackgroundMap.remove(adr1);
    dataBackgroundMap.remove(adr2);
    if (d2.isValid()) dataBackgroundMap[adr1]=d2;
    if (d1.isValid()) dataBackgroundMap[adr2]=d1;

    d1=dataCheckedMap.value(adr1, QVariant());
    d2=dataCheckedMap.value(adr2, QVariant());
    dataCheckedMap.remove(adr1);
    dataCheckedMap.remove(adr2);
    if (d2.isValid()) dataCheckedMap[adr1]=d2;
    if (d1.isValid()) dataCheckedMap[adr2]=d1;

    QHash<int, QVariant> hd1, hd2;
    hd1=moreDataMap.value(adr1, QHash<int, QVariant>());
    hd2=moreDataMap.value(adr2, QHash<int, QVariant>());
    moreDataMap.remove(adr1);
    moreDataMap.remove(adr2);
    if (hd2.size()>0) moreDataMap[adr1]=hd2;
    if (hd1.size()>0) moreDataMap[adr2]=hd1;

    if (doEmitSignals) {
        emit dataChanged(index(row1, column1), index(row1, column1));
        emit dataChanged(index(row2, column2), index(row2, column2));
    }
}

void QFTableModel::resize(quint16 rows, quint16 columns) {
    if (readonly) return;
    //std::cout<<"  resize("<<rows<<", "<<columns<<"): 1\n";
    //quint16 oldrows=this->rows;
    int oldcolumns=this->columns;
    int oldrows=this->rows;

    if (columns>oldcolumns) {
        for (int i=oldcolumns; i<=columns; i++) {
            columnNames.append(QString::number(i));
        }
    } else if (columns<oldcolumns) {
        for (int i=oldcolumns-1; i>=columns; i--) {
            if (!columnNames.isEmpty()) columnNames.removeLast();
        }
    }


    //std::cout<<"  resize("<<rows<<", "<<columns<<"): 2\n";
    this->rows=rows;
    this->columns=columns;

    QList<quint32> idx=dataMap.keys();
    QList<quint32> didx;
    //std::cout<<"  resize("<<rows<<", "<<columns<<"): 3\n";
    if (idx.size()>0) for (int i = idx.size()-1; i>=0; i--) {
        quint16 r=UInt32ToRow(idx[i]);
        quint16 c=UInt32ToColumn(idx[i]);
        if ((r>=rows) || (c>=columns)) {
            didx.append(idx[i]);
        }
    }
    //std::cout<<"  resize("<<rows<<", "<<columns<<"): 4\n";
    if (didx.size()>0) for (int i = didx.size()-1; i>=0; i--) {
        dataMap.remove(didx[i]);
        if (dataEditMap.contains(didx[i])) dataEditMap.remove(didx[i]);
        if (dataBackgroundMap.contains(didx[i])) dataBackgroundMap.remove(didx[i]);
        if (dataCheckedMap.contains(didx[i])) dataCheckedMap.remove(didx[i]);
        if (moreDataMap.contains(didx[i])) moreDataMap.remove(didx[i]);
    }

    /*for (int c=0; c<columns; c++) {
        for (int r=0; r<rows; r++) {
            if (c>=oldcolumns || r>=oldrows) dataEditMap[xyAdressToUInt32(r,c)]=defaultEditValue;
        }
    }*/
    //std::cout<<"  resize("<<rows<<", "<<columns<<"): 5\n";
    if (doEmitSignals)  {
        if (columns>oldcolumns) {
            beginInsertColumns(QModelIndex(), oldcolumns, columns-1);
            endInsertColumns();
        }
        if (columns<oldcolumns) {
            beginRemoveColumns(QModelIndex(), columns, oldcolumns-1);
            endRemoveColumns();
        }
        if (rows>oldrows) {
            beginInsertRows(QModelIndex(), oldrows, rows-1);
            endInsertRows();
        }
        if (rows<oldrows) {
            beginRemoveRows(QModelIndex(), rows, oldrows-1);
            endRemoveRows();
        }
        //reset();
    }
    //std::cout<<"  resize("<<rows<<", "<<columns<<"): 6\n";
}

void QFTableModel::insertRow(quint16 r) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    appendRow();
    if (r<rows-1) for (int i=rows-2; i>=r; i--) {
        for (int c=0; c<columns; c++) {
            copyCell(i+1, c, i, c);
        }
    }
    for (int c=0; c<columns; c++) {
        setCell(r, c, defaultEditValue);
        quint32 a=xyAdressToUInt32(r, c);
        dataEditMap.remove(a);
        dataBackgroundMap.remove(a);
        dataCheckedMap.remove(a);
        moreDataMap.remove(a);
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) {
        beginInsertRows(QModelIndex(),r,r);
        endInsertRows();
        //reset();
    }
}

void QFTableModel::insertColumn(quint16 c) {
    //std::cout<<"  insertColumn("<<c<<")\n";
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    appendColumn();
    while (columnNames.size()<columns) columnNames.append("");
    if (c<columns-1) for (int i=columns-2; i>=c; i--) {
        columnNames[i+1]=columnNames[i];
        for (int r=0; r<rows; r++) {
            copyCell(r, i+1, r, i);
        }
    }
    for (int r=0; r<rows; r++) {
        setCell(r, c, defaultEditValue);
        quint32 a=xyAdressToUInt32(r, c);
        dataEditMap.remove(a);
        dataBackgroundMap.remove(a);
        dataCheckedMap.remove(a);
        moreDataMap.remove(a);
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) {
        //reset();
        beginInsertColumns(QModelIndex(),c,c);
        endInsertColumns();
    }
    //std::cout<<"  insertColumn("<<c<<") ... DONE\n";
}

void QFTableModel::deleteRow(quint16 r) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    if (r>=rows ) return;
    for (int i=r; i<rows-1; i++) {
        for (int c=0; c<columns; c++) {
            copyCell(i, c, i+1, c);
        }
    }
    resize(rows-1, columns);
    doEmitSignals=oldEmit;
    if (doEmitSignals) {
        beginRemoveRows(QModelIndex(), r, r);
        endRemoveRows();
        //reset();
    }
}

/*
void QFTableModel::deleteRows(QList<quint16> rs) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    int removed=0;
    int lastR=-1;
    int startR=-1;
    QList<QPair<int, int> > events;
    for (int i=rs.size()-1; i>=0; i--) {
        if (rs[i]<rows ) {
            for (int i=rs[i]; i<rows-1; i++) {
                for (int c=0; c<columns; c++) {
                    setCell(i, c, cell(i+1, c));
                }
            }
            if (lastR==-1) {
                lastR=rs[i];
                startR=rs[i];
            } else {
                if (rs[i]-lastR>1) {
                    events.append(qMakePair(startR, lastR));
                    startR=rs[i];
                }
                lastR=rs[i];
            }
            removed++;
        }
    }
    if (removed>0) events.append(qMakePair(startR, lastR));
    resize(rows-removed, columns);
    doEmitSignals=oldEmit;
    if (doEmitSignals && removed>0) {
        for (int i=0; i<events.size(); i++) {
            beginRemoveRows(QModelIndex(), events[i].first, events[i].second);
            endRemoveRows();
        }
        //reset();
    }
}


void QFTableModel::deleteColumns(QList<quint16> rs) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    int removed=0;
    int lastR=-1;
    int startR=-1;
    QList<QPair<int, int> > events;
    for (int i=rs.size()-1; i>=0; i--) {
        if (rs[i]<columns) {
            for (int r=0; r<rows; r++) {
                setCell(r, rs[i], cell(r, rs[i]+1));
            }
            if (rs[i]+1<columnNames.size()) columnNames[rs[i]]=columnNames[rs[i]+1];
            if (lastR==-1) {
                lastR=rs[i];
                startR=rs[i];
            } else {
                if (rs[i]-lastR>1) {
                    events.append(qMakePair(startR, lastR));
                    startR=rs[i];
                }
                lastR=rs[i];
            }
            removed++;
        }
    }
    if (removed>0) events.append(qMakePair(startR, lastR));
    resize(rows, columns-removed);
    doEmitSignals=oldEmit;
    if (doEmitSignals) {
        for (int i=0; i<events.size(); i++) {
            beginRemoveColumns(QModelIndex(), events[i].first, events[i].second);
            endRemoveColumns();
        }
    }
}*/

void QFTableModel::deleteColumn(quint16 c) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    if (c>=columns) return;
    for (int i=c; i<columns-1; i++) {
        for (int r=0; r<rows; r++) {
            copyCell(r, i, r, i+1);
        }
        if (i+1<columnNames.size()) columnNames[i]=columnNames[i+1];
    }
    resize(rows, columns-1);
    doEmitSignals=oldEmit;
    if (doEmitSignals) {
        beginRemoveColumns(QModelIndex(), c, c);
        endRemoveColumns();
//        reset();
    }
}

void QFTableModel::emptyColumn(quint16 c)
{
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    if (c>=columns) return;

    for (int r=0; r<rows; r++) {
        deleteCell(r, c);
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) {
        emit dataChanged(index(0, c), index(rows-1, c));
    }
}

void QFTableModel::deleteCell(quint16 row, quint16 column) {
    if (readonly) return;
    quint32 a=xyAdressToUInt32(row, column);
    dataMap.remove(a);
    dataEditMap.remove(a);
    dataBackgroundMap.remove(a);
    dataCheckedMap.remove(a);
    moreDataMap.remove(a);
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::deleteCells(QModelIndexList selection) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    int r1=0, r2=rows-1;
    int c1=0, c2=columns-1;
    bool first=true;
    for (int i=0; i<selection.size(); i++) {
        deleteCell(selection[i].row(), selection[i].column());
        if (first) {
            r1=r2=selection[i].row();
            c1=c2=selection[i].column();
            first=false;
        } else {
            if (selection[i].row()<r1 ) r1=selection[i].row();
            if (selection[i].row()>r2 ) r2=selection[i].row();
            if (selection[i].column()<c1 ) c1=selection[i].column();
            if (selection[i].column()>c2 ) c2=selection[i].column();
        }
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) {
        //reset();
        for (int i=0; i<selection.size(); i++) {
            emit dataChanged(index(r1,c1),index(r2,c2));
        }
    }
}

void QFTableModel::clear() {
    if (readonly) return;
    rows=0;
    columns=0;
    dataMap.clear();
    dataEditMap.clear();
    dataBackgroundMap.clear();
    dataCheckedMap.clear();
    moreDataMap.clear();
    columnNames.clear();
    if (doEmitSignals) reset();
}

bool QFTableModel::changeDatatype(quint16 row, quint16 column, QVariant::Type newType) {
    if (readonly || (row>=rows) || (column>=columns)) return false;
    quint32 a=xyAdressToUInt32(row, column);
    if (dataMap.contains(a)) {
        if (dataEditMap[a].canConvert(newType)) {
            dataEditMap[a].convert(newType);
        } else {
            dataEditMap[a]=QVariant(newType);
        }

        if (dataMap[a].canConvert(newType)) {
            dataMap[a].convert(newType);
            //if (dataEditMap.contains(a)) dataEditMap[a].convert(newType);
            return true;
        } else {
            dataMap[a]=QVariant(newType);            
            return false;
        }
    } else {
        dataMap[a]=QVariant(newType);
        dataEditMap[a]=QVariant(newType);
        return true;
    }
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setCell(quint16 row, quint16 column, QVariant value) {
    //std::cout<<"setCell("<<row<<", "<<column<<", '"<<value.toString().toStdString()<<"' ["<<value.typeName()<<"])\n";
    if (readonly || (row>=rows) || (column>=columns)) return;
    quint32 a=xyAdressToUInt32(row, column);
    dataMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setColumn(quint16 column, QList<QVariant>& value)
{
    //std::cout<<"setCell("<<row<<", "<<column<<", '"<<value.toString().toStdString()<<"' ["<<value.typeName()<<"])\n";
    if (readonly || (column>=columns)) return;
    quint16 row=0;
    for (int i=0; i<value.size(); i++) {
        if (row<rows) {
            quint32 a=xyAdressToUInt32(row, column);
            dataMap[a]=value[i];
        }
        row++;
    }
    if (doEmitSignals) emit dataChanged(index(0, column), index(row-1, column));
}


void QFTableModel::copyCell(quint16 row, quint16 column, quint16 row_old, quint16 column_old)
{
    if (readonly || (row>=rows) || (column>=columns) || (row_old>=rows) || (column_old>=columns)) return;
    quint32 a=xyAdressToUInt32(row, column);
    quint32 a_old=xyAdressToUInt32(row_old, column_old);

    QVariant d;
    d=dataMap.value(a_old, QVariant());
    dataMap.remove(a);
    if (d.isValid()) dataMap[a]=d;

    d=dataEditMap.value(a_old, QVariant());
    dataEditMap.remove(a);
    if (d.isValid()) dataEditMap[a]=d;

    d=dataBackgroundMap.value(a_old, QVariant());
    dataBackgroundMap.remove(a);
    if (d.isValid()) dataBackgroundMap[a]=d;

    d=dataCheckedMap.value(a_old, QVariant());
    dataCheckedMap.remove(a);
    if (d.isValid()) dataCheckedMap[a]=d;

    QHash<int, QVariant> hd;
    hd=moreDataMap.value(a_old, QHash<int, QVariant>());
    moreDataMap.remove(a);
    if (hd.size()>0) moreDataMap[a]=hd;
}

void QFTableModel::setCellCreate(quint16 row, quint16 column, QVariant value) {
    if (readonly) return;
    if ((row>=rows) || (column>=columns)) {
        resize(qMax(rows, quint16(row+1)), qMax(columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    dataMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setColumnCreate(quint16 column, QList<QVariant>& value)
{
    if (readonly) return;
    if ((value.size()>rows) || (column>=columns)) {
        resize(qMax(rows, quint16(value.size())), qMax(columns, quint16(column+1)));
    }

    setColumn(column, value);


}
void QFTableModel::setCellEditRole(quint16 row, quint16 column, QVariant value) {
    //std::cout<<"setCell("<<row<<", "<<column<<", '"<<value.toString().toStdString()<<"' ["<<value.typeName()<<"])\n";
    if (readonly || (row>=rows) || (column>=columns)) return;
    quint32 a=xyAdressToUInt32(row, column);
    dataEditMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setCellEditRoleCreate(quint16 row, quint16 column, QVariant value) {
    if (readonly) return;
    if ((row>=rows) || (column>=columns)) {
        resize(qMax(rows, quint16(row+1)), qMax(columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    dataEditMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setCellBackgroundRole(quint16 row, quint16 column, QVariant value) {
    if (readonly || (row>=rows) || (column>=columns)) return;
    quint32 a=xyAdressToUInt32(row, column);
    dataBackgroundMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setCellBackgroundRoleCreate(quint16 row, quint16 column, QVariant value) {
    if (readonly) return;
    if ((row>=rows) || (column>=columns)) {
        resize(qMax(rows, quint16(row+1)), qMax(columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    dataBackgroundMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setCellCheckedRole(quint16 row, quint16 column, QVariant value) {
    if (readonly || (row>=rows) || (column>=columns)) return;
    quint32 a=xyAdressToUInt32(row, column);
    dataCheckedMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setCellCheckedRoleCreate(quint16 row, quint16 column, QVariant value) {
    if (readonly) return;
    if ((row>=rows) || (column>=columns)) {
        resize(qMax(rows, quint16(row+1)), qMax(columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    dataCheckedMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}


quint16 QFTableModel::getAddRow(quint16 column, QVariant data) {
    if (readonly || (column>=columns)) return 0;
    if (rows<=0) {
        appendRow();
        setCell(0, column, data);
        return 0;
    }
    //quint16 row=0;
    for (quint16 r=0; r<rows; r++) {
        if (cell(r, column)==data) return r;
    }
    appendRow();
    setCell(rows-1, column, data);
    return rows-1;
}

void QFTableModel::setColumnHeaderData(quint16 column, int role, const QVariant &data)
{
    headerDataMap[column].insert(role, data);
}

QVariant QFTableModel::getColumnHeaderData(quint16 column, int role) const
{
    if (headerDataMap.contains(column)) return headerDataMap[column].value(role, QVariant());
    return QVariant();
}

bool QFTableModel::hasColumnHeaderData(quint16 column, int role) const
{
    if (headerDataMap.contains(column)) return headerDataMap[column].contains(role);
    return false;
}

void QFTableModel::setDefaultEditValue(QVariant defaultEditValue)
{
    this->defaultEditValue=defaultEditValue;
}

QStringList QFTableModel::getColumnTitles() const
{
    QStringList sl;
    for (int i=0; i<columnCount(); i++) {
        sl.append(headerData(i, Qt::Horizontal).toString());
    }
    return sl;
}

QVariant QFTableModel::cell(quint16 row, quint16 column) const {
    if ((row>=rows) || (column>=columns)) return QVariant();
    quint32 a=xyAdressToUInt32(row, column);
    if (dataMap.contains(a)) return dataMap[a];
    return QVariant();
}

void QFTableModel::setCellUserRole(int role, quint16 row, quint16 column, QVariant value) {
    if (readonly) return;
    if ((row>=rows) || (column>=columns)) {
        resize(qMax(rows, quint16(row+1)), qMax(columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    moreDataMap[a].insert(role, value);
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));

}

void QFTableModel::setCellUserRoleCreate(int role, quint16 row, quint16 column, QVariant value)
{
    if (readonly) return;
    if ((row>=rows) || (column>=columns)) {
        resize(qMax(rows, quint16(row+1)), qMax(columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    moreDataMap[a].insert(role, value);
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));

}

QVariant QFTableModel::cellEditRole(quint16 row, quint16 column) const {
    if ((row>=rows) || (column>=columns)) return QVariant();
    quint32 a=xyAdressToUInt32(row, column);
    if (dataEditMap.contains(a)) return dataEditMap[a];
    return QVariant();
}

QVariant QFTableModel::cellUserRole(int role, quint16 row, quint16 column) const
{
    if ((row>=rows) || (column>=columns)) return QVariant();
    quint32 a=xyAdressToUInt32(row, column);
    if (moreDataMap.contains(a)) return moreDataMap[a].value(role, QVariant());
    return QVariant();
}

void QFTableModel::setColumnTitle(quint16 column, QString name) {
    //std::cout<<"setColumnTitle("<<column<<", '"<<name.toStdString()<<")\n";
    if (readonly || (column>=columns)) return;
    for (int c=columnNames.size(); c<columns; c++) columnNames.append(QString::number(c));
    if (column<columnNames.size()) columnNames[column]=name;
    if (doEmitSignals) emit headerDataChanged(Qt::Horizontal, column, column);
}

void QFTableModel::setColumnTitleCreate(quint16 column, QString name) {
    //std::cout<<"setColumnTitle("<<column<<", '"<<name.toStdString()<<")\n";
    if (readonly) return;
    resize(rows, qMax(columns, quint16(column+1)));
    //qDebug()<<"setColumnTitleCreate("<<column<<", "<<name<<"):     columnNames.size()="<<columnNames.size();
    if (column<columnNames.size()) columnNames[column]=name;
    if (doEmitSignals) emit headerDataChanged(Qt::Horizontal, column, column);
}

QString QFTableModel::columnTitle(quint16 column) const {
    if (column>=columns) return QString("");
    if (column<columnNames.size()) return columnNames[column];
    return "";
}

bool QFTableModel::saveSYLK(const QString& filename, char format, int precision) {
    // try output SYLK file
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("ISO-8859-1"));
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    out.setLocale(loc);


    // write SYLK header
    out<<"ID;P\n";

    // write column headers
    for (quint16 c=0; c<columns; c++) {
        if (c<columnNames.size()) out<<QString("C;Y1;X%2;K\"%1\"\n").arg(columnNames[c]).arg(c+1);
        out<<QString("F;Y1;X%2;SDB\n").arg(c+1);
    }

    // write data
    for (quint16 r=0; r<rows; r++) {
        for (quint16 c=0; c<columns; c++) {
            quint32 a=xyAdressToUInt32(r, c);
            if (dataMap.contains(a)) {
                QVariant& v=dataMap[a];
                QString num="";
                switch(v.type()) {
                    case QVariant::Double:
                        num=loc.toString(v.toDouble(), format, precision);
                        break;
                    case QVariant::Int:
                    case QVariant::LongLong:
                        num=loc.toString(v.toULongLong());
                        break;
                    case QVariant::UInt:
                    case QVariant::ULongLong:
                        num=loc.toString(v.toDouble(), format, precision);
                        break;
                    default:
                        num=QString("\"%1\"").arg(v.toString().replace(';', ",").replace('\n', " ").replace('\"', " ").replace('\r', " ").replace('\t', " "));
                        break;
                }
                out<<QString("C;X%1;Y%2;N;K%3\n").arg(c+1).arg(r+2).arg(num);
            }
            //std::cout<<"("<<i<<", "<<c<<")\n";
        }
    }
    out<<QString("E\n\n");
    return true;
}

bool QFTableModel::saveCSV(const QString& filename, QString column_separator, char decimal_separator, QString header_start, char format, int precision, QModelIndexList selection) {
    // try output file
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    return saveCSV(out,  column_separator,  decimal_separator,  header_start,  format,  precision, true, selection);
}

bool QFTableModel::saveCSV(QTextStream &out, QString column_separator, char decimal_separator, QString header_start, char format, int precision, bool setCodecAndEncoding, QModelIndexList selection) {
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    if (setCodecAndEncoding) {
        out.setCodec(QTextCodec::codecForName("ISO-8859-1"));
        out.setLocale(loc);
    }

    QSet<int> usedCols, usedRows;
    bool saveCompleteTable=selection.isEmpty();
    for (int i=0; i<selection.size(); i++) {
        usedCols.insert(selection[i].column());
        usedRows.insert(selection[i].row());
    }

    // write column headers
    out<<header_start<<" ";
    int cc=0;
    for (quint16 c=0; c<columns; c++) {
        if (usedCols.contains(c)) {
            if (cc>0) out<<column_separator;
            if (c<columnNames.size()) out<<QString("\"%1\"").arg(columnNames[c]).arg(c+1);
            cc++;
        }
    }
    out<<"\n";


    // write data
    for (quint16 r=0; r<rows; r++) {
        if (usedRows.contains(r)) {
            bool first=true;
            for (quint16 c=0; c<columns; c++) {
                if (usedCols.contains(c)) {
                    quint32 a=xyAdressToUInt32(r, c);
                    if (dataMap.contains(a)) {
                        QVariant& v=dataMap[a];
                        QString num="";
                        switch(v.type()) {
                            case QVariant::Double:
                                num=loc.toString(v.toDouble(), format, precision);
                                break;
                            case QVariant::Int:
                            case QVariant::LongLong:
                                num=loc.toString(v.toULongLong());
                                break;
                            case QVariant::UInt:
                            case QVariant::ULongLong:
                                num=loc.toString(v.toDouble(), format, precision);
                                if (decimal_separator!='.') {
                                    num=num.replace('.', decimal_separator);
                                }
                                break;
                            default:
                                num=QString("\"%1\"").arg(v.toString());
                                break;
                        }
                        //out<<num<<column_separator;
                        if (!first) out<<column_separator;
                        out<<num;
                        first=false;
                    }
                }
            }
            out<<"\n";
        }
    }
    out<<QString("\n\n");
    return true;
}


bool QFTableModel::readCSV(const QString& filename, char column_separator, char decimal_separator, QString header_start, char comment_start) {
    // try output file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&file);

    return readCSV(in, column_separator,  decimal_separator,  header_start,  comment_start, 0, 0, true);
}

bool QFTableModel::readCSV(QTextStream &in, char column_separator, char decimal_separator, QString header_start, char comment_start, int start_row, int start_col, bool clearTable) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    bool ro=readonly;
    readonly=false;
    if (clearTable) clear();
    //std::cout<<"      opening '"<<filename.toStdString()<<"'\n";
    QString line=in.readLine();
    bool header_read=false;
    quint16 row=0, rows=0, column=0, columns=0;
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    bool hasTitle=false;
    bool hasData=false;
    QMap<int,QString> specialHeader;
    while (!line.isNull()) {
        bool dataread=false;
        line=line;
        //std::cout<<"read: <"<<line.toStdString()<<">\n";
        if (line.size()>0) {
            if (line.startsWith(header_start)) {
                header_read=true;
                line=line.right(line.size()-header_start.size());
                QStringList sl=line.split(QString(column_separator));
                columns=(columns>sl.size())?columns:sl.size();
                /*if (clearTable) {
                    resize(rows+start_row, columns+start_col);
                    //columnNames.clear();
                } else {
                    resize(qMax(rowCount(), rows+start_row), qMax(columnCount(), columns+start_col));
                }*/
                //while (columnNames.size()<sl.size()+start_col) {
                //    columnNames.append("");
                //}
                for (int i=0; i<sl.size(); i++) {
                    int c=start_col+i;
                    QString n=sl[i].trimmed();
                    if (n[0]=='\"' || n[0]=='\'') {
                        n=n.mid(1, n.size()-2);
                    }
                    //if (c<columnNames.size()) columnNames[c]=n;
                    setColumnTitleCreate(c, n);
                }
                hasTitle=true;
            } else if (line.startsWith(comment_start) && !hasTitle && !hasData) {
                header_read=true;
                line=line.right(line.size()-1);
                QStringList sl=line.split(QString(column_separator));
                columns=(columns>sl.size())?columns:sl.size();
                /*if (clearTable) {
                    resize(rows+start_row, columns+start_col);
                    //columnNames.clear();
                } else {
                    resize(qMax(rowCount(), rows+start_row), qMax(columnCount(), columns+start_col));
                }*/
                //while (columnNames.size()<sl.size()+start_col) {
                //    columnNames.append("");
                //}
                for (int i=0; i<sl.size(); i++) {
                    int c=start_col+i;
                    QString n=sl[i].trimmed();
                    if (n[0]=='\"' || n[0]=='\'') {
                        n=n.mid(1, n.size()-2);
                    }
                    //if (c<columnNames.size()) columnNames[c]=n;
                    specialHeader[c]=n;
                }
            } else if (!line.startsWith(comment_start)) {
                int i=0;
                while (i<line.size()) {
                    QChar c=line[i];
                    if (c=='\'') {
                        i++;
                        QString s="";
                        while (i<line.size() && line[i]!='\'') {
                            if (line[i]!='\'') s=s+line[i];
                            i++;
                        }
                        //resize(rows, columns);
                        setCellCreate(row+start_row, column+start_col, s);
                        hasData=true;
                        //qDebug()<<"  <"<<row+start_row<<", "<<column+start_col<<">="<<s<<"\n";
                        dataread=true;
                    } else if (c=='\"') {
                        i++;
                        QString s="";
                        while (i<line.size() && line[i]!='\"') {
                            if (line[i]!='\"') s=s+line[i];
                            i++;
                        }
                        //resize(rows, columns);
                        hasData=true; setCellCreate(row+start_row, column+start_col, s);
                        //qDebug()<<"  <"<<row+start_row<<", "<<column+start_col<<">="<<s<<"\n";
                        dataread=true;
                    } else if (c==comment_start) {
                        i=line.size();
                    } else if (c==column_separator) {
                        column++;
                    } else if ((c=='-') || (c=='+') || (c==decimal_separator) || (c>='0' && c<='9')) {
                        QString s="";
                        while (i<line.size() && line[i]!=column_separator) {
                            QChar cc=line[i];
                            if ((cc=='-') || (cc=='+') ||
                                (cc=='0') || (cc=='1') || (cc=='2') || (cc=='3') || (cc=='4') || (cc=='5') || (cc=='6') || (cc=='7') || (cc=='8') || (cc=='9') ||
                                (cc=='E') || (cc=='e')) s=s+cc;
                            if (cc==decimal_separator) s=s+'.';
                            i++;
                        }
                        if (i<line.size()) i--;
                        bool ok=false;
                        double d=loc.toDouble(s, &ok);
                        if (ok) {
                            //resize(rows, columns);
                            if (d==round(d)) {
                                hasData=true; setCellCreate(row+start_row, column+start_col, ((qlonglong)d));
                                //std::cout<<"  <"<<row<<"/"<<rows<<", "<<column<<"/"<<columns<<">="<<(qlonglong)d<<"\n";
                                //qDebug()<<"  <"<<row+start_row<<", "<<column+start_col<<">="<<(qlonglong)d<<"\n";
                                dataread=true;
                            } else {
                                hasData=true; setCellCreate(row+start_row, column+start_col, d);
                                //std::cout<<"  <"<<row<<"/"<<rows<<", "<<column<<"/"<<columns<<">="<<d<<"\n";
                                //qDebug()<<"  <"<<row+start_row<<", "<<column+start_col<<">="<<d<<"\n";
                                dataread=true;
                            }
                        } else {
                            QDateTime dt=loc.toDateTime(s);
                            //resize(rows, columns);
                            if (dt.isValid()) {
                                hasData=true; setCellCreate(row+start_row, column+start_col, dt);
                                //qDebug()<<"  <"<<row+start_row<<", "<<column+start_col<<">="<<dt<<"\n";
                                dataread=true;
                            } else {
                                hasData=true; setCellCreate(row+start_row, column+start_col, s);
                                //qDebug()<<"  <"<<row+start_row<<", "<<column+start_col<<">="<<s<<"\n";
                                dataread=true;
                            }
                        }
                    } else if (c!=' ') {
                        i++;
                        QString s=c;
                        while (i<line.size() && line[i]!=column_separator) {
                            if (line[i]!=column_separator) s=s+line[i];
                            i++;
                        }
                        if (i<line.size()) i--;
                        s=s.trimmed();
                        //resize(rows, columns);
                        hasData=true;
                        if (s.toLower()=="true" || s.toLower()=="yes" || s.toLower()=="ja") setCellCreate(row+start_row, column+start_col, true);
                        else if (s.toLower()=="false" || s.toLower()=="no" || s.toLower()=="nein") setCellCreate(row+start_row, column+start_col, false);
                        else {
                            bool okd=false;
                            double cd=s.toDouble(&okd);
                            bool okl=false;
                            qlonglong cl=s.toLongLong(&okl);
                            if (!okd && !okl) {
                                setCellCreate(row+start_row, column+start_col, s);
                            } else {
                                setCellCreate(row+start_row, column+start_col, cd);
                            }
                        }
                        //qDebug()<<"  <"<<row+start_row<<", "<<column+start_col<<">="<<s<<"\n";
                        //std::cout<<"  <"<<row<<", "<<column<<">="<<s.toStdString()<<"\n";
                        dataread=true;
                    }
                    i++;
                }
            }
        }
        line = in.readLine();
        if (dataread) row++;
        if (row+1>rows) rows=row+1;
        if (column+1>columns) columns=column+1;
        column=0;
    }

    if (!hasTitle && specialHeader.size()>0 && abs(specialHeader.size()-columns)<columns/10) {
        QMapIterator<int,QString> it(specialHeader);
        while (it.hasNext()) {
            it.next();
            setColumnTitleCreate(it.key(), it.value());
        }
    }

    //std::cout<<"rows="<<rows<<"   columns="<<columns<<std::endl;
    //this->rows=rows;
    //this->columns=columns;
    readonly=ro;
    doEmitSignals=oldEmit;
    if (doEmitSignals) reset();
    return true;
}

QString QFTableModel::saveXML(QModelIndexList selection, bool createXMLFragment)
{
    QString xml;
    {
        QXmlStreamWriter w(&xml);
        //w.setAutoFormattingIndent(2);
        //w.setAutoFormatting(true);
        if (!createXMLFragment)  w.writeStartDocument();
        w.writeStartElement("qfrdrtable");
        w.writeStartElement("columns");
        int smallestRow=rowCount();
        int smallestColumn=columnCount();
        for (int i=0; i<selection.size(); i++) {
            if (selection[i].column()<smallestColumn) smallestColumn=selection[i].column();
            if (selection[i].row()<smallestRow) smallestRow=selection[i].row();
        }
        for (int c=0; c<columnCount(); c++) {
            bool used=selection.isEmpty();
            if (!used) {
                for (int i=0; i<selection.size(); i++) {
                    if (selection[i].column()==c) {
                        used=true;
                        break;
                    }
                }
            }
            if (used) {
                w.writeStartElement("col");
                w.writeAttribute("col", QString::number(c-smallestColumn));
                w.writeAttribute("name", columnTitle(c));
                if (headerDataMap.contains(c)) {
                    QHashIterator<int, QVariant> it(headerDataMap[c]);
                    while (it.hasNext()) {
                        it.next();
                        w.writeAttribute(QString("more_type%1").arg(it.key()), getQVariantType(it.value()));
                        w.writeAttribute(QString("more_data%1").arg(it.key()), getQVariantData(it.value()));

                    }
                }
                w.writeEndElement();
            }
        }
        w.writeEndElement();
        w.writeStartElement("data");
        for (int r=0; r<rowCount(); r++) {
            for (int c=0; c<columnCount(); c++) {
                if (selection.isEmpty() || selection.contains(index(r, c))) {
                    w.writeStartElement("cell");
                    w.writeAttribute("row", QString::number(r-smallestRow));
                    w.writeAttribute("col", QString::number(c-smallestColumn));
                    QVariant check=data(index(r, c), Qt::CheckStateRole);
                    if (check.isValid()) w.writeAttribute("check", QString::number(check.toInt()));
                    QVariant d=cell(r,c);
                    quint32 a=xyAdressToUInt32(r,c);
                    w.writeAttribute("type", getQVariantType(d));
                    if (dataCheckedMap.contains(a)) w.writeAttribute("checked", QString::number(dataCheckedMap[a].toInt()));
                    if (moreDataMap.contains(a)) {
                        QHashIterator<int, QVariant> it(moreDataMap[a]);
                        while (it.hasNext()) {
                            it.next();
                            w.writeAttribute(QString("more_type%1").arg(it.key()), getQVariantType(it.value()));
                            w.writeAttribute(QString("more_data%1").arg(it.key()), getQVariantData(it.value()));

                        }
                    }
                    w.writeCharacters(getQVariantData(d));
                    w.writeEndElement();
                }
            }
        }
        w.writeEndElement();
        w.writeEndElement();
        if (!createXMLFragment)  w.writeEndDocument();
    }
    return xml;
}

bool QFTableModel::saveXML(const QString &filename, QModelIndexList selection)
{
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream out(&f);
        out<<saveXML(selection, false);
        f.close();
        return true;
    }
    return false;
}

void QFTableModel::copy(QModelIndexList selection, bool createXMLFragment) {
    QString xml=saveXML(selection, createXMLFragment);
    QMimeData* mime=new QMimeData();
    mime->setData("quickfit3/qfrdrtable", xml.toUtf8());
    QString csv;
    QTextStream out(&csv);
    QLocale loc;
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    saveCSV(out, "\t", loc.decimalPoint().toLatin1());
    mime->setText(csv);
    QApplication::clipboard()->setMimeData(mime);
}

void QFTableModel::paste(int row_start, int column_start) {
    QClipboard* clip=QApplication::clipboard();
    const QMimeData* mime=clip->mimeData();

    int row=row_start;
    int column=column_start;
    if (row_start<0) row=0;
    if (column_start<0) column=0;

    bool oldEmit=doEmitSignals;
    doEmitSignals=false;


    /*for (int i=0; i<mime->formats().size(); i++) {
        //qDebug()<<mime->formats().at(i)<<":";
        //qDebug()<<mime->data(mime->formats().at(i));
        //qDebug()<<"----------------------------------------------------------------------------------";
    }*/
    if (mime && mime->hasFormat("quickfit3/qfrdrtable")) {
        QString data=QString::fromUtf8(mime->data("quickfit3/qfrdrtable").data());
        //qDebug()<<"pasting quickfit3/qfrdrtable";
        readXML(data, row, column, false);
    } else if (mime && mime->hasFormat("jkqtplotter/csv")) {
        QString data=QString::fromUtf8(mime->data("jkqtplotter/csv").data());
        //qDebug()<<"pasting jkqtplotter/csv: \n"<<data;
        readXML(data, row, column, false);
        QTextStream in(&data);
        readCSV(in, ',', '.', "#!", '#', row, column, false);
    } else if (mime && mime->hasFormat("quickfit/csv")) {
        QString data=QString::fromUtf8(mime->data("quickfit/csv").data());
        //qDebug()<<"pasting quickfit/csv: \n"<<data;
        readXML(data, row, column, false);
        QTextStream in(&data);
        readCSV(in, ',', '.', "#!", '#', row, column, false);
    } else if (mime && mime->hasText()) {
        QString data=mime->text();
        //qDebug()<<"pasting text: \n"<<data;
        QLocale loc;
        loc.setNumberOptions(QLocale::OmitGroupSeparator);
        QTextStream in(&data);
        QStringList sl=data.split("\n");
        char sep='\t';
        char dec=loc.decimalPoint().toLatin1();
        if (sl.size()>0) {
            QString d=sl[0];
            d=d.remove(dec);
            int cnt=d.count('\n');
            int cntSem=d.count(';');
            int cntCom=d.count(',');
            if (cntSem>cnt && ';'!=dec) sep=';';
            if (cntCom>cnt && ','!=dec) sep=',';
        }
        //qDebug()<<"sep='"<<sep<<"'    dec='"<<dec<<"'";
        readCSV(in, sep, dec, "#!", '#', row, column, false);
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) reset();
}

bool QFTableModel::getDoEmitSignals() const
{
    return doEmitSignals;
}

void QFTableModel::enableSignals(bool emitReset)
{
    doEmitSignals=true;
    if (emitReset) reset();
}

void QFTableModel::disableSignals()
{
    doEmitSignals=false;
}

bool QFTableModel::readXML(const QString &data, int start_row, int start_col, bool clearTable) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    int srow=start_row;
    int scol=start_col;
    if (srow<0) srow=0;
    if (scol<0) scol=0;
    if (clearTable) {
        clear();
    }
    QDomDocument doc;
    if (doc.setContent(data)) {
        QDomElement er=doc.firstChildElement("qfrdrtable");
        if (!er.isNull()) {
            if (clearTable) {
                QDomElement e=er.firstChildElement("columns");
                if (!e.isNull()) {
                    e=e.firstChildElement("col");
                    while (!e.isNull()) {
                        int c=scol+e.attribute("col").toInt();
                        QString t=e.attribute("name");
                        setColumnTitleCreate(c, t);

                        QDomNamedNodeMap nm=e.attributes();
                        QRegExp rxAtrD("more\\_data(\\d+)");
                        for (int na=0; na<nm.size(); na++) {
                            //qDebug()<<na;
                            QDomAttr atr=nm.item(na).toAttr();

                            if (!atr.isNull()) {
                                //qDebug()<<atr.name()<<" = "<<atr.value();
                                if (rxAtrD.indexIn(atr.name())>=0) {
                                    //qDebug()<< "   num = "<<rxAtrD.cap(1);
                                    QString typ=e.attribute(QString("more_type%1").arg(rxAtrD.cap(1)));
                                    //qDebug()<< "   type = "<<typ;
                                    QVariant md=getQVariantFromString(typ, atr.value());
                                    setColumnHeaderData(c, rxAtrD.cap(1).toInt(), md);
                                }
                            }
                        }


                        e=e.nextSiblingElement("col");
                    }
                }
            }
            QDomElement e=er.firstChildElement("data");
            if (!e.isNull()) {
                e=e.firstChildElement("cell");
                while (!e.isNull()) {
                    int r=srow+e.attribute("row").toInt();
                    int c=scol+e.attribute("col").toInt();
                    QString t=e.attribute("type");
                    QString ds=e.text();
                    QVariant d=getQVariantFromString(t, ds);
                    setCellCreate(r, c, d);

                    if (e.hasAttribute("checked")) {
                        setCellCheckedRoleCreate(r,c,e.attribute("checked").toInt());
                    }
                    QDomNamedNodeMap nm=e.attributes();
                    QRegExp rxAtrD("more\\_data(\\d+)");
                    for (int na=0; na<nm.size(); na++) {
                        //qDebug()<<na;
                        QDomAttr atr=nm.item(na).toAttr();

                        if (!atr.isNull()) {
                            //qDebug()<<atr.name()<<" = "<<atr.value();
                            if (rxAtrD.indexIn(atr.name())>=0) {
                                //qDebug()<< "   num = "<<rxAtrD.cap(1);
                                QString typ=e.attribute(QString("more_type%1").arg(rxAtrD.cap(1)));
                                //qDebug()<< "   type = "<<typ;
                                QVariant md=getQVariantFromString(typ, atr.value());
                                setCellUserRoleCreate(rxAtrD.cap(1).toInt(), r, c, md);
                            }
                        }
                    }

                    e=e.nextSiblingElement("cell");
                }
            }
            doEmitSignals=oldEmit;
            if (doEmitSignals) reset();
            return true;
        }
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) reset();
    return false;
}

bool QFTableModel::readXML(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    return readXML(file.readAll(), 0, 0, true);
}
