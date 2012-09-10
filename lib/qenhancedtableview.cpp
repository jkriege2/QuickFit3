#include "qenhancedtableview.h"
#include <QDebug>
#include <QSet>
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QAction>

QEnhancedTableView::QEnhancedTableView(QWidget* parent):
    QTableView(parent)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* act;

    act=new QAction(QIcon(":/lib/copy16.png"), tr("Copy Selection to Clipboard (for Excel ...)"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(copySelectionToExcel()));
    addAction(act);
    act=new QAction(QIcon(":/lib/copy16_nohead.png"), tr("Copy Selection to clipboard (for Excel ...) without header row/column"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(copySelectionToExcelNoHead()));
    addAction(act);
}

QEnhancedTableView::~QEnhancedTableView()
{
    //dtor
}


void QEnhancedTableView::copySelectionToExcelNoHead(int copyrole) {
    copySelectionToExcel(copyrole, false);
}

void QEnhancedTableView::copySelectionToExcel(int copyrole, bool storeHead) {
    if (!model()) return;
    if (!selectionModel()) return;
    QModelIndexList sel=selectionModel()->selectedIndexes();
    if (sel.size()==1) {
        QVariant vdata=sel[0].data(copyrole);
        QString txt="";
        switch (vdata.type()) {
            case QVariant::Int:
            case QVariant::LongLong:
            case QVariant::UInt:
            case QVariant::ULongLong:
            case QVariant::Bool:
                txt=vdata.toString();
                break;
            case QVariant::Double:
                txt=QLocale().toString(vdata.toDouble());
                break;
            case QVariant::PointF:
                txt=QLocale().toString(vdata.toPointF().x());
                break;
            default:
                txt=QString("\"%1\"").arg(vdata.toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " "));
                break;
        }
         QApplication::clipboard()->setText(txt);
    } else {
        QSet<int> rows, cols;
        int colmin=0;
        int rowmin=0;
        for (int i=0; i<sel.size(); i++) {
            int r=sel[i].row();
            int c=sel[i].column();
            rows.insert(r);
            cols.insert(c);
            if (i==0) {
                colmin=c;
                rowmin=r;
            } else {
                if (c<colmin) colmin=c;
                if (r<rowmin) rowmin=r;
            }
        }
        QList<int> rowlist=QList<int>::fromSet(rows);
        qSort(rowlist.begin(), rowlist.end());
        QList<int> collist=QList<int>::fromSet(cols);
        qSort(collist.begin(), collist.end());
        int rowcnt=rowlist.size();
        int colcnt=collist.size();
        QList<QStringList> data;

        // header row:
        //
        //  <EMPTY> | <HOR_HEDER1> | <HOR_HEADER2> | ...
        QStringList hrow;
        if (storeHead) {
            hrow.append(""); // empty header for first column (vertical headers!)
            for (int c=0; c<colcnt; c++) {
                hrow.append(QString("\"%1\"").arg(model()->headerData(collist[c], Qt::Horizontal).toString()));
            }
            data.append(hrow);
        }

        // now add dta rows:
        //
        //               <~~~~~~~~~ colcnt times ~~~~~~~~~~>
        //  <VER_HEADER> | <EMPTY> | <EMPTY> | ... | <EMPTY>
        for (int r=0; r<rowcnt; r++) {
            QStringList row;
            if (storeHead) row.append(QString("\"%1\"").arg(model()->headerData(rowlist[r], Qt::Vertical).toString())); // vertical header
            for (int c=0; c<colcnt; c++) {
                row.append(""); // empty columns for data
            }
            data.append(row);
        }
        for (int i=0; i<sel.size(); i++) {
            int r=rowlist.indexOf(sel[i].row());
            int c=collist.indexOf(sel[i].column());
            QVariant vdata=sel[i].data(copyrole);
            QString txt="";
            switch (vdata.type()) {
                case QVariant::Int:
                case QVariant::LongLong:
                case QVariant::UInt:
                case QVariant::ULongLong:
                case QVariant::Bool:
                    txt=vdata.toString();
                    break;
                case QVariant::Double:
                    txt=QLocale().toString(vdata.toDouble());
                    break;
                case QVariant::PointF:
                    txt=QLocale().toString(vdata.toPointF().x());
                    break;
                default:
                    txt=QString("\"%1\"").arg(vdata.toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " "));
                    break;
            }
            int shift=0;
            if (storeHead) shift=1;
            if ((r>=0) && (c>=0) && (r<=data.size()) && (c<=colcnt))data[r+shift][c+shift]=txt;
        }

        QString result="";
        for (int r=0; r<data.size(); r++) {
            result+=data[r].join("\t")+"\n";
        }
        QApplication::clipboard()->setText(result);
    }
}

