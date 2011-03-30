#include "qenhancedtableview.h"

#include <QSet>
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>

QEnhancedTableView::QEnhancedTableView(QWidget* parent):
    QTableView(parent)
{
    //ctor
}

QEnhancedTableView::~QEnhancedTableView()
{
    //dtor
}


void QEnhancedTableView::copySelectionToExcel() {
    if (!model()) return;
    if (!selectionModel()) return;
    QModelIndexList sel=selectionModel()->selectedIndexes();
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
    hrow.append(""); // empty header for first column (vertical headers!)
    for (int c=0; c<colcnt; c++) {
        hrow.append(QString("\"%1\"").arg(model()->headerData(collist[c], Qt::Horizontal).toString()));
    }
    data.append(hrow);

    // now add dta rows:
    //
    //               <~~~~~~~~~ colcnt times ~~~~~~~~~~>
    //  <VER_HEADER> | <EMPTY> | <EMPTY> | ... | <EMPTY>
    for (int r=0; r<rowcnt; r++) {
        QStringList row;
        row.append(QString("\"%1\"").arg(model()->headerData(rowlist[r], Qt::Vertical).toString())); // vertical header
        for (int c=0; c<colcnt; c++) {
            row.append(""); // empty columns for data
        }
        data.append(row);
    }
    for (int i=0; i<sel.size(); i++) {
        int r=rowlist.indexOf(sel[i].row());
        int c=collist.indexOf(sel[i].column());
        QVariant vdata=sel[i].data(Qt::EditRole);
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
                txt=QString("\"%1\"").arg(vdata.toString());
                break;
        }
        if ((r>=0) && (c>=0) && (r<=data.size()) && (c<=colcnt))data[r+1][c+1]=txt;
    }

    QString result="";
    for (int r=0; r<data.size(); r++) {
        result+=data[r].join("\t")+"\n";
    }
    QApplication::clipboard()->setText(result);
}

void QEnhancedTableView::keyPressEvent(QKeyEvent* event) {
    if (event->matches(QKeySequence::Copy)) {
        copySelectionToExcel();
        event->accept();
    } else QTableView::keyPressEvent(event);
}


