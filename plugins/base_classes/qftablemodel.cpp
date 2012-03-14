#include "qftablemodel.h"
#include <QDebug>

QFTableModel::QFTableModel(QObject * parent):
    QAbstractTableModel(parent)
{
    readonly=true;
    readonlyButStillCheckable=false;
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
    //qDebug()<<index<<"("<<role<<" chk="<<Qt::CheckStateRole<<")"<<value;
    if (index.isValid() && role == Qt::EditRole && (!readonly || readonlyButStillCheckable)) {
        quint32 a=xyAdressToUInt32(index.row(), index.column());
        QVariant old=QVariant();
        if (dataMap.contains(a)) old=dataMap[a];
        dataMap[a]=value;
        hasDataChanged=hasDataChanged | (old!=value);
        emit dataChanged(index, index);
        return true;
    } else if (index.isValid() && role == Qt::CheckStateRole && (!readonly || readonlyButStillCheckable)) {
        quint32 a=xyAdressToUInt32(index.row(), index.column());
        QVariant old=QVariant();
        if (dataCheckedMap.contains(a)) old=dataCheckedMap[a];
        dataCheckedMap[a]=value;
        hasDataChanged=hasDataChanged | (old!=value);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

void QFTableModel::resize(quint16 rows, quint16 columns) {
    if (readonly) return;
    std::cout<<"  resize("<<rows<<", "<<columns<<"): 1\n";
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


    std::cout<<"  resize("<<rows<<", "<<columns<<"): 2\n";
    this->rows=rows;
    this->columns=columns;

    QList<quint32> idx=dataMap.keys();
    QList<quint32> didx;
    std::cout<<"  resize("<<rows<<", "<<columns<<"): 3\n";
    if (idx.size()>0) for (int i = idx.size()-1; i>=0; i--) {
        quint16 r=UInt32ToRow(idx[i]);
        quint16 c=UInt32ToColumn(idx[i]);
        if ((r>=rows) || (c>=columns)) {
            didx.append(idx[i]);
        }
    }
    std::cout<<"  resize("<<rows<<", "<<columns<<"): 4\n";
    if (didx.size()>0) for (int i = didx.size()-1; i>=0; i--) {
        dataMap.remove(didx[i]);
        if (dataEditMap.contains(didx[i])) dataEditMap.remove(didx[i]);
    }
    std::cout<<"  resize("<<rows<<", "<<columns<<"): 5\n";
    reset();
    std::cout<<"  resize("<<rows<<", "<<columns<<"): 6\n";
}

void QFTableModel::insertRow(quint16 r) {
    appendRow();
    if (r<rows-1) for (int i=rows-2; i>=r; i--) {
        for (int c=0; c<columns; c++) {
            setCell(i+1, c, cell(i, c));
        }
    }
    for (int c=0; c<columns; c++) {
        setCell(r, c, QVariant());
    }
    reset();
}

void QFTableModel::insertColumn(quint16 c) {
    //std::cout<<"  insertColumn("<<c<<")\n";
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
    reset();
    //std::cout<<"  insertColumn("<<c<<") ... DONE\n";
}

void QFTableModel::deleteRow(quint16 r) {
    if (r>=rows ) return;
    for (int i=r; i<rows-1; i++) {
        for (int c=0; c<columns; c++) {
            setCell(i, c, cell(i+1, c));
        }
    }
    resize(rows-1, columns);
    reset();
}

void QFTableModel::deleteColumn(quint16 c) {
    if (c>=columns) return;
    for (int i=c; i<columns-1; i++) {
        for (int r=0; r<rows; r++) {
            setCell(r, i, cell(r, i+1));
        }
        if (i+1<columnNames.size()) columnNames[i]=columnNames[i+1];
    }
    resize(rows, columns-1);
    reset();
}

void QFTableModel::clear() {
    if (readonly) return;
    rows=0;
    columns=0;
    dataMap.clear();
    dataEditMap.clear();
    columnNames.clear();
    reset();
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
    reset();
}

void QFTableModel::setCell(quint16 row, quint16 column, QVariant value) {
    //std::cout<<"setCell("<<row<<", "<<column<<", '"<<value.toString().toStdString()<<"' ["<<value.typeName()<<"])\n";
    if (readonly || (row>=rows) || (column>=columns)) return;
    quint32 a=xyAdressToUInt32(row, column);
    dataMap[a]=value;
}

void QFTableModel::setCellEditRole(quint16 row, quint16 column, QVariant value) {
    //std::cout<<"setCell("<<row<<", "<<column<<", '"<<value.toString().toStdString()<<"' ["<<value.typeName()<<"])\n";
    if (readonly || (row>=rows) || (column>=columns)) return;
    quint32 a=xyAdressToUInt32(row, column);
    dataEditMap[a]=value;
}

void QFTableModel::setCellBackgroundRole(quint16 row, quint16 column, QVariant value) {
    if (readonly || (row>=rows) || (column>=columns)) return;
    quint32 a=xyAdressToUInt32(row, column);
    dataBackgroundMap[a]=value;}

void QFTableModel::setCellCheckedRole(quint16 row, quint16 column, QVariant value) {
    if (readonly || (row>=rows) || (column>=columns)) return;
    quint32 a=xyAdressToUInt32(row, column);
    dataCheckedMap[a]=value;
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
    reset();
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

bool QFTableModel::saveCSV(const QString& filename, QString column_separator, char decimal_separator, QString header_start, char format, int precision) {
    // try output file
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("ISO-8859-1"));
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    out.setLocale(loc);

    // write column headers
    out<<header_start<<" ";
    for (quint16 c=0; c<columns; c++) {
        if (c>0) out<<column_separator;
        if (c<columnNames.size()) out<<QString("\"%1\"").arg(columnNames[c]).arg(c+1);
    }
    out<<"\n";


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
                        if (decimal_separator!='.') {
                            num=num.replace('.', decimal_separator);
                        }
                        break;
                    default:
                        num=QString("\"%1\"").arg(v.toString());
                        break;
                }
                out<<num<<column_separator;
            }
        }
        out<<"\n";
    }
    out<<QString("\n\n");
    return true;
}