int copySelectionAsValueErrorToExcelcompare_firstrole;

bool copySelectionAsValueErrorToExcelcompare(const QPair<int, int>& s1, const QPair<int, int>& s2) {
    if (s1.first<s2.first) return true;
    if (s1.second==copySelectionAsValueErrorToExcelcompare_firstrole) return true;
    return false;
}

void QEnhancedTableView::copySelectionAsValueErrorToExcel(int valuerole, int errorrole, bool storeHead, Qt::Orientation orientation) {
    if (valuerole==errorrole) {
        copySelectionToExcel(valuerole);
    } else {
        if (!model()) return;
        if (!selectionModel()) return;
        QModelIndexList sel=selectionModel()->selectedIndexes();
        QSet<QPair<int, int> > rows, cols;
        int colmin=0;
        int rowmin=0;
        for (int i=0; i<sel.size(); i++) {
            int r=sel[i].row();
            int c=sel[i].column();
            rows.insert(qMakePair(r,valuerole) );
            cols.insert(qMakePair(c,valuerole));
            if (orientation==Qt::Horizontal) cols.insert(qMakePair(c,errorrole));
            else rows.insert(qMakePair(r,errorrole));
            if (i==0) {
                colmin=c;
                rowmin=r;
            } else {
                if (c<colmin) colmin=c;
                if (r<rowmin) rowmin=r;
            }
        }
        copySelectionAsValueErrorToExcelcompare_firstrole=valuerole;
        QList<QPair<int, int> > rowlist=QList<QPair<int, int> >::fromSet(rows);
        qSort(rowlist.begin(), rowlist.end());
        QList<QPair<int, int> > collist=QList<QPair<int, int> >::fromSet(cols);
        qSort(collist.begin(), collist.end());
        int rowcnt=rowlist.size();
        int colcnt=collist.size();
        QList<QStringList> data;

        // header row:
        //
        //  <EMPTY> | <HOR_HEDER1> | <HOR_HEADER2> | ...
        QStringList hrow;
        if (storeHead) {
            hrow.append(""); // empty header for first column (vertical headers!)
            for (int c=0; c<colcnt; c++) {
                if (collist[c].second==valuerole) hrow.append(QString("\"%1\"").arg(model()->headerData(collist[c].first, Qt::Horizontal).toString()));
                else hrow.append(QString("\"error: %1\"").arg(model()->headerData(collist[c].first, Qt::Horizontal).toString()));
            }
            data.append(hrow);
        }

        // now add dta rows:
        //
        //               <~~~~~~~~~ colcnt times ~~~~~~~~~~>
        //  <VER_HEADER> | <EMPTY> | <EMPTY> | ... | <EMPTY>
        for (int r=0; r<rowcnt; r++) {
            QStringList row;
            if (storeHead) {
                if (rowlist[r].second==valuerole) row.append(QString("\"%1\"").arg(model()->headerData(rowlist[r].first, Qt::Vertical).toString())); // vertical header
                else row.append(QString("\"error: %1\"").arg(model()->headerData(rowlist[r].first, Qt::Vertical).toString())); // vertical header
            }
            for (int c=0; c<colcnt; c++) {
                row.append(""); // empty columns for data
            }
            data.append(row);
        }
        for (int i=0; i<sel.size(); i++) {
            int r=-1;
            int c=-1;
            for (int ri=0; ri<rowlist.size(); ri++) {
                if (rowlist[ri].first==sel[i].row() && rowlist[ri].second==valuerole) {
                    r=ri;
                    break;
                }
            }
            for (int ci=0; ci<collist.size(); ci++) {
                if (collist[ci].first==sel[i].column() && collist[ci].second==valuerole) {
                    c=ci;
                    break;
                }
            }

            QVariant vdata=sel[i].data(valuerole);
            QVariant edata=sel[i].data(errorrole);
            QString txt="", etxt="";
            switch (vdata.type()) {
                case QVariant::Int:
                case QVariant::LongLong:
                case QVariant::UInt:
                case QVariant::ULongLong:
                case QVariant::Bool:
                    txt=vdata.toString();
                    break;
                case QVariant::Double:
                    txt=QLocale().toString(vdata.toDouble());
                    break;
                case QVariant::PointF:
                    txt=QLocale().toString(vdata.toPointF().x());
                    break;
                default:
                    txt=QString("\"%1\"").arg(vdata.toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " "));
                    break;
            }
            switch (edata.type()) {
                case QVariant::Int:
                case QVariant::LongLong:
                case QVariant::UInt:
                case QVariant::ULongLong:
                case QVariant::Bool:
                    etxt=edata.toString();
                    break;
                case QVariant::Double:
                    etxt=QLocale().toString(edata.toDouble());
                    break;
                case QVariant::PointF:
                    etxt=QLocale().toString(edata.toPointF().x());
                    break;
                default:
                    etxt=QString("\"%1\"").arg(edata.toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " "));
                    break;
            }
            int shift=0;
            if (storeHead) shift=1;
            if ((r>=0) && (c>=0) && (r<=data.size()) && (c<=colcnt)) {
                data[r+shift][c+shift]=txt;
                if (orientation==Qt::Horizontal) {
                    if (c+1<=colcnt) data[r+shift][c+1+shift]=etxt;
                } else {
                    if (r+1<=rowcnt) data[r+1+shift][c+shift]=etxt;
                }
            }
        }

        QString result="";
        for (int r=0; r<data.size(); r++) {
            result+=data[r].join("\t")+"\n";
        }
        QApplication::clipboard()->setText(result);
    }
}

