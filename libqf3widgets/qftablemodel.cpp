#include "qftablemodel.h"
#include <QDebug>
#include <QMessageBox>

QFTableModel::QFTableModel(QObject * parent):
    QAbstractTableModel(parent)
{
    readonly=true;
    readonlyButStillCheckable=false;
    doEmitSignals=true;
    defaultEditValue=QVariant();
    verticalHeaderShowRowNumbers=false;
    state.rows=1;
    state.columns=1;
    undoCurrentPosition=-1;
    undoEnabled=true;
    undoMaxSteps=50;
    undoIsMultiStep=0;
    rdlID=0;
    actUndo=new QAction(QIcon(":/lib/undo.png"), tr("Undo"), this);
    actUndo->setShortcut(QKeySequence::Undo);
    actRedo=new QAction(QIcon(":/lib/redo.png"), tr("Redo"), this);
    actRedo->setShortcut(QKeySequence::Redo);
    setUndoEnabled(false);
    //quint32 a=xyAdressToUInt32(5, 5);
    //std::cout<<"adress test: "<<a<<" => row="<<UInt32ToRow(a)<<", column="<<UInt32ToColumn(a)<<"\n";
}

QFTableModel::~QFTableModel()
{
    //dtor
}

int QFTableModel::rowCount(const QModelIndex &parent) const {
    //std::cout<<"rowCount: "<<state.rows<<"\n";
    return state.rows;
};
int QFTableModel::columnCount(const QModelIndex &parent) const {
    //std::cout<<"columnCount: "<<state.columns<<"\n";
    return state.columns;
};


