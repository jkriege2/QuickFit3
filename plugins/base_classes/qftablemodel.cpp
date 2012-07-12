#include "qftablemodel.h"
#include <QDebug>

QFTableModel::QFTableModel(QObject * parent):
    QAbstractTableModel(parent)
{
    readonly=true;
    readonlyButStillCheckable=false;
    doEmitSignals=true;
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
            if (dataEditMap.contains(a)) return dataEditMap[a];
            else return dataMap[a];
        } else if (role == Qt::CheckStateRole) {
            if (dataCheckedMap.contains(a)) return dataCheckedMap[a];
        } else if (role == Qt::BackgroundRole) {
            if (dataBackgroundMap.contains(a)) return dataBackgroundMap[a];
        } else if (role == Qt::ToolTipRole) {
            if (dataMap.contains(a)) return tr("row: <i>%2</i><br>column: <i>%3 (\"%4\")</i><br>datatype: <i>%1</i><br>data: <b>%5</b>").arg(getQVariantType(dataMap[a])).arg(index.row()).arg(index.column()).arg(columnNames.value(index.column(), "")).arg(getQVariantData(index.data()));
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
         if (section<columnNames.size()) return columnNames[section];
     }

     return QVariant();
}

bool QFTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;

    //qDebug()<<index<<"("<<role<<" chk="<<Qt::CheckStateRole<<")"<<value;
    if (index.isValid() && role == Qt::EditRole && (!readonly || readonlyButStillCheckable)) {
        quint32 a=xyAdressToUInt32(index.row(), index.column());
        QVariant old=QVariant();
        if (dataMap.contains(a)) old=dataMap[a];
        dataMap[a]=value;
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
    }
    doEmitSignals=oldEmit;
    return false;
}

void QFTableModel::resize(quint16 rows, quint16 columns) {
    if (readonly) return;
    //std::cout<<"  resize("<<rows<<", "<<columns<<"): 1\n";
    //quint16 oldrows=this->rows;
    quint16 oldcolumns=this->columns;

    if (columns>oldcolumns) {
        for (int i=oldcolumns; i<columns; i++) {
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
    }
    //std::cout<<"  resize("<<rows<<", "<<columns<<"): 5\n";
    if (doEmitSignals)reset();
    //std::cout<<"  resize("<<rows<<", "<<columns<<"): 6\n";
}

void QFTableModel::insertRow(quint16 r) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    appendRow();
    if (r<rows-1) for (int i=rows-2; i>=r; i--) {
        for (int c=0; c<columns; c++) {
            setCell(i+1, c, cell(i, c));
        }
    }
    for (int c=0; c<columns; c++) {
        setCell(r, c, QVariant());
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals)reset();
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
            setCell(r, i+1, cell(r, i));
        }
    }
    for (int r=0; r<rows; r++) {
        setCell(r, c, QVariant());
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals)reset();
    //std::cout<<"  insertColumn("<<c<<") ... DONE\n";
}

void QFTableModel::deleteRow(quint16 r) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    if (r>=rows ) return;
    for (int i=r; i<rows-1; i++) {
        for (int c=0; c<columns; c++) {
            setCell(i, c, cell(i+1, c));
        }
    }
    resize(rows-1, columns);
    doEmitSignals=oldEmit;
    if (doEmitSignals)reset();
}

void QFTableModel::deleteColumn(quint16 c) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    if (c>=columns) return;
    for (int i=c; i<columns-1; i++) {
        for (int r=0; r<rows; r++) {
            setCell(r, i, cell(r, i+1));
        }
        if (i+1<columnNames.size()) columnNames[i]=columnNames[i+1];
    }
    resize(rows, columns-1);
    doEmitSignals=oldEmit;
    if (doEmitSignals)reset();
}

void QFTableModel::deleteCell(quint16 row, quint16 column) {
    if (readonly) return;
    quint32 a=xyAdressToUInt32(row, column);
    dataMap.remove(a);
    dataEditMap.remove(a);
    dataBackgroundMap.remove(a);
    dataCheckedMap.remove(a);
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
}

void QFTableModel::deleteCells(QModelIndexList selection) {
    bool oldEmit=doEmitSignals;
    doEmitSignals=false;
    for (int i=0; i<selection.size(); i++) {
        deleteCell(selection[i].row(), selection[i].column());
    }
    doEmitSignals=oldEmit;
    if (doEmitSignals) reset();
}

void QFTableModel::clear() {
    if (readonly) return;
    rows=0;
    columns=0;
    dataMap.clear();
    dataEditMap.clear();
    dataBackgroundMap.clear();
    dataCheckedMap.clear();
    columnNames.clear();
    if (doEmitSignals) reset();
}

bool QFTableModel::changeDatatype(quint16 row, quint16 column, QVariant::Type newType) {
    if (readonly || (row>=rows) || (column>=columns)) return false;
    quint32 a=xyAdressToUInt32(row, column);
    if (dataMap.contains(a)) {
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

void QFTableModel::setCellCreate(quint16 row, quint16 column, QVariant value) {
    if (readonly) return;
    if ((row>=rows) || (column>=columns)) {
        resize(qMax(rows, quint16(row+1)), qMax(columns, quint16(column+1)));
    }
    quint32 a=xyAdressToUInt32(row, column);
    dataMap[a]=value;
    if (doEmitSignals) emit dataChanged(index(row, column), index(row, column));
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

QVariant QFTableModel::cell(quint16 row, quint16 column) const {
    if ((row>=rows) || (column>=columns)) return QVariant();
    quint32 a=xyAdressToUInt32(row, column);
    if (dataMap.contains(a)) return dataMap[a];
    return QVariant();
}

QVariant QFTableModel::cellEditRole(quint16 row, quint16 column) const {
    if ((row>=rows) || (column>=columns)) return QVariant();
    quint32 a=xyAdressToUInt32(row, column);
    if (dataEditMap.contains(a)) return dataEditMap[a];
    return QVariant();
}
void QFTableModel::setColumnTitle(quint16 column, QString name) {
    //std::cout<<"setColumnTitle("<<column<<", '"<<name.toStdString()<<")\n";
    if (readonly || (column>=columns)) return;
    if (column<columnNames.size()) columnNames[column]=name;
    if (doEmitSignals) reset();
}

void QFTableModel::setColumnTitleCreate(quint16 column, QString name) {
    //std::cout<<"setColumnTitle("<<column<<", '"<<name.toStdString()<<")\n";
    if (readonly) return;
    resize(rows, qMax(columns, quint16(column+1)));
    if (column<columnNames.size()) columnNames[column]=name;
    if (doEmitSignals) reset();
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
            } else if (line.startsWith(comment_start) && &hasTitle && !hasData) {
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

void QFTableModel::copy(QModelIndexList selection, bool createXMLFragment) {
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
                    w.writeAttribute("type", getQVariantType(d));
                    w.writeCharacters(getQVariantData(d));
                    w.writeEndElement();
                }
            }
        }
        w.writeEndElement();
        w.writeEndElement();
        if (!createXMLFragment)  w.writeEndDocument();
    }
    QMimeData* mime=new QMimeData();
    mime->setData("quickfit3/qfrdrtable", xml.toUtf8());
    QString csv;
    QTextStream out(&csv);
    QLocale loc;
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
        readXML(data, row, column, false);
    } else if (mime && mime->hasText()) {
        QString data=mime->text();
        QLocale loc;
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

                    if (e.hasAttribute("check")) {

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