void QEnhancedTableView::copySelectionAsMedianQuantilesToExcel(int medianrole, int q25role, int q75role, bool storeHead, Qt::Orientation orientation)
{
    if (q25role==-1 && q75role==-1) {
        copySelectionToExcel(medianrole);
    } else if (q25role!=-1 && q75role==-1) {
        copySelectionAsValueErrorToExcel(medianrole, q25role, storeHead, orientation);
    } else {
        if (!model()) return;
        if (!selectionModel()) return;
        QModelIndexList sel=selectionModel()->selectedIndexes();
        QSet<QPair<int, int> > rows, cols;
        int colmin=0;
        int rowmin=0;
        for (int i=0; i<sel.size(); i++) {
            int r=sel[i].row();
            int c=sel[i].column();
            rows.insert(qMakePair(r,medianrole) );
            cols.insert(qMakePair(c,medianrole));
            if (orientation==Qt::Horizontal) {
                cols.insert(qMakePair(c,q25role));
                cols.insert(qMakePair(c,q75role));
            } else {
                rows.insert(qMakePair(r,q25role));
                rows.insert(qMakePair(r,q75role));
            }
            if (i==0) {
                colmin=c;
                rowmin=r;
            } else {
                if (c<colmin) colmin=c;
                if (r<rowmin) rowmin=r;
            }
        }
        copySelectionAsValueErrorToExcelcompare_firstrole=medianrole;
        QList<QPair<int, int> > rowlist=QList<QPair<int, int> >::fromSet(rows);
        qSort(rowlist.begin(), rowlist.end());
        QList<QPair<int, int> > collist=QList<QPair<int, int> >::fromSet(cols);
        qSort(collist.begin(), collist.end());
        int rowcnt=rowlist.size();
        int colcnt=collist.size();
        QList<QStringList> data;

        // header row:
        //
        //  <EMPTY> | <HOR_HEDER1> | <HOR_HEADER2> | ...
        QStringList hrow;
        if (storeHead) {
            hrow.append(""); // empty header for first column (vertical headers!)
            for (int c=0; c<colcnt; c++) {
                if (collist[c].second==medianrole) hrow.append(QString("\"%1\"").arg(model()->headerData(collist[c].first, Qt::Horizontal).toString()));
                else if  (collist[c].second==q25role) hrow.append(QString("\"25% quantile: %1\"").arg(model()->headerData(collist[c].first, Qt::Horizontal).toString()));
                else if  (collist[c].second==q75role) hrow.append(QString("\"75% quantile: %1\"").arg(model()->headerData(collist[c].first, Qt::Horizontal).toString()));
            }
            data.append(hrow);
        }

        // now add dta rows:
        //
        //               <~~~~~~~~~ colcnt times ~~~~~~~~~~>
        //  <VER_HEADER> | <EMPTY> | <EMPTY> | ... | <EMPTY>
        for (int r=0; r<rowcnt; r++) {
            QStringList row;
            if (storeHead) {
                if (rowlist[r].second==medianrole) row.append(QString("\"%1\"").arg(model()->headerData(rowlist[r].first, Qt::Vertical).toString())); // vertical header
                else if (rowlist[r].second==q25role) row.append(QString("\"25% quantile: %1\"").arg(model()->headerData(rowlist[r].first, Qt::Vertical).toString())); // vertical header
                else if (rowlist[r].second==q75role) row.append(QString("\"75% quantile: %1\"").arg(model()->headerData(rowlist[r].first, Qt::Vertical).toString())); // vertical header
            }
            for (int c=0; c<colcnt; c++) {
                row.append(""); // empty columns for data
            }
            data.append(row);
        }
        for (int i=0; i<sel.size(); i++) {
            int r=-1;
            int c=-1;
            for (int ri=0; ri<rowlist.size(); ri++) {
                if (rowlist[ri].first==sel[i].row() && rowlist[ri].second==medianrole) {
                    r=ri;
                    break;
                }
            }
            for (int ci=0; ci<collist.size(); ci++) {
                if (collist[ci].first==sel[i].column() && collist[ci].second==medianrole) {
                    c=ci;
                    break;
                }
            }

            QVariant vdata=sel[i].data(medianrole);
            QVariant q25data=sel[i].data(q25role);
            QVariant q75data=sel[i].data(q75role);
            QString txt="", q25txt="", q75txt="";
            switch (vdata.type()) {
                case QVariant::Int:
                case QVariant::LongLong:
                case QVariant::UInt:
                case QVariant::ULongLong:
                case QVariant::Bool:
                    txt=vdata.toString();
                    break;
                case QVariant::Double:
                    txt=QLocale().toString(vdata.toDouble());
                    break;
                case QVariant::PointF:
                    txt=QLocale().toString(vdata.toPointF().x());
                    break;
                default:
                    txt=QString("\"%1\"").arg(vdata.toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " "));
                    break;
            }
            switch (q25data.type()) {
                case QVariant::Int:
                case QVariant::LongLong:
                case QVariant::UInt:
                case QVariant::ULongLong:
                case QVariant::Bool:
                    q25txt=q25data.toString();
                    break;
                case QVariant::Double:
                    q25txt=QLocale().toString(q25data.toDouble());
                    break;
                case QVariant::PointF:
                    q25txt=QLocale().toString(q25data.toPointF().x());
                    break;
                default:
                    q25txt=QString("\"%1\"").arg(q25data.toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " "));
                    break;
            }
            switch (q75data.type()) {
                case QVariant::Int:
                case QVariant::LongLong:
                case QVariant::UInt:
                case QVariant::ULongLong:
                case QVariant::Bool:
                    q75txt=q75data.toString();
                    break;
                case QVariant::Double:
                    q75txt=QLocale().toString(q75data.toDouble());
                    break;
                case QVariant::PointF:
                    q75txt=QLocale().toString(q75data.toPointF().x());
                    break;
                default:
                    q75txt=QString("\"%1\"").arg(q75data.toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " "));
                    break;
            }
            int shift=0;
            if (storeHead) shift=1;
            if ((r>=0) && (c>=0) && (r<=data.size()) && (c<=colcnt)) {
                data[r+shift][c+shift]=txt;
                if (orientation==Qt::Horizontal) {
                    if (c+1<=colcnt) data[r+shift][c+1+shift]=q25txt;
                    if (c+2<=colcnt) data[r+shift][c+2+shift]=q75txt;
                } else {
                    if (r+1<=rowcnt) data[r+1+shift][c+shift]=q25txt;
                    if (r+2<=rowcnt) data[r+2+shift][c+shift]=q75txt;
                }
            }
        }

        QString result="";
        for (int r=0; r<data.size(); r++) {
            result+=data[r].join("\t")+"\n";
        }
        QApplication::clipboard()->setText(result);
    }
}



