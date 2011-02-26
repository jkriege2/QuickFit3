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
    for (int i=0; i<sel.size(); i++) {
        int r=sel[i].row();
        int c=sel[i].column();
        rows.insert(r);
        cols.insert(c);
    }
    int rowcnt=rows.size();
    int colcnt=cols.size();
    QList<QStringList> data;
    QStringList hrow;
    for (int c=0; c<colcnt+1; c++) {
        hrow.append(QString("\"%1\"").arg(model()->headerData(c, Qt::Horizontal).toString()));
    }
    data.append(hrow);
    for (int r=0; r<rowcnt; r++) {
        QStringList row;
        row.append(QString("\"%1\"").arg(model()->headerData(r, Qt::Vertical).toString()));
        for (int c=0; c<colcnt; c++) {
            row.append("");
        }
        data.append(row);
    }
    for (int i=0; i<sel.size(); i++) {
        int r=sel[i].row();
        int c=sel[i].column();
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
        data[r+1][c+1]=txt;
    }

    QString result="";
    for (int r=0; r<rowcnt; r++) {
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