QVariant QFTableModel::data(const QModelIndex &index, int role) const {
    if (index.isValid()) {
        quint32 a=xyAdressToUInt32(index.row(), index.column());
        //std::cout<<"QFTableModel::data("<<a<<")"<<std::endl;
        if (role == Qt::DisplayRole) {
            if (state.dataMap.contains(a)) return state.dataMap[a];
        } else if (role == Qt::EditRole) {
            QVariant v=QVariant();
            if (state.dataEditMap.contains(a)) v=state.dataEditMap.value(a, v);
            if (state.dataMap.contains(a)) v=state.dataMap.value(a, v);
            if (!v.isValid()) v=defaultEditValue;
            return v;
        } else if (role == Qt::CheckStateRole) {
            if (state.dataCheckedMap.contains(a)) return state.dataCheckedMap[a];
        } else if (role == Qt::BackgroundRole) {
            if (state.dataBackgroundMap.contains(a)) return state.dataBackgroundMap[a];
        } else if (role == Qt::ToolTipRole) {
            if (state.dataMap.contains(a)) return tr("row: <i>%2</i><br>column: <i>%3 (\"%4\")</i><br>datatype: <i>%1</i><br>data: <b>%5</b>").arg(getQVariantType(state.dataMap[a])).arg(index.row()).arg(index.column()).arg(state.columnNames.value(index.column(), "")).arg(getQVariantData(index.data()));
        } else if (role >= Qt::UserRole) {
            if (state.moreDataMap.contains(a)) {
                return state.moreDataMap[a].value(role, QVariant());
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
     if (orientation == Qt::Horizontal) {
         if (role==Qt::DisplayRole) {
            if (section<state.columnNames.size()) return state.columnNames[section];
         } else if (state.headerDataMap.contains(section)) {
             return state.headerDataMap[section].value(role, QVariant());
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
        addUndoStep();
        quint32 a=xyAdressToUInt32(index.row(), index.column());
        QVariant old=QVariant();
        if (state.dataMap.contains(a)) old=state.dataMap[a];
        state.dataMap[a]=value;
        //state.dataEditMap.remove(a);
        hasDataChanged=hasDataChanged | (old!=value);
        doEmitSignals=oldEmit;
        if (doEmitSignals) emit dataChanged(index, index);
        return true;
    } else if (index.isValid() && role == Qt::CheckStateRole && (!readonly || readonlyButStillCheckable)) {
        addUndoStep();
        quint32 a=xyAdressToUInt32(index.row(), index.column());
        QVariant old=QVariant();
        if (state.dataCheckedMap.contains(a)) old=state.dataCheckedMap[a];
        state.dataCheckedMap[a]=value;
        hasDataChanged=hasDataChanged | (old!=value);
        doEmitSignals=oldEmit;
        if (doEmitSignals) emit dataChanged(index, index);
        return true;
    } else if (index.isValid() && role >= Qt::UserRole && !readonly) {
        addUndoStep();
        quint32 a=xyAdressToUInt32(index.row(), index.column());
        QVariant old=QVariant();
        if (state.moreDataMap.contains(a)) old=state.moreDataMap[a].value(role, QVariant());
        state.moreDataMap[a].insert(role, value);
        hasDataChanged=hasDataChanged | (old!=value);
        doEmitSignals=oldEmit;
        if (doEmitSignals) emit dataChanged(index, index);
        return true;
    }
    doEmitSignals=oldEmit;
    return false;
}

QVariantList QFTableModel::getColumnData(int column, int role) const
{
    QVariantList vl;
    if (column>=0 && column<state.columns) {
        for (int r=0; r<state.rows; r++) {
            vl<<cell(r, column);
        }
    }
    return vl;
}

QVector<double> QFTableModel::getColumnDataAsNumbers(int column, int role) const
{
    QVector<double> vl;
    if (column>=0 && column<state.columns) {
        for (int r=state.rows-1; r>=0; r--) {
            QVariant d=cell(r, column);
            //qDebug()<<column<<","<<r<<": "<<d;
            bool ok=false;
            double dd=d.toDouble(&ok);
            if (d.isValid() && (d.type()==QVariant::Double || d.type()==QVariant::Int || d.type()==QVariant::UInt || d.type()==QVariant::Bool || d.type()==QVariant::LongLong || d.type()==QVariant::ULongLong) && ok) {
                vl.push_front(dd);
            } else if (vl.size()>0) {
                vl.push_front(NAN);
            }
        }
    }
    return vl;
}

void QFTableModel::swapCells(quint16 row1, quint16 column1, quint16 row2, quint16 column2) {
    if (readonly) return;

    addUndoStep();

    quint32 adr1=xyAdressToUInt32(row1, column1);
    quint32 adr2=xyAdressToUInt32(row2, column2);
    QVariant d1, d2;

    d1=state.dataMap.value(adr1, QVariant());
    d2=state.dataMap.value(adr2, QVariant());
    state.dataMap.remove(adr1);
    state.dataMap.remove(adr2);
    if (d2.isValid()) state.dataMap[adr1]=d2;
    if (d1.isValid()) state.dataMap[adr2]=d1;

    d1=state.dataEditMap.value(adr1, QVariant());
    d2=state.dataEditMap.value(adr2, QVariant());
    state.dataEditMap.remove(adr1);
    state.dataEditMap.remove(adr2);
    if (d2.isValid()) state.dataEditMap[adr1]=d2;
    if (d1.isValid()) state.dataEditMap[adr2]=d1;

    d1=state.dataBackgroundMap.value(adr1, QVariant());
    d2=state.dataBackgroundMap.value(adr2, QVariant());
    state.dataBackgroundMap.remove(adr1);
    state.dataBackgroundMap.remove(adr2);
    if (d2.isValid()) state.dataBackgroundMap[adr1]=d2;
    if (d1.isValid()) state.dataBackgroundMap[adr2]=d1;

    d1=state.dataCheckedMap.value(adr1, QVariant());
    d2=state.dataCheckedMap.value(adr2, QVariant());
    state.dataCheckedMap.remove(adr1);
    state.dataCheckedMap.remove(adr2);
    if (d2.isValid()) state.dataCheckedMap[adr1]=d2;
    if (d1.isValid()) state.dataCheckedMap[adr2]=d1;

    QHash<int, QVariant> hd1, hd2;
    hd1=state.moreDataMap.value(adr1, QHash<int, QVariant>());
    hd2=state.moreDataMap.value(adr2, QHash<int, QVariant>());
    state.moreDataMap.remove(adr1);
    state.moreDataMap.remove(adr2);
    if (hd2.size()>0) state.moreDataMap[adr1]=hd2;
    if (hd1.size()>0) state.moreDataMap[adr2]=hd1;

    if (doEmitSignals) {
        emit dataChanged(index(row1, column1), index(row1, column1));
        emit dataChanged(index(row2, column2), index(row2, column2));
    }
}

void QFTableModel::resize(quint16 rows, quint16 columns) {
    if (readonly) return;

    startMultiUndo();

    //std::cout<<"  resize("<<state.rows<<", "<<state.columns<<"): 1\n";
    //quint16 oldrows=this->state.rows;
    int oldcolumns=this->state.columns;
    int oldrows=this->state.rows;

    int j=0;
    while (state.columnNames.size()<oldcolumns) {state.columnNames.append(QString::number(j)); j++;}

    if (columns>oldcolumns) {
        for (int i=oldcolumns; i<=columns; i++) {
            int j=state.columnNames.size();
            while (i>=state.columnNames.size()) {
                state.columnNames.append(QString::number(j));
                j++;
            }
            emit columnAdded(i);
        }
    } else if (columns<oldcolumns) {
        for (int i=oldcolumns-1; i>=columns; i--) {
            if (!state.columnNames.isEmpty()) state.columnNames.removeLast();
            emit columnRemoved(i);
        }
    }


    //std::cout<<"  resize("<<state.rows<<", "<<state.columns<<"): 2\n";
    this->state.rows=rows;
    this->state.columns=columns;

    QList<quint32> idx=state.dataMap.keys();
    QList<quint32> didx;
    //std::cout<<"  resize("<<state.rows<<", "<<state.columns<<"): 3\n";
    if (idx.size()>0) for (int i = idx.size()-1; i>=0; i--) {
        quint16 r=UInt32ToRow(idx[i]);
        quint16 c=UInt32ToColumn(idx[i]);
        if ((r>=rows) || (c>=columns)) {
            didx.append(idx[i]);
        }
    }
    //std::cout<<"  resize("<<state.rows<<", "<<state.columns<<"): 4\n";
    if (didx.size()>0) for (int i = didx.size()-1; i>=0; i--) {
        state.dataMap.remove(didx[i]);
        if (state.dataEditMap.contains(didx[i])) state.dataEditMap.remove(didx[i]);
        if (state.dataBackgroundMap.contains(didx[i])) state.dataBackgroundMap.remove(didx[i]);
        if (state.dataCheckedMap.contains(didx[i])) state.dataCheckedMap.remove(didx[i]);
        if (state.moreDataMap.contains(didx[i])) state.moreDataMap.remove(didx[i]);
    }

    /*for (int c=0; c<state.columns; c++) {
        for (int r=0; r<state.rows; r++) {
            if (c>=oldcolumns || r>=oldrows) state.dataEditMap[xyAdressToUInt32(r,c)]=defaultEditValue;
        }
    }*/
    //std::cout<<"  resize("<<state.rows<<", "<<state.columns<<"): 5\n";
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
    endMultiUndo();
    //std::cout<<"  resize("<<state.rows<<", "<<state.columns<<"): 6\n";
}

void QFTableModel::insertRow(quint16 r) {
    startMultiUndo();

    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    appendRow();
    if (r<state.rows-1) for (int i=state.rows-2; i>=r; i--) {
        for (int c=0; c<state.columns; c++) {
            copyCell(i+1, c, i, c);
        }
    }
    for (int c=0; c<state.columns; c++) {
        setCell(r, c, defaultEditValue);
        quint32 a=xyAdressToUInt32(r, c);
        state.dataEditMap.remove(a);
        state.dataBackgroundMap.remove(a);
        state.dataCheckedMap.remove(a);
        state.moreDataMap.remove(a);
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) {
        beginInsertRows(QModelIndex(),r,r);
        endInsertRows();
        //reset();
    }
    endMultiUndo();
}

void QFTableModel::insertColumn(quint16 c) {
    //std::cout<<"  insertColumn("<<c<<")\n";
    startMultiUndo();
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    appendColumn();
    while (state.columnNames.size()<state.columns) state.columnNames.append("");
    if (c<state.columns-1) for (int i=state.columns-2; i>=c; i--) {
        state.columnNames[i+1]=state.columnNames[i];
        if (state.headerDataMap.contains(i)) state.headerDataMap[i+1]=state.headerDataMap[i];
        for (int r=0; r<state.rows; r++) {
            copyCell(r, i+1, r, i);
        }
    }
    for (int r=0; r<state.rows; r++) {
        setCell(r, c, defaultEditValue);
        quint32 a=xyAdressToUInt32(r, c);
        state.dataEditMap.remove(a);
        state.dataBackgroundMap.remove(a);
        state.dataCheckedMap.remove(a);
        state.moreDataMap.remove(a);
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) {
        //reset();
        beginInsertColumns(QModelIndex(),c,c);
        endInsertColumns();
        emit columnAdded(c);
    }
    endMultiUndo();
    //std::cout<<"  insertColumn("<<c<<") ... DONE\n";
}

void QFTableModel::deleteRow(quint16 r) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    if (r>=state.rows ) return;
    startMultiUndo();
    for (int i=r; i<state.rows-1; i++) {
        for (int c=0; c<state.columns; c++) {
            copyCell(i, c, i+1, c);
        }
    }
    resize(state.rows-1, state.columns);
    doEmitSignals=oldEmit;
    if (doEmitSignals) {
        beginRemoveRows(QModelIndex(), r, r);
        endRemoveRows();
        //reset();
    }
    endMultiUndo();
}


void QFTableModel::deleteColumn(quint16 c) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    if (c>=state.columns) return;
    startMultiUndo();
    for (int i=c; i<state.columns-1; i++) {
        for (int r=0; r<state.rows; r++) {
            copyCell(r, i, r, i+1);
        }
        if (i+1<state.columnNames.size()) state.columnNames[i]=state.columnNames[i+1];
        if (state.headerDataMap.contains(i+1)) state.headerDataMap[i]=state.headerDataMap[i+1];
    }
    resize(state.rows, state.columns-1);
    doEmitSignals=oldEmit;
    if (doEmitSignals) {
        beginRemoveColumns(QModelIndex(), c, c);
        endRemoveColumns();
        emit columnRemoved(c);
//        reset();
    }
    endMultiUndo();
}

void QFTableModel::emptyColumn(quint16 c)
{
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    if (c>=state.columns) return;

    startMultiUndo();

    for (int r=0; r<state.rows; r++) {
        deleteCell(r, c);
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) {
        emit dataChanged(index(0, c), index(state.rows-1, c));
    }

    endMultiUndo();
}

void QFTableModel::deleteCell(quint16 row, quint16 column) {
    if (readonly) return;

    addUndoStep();

    quint32 a=xyAdressToUInt32(row, column);
    state.dataMap.remove(a);
    state.dataEditMap.remove(a);
    state.dataBackgroundMap.remove(a);
    state.dataCheckedMap.remove(a);
    state.moreDataMap.remove(a);
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::deleteCells(QModelIndexList selection) {

    startMultiUndo();
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    int r1=0, r2=state.rows-1;
    int c1=0, c2=state.columns-1;
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
    endMultiUndo();
}


void QFTableModel::clear() {
    if (readonly) return;
    addUndoStep();
    state.rows=0;
    state.columns=0;
    state.dataMap.clear();
    state.dataEditMap.clear();
    state.dataBackgroundMap.clear();
    state.dataCheckedMap.clear();
    state.moreDataMap.clear();
    state.columnNames.clear();
    state.headerDataMap.clear();
    if (doEmitSignals) reset();
}

bool QFTableModel::changeDatatype(quint16 row, quint16 column, QVariant::Type newType) {
    if (readonly || (row>=state.rows) || (column>=state.columns)) return false;
    addUndoStep();
    quint32 a=xyAdressToUInt32(row, column);
    if (state.dataMap.contains(a)) {
        if (state.dataEditMap[a].canConvert(newType)) {
            state.dataEditMap[a].convert(newType);
        } else {
            state.dataEditMap[a]=QVariant(newType);
        }

        if (state.dataMap[a].canConvert(newType)) {
            state.dataMap[a].convert(newType);
            //if (state.dataEditMap.contains(a)) state.dataEditMap[a].convert(newType);
            return true;
        } else {
            state.dataMap[a]=QVariant(newType);
            return false;
        }
    } else {
        state.dataMap[a]=QVariant(newType);
        state.dataEditMap[a]=QVariant(newType);
        return true;
    }
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setCell(quint16 row, quint16 column, QVariant value) {
    //std::cout<<"setCell("<<row<<", "<<column<<", '"<<value.toString().toStdString()<<"' ["<<value.typeName()<<"])\n";
    if (readonly || (row>=state.rows) || (column>=state.columns)) return;
    addUndoStep();
    quint32 a=xyAdressToUInt32(row, column);
    state.dataMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setColumn(quint16 column, const QList<QVariant>& value)
{
    //std::cout<<"setCell("<<row<<", "<<column<<", '"<<value.toString().toStdString()<<"' ["<<value.typeName()<<"])\n";
    if (readonly || (column>=state.columns)) return;
    addUndoStep();
    quint16 row=0;
    for (int i=0; i<value.size(); i++) {
        if (row<state.rows) {
            quint32 a=xyAdressToUInt32(row, column);
            state.dataMap[a]=value[i];
        }
        row++;
    }
    if (doEmitSignals) emit dataChanged(index(0, column), index(row-1, column));
}

void QFTableModel::setColumn(quint16 column, const QVector<double> &value)
{
    //std::cout<<"setCell("<<row<<", "<<column<<", '"<<value.toString().toStdString()<<"' ["<<value.typeName()<<"])\n";
    if (readonly || (column>=state.columns)) return;
    addUndoStep();
    quint16 row=0;
    for (int i=0; i<value.size(); i++) {
        if (row<state.rows) {
            quint32 a=xyAdressToUInt32(row, column);
            state.dataMap[a]=value[i];
        }
        row++;
    }
    if (doEmitSignals) emit dataChanged(index(0, column), index(row-1, column));
}

void QFTableModel::setColumn(quint16 column, const double *value, int N)
{
    //std::cout<<"setCell("<<row<<", "<<column<<", '"<<value.toString().toStdString()<<"' ["<<value.typeName()<<"])\n";
    if (readonly || (column>=state.columns)) return;
    addUndoStep();
    quint16 row=0;
    for (int i=0; i<N; i++) {
        if (row<state.rows) {
            quint32 a=xyAdressToUInt32(row, column);
            state.dataMap[a]=value[i];
        }
        row++;
    }
    if (doEmitSignals) emit dataChanged(index(0, column), index(row-1, column));
}


void QFTableModel::copyCell(quint16 row, quint16 column, quint16 row_old, quint16 column_old)
{
    if (readonly || (row>=state.rows) || (column>=state.columns) || (row_old>=state.rows) || (column_old>=state.columns)) return;

    addUndoStep();

    quint32 a=xyAdressToUInt32(row, column);
    quint32 a_old=xyAdressToUInt32(row_old, column_old);

    QVariant d;
    d=state.dataMap.value(a_old, QVariant());
    state.dataMap.remove(a);
    if (d.isValid()) state.dataMap[a]=d;

    d=state.dataEditMap.value(a_old, QVariant());
    state.dataEditMap.remove(a);
    if (d.isValid()) state.dataEditMap[a]=d;

    d=state.dataBackgroundMap.value(a_old, QVariant());
    state.dataBackgroundMap.remove(a);
    if (d.isValid()) state.dataBackgroundMap[a]=d;

    d=state.dataCheckedMap.value(a_old, QVariant());
    state.dataCheckedMap.remove(a);
    if (d.isValid()) state.dataCheckedMap[a]=d;

    QHash<int, QVariant> hd;
    hd=state.moreDataMap.value(a_old, QHash<int, QVariant>());
    state.moreDataMap.remove(a);
    if (hd.size()>0) state.moreDataMap[a]=hd;
}

void QFTableModel::setCellCreate(quint16 row, quint16 column, QVariant value) {
    if (readonly) return;
    startMultiUndo();
    if ((row>=state.rows) || (column>=state.columns)) {
        resize(qMax(state.rows, quint16(row+1)), qMax(state.columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    state.dataMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
    endMultiUndo();
}

void QFTableModel::setColumnCreate(quint16 column, const QList<QVariant>& value)
{
    if (readonly) return;
    startMultiUndo();
    if ((value.size()>state.rows) || (column>=state.columns)) {
        resize(qMax(state.rows, quint16(value.size())), qMax(state.columns, quint16(column+1)));
    }

    setColumn(column, value);
    endMultiUndo();

}

void QFTableModel::setColumnCreate(quint16 column, const QVector<double> &value)
{
    if (readonly) return;
    startMultiUndo();
    if ((value.size()>state.rows) || (column>=state.columns)) {
        resize(qMax(state.rows, quint16(value.size())), qMax(state.columns, quint16(column+1)));
    }

    setColumn(column, value);
    endMultiUndo();
}

void QFTableModel::setColumnCreate(quint16 column, const double *value, int N)
{
    if (readonly) return;
    startMultiUndo();
    if ((N>state.rows) || (column>=state.columns)) {
        resize(qMax(state.rows, quint16(N)), qMax(state.columns, quint16(column+1)));
    }

    setColumn(column, value, N);
    endMultiUndo();
}

void QFTableModel::setCellEditRole(quint16 row, quint16 column, QVariant value) {
    //std::cout<<"setCell("<<row<<", "<<column<<", '"<<value.toString().toStdString()<<"' ["<<value.typeName()<<"])\n";
    if (readonly || (row>=state.rows) || (column>=state.columns)) return;
    addUndoStep();
    quint32 a=xyAdressToUInt32(row, column);
    state.dataEditMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setCellEditRoleCreate(quint16 row, quint16 column, QVariant value) {
    if (readonly) return;
    startMultiUndo();
    if ((row>=state.rows) || (column>=state.columns)) {
        resize(qMax(state.rows, quint16(row+1)), qMax(state.columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    state.dataEditMap[a]=value;
    endMultiUndo();
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setCellBackgroundRole(quint16 row, quint16 column, QVariant value) {
    if (readonly || (row>=state.rows) || (column>=state.columns)) return;
    addUndoStep();
    quint32 a=xyAdressToUInt32(row, column);
    state.dataBackgroundMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setCellBackgroundRoleCreate(quint16 row, quint16 column, QVariant value) {
    if (readonly) return;
    startMultiUndo();
    if ((row>=state.rows) || (column>=state.columns)) {
        resize(qMax(state.rows, quint16(row+1)), qMax(state.columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    state.dataBackgroundMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
    endMultiUndo();
}

void QFTableModel::setCellCheckedRole(quint16 row, quint16 column, QVariant value) {
    if (readonly || (row>=state.rows) || (column>=state.columns)) return;
    addUndoStep();
    quint32 a=xyAdressToUInt32(row, column);
    state.dataCheckedMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::setCellCheckedRoleCreate(quint16 row, quint16 column, QVariant value) {
    if (readonly) return;
    startMultiUndo();
    if ((row>=state.rows) || (column>=state.columns)) {
        resize(qMax(state.rows, quint16(row+1)), qMax(state.columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    state.dataCheckedMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
    endMultiUndo();
}


quint16 QFTableModel::getAddRow(quint16 column, QVariant data) {
    if (readonly || (column>=state.columns)) return 0;
    if (state.rows<=0) {
        startMultiUndo();
        appendRow();
        setCell(0, column, data);
        endMultiUndo();
        return 0;
    }
    //quint16 row=0;
    for (quint16 r=0; r<state.rows; r++) {
        if (cell(r, column)==data) return r;
    }
    startMultiUndo();
    appendRow();
    setCell(state.rows-1, column, data);
    endMultiUndo();
    return state.rows-1;
}

void QFTableModel::setColumnHeaderData(quint16 column, int role, const QVariant &data)
{
    addUndoStep();
    state.headerDataMap[column].insert(role, data);
}

QVariant QFTableModel::getColumnHeaderData(quint16 column, int role) const
{
    if (state.headerDataMap.contains(column)) return state.headerDataMap[column].value(role, QVariant());
    return QVariant();
}

bool QFTableModel::hasColumnHeaderData(quint16 column, int role) const
{
    if (state.headerDataMap.contains(column)) return state.headerDataMap[column].contains(role);
    return false;
}

QList<quint32> QFTableModel::getColumnHeaderDataRoles() const
{
    return state.headerDataMap.keys();
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
    if ((row>=state.rows) || (column>=state.columns)) return QVariant();
    quint32 a=xyAdressToUInt32(row, column);
    if (state.dataMap.contains(a)) return state.dataMap[a];
    return QVariant();
}

void QFTableModel::setCellUserRole(int role, quint16 row, quint16 column, QVariant value) {
    if (readonly) return;
    startMultiUndo();
    if ((row>=state.rows) || (column>=state.columns)) {
        resize(qMax(state.rows, quint16(row+1)), qMax(state.columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    state.moreDataMap[a].insert(role, value);
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
    endMultiUndo();
}

void QFTableModel::setCellUserRoleCreate(int role, quint16 row, quint16 column, QVariant value)
{
    if (readonly) return;
    startMultiUndo();
    if ((row>=state.rows) || (column>=state.columns)) {
        resize(qMax(state.rows, quint16(row+1)), qMax(state.columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    state.moreDataMap[a].insert(role, value);
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
    endMultiUndo();
}

QVariant QFTableModel::cellEditRole(quint16 row, quint16 column) const {
    if ((row>=state.rows) || (column>=state.columns)) return QVariant();
    quint32 a=xyAdressToUInt32(row, column);
    if (state.dataEditMap.contains(a)) return state.dataEditMap[a];
    return QVariant();
}

QVariant QFTableModel::cellUserRole(int role, quint16 row, quint16 column) const
{
    if ((row>=state.rows) || (column>=state.columns)) return QVariant();
    quint32 a=xyAdressToUInt32(row, column);
    if (state.moreDataMap.contains(a)) return state.moreDataMap[a].value(role, QVariant());
    return QVariant();
}

void QFTableModel::setColumnTitle(quint16 column, QString name) {
    //std::cout<<"setColumnTitle("<<column<<", '"<<name.toStdString()<<")\n";
    if (readonly || (column>=state.columns)) return;
    addUndoStep();
    for (int c=state.columnNames.size(); c<state.columns; c++) state.columnNames.append(QString::number(c));
    if (column<state.columnNames.size()) state.columnNames[column]=name;
    if (doEmitSignals) {
        emit headerDataChanged(Qt::Horizontal, column, column);
    }
    emit columnTitleChanged(column);
}

void QFTableModel::setColumnTitleCreate(quint16 column, QString name) {
    //std::cout<<"setColumnTitle("<<column<<", '"<<name.toStdString()<<")\n";
    if (readonly) return;
    startMultiUndo();
    resize(state.rows, qMax(state.columns, quint16(column+1)));
    //qDebug()<<"setColumnTitleCreate("<<column<<", "<<name<<"):     state.columnNames.size()="<<state.columnNames.size();
    if (column<state.columnNames.size()) state.columnNames[column]=name;
    if (doEmitSignals) emit headerDataChanged(Qt::Horizontal, column, column);
    emit columnTitleChanged(column);
    endMultiUndo();
}

QString QFTableModel::columnTitle(quint16 column) const {
    if (column>=state.columns) return QString("");
    if (column<state.columnNames.size()) return state.columnNames[column];
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
    for (quint16 c=0; c<state.columns; c++) {
        if (c<state.columnNames.size()) out<<QString("C;Y1;X%2;K\"%1\"\n").arg(state.columnNames[c]).arg(c+1);
        out<<QString("F;Y1;X%2;SDB\n").arg(c+1);
    }

    // write data
    for (quint16 r=0; r<state.rows; r++) {
        for (quint16 c=0; c<state.columns; c++) {
            quint32 a=xyAdressToUInt32(r, c);
            if (state.dataMap.contains(a)) {
                QVariant& v=state.dataMap[a];
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

    QSet<int> usedCols, usedrows;
    bool saveCompleteTable=selection.isEmpty();
    for (int i=0; i<selection.size(); i++) {
        usedCols.insert(selection[i].column());
        usedrows.insert(selection[i].row());
    }

    // write column headers
    out<<header_start<<" ";
    int cc=0;
    for (quint16 c=0; c<state.columns; c++) {
        if (usedCols.contains(c) || saveCompleteTable) {
            if (cc>0) out<<column_separator;
            if (c<state.columnNames.size()) out<<QString("\"%1\"").arg(state.columnNames[c]).arg(c+1);
            cc++;
        }
    }
    out<<"\n";


    // write data
    for (quint16 r=0; r<state.rows; r++) {
        if (usedrows.contains(r)||saveCompleteTable) {
            bool first=true;
            for (quint16 c=0; c<state.columns; c++) {
                if (usedCols.contains(c) || saveCompleteTable) {
                    if (!first) out<<column_separator;
                    first=false;
                    quint32 a=xyAdressToUInt32(r, c);
                    if (state.dataMap.contains(a)) {
                        QVariant& v=state.dataMap[a];
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
                        out<<num;
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
    startMultiUndo();
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
                state.columns=(state.columns>sl.size())?state.columns:sl.size();
                for (int i=0; i<sl.size(); i++) {
                    int c=start_col+i;
                    QString n=sl[i].trimmed();
                    if (n[0]=='\"' || n[0]=='\'') {
                        n=n.mid(1, n.size()-2);
                    }
                    //if (c<state.columnNames.size()) state.columnNames[c]=n;
                    setColumnTitleCreate(c, n);
                }
                hasTitle=true;
            } else if (line.startsWith(comment_start) && !hasTitle && !hasData) {
                header_read=true;
                line=line.right(line.size()-1);
                QStringList sl=line.split(QString(column_separator));
                state.columns=(state.columns>sl.size())?state.columns:sl.size();

                for (int i=0; i<sl.size(); i++) {
                    int c=start_col+i;
                    QString n=sl[i].trimmed();
                    if (n[0]=='\"' || n[0]=='\'') {
                        n=n.mid(1, n.size()-2);
                    }
                    //if (c<state.columnNames.size()) state.columnNames[c]=n;
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
                        //resize(state.rows, state.columns);
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
                        //resize(state.rows, state.columns);
                        hasData=true; setCellCreate(row+start_row, column+start_col, s);
                        //qDebug()<<"  <"<<row+start_row<<", "<<column+start_col<<">="<<s<<"\n";
                        dataread=true;
                    } else if (c==comment_start) {
                        i=line.size();
                    } else if (c==column_separator || (column_separator==' ' && c.isSpace())) {
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
                            //resize(state.rows, state.columns);
                            if (d==round(d)) {
                                hasData=true; setCellCreate(row+start_row, column+start_col, ((qlonglong)d));
                                //std::cout<<"  <"<<row<<"/"<<state.rows<<", "<<column<<"/"<<state.columns<<">="<<(qlonglong)d<<"\n";
                                //qDebug()<<"  <"<<row+start_row<<", "<<column+start_col<<">="<<(qlonglong)d<<"\n";
                                dataread=true;
                            } else {
                                hasData=true; setCellCreate(row+start_row, column+start_col, d);
                                //std::cout<<"  <"<<row<<"/"<<state.rows<<", "<<column<<"/"<<state.columns<<">="<<d<<"\n";
                                //qDebug()<<"  <"<<row+start_row<<", "<<column+start_col<<">="<<d<<"\n";
                                dataread=true;
                            }
                        } else {
                            QDateTime dt=loc.toDateTime(s);
                            //resize(state.rows, state.columns);
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
                        //resize(state.rows, state.columns);
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
        if (row+1>state.rows) state.rows=row+1;
        if (column+1>state.columns) state.columns=column+1;
        column=0;
    }

    //qDebug()<<hasTitle<<specialHeader.size()<<state.columns<<specialHeader;

    if (!hasTitle && specialHeader.size()>0 && specialHeader.size()>=qMax(0,state.columns/10)) {
        QMapIterator<int,QString> it(specialHeader);
        while (it.hasNext()) {
            it.next();
            setColumnTitleCreate(it.key(), it.value());
            //qDebug()<<"title("<<it.key()<<") = "<<it.value();
        }
    }

    //std::cout<<"state.rows="<<state.rows<<"   state.columns="<<state.columns<<std::endl;
    //this->state.rows=state.rows;
    //this->state.columns=state.columns;
    readonly=ro;
    doEmitSignals=oldEmit;
    if (doEmitSignals) reset();
    endMultiUndo();
    return true;
}

QString QFTableModel::saveXML(QModelIndexList selection, bool createXMLFragment, bool template_only)
{
    QString xml;
    {
        QXmlStreamWriter w(&xml);
        //w.setAutoFormattingIndent(2);
        //w.setAutoFormatting(true);
        if (!createXMLFragment)  w.writeStartDocument();
        w.writeStartElement("qfrdrtable");
        w.writeStartElement("state.columns");
        int smallestRow=rowCount();
        int smallestColumn=columnCount();
        if (selection.size()>0) {
            for (int i=0; i<selection.size(); i++) {
                if (selection[i].column()<smallestColumn) smallestColumn=selection[i].column();
                if (selection[i].row()<smallestRow) smallestRow=selection[i].row();
            }
        } else {
            smallestRow=0;
            smallestColumn=0;
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
                if (state.headerDataMap.contains(c)) {
                    QHashIterator<int, QVariant> it(state.headerDataMap[c]);
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
        if (!template_only) {
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
                        if (state.dataCheckedMap.contains(a)) w.writeAttribute("checked", QString::number(state.dataCheckedMap[a].toInt()));
                        if (state.moreDataMap.contains(a)) {
                            QHashIterator<int, QVariant> it(state.moreDataMap[a]);
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
        }
        w.writeEndElement();
        if (!createXMLFragment)  w.writeEndDocument();
    }
    return xml;
}

bool QFTableModel::saveXML(const QString &filename, QModelIndexList selection, bool createXMLFragment, bool template_only)
{
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream out(&f);
        out<<saveXML(selection, createXMLFragment, template_only);
        f.close();
        return true;
    }
    return false;
}

void QFTableModel::copy(QModelIndexList selection, bool createXMLFragment, bool template_only) {
    QString xml=saveXML(selection, createXMLFragment, template_only);
    QMimeData* mime=new QMimeData();
    if (template_only) {
        //qDebug()<<"template:"<<xml;
        mime->setData("quickfit3/qfrdrtable_template", xml.toUtf8());
    } else {
        QString tsv, csv;
        {
            QTextStream out(&tsv);
            QLocale loc;
            loc.setNumberOptions(QLocale::OmitGroupSeparator);
            saveCSV(out, "\t", loc.decimalPoint().toLatin1(), QString(""), 'g', 6, true, selection);
        }
        {
            QTextStream out(&csv);
            QLocale loc;
            loc.setNumberOptions(QLocale::OmitGroupSeparator);
            saveCSV(out, ", ", '.', QString("#!"), 'g', 6, true, selection);
        }

        mime->setData("jkqtplotter/csv", csv.toUtf8());
        mime->setData("application/vnd.ms-excel", tsv.toLocal8Bit());
        mime->setData("quickfit/csv", csv.toUtf8());
        mime->setData("text/csv", csv.toLocal8Bit());
        mime->setData("text/comma-separated-values", csv.toLocal8Bit());

        mime->setData("text/plain", csv.toLocal8Bit());
        mime->setData("quickfit3/qfrdrtable", xml.toUtf8());
    }
    QApplication::clipboard()->setMimeData(mime);
}

void QFTableModel::paste(int row_start, int column_start) {
    QClipboard* clip=QApplication::clipboard();
    const QMimeData* mime=clip->mimeData();

    startMultiUndo();

    int row=row_start;
    int column=column_start;
    if (row_start<0) row=0;
    if (column_start<0) column=0;

    bool oldEmit=doEmitSignals;
    doEmitSignals=false;


    for (int i=0; i<mime->formats().size(); i++) {
        //qDebug()<<mime->formats().at(i)<<":";
        //qDebug()<<mime->data(mime->formats().at(i));
        //qDebug()<<"----------------------------------------------------------------------------------";
    }
    if (mime && (mime->hasFormat("quickfit3/qfrdrtable")|| mime->hasFormat("application/x-qt-windows-mime;value=\"quickfit3/qfrdrtable\""))) {
        QString data=QString::fromUtf8(mime->data("quickfit3/qfrdrtable").data());
        //qDebug()<<"pasting quickfit3/qfrdrtable";
        readXML(data, row, column, false, false, QFTableModel::rhmAskOverwrite);
    } else if (mime && mime->hasFormat("quickfit3/qfrdrtable_template")) {
        QString data=QString::fromUtf8(mime->data("quickfit3/qfrdrtable_template").data());
        //qDebug()<<"pasting quickfit3/qfrdrtable_template";
        readXML(data, row, column, false, true, QFTableModel::rhmAskOverwrite);
    } else if (mime && mime->hasFormat("jkqtplotter/csv")) {
        QString data=QString::fromUtf8(mime->data("jkqtplotter/csv").data());
        //qDebug()<<"pasting jkqtplotter/csv: \n"<<data;
        //readXML(data, row, column, false);
        QTextStream in(&data);
        readCSV(in, ',', '.', "#!", '#', row, column, false);
    } else if (mime && mime->hasFormat("quickfit/csv")) {
        QString data=QString::fromUtf8(mime->data("quickfit/csv").data());
        //qDebug()<<"pasting quickfit/csv: \n"<<data;
        //readXML(data, row, column, false);
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
        char comment='#';
        QString headercomment="";
        if (sl.size()>0) {
            QString d;
            int i=0;
            int cntl=0;
            while (i<sl.size() && cntl<20) {
                QString line=sl[i].trimmed();
                if (line.size()>0) {
                    QChar c0=line.at(0);
                    QChar c1='\0';
                    if (line.size()>1) c1=line.at(1);
                    if (c0.isDigit() || c0=='-' || c0=='+' || c0=='#') {
                        d=d+sl[i]+"\n";
                        cntl++;
                    }
                    if (c0=='#') { comment='#'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='%') { comment='%'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='&') { comment='&'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0==';') { comment=';'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='§') { comment='§'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='!') { comment='!'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='/') { comment='/'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='\\') { comment='\\'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='*') { comment='*'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='~') { comment='~'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='\'' && !line.contains('\'')) { comment='\''; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='\"' && !line.contains('\"')) { comment='\"'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='?') { comment='?'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='!') { comment='!'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='°') { comment='°'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='<') { comment='°'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='>') { comment='°'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                    else if (c0=='=') { comment='°'; if (QString("!§$&?°#~*+\\/<>").contains(c1)) headercomment=QString(comment)+QString(c1); }
                }
                i++;
            }
            double icntDot=d.count('.');
            double icntCom=d.count(',');
            d=d.remove(dec);
            int cnt=d.count('\n');
            int cntSem=d.count(';');
            int cntCom=d.count(',');
            int cntTab=d.count('\t');
            if (cntSem>=cnt && ';'!=dec) sep=';';
            if (cntCom>=cnt && ','!=dec) sep=',';
            if (cntTab>=cnt && cntSem<=0) sep='\t';
            if (cntTab>=cnt && cntSem<=0 && (cntCom<=0 || (cntCom>0 && icntDot>0))) sep='\t';
            if (dec==',' && sep==',') dec='.';
            if (sep!=',' && dec==',' && icntCom==0 && icntDot>0) dec='.';
            if (sep!=',' && dec=='.' && icntCom>0 && icntDot==0) dec=',';
            if (dec==',' && icntCom<=0 && icntDot>0 ) dec='.';
            if (icntDot>0 && icntCom>0) { dec='.'; sep=',';}
            if (sep==';' && comment==';') { comment='#'; headercomment=""; }
            if (headercomment.size()<=0) { headercomment=comment; headercomment+="!"; }
            //qDebug()<<"icntDot="<<icntDot;
            //qDebug()<<"icntCom="<<icntCom;
            //qDebug()<<"cnt="<<cnt;
            //qDebug()<<"cntSem="<<cntSem;
            //qDebug()<<"cntCom="<<cntCom;
            //qDebug()<<"cntTab="<<cntTab;
        }

        //qDebug()<<"sep='"<<sep<<"'    dec='"<<dec<<"'";
        readCSV(in, sep, dec, headercomment, comment, row, column);
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) reset();
    endMultiUndo();
}

void QFTableModel::pasteHeaderTemplate(int row_start, int column_start)
{
    QClipboard* clip=QApplication::clipboard();
    const QMimeData* mime=clip->mimeData();

    startMultiUndo();

    int row=row_start;
    int column=column_start;
    if (row_start<0) row=0;
    if (column_start<0) column=0;

    bool oldEmit=doEmitSignals;
    doEmitSignals=false;



    if (mime && (mime->hasFormat("quickfit3/qfrdrtable")|| mime->hasFormat("application/x-qt-windows-mime;value=\"quickfit3/qfrdrtable\""))) {
        QString data=QString::fromUtf8(mime->data("quickfit3/qfrdrtable").data());
        //qDebug()<<"pasting quickfit3/qfrdrtable";
        readXML(data, row, column, false, true, QFTableModel::rhmAskOverwrite);
    } else if (mime && mime->hasFormat("quickfit3/qfrdrtable_template")) {
        QString data=QString::fromUtf8(mime->data("quickfit3/qfrdrtable_template").data());
        //qDebug()<<"pasting quickfit3/qfrdrtable_template";
        readXML(data, row, column, false, true, QFTableModel::rhmAskOverwrite);

    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) reset();
    endMultiUndo();
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


bool QFTableModel::readXML(const QString &data, int start_row, int start_col, bool clearTable, bool read_template_only, QFTableModel::ReadHeaderMode alsoReadHeaders) {
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
    bool overwriteHeader=false;
    bool firstOverwrite=true;
    if (doc.setContent(data)) {
        startMultiUndo();
        QDomElement er=doc.firstChildElement("qfrdrtable");
        if (!er.isNull()) {
            if (clearTable || read_template_only || (alsoReadHeaders!=QFTableModel::rhmDontReadHeader)) {
                QDomElement e=er.firstChildElement("state.columns");
                if (!e.isNull()) {
                    e=e.firstChildElement("col");
                    while (!e.isNull()) {
                        int c=scol+e.attribute("col").toInt();
                        QString t=e.attribute("name");
                        if (clearTable || read_template_only || alsoReadHeaders==QFTableModel::rhmReadHeader) setColumnTitleCreate(c, t);
                        else if (alsoReadHeaders==QFTableModel::rhmOverwriteNondefault || alsoReadHeaders==QFTableModel::rhmAskOverwrite) {
                            QString oldt=state.columnNames.value(c, "");
                            bool ok=false;
                            QRegExp rxDefault1("^\\d+\\s*\\:\\s*\\d+$");
                            QRegExp rxDefault2("^\\s*\\d+\\s*$");
                            //int ildtn=oldt.toInt(&ok);
                            ok=((rxDefault1.indexIn(oldt)==0) || (rxDefault2.indexIn(oldt)==0));
                            //qDebug()<<oldt<<ok<<rxDefault1.indexIn(oldt)<<rxDefault2.indexIn(oldt)<<oldt.isEmpty();
                            if (alsoReadHeaders==QFTableModel::rhmAskOverwrite && firstOverwrite && !oldt.isEmpty() && oldt!=t && !ok) {
                                //qDebug()<<oldt<<ok;
                                firstOverwrite=false;
                                overwriteHeader=QMessageBox::question(NULL, tr("Overwrite Headers?"), tr("Overwrite already existing table headers?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes;
                            }
                            if (oldt.isEmpty() || ok || overwriteHeader) {
                                setColumnTitleCreate(c, t);
                            }
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
                                    if (clearTable || read_template_only || alsoReadHeaders==QFTableModel::rhmReadHeader) setColumnHeaderData(c, rxAtrD.cap(1).toInt(), md);
                                    else if (alsoReadHeaders==QFTableModel::rhmOverwriteNondefault || alsoReadHeaders==QFTableModel::rhmAskOverwrite) {
                                        QVariant hdold=getColumnHeaderData(c, rxAtrD.cap(1).toInt());
                                        if (alsoReadHeaders==QFTableModel::rhmAskOverwrite && firstOverwrite && hdold.isValid() && hdold!=md && !hdold.isNull()) {
                                            firstOverwrite=false;
                                            overwriteHeader=QMessageBox::question(NULL, tr("Overwrite Headers?"), tr("Overwrite already existing table header properties?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes;
                                        }
                                        if (!hdold.isValid() || hdold.isNull() || overwriteHeader)  {
                                            setColumnHeaderData(c, rxAtrD.cap(1).toInt(), md);
                                        }
                                    }
                                }
                            }
                        }


                        e=e.nextSiblingElement("col");
                    }
                }
            }
            QDomElement e=er.firstChildElement("data");
            if (!read_template_only && !e.isNull()) {
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
            endMultiUndo();
            return true;
        }
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) reset();
    return false;
}

bool QFTableModel::readXMLFile(const QString &filename, int start_row, int start_col, bool clearTable, bool read_template_only) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    return readXML(file.readAll(), start_row, start_col, clearTable, read_template_only);
}


QFTableModelColumnHeaderModel::QFTableModelColumnHeaderModel(QFTableModel *table, QObject *parent):
    QAbstractListModel(parent)
{
    model=NULL;
    hasNone=false;
    setModel(table);
}

QFTableModelColumnHeaderModel::~QFTableModelColumnHeaderModel()
{
}


void QFTableModelColumnHeaderModel::setModel(QFTableModel *model)
{
    if (this->model) {
        disconnect(this->model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)), this, SLOT(rebuildModel()));
        disconnect(this->model, SIGNAL(layoutChanged()), this, SLOT(rebuildModel()));
    }
    this->model=model;
    if (model) {
        connect(model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)), this, SLOT(rebuildModel()));
        connect(model, SIGNAL(layoutChanged()), this, SLOT(rebuildModel()));
    }
    rebuildModel();
}

void QFTableModelColumnHeaderModel::setHasNone(bool hasNone)
{
    this->hasNone=hasNone;
    rebuildModel();
}

QVariant QFTableModelColumnHeaderModel::data(const QModelIndex &index, int role) const
{
    int row=index.row();
    if (role==Qt::DisplayRole || role==Qt::EditRole) {
        if (hasNone) {
            if (row==0) return tr("--- none ---");
            return tr("#%1: %2").arg(row).arg(model->getColumnTitles().value(row-1, ""));
        } else {
            return tr("#%1: %2").arg(row+1).arg(model->getColumnTitles().value(row, ""));
        }
    } else if (role==Qt::UserRole) {
        if (hasNone) {
            if (row==0) return QVariant();
            return model->getColumnTitles().value(row-1, "");
        } else {
            return model->getColumnTitles().value(row, "");
        }
    }
    return QVariant();
}

Qt::ItemFlags QFTableModelColumnHeaderModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

QVariant QFTableModelColumnHeaderModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal && section==0) return tr("column title");
    return QVariant();
}

int QFTableModelColumnHeaderModel::rowCount(const QModelIndex &parent) const
{
    if (model) {
        if (hasNone) return model->columnCount()+1;
        return model->columnCount();
    }
    return 0;
}

void QFTableModelColumnHeaderModel::rebuildModel()
{
    reset();
}

void QFTableModelColumnHeaderModel::nameChanged(int i)
{
    emit dataChanged(index(i), index(i));
}

void QFTableModelColumnHeaderModel::nameDeleted(int i)
{
    //emit state.rowsRemoved(QModelIndex(), i, i);
}

void QFTableModelColumnHeaderModel::nameAdded(int i)
{
    //emit state.rowsInserted(QModelIndex(), i, i);
}







void QFTableModel::undo()
{
    if (!undoEnabled || readonly) return;
    //qDebug()<<"undo (undoCurrentPosition="<<undoCurrentPosition<<" list.size="<<undoList.size()<<") ...";
    if (undoList.size()>0 && undoCurrentPosition>=0 && undoCurrentPosition<undoList.size()) {
        if (undoCurrentPosition<=0) {
            addUndoStep();
            undoCurrentPosition=1;
        }
        state=undoList[undoCurrentPosition];
        undoCurrentPosition++;

        //qDebug()<<"undone ... calling reset";
        reset();
    }
    emitUndoRedoSignals();
    //qDebug()<<"undone ... DONE ...      (undoCurrentPosition="<<undoCurrentPosition<<" list.size="<<undoList.size()<<")   l="<<undoListT;
}

void QFTableModel::redo()
{
    if (!undoEnabled || readonly) return;
    //qDebug()<<"redo (undoCurrentPosition="<<undoCurrentPosition<<" list.size="<<undoList.size()<<") ...";
    if (undoList.size()>0 && undoCurrentPosition>0 && undoCurrentPosition<=undoList.size()) {
        state=undoList[undoCurrentPosition-1];
        undoCurrentPosition--;
        //qDebug()<<"redone ... calling reset";
        reset();
    }
    emitUndoRedoSignals();
    //qDebug()<<"redone ... DONE ...      (undoCurrentPosition="<<undoCurrentPosition<<" list.size="<<undoList.size()<<")   l="<<undoListT;
}


void QFTableModel::clearUndo()
{
    if (!undoEnabled) return;

    undoList.clear();
    undoListT.clear();
    rdlID=0;
    undoIsMultiStep=0;
    undoCurrentPosition=-1;
    emitUndoRedoSignals();
}

void QFTableModel::startMultiUndo()
{
    if (!undoEnabled) return;

    if (undoIsMultiStep<=0) {
        addUndoStep();
        undoIsMultiStep=0;
    }
    undoIsMultiStep++;

    //emitUndoRedoSignals();
}

void QFTableModel::endMultiUndo()
{
    if (!undoEnabled) return;
    undoIsMultiStep--;

    //emitUndoRedoSignals();
}

void QFTableModel::clearMultiUndo()
{
    if (!undoEnabled) return;
    undoIsMultiStep=0;
    addUndoStep();
    emitUndoRedoSignals();
}

void QFTableModel::setUndoEnabled(bool enabled)
{
    if (enabled!=undoEnabled) {
        undoEnabled=enabled;
        undoList.clear();
        undoListT.clear();
        rdlID=0;
        undoIsMultiStep=0;
        undoCurrentPosition=-1;
    }
    emitUndoRedoSignals();
}

QAction *QFTableModel::getUndoAction() const
{
    return actUndo;
}

QAction *QFTableModel::getRedoAction() const
{
    return actRedo;
}

void QFTableModel::addUndoStep()
{
    if (!undoEnabled || undoIsMultiStep>0) return;


    //qDebug()<<this<<" addUndoStep: pre: cur="<<undoCurrentPosition<<"  list.size="<<undoList.size()<< "   l="<<undoListT;
    while (undoCurrentPosition>0 && undoList.size()>0) {
        undoList.pop_front();
        undoListT.pop_front();
        undoCurrentPosition--;
    }

    undoListT.prepend(rdlID);
    rdlID++;
    undoList.prepend(state);
    undoCurrentPosition=0;
    while (undoList.size()>undoMaxSteps) {
        undoList.pop_back();
        undoListT.pop_back();
    }

    //qDebug()<<this<<" addUndoStep: cur="<<undoCurrentPosition<<"  list.size="<<undoList.size()<< "   l="<<undoListT;
    emitUndoRedoSignals();

}

void QFTableModel::emitUndoRedoSignals(bool alwaysEmit)
{
    if (!undoEnabled) {
        actUndo->setEnabled(false);
        actRedo->setEnabled(false);
        if (alwaysEmit) {
            emit undoAvailable(false);
            emit redoAvailable(false);
        }
        return;
    }
    bool ua=false;
    bool ra=false;

    if (undoList.size()>0) {
        if (undoCurrentPosition<=0) {
            ua=true;
            ra=false;
        } else if (undoCurrentPosition>undoList.size()-1) {
            ua=false;
            ra=true;
        } else {
            ua=true;
            ra=true;
        }
    }

    actUndo->setEnabled(ua);
    actRedo->setEnabled(ra);

    emit undoAvailable(ua);
    emit redoAvailable(ra);
}