/*bool QFTableModel::readCSV(const QString& filename, const QString& column_separator, const QString& decimal_separator, const QString& header_start, const QString& comment_start) {
    char ccolumn_separator=',';
    char cdecimal_separator='.';
    char ccomment_start='#';
    if (column_separator.size()>0) ccolumn_separator=column_separator[0].toAscii();
    if (decimal_separator.size()>0) cdecimal_separator=decimal_separator[0].toAscii();
    if (comment_start.size()>0) ccomment_start=comment_start[0].toAscii();
    return readCSV(filename, ccolumn_separator, cdecimal_separator, header_start, ccomment_start);
}*/

bool QFTableModel::readCSV(const QString& filename, char column_separator, char decimal_separator, QString header_start, char comment_start) {
    // try output file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&file);
    bool ro=readonly;
    readonly=false;
    clear();
    //std::cout<<"      opening '"<<filename.toStdString()<<"'\n";
    QString line=in.readLine();
    bool header_read=false;
    quint16 row=0, rows=0, column=0, columns=0;
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    while (!line.isNull()) {
        bool dataread=false;
        line=line.trimmed();
        //std::cout<<"read: <"<<line.toStdString()<<">\n";
        if (line.size()>0) {
            if (line.startsWith(header_start)) {
                header_read=true;
                line=line.right(line.size()-header_start.size()).trimmed();
                QStringList sl=line.split(QString(column_separator));
                columns=(columns>sl.size())?columns:sl.size();
                resize(rows, columns);
                columnNames.clear();
                for (int i=0; i<sl.size(); i++) {
                    QString n=sl[i].trimmed();
                    if (n[0]=='\"' || n[0]=='\'') {
                        n=n.mid(1, n.size()-2);
                    }
                    columnNames.append(n);
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
                        resize(rows, columns);
                        setCell(row, column, s);
                        //std::cout<<"  <"<<row<<", "<<column<<">="<<s.toStdString()<<"\n";
                        dataread=true;
                    } else if (c=='\"') {
                        i++;
                        QString s="";
                        while (i<line.size() && line[i]!='\"') {
                            if (line[i]!='\"') s=s+line[i];
                            i++;
                        }
                        resize(rows, columns);
                        setCell(row, column, s);
                        //std::cout<<"  <"<<row<<", "<<column<<">="<<s.toStdString()<<"\n";
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
                            resize(rows, columns);
                            if (d==round(d)) {
                                setCell(row, column, ((qlonglong)d));
                                //std::cout<<"  <"<<row<<"/"<<rows<<", "<<column<<"/"<<columns<<">="<<(qlonglong)d<<"\n";
                                dataread=true;
                            } else {
                                setCell(row, column, d);
                                //std::cout<<"  <"<<row<<"/"<<rows<<", "<<column<<"/"<<columns<<">="<<d<<"\n";
                                dataread=true;
                            }
                        } else {
                            QDateTime dt=loc.toDateTime(s);
                            resize(rows, columns);
                            if (dt.isValid()) {
                                setCell(row, column, dt);
                                dataread=true;
                            } else {
                                setCell(row, column, s);
                                dataread=true;
                            }
                        }
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
    } ;

    //std::cout<<"rows="<<rows<<"   columns="<<columns<<std::endl;
    this->rows=rows;
    this->columns=columns;
    readonly=ro;
    reset();
    return true;

}