QString QEnhancedTableView::toHtml(int borderWidth, bool non_breaking, int fontSizePt) const {
    if (!model()) return "";
    QString fsstyle="";
    if (fontSizePt>0) {
        fsstyle=QString(" font-size: %1pt;").arg(fontSizePt);
    }
    QString html=QString("<table border=\"%1\" cellspacing=\"0\" cellpadding=\"0\">").arg(borderWidth);
    for (int row=-1; row<model()->rowCount(); row++) {
        html+="<tr>";
        if (row==-1) {
            for (int col=-1; col<model()->columnCount(); col++) {
                html+=QString("<th style=\"white-space: nowrap;%1\"><nobr>").arg(fsstyle);
                if (col>=0) {
                    html+=model()->headerData(col, Qt::Horizontal).toString();
                }
                html+="</nobr></th>";
            }
        } else {
            for (int col=-1; col<model()->columnCount(); col++) {
                if (col==-1) {
                    html+=QString("<th style=\"white-space: nowrap;%1\"><nobr>").arg(fsstyle);
                    html+=model()->headerData(row, Qt::Vertical).toString();
                    html+="</nobr></th>";
                } else {
                    QModelIndex index=model()->index(row, col);
                    QVariant check=index.data(Qt::CheckStateRole);
                    QBrush back=index.data(Qt::BackgroundRole).value<QBrush>();
                    QString style=fsstyle+"white-space: nowrap; ";
                    //qDebug()<<"r="<<row<<"\tc="<<col<<"\tcolor="<<back.color().name();
                    if (back.color()!=QColor(0,0,0) && index.data(Qt::BackgroundRole).isValid()) style=QString("background: %1;").arg(back.color().name());
                    if (style.isEmpty()) html+=QString("<td style=\"%1\"><nobr>").arg(fsstyle);
                    else html+=QString("<td style=\"%1\"><nobr>").arg(style);
                    if (check.isValid()) {
                        if (check.toInt()!=0) {
                            html+= QString("|&times;|&nbsp;&nbsp;");
                        } else {
                            html+= QString("|&nbsp;|&nbsp;&nbsp;");
                        }
                    }
                    html+=index.data().toString();
                    html+="</nobr></td>";
                }
            }
        }
        html+="</tr>";

    }
    html+="</table>";
    return html;
}



void QEnhancedTableView::keyPressEvent(QKeyEvent* event) {
    if (event->matches(QKeySequence::Copy)) {
        copySelectionToExcel(Qt::EditRole, false);
        event->accept();
    } else QTableView::keyPressEvent(event);
    emit keyPressed(event->key(), event->modifiers(), event->text());
}
