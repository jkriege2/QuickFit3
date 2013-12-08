#include "qenhancedtableview.h"
#include <QDebug>
#include <QSet>
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QAction>
#include "qftools.h"
#include <QLocale>
#include "matlabtools.h"
#include "qftextdocumentprintpreview.h"
#include "programoptions.h"
#include <QDialog>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QPrinter>
#include <QHeaderView>
#include <QPrintDialog>
#include <QDialogButtonBox>
#include "qfhtmldelegate.h"
#include <QSvgGenerator>
#include "datatools.h"

QEnhancedTableView::QEnhancedTableView(QWidget* parent):
    QTableView(parent)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* act;

    actCopyExcel=new QAction(QIcon(":/lib/copy16.png"), tr("Copy Selection to Clipboard (for Excel ...)"), this);
    connect(actCopyExcel, SIGNAL(triggered()), this, SLOT(copySelectionToExcel()));
    addAction(actCopyExcel);
    actCopyExcelNoHead=new QAction(QIcon(":/lib/copy16_nohead.png"), tr("Copy Selection to clipboard (for Excel ...) without header row/column"), this);
    actCopyExcelNoHead->setShortcut(QKeySequence::Copy);
    connect(actCopyExcelNoHead, SIGNAL(triggered()), this, SLOT(copySelectionToExcelNoHead()));
    addAction(actCopyExcelNoHead);
    actCopyMatlab=new QAction(QIcon(":/lib/copy16_matlab.png"), tr("Copy Selection to clipboard (Matlab)"), this);
    connect(actCopyMatlab, SIGNAL(triggered()), this, SLOT(copySelectionToMatlabNoHead()));
    addAction(actCopyMatlab);
    actCopyImage=new QAction(QIcon(":/lib/copy16.png"), tr("Copy table as image"), this);
    connect(actCopyImage, SIGNAL(triggered()), this, SLOT(copyAsImage()));
    addAction(actCopyImage);
    actPrint=new QAction(QIcon(":/lib/print.png"), tr("&Print Table"), this);
    actPrint->setShortcut(QKeySequence::Print);
    connect(actPrint, SIGNAL(triggered()), this, SLOT(print()));
    addAction(actPrint);

    menuSave=new QMenu(tr("save table"), this);
    menuSave->setIcon(QIcon(":/lib/save16.png"));
    addAction(menuSave->menuAction());

    actSave=new QAction(QIcon(":/lib/save16.png"), tr("&Save table"), this);
    connect(actSave, SIGNAL(triggered()), this, SLOT(save()));
    menuSave->addAction(actSave);
    actSaveFlipped=new QAction(QIcon(":/lib/save16.png"), tr("&Save table, flipped"), this);
    connect(actSaveFlipped, SIGNAL(triggered()), this, SLOT(saveFlipped()));
    menuSave->addAction(actSaveFlipped);
    actSaveExtended=new QAction(QIcon(":/lib/save16.png"), tr("&Save table, extended form"), this);
    connect(actSaveExtended, SIGNAL(triggered()), this, SLOT(saveExtended()));
    menuSave->addAction(actSaveExtended);
    actSaveExtendedFlipped=new QAction(QIcon(":/lib/save16.png"), tr("&Save table, extended form, flipped"), this);
    connect(actSaveExtendedFlipped, SIGNAL(triggered()), this, SLOT(saveExtendedFlipped()));
    menuSave->addAction(actSaveExtendedFlipped);
}

QEnhancedTableView::~QEnhancedTableView()
{
    //dtor
}


void QEnhancedTableView::copySelectionToExcelNoHead(int copyrole) {
    copySelectionToExcel(copyrole, false);
}

void QEnhancedTableView::copySelectionToMatlabNoHead(int copyrole, bool flipped)
{
    if (!model()) return;
    if (!selectionModel()) return;
    QList<QList<QVariant> > csvData;
    QStringList colnames;
    QStringList rownames;
    getVariantDataTable(copyrole, csvData, colnames, rownames);

    if (selectionModel()->selectedIndexes().size()==1) dataExpand(csvData, &colnames);
    else dataReduce(csvData, &colnames);

    if (flipped)  {
        csvData=dataRotate(csvData);
        qSwap(colnames, rownames);
    }
    QFDataExportHandler::copyMatlab(csvData);

}

void QEnhancedTableView::copySelectionToExcelExpanded(int copyrole, bool storeHead, bool flipped)
{
    if (!model()) return;
    if (!selectionModel()) return;
    QList<QList<QVariant> > csvData;
    QStringList colnames;
    QStringList rownames;
    getVariantDataTable(copyrole, csvData, colnames, rownames);
    //qDebug()<<csvData.size()<<colnames.size();
    dataExpand(csvData, &colnames);
    if (flipped)  {
        csvData=dataRotate(csvData);
        qSwap(colnames, rownames);
    }
    //qDebug()<<csvData.size()<<colnames.size();
    if (storeHead) QFDataExportHandler::copyCSV(csvData, colnames, rownames);
    else QFDataExportHandler::copyCSV(csvData);
}

void QEnhancedTableView::copySelectionToMatlabExpandedNoHead(int copyrole, bool flipped)
{
    if (!model()) return;
    if (!selectionModel()) return;
    QList<QList<QVariant> > csvData;
    QStringList colnames;
    QStringList rownames;
    getVariantDataTable(copyrole, csvData, colnames, rownames);
    dataExpand(csvData, &colnames);
    if (flipped)  {
        csvData=dataRotate(csvData);
        qSwap(colnames, rownames);
    }
    QFDataExportHandler::copyMatlab(csvData);
}

void QEnhancedTableView::getVariantDataTable(int copyrole, QList<QList<QVariant> > &csvData, QStringList &colnames, QStringList &rownames) const
{
    if (!model()) return;
    if (!selectionModel()) return;
    QModelIndexList sel=selectionModel()->selectedIndexes();
    QLocale loc=QLocale::system();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    if (sel.size()==1) {
        QVariant vdata=sel[0].data(copyrole);
        csvData<<QList<QVariant>();
        csvData.first()<<vdata;
        colnames<<model()->headerData(sel[0].column(), Qt::Horizontal).toString();
        rownames<<model()->headerData(sel[0].row(), Qt::Vertical).toString();
        //qDebug()<<vdata<<csvData<<csvData.first();
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
        csvData.clear();
        colnames.clear();
        rownames.clear();
        //qDebug()<<rowlist<<collist;
        //qDebug()<<rowcnt<<colcnt;
        for (int c=0; c<colcnt; c++) {
            csvData.append(QList<QVariant>());
            for (int r=0; r<rowcnt; r++) {
                csvData[c].append(QVariant());
            }
            colnames<<model()->headerData(collist[c], Qt::Horizontal).toString();
        }
        for (int r=0; r<rowcnt; r++) {
            rownames<<model()->headerData(rowlist[r], Qt::Vertical).toString();
        }
        //qDebug()<<colnames;
        //qDebug()<<rownames;
        for (int i=0; i<sel.size(); i++) {
            int r=rowlist.indexOf(sel[i].row());
            int c=collist.indexOf(sel[i].column());
            int cc=collist.indexOf(c);
            int rr=rowlist.indexOf(r);
            if (c>=0 && c<colcnt && r>=0 && r<rowcnt) {
                csvData[c].operator[](r)=sel[i].data(copyrole);
                //qDebug()<<"rc="<<r<<c<<"  rrcc="<<rr<<cc<<"  val="<<sel[i].data(copyrole);
            }
        }
    }
}

void QEnhancedTableView::copySelectionToExcel(int copyrole, bool storeHead, bool flipped) {
    if (!model()) return;
    if (!selectionModel()) return;
    QList<QList<QVariant> > csvData;
    QStringList colnames;
    QStringList rownames;
    getVariantDataTable(copyrole, csvData, colnames, rownames);

    //if (selectionModel()->selectedIndexes().size()==1) dataExpand(csvData, &colnames);
    if (csvData.size()==1) dataExpand(csvData, &colnames);
    else dataReduce(csvData, &colnames);

    if (flipped)  {
        csvData=dataRotate(csvData);
        qSwap(colnames, rownames);
    }
    //qDebug()<<csvData.size()<<colnames.size();
    if (storeHead) QFDataExportHandler::copyCSV(csvData, colnames, rownames);
    else QFDataExportHandler::copyCSV(csvData);


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
        QLocale loc=QLocale::system();
        loc.setNumberOptions(QLocale::OmitGroupSeparator);

        // header row:
        //
        //  <EMPTY> | <HOR_HEDER1> | <HOR_HEADER2> | ...
        QStringList hrow;
        if (storeHead) {
            hrow.append(""); // empty header for first column (vertical headers!)
            for (int c=0; c<colcnt; c++) {
                if (collist[c].second==valuerole) hrow.append(QString("\"%1\"").arg(model()->headerData(collist[c].first, Qt::Horizontal).toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " ")));
                else hrow.append(QString("\"error: %1\"").arg(model()->headerData(collist[c].first, Qt::Horizontal).toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " ")));
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
                if (rowlist[r].second==valuerole) row.append(QString("\"%1\"").arg(model()->headerData(rowlist[r].first, Qt::Vertical).toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " "))); // vertical header
                else row.append(QString("\"error: %1\"").arg(model()->headerData(rowlist[r].first, Qt::Vertical).toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " "))); // vertical header
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
                    txt=loc.toString(vdata.toDouble());
                    break;
                case QVariant::PointF:
                    txt=loc.toString(vdata.toPointF().x());
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
                    etxt=loc.toString(edata.toDouble());
                    break;
                case QVariant::PointF:
                    etxt=loc.toString(edata.toPointF().x());
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
        QLocale loc=QLocale::system();
        loc.setNumberOptions(QLocale::OmitGroupSeparator);

        // header row:
        //
        //  <EMPTY> | <HOR_HEDER1> | <HOR_HEADER2> | ...
        QStringList hrow;
        if (storeHead) {
            hrow.append(""); // empty header for first column (vertical headers!)
            for (int c=0; c<colcnt; c++) {
                if (collist[c].second==medianrole) hrow.append(QString("\"%1\"").arg(model()->headerData(collist[c].first, Qt::Horizontal).toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " ")));
                else if  (collist[c].second==q25role) hrow.append(QString("\"25% quantile: %1\"").arg(model()->headerData(collist[c].first, Qt::Horizontal).toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " ")));
                else if  (collist[c].second==q75role) hrow.append(QString("\"75% quantile: %1\"").arg(model()->headerData(collist[c].first, Qt::Horizontal).toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " ")));
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
                if (rowlist[r].second==medianrole) row.append(QString("\"%1\"").arg(model()->headerData(rowlist[r].first, Qt::Vertical).toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " "))); // vertical header
                else if (rowlist[r].second==q25role) row.append(QString("\"25% quantile: %1\"").arg(model()->headerData(rowlist[r].first, Qt::Vertical).toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " "))); // vertical header
                else if (rowlist[r].second==q75role) row.append(QString("\"75% quantile: %1\"").arg(model()->headerData(rowlist[r].first, Qt::Vertical).toString().replace('"', "''").replace('\n', "\\n ").replace('\r', "\\r ").replace('\t', " "))); // vertical header
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
                    txt=loc.toString(vdata.toDouble());
                    break;
                case QVariant::PointF:
                    txt=loc.toString(vdata.toPointF().x());
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
                    q25txt=loc.toString(q25data.toDouble());
                    break;
                case QVariant::PointF:
                    q25txt=loc.toString(q25data.toPointF().x());
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
                    q75txt=loc.toString(q75data.toDouble());
                    break;
                case QVariant::PointF:
                    q75txt=loc.toString(q75data.toPointF().x());
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
                html+=QString("<th style=\"white-space: nowrap;%1 \"><nobr>").arg(fsstyle);
                if (col>=0) {
                    html+=model()->headerData(col, Qt::Horizontal).toString();
                }
                html+="</nobr></th>";
            }
        } else {
            for (int col=-1; col<model()->columnCount(); col++) {
                if (col==-1) {
                    html+=QString("<th style=\"white-space: nowrap;%1 \"><nobr>").arg(fsstyle);
                    html+=model()->headerData(row, Qt::Vertical).toString();
                    html+="</nobr></th>";
                } else {
                    QModelIndex index=model()->index(row, col);
                    QVariant check=index.data(Qt::CheckStateRole);
                    QBrush back=index.data(Qt::BackgroundRole).value<QBrush>();
                    QString style=fsstyle+" white-space: nowrap; ";
                    //qDebug()<<"r="<<row<<"\tc="<<col<<"\tcolor="<<back.color().name();
                    if (back.color()!=QColor(0,0,0) && index.data(Qt::BackgroundRole).isValid()) style+=QString(" background: %1;").arg(back.color().name());
                    if (style.isEmpty()) html+=QString("<td style=\"%1 \"><nobr>").arg(fsstyle);
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

QAction *QEnhancedTableView::getActCopyExcel() const
{
    return actCopyExcel;
}

QAction *QEnhancedTableView::getActCopyExcelNoHead() const
{
    return actCopyExcelNoHead;
}

QAction *QEnhancedTableView::getActCopyMatlab() const
{
    return actCopyMatlab;
}

QAction *QEnhancedTableView::getActPrint() const
{
    return actPrint;
}

QAction *QEnhancedTableView::getActSave() const
{
    return actSave;
}

QAction *QEnhancedTableView::getActSaveExtended() const
{
    return actSaveExtended;
}

void QEnhancedTableView::addActionsToToolbar(QToolBar *tb) const
{
    tb->addAction(actCopyExcel);
    tb->addAction(actCopyExcelNoHead);
    tb->addAction(actCopyMatlab);
    tb->addSeparator();
    tb->addAction(actPrint);
}

void QEnhancedTableView::addActionsToMenu(QMenu *tb) const
{
    tb->addAction(actCopyExcel);
    tb->addAction(actCopyExcelNoHead);
    tb->addAction(actCopyMatlab);
    tb->addSeparator();
    tb->addAction(actPrint);
}



void QEnhancedTableView::keyPressEvent(QKeyEvent* event) {
    if (event->matches(QKeySequence::Copy)) {
        copySelectionToExcel(Qt::EditRole, false);
        event->accept();
    } else if (event->matches(QKeySequence::Print)) {
        print();
        event->accept();
    } else QTableView::keyPressEvent(event);
    emit keyPressed(event->key(), event->modifiers(), event->text());
}

void QEnhancedTableView::changeHTMLFontSize(QTextDocument *txt, double fontSize)
{
    txt->setHtml(toHtml(1, false, round(fontSize)));
}

/*void QEnhancedTableView::print()
{
    QTextDocument txt;
    txt.setHtml(toHtml());
    QPrinter* p=new QPrinter();
    p->setPrinterName(ProgramOptions::getConfigValue("QEnhancedTableView/lastPrinter", p->printerName()).toString());

    QPrintDialog *dialog = new QPrintDialog(p, NULL);
    dialog->setWindowTitle(tr("Print Table"));
    if (dialog->exec() != QDialog::Accepted) {
        delete p;
        return;
    }
    ProgramOptions::setConfigValue("QEnhancedTableView/lastPrinter", p->printerName());

    p->setPageMargins(10,10,10,10,QPrinter::Millimeter);


    QFTextDocumentPrintPreview* dlg=new QFTextDocumentPrintPreview(p, &txt, this, true);
    connect(dlg, SIGNAL(setDocumentFontSize(QTextDocument*,double)), this, SLOT(changeHTMLFontSize(QTextDocument*,double)));
    dlg->setWindowTitle(tr("Print Table: Preview"));
    if (dlg->exec()) {
        dlg->print();
    }
    delete dlg;
    delete p;
}
*/





void QEnhancedTableView::print()
{
    QPrinter* tablePrinter=getPrinter(NULL);

    if (tablePrinter) {
        QDialog* dlg=new QDialog(this);
        dlg->setWindowTitle(tr("Table print options ..."));
        QGridLayout* lay=new QGridLayout(dlg);
        dlg->setLayout(lay);
        lay->addWidget(new QLabel(tr("<b>scaling:</b>")), 0,0);
        QCheckBox* chk1Wide=new QCheckBox(tr("one page wide"), dlg);
        QCheckBox* chk1High=new QCheckBox(tr("one page high"), dlg);
        chk1Wide->setChecked(ProgramOptions::getConfigValue("QEnhancedTableView/onepagewide", chk1Wide->isChecked()).toBool());
        chk1High->setChecked(ProgramOptions::getConfigValue("QEnhancedTableView/onepagehigh", chk1High->isChecked()).toBool());

        lay->addWidget(chk1Wide, 0, 1);
        lay->addWidget(chk1High, 1, 1);

        QDialogButtonBox* btns=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,Qt::Horizontal,dlg);
        connect(btns, SIGNAL(accepted()), dlg, SLOT(accept()));
        connect(btns, SIGNAL(rejected()), dlg, SLOT(reject()));
        lay->addWidget(btns, 2, 0,1,2);

        if (dlg->exec()) {
            ProgramOptions::setConfigValue("QEnhancedTableView/onepagewide", chk1Wide->isChecked());
            ProgramOptions::setConfigValue("QEnhancedTableView/onepagehigh", chk1High->isChecked());
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            print(tablePrinter, chk1Wide->isChecked(), chk1High->isChecked());
            QApplication::restoreOverrideCursor();
        }
        delete dlg;
        delete tablePrinter;
    }
}

void QEnhancedTableView::save(int copyrole, bool flipped, bool storeHead)
{
    if (!model()) return;
    if (!selectionModel()) return;
    QList<QList<QVariant> > csvData;
    QStringList colnames;
    QStringList rownames;
    getVariantDataTable(copyrole, csvData, colnames, rownames);

    if (csvData.size()==1) dataExpand(csvData, &colnames);
    else dataReduce(csvData, &colnames);

    if (flipped)  {
        csvData=dataRotate(csvData);
        qSwap(colnames, rownames);
    }
    //qDebug()<<csvData.size()<<colnames.size();
    if (storeHead) QFDataExportHandler::save(csvData, colnames, rownames);
    else QFDataExportHandler::save(csvData);


}

void QEnhancedTableView::saveExtended(int copyrole, bool flipped, bool storeHead)
{
    if (!model()) return;
    if (!selectionModel()) return;
    QList<QList<QVariant> > csvData;
    QStringList colnames;
    QStringList rownames;
    getVariantDataTable(copyrole, csvData, colnames, rownames);
    //qDebug()<<csvData.size()<<colnames.size();
    dataExpand(csvData, &colnames);
    if (flipped)  {
        csvData=dataRotate(csvData);
        qSwap(colnames, rownames);
    }
    //qDebug()<<csvData.size()<<colnames.size();
    if (storeHead) QFDataExportHandler::save(csvData, colnames, rownames);
    else QFDataExportHandler::save(csvData);

}

void QEnhancedTableView::saveFlipped(int copyRole)
{
    save(copyRole, true, true);
}

void QEnhancedTableView::saveExtendedFlipped(int copyRole)
{
    saveExtended(copyRole, true, true);
}



void QEnhancedTableView::copyAsImage()
{
    clearSelection();


    /// PRINT HERE //////////////////////////////////////////////////////////////////////////////////
    // calculate the total width/height table would need without scaling
    const int rows = model()->rowCount();
    const int cols = model()->columnCount();
    double vhw=verticalHeader()->width()+8;
    double totalWidth = vhw;
    double minWidth=1e33;
    double maxWidth=0;
    for (int c = -1; c < cols; ++c)
    {
        double w=columnWidth(c);
        totalWidth += w;
        if (w<minWidth) minWidth=w;
        if (w>maxWidth) maxWidth=w;
    }
    double hhh=horizontalHeader()->height()+8;
    double totalHeight = hhh;
    double minHeight=1e33;
    double maxHeight=0;
    for (int r = 0; r < rows; ++r)
    {
       double h=rowHeight(r);
       totalHeight += h;
       if (h<minHeight) minHeight=h;
       if (h>maxHeight) maxHeight=h;
    }

    QFHTMLDelegate* htmld=dynamic_cast<QFHTMLDelegate*>(itemDelegate());
    if (htmld) {
        htmld->setPrintMode(true);
    }

    QPixmap pix(totalWidth, totalHeight);

    {
        QPainter painter(&pix);
        paint(painter);
        painter.end();
    }


    QByteArray svg;
    {
        QSvgGenerator generator;
        QBuffer buffer(&svg);
        generator.setOutputDevice(&buffer);
        QPainter painter(&generator);
        paint(painter);
        painter.end();
    }

    QClipboard *clipboard = QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setImageData(pix);
    mime->setData("image/svg+xml", svg);
    clipboard->setMimeData(mime);

    if (htmld) {
        htmld->setPrintMode(false);
    }
}



void QEnhancedTableView::print(QPrinter *printer, bool onePageWide, bool onePageHigh)
{
    QPrinter* p=printer;


     //p->setPageMargins(10,10,10,10,QPrinter::Millimeter);

     /*if (width()>height()) {
         p->setOrientation(QPrinter::Landscape);
     } else {
         p->setOrientation(QPrinter::Portrait);
     }*/

     clearSelection();


     /// PRINT HERE //////////////////////////////////////////////////////////////////////////////////
     // calculate the total width/height table would need without scaling
     const int rows = model()->rowCount();
     const int cols = model()->columnCount();
     double vhw=verticalHeader()->width()+8;
     double totalWidth = vhw;
     double minWidth=1e33;
     double maxWidth=0;
     for (int c = -1; c < cols; ++c)
     {
         double w=columnWidth(c);
         totalWidth += w;
         if (w<minWidth) minWidth=w;
         if (w>maxWidth) maxWidth=w;
     }
     double hhh=horizontalHeader()->height()+8;
     double totalHeight = hhh;
     double minHeight=1e33;
     double maxHeight=0;
     for (int r = 0; r < rows; ++r)
     {
        double h=rowHeight(r);
        totalHeight += h;
        if (h<minHeight) minHeight=h;
        if (h>maxHeight) maxHeight=h;
     }
     double scale=1.0;
     // adjust scale, so the widest/highest column fits on one page
     /*if (maxWidth*scale>p->pageRect().width()) scale=p->pageRect().width()/maxWidth;
     if (maxHeight*scale>p->pageRect().height()) scale=p->pageRect().height()/maxHeight;*/
     if (onePageWide) {
         if (totalWidth>p->pageRect().width()) scale=p->pageRect().width()/totalWidth;
     }
     if (onePageHigh) {
         if (totalHeight>p->pageRect().height()) scale=qMin(scale, p->pageRect().height()/totalHeight);
     }

     //qDebug()<<scale;

     // print scaled pixmap

     int pagesWide=1;
     int pagesHigh=1;
     QList<int> pageCols, pageRows;
     pageCols<<0;
     pageRows<<0;
     { // find number of pages needed
         double x=vhw, x0=vhw;
         if (!onePageWide) {
             for (int c=0; c<cols; c++) {
                 double cw=columnWidth(c);
                 if (x+cw>p->pageRect().width()/scale) {
                     pagesWide++;
                     x=0;
                     pageCols<<c;
                 } else {
                     x=x+cw;
                     x0=x0+cw;
                 }
             }
         }
         if (pageCols.size()>0 && cols>pageCols.last()) pageCols<<cols;
         if (pageCols.size()==1) pageCols<<cols;
         double y=hhh, y0=hhh;
         if (!onePageHigh) {
             for (int r=0; r<rows; r++) {
                 double rh=rowHeight(r);
                 if (y+rh>p->pageRect().height()/scale) {
                     pagesHigh++;
                     pageRows<<r;
                     y=hhh;
                 } else {
                     y=y+rh;
                     y0=y0+rh;
                 }
             }
         }
         if (pageRows.size()>0 && rows>pageRows.last()) pageRows<<rows;
         if (pageRows.size()==1) pageRows<<rows;
     }


     QFHTMLDelegate* htmld=dynamic_cast<QFHTMLDelegate*>(itemDelegate());
     if (htmld) {
         htmld->setPrintMode(true);
     }
     QPainter painter(p);
     paint(painter, scale, -1, hhh, vhw, pageCols, pageRows, p);
     painter.end();
     if (htmld) {
         htmld->setPrintMode(false);
     }




     /// PRINT DONE //////////////////////////////////////////////////////////////////////////////////
}

void QEnhancedTableView::paint(QPainter &painter, QRect pageRect)
{
    QFHTMLDelegate* htmld=dynamic_cast<QFHTMLDelegate*>(itemDelegate());
    bool oldP=false;
    if (htmld) {
        oldP=htmld->getPrintMode();
        htmld->setPrintMode(true);
    }

    QRect pageRec=pageRect;
    if (pageRec.width()==0 || pageRec.height()==0) pageRec=QRect(QPoint(0,0), getTotalSize().toSize());
    painter.save();
    painter.translate(pageRec.topLeft());
    QSizeF size=getTotalSize();
    double vhw=verticalHeader()->width()+8;
    double hhh=horizontalHeader()->height()+8;
    double scaleX=size.width()/double(pageRec.width());
    double scaleY=size.height()/double(pageRec.height());
    double scale=qMin(scaleX, scaleY);
    QList<int>pageCols, pageRows;
    pageCols<<0<<model()->columnCount();
    pageRows<<0<<model()->rowCount();
    paint(painter, scale, -1, hhh, vhw, pageCols, pageRows);
    painter.restore();

    if (htmld) {
        htmld->setPrintMode(oldP);
    }
}

QSizeF QEnhancedTableView::getTotalSize() const
{
    const int rows = model()->rowCount();
    const int cols = model()->columnCount();
    double vhw=verticalHeader()->width()+8;
    double totalWidth = vhw;
    for (int c = -1; c < cols; ++c)
    {
        double w=columnWidth(c);
        totalWidth += w;
    }
    double hhh=horizontalHeader()->height()+8;
    double totalHeight = hhh;
    for (int r = 0; r < rows; ++r)
    {
       double h=rowHeight(r);
       totalHeight += h;
    }

    return QSizeF((totalWidth), (totalHeight));
}


void QEnhancedTableView::paint(QPainter &painter, double scale, int page, double hhh, double vhw, const QList<int>& pageCols, const QList<int>& pageRows, QPrinter* p)
{
    painter.save();
    QStyleOptionViewItem option = viewOptions();
    painter.scale(scale, scale);
    QPen headerPen("black");
    headerPen.setWidth(2);
    QPen cellPen("gray");
    cellPen.setWidth(1);
    QFont headerFont=horizontalHeader()->font();
    headerFont.setBold(true);
    int pagesWide=pageCols.size()-1;
    int pagesHigh=pageRows.size()-1;
    //painter.translate(p->pageRect().topLeft());
    int pageCnt=0;
    for (int ph=0; ph<pageRows.size()-1; ph++) {
        for (int pw=0; pw<pageCols.size()-1; pw++) {
            if (page<0 || page==pageCnt) {
                //qDebug()<<"print page "<<ph<<"/"<<pageRows.size()<<pagesHigh<<"    "<<pw<<"/"<<pageCols.size()<<pagesWide;
                //painter.drawPicture(p->pageRect().topLeft(), pic);
                double y=0;
                if (ph==0) {
                    y=hhh;
                }
                //qDebug()<<"   rows = "<<pageRows[ph]<<"..."<<pageRows[ph+1]-1;
                //qDebug()<<"   cols = "<<pageCols[pw]<<"..."<<pageCols[pw+1]-1;
                for (int r=pageRows[ph]; r<pageRows[ph+1]; r++) {
                    double x=0;
                    int rh=rowHeight(r);
                    if (pw%pagesWide==0) {
                        x=0;
                        option.rect = QRect((int)x, (int)y, vhw, rh);
                        //verticalHeader()->itemDelegate()->paint(&painter, option, model()->index(r, c, QModelIndex()));
                        x=vhw;
                    }
                    for (int c=pageCols[pw]; c<pageCols[pw+1]; c++) {
                        double cw=columnWidth(c);
                        option.rect = QRect((int)x, (int)y, cw, rh);
                        itemDelegate()->paint(&painter, option, model()->index(r, c, QModelIndex()));
                        painter.setPen(cellPen);
                        painter.drawRect(option.rect);
                        x=x+cw;

                    }
                    y=y+rh;
                }

                if (ph==0) {
                    y=0;
                    int x=0;
                    if (pw%pagesWide==0) x=vhw;
                    painter.setPen(headerPen);
                    for (int c=pageCols[pw]; c<pageCols[pw+1]; c++) {
                        QRect rec=QRect(x, (int)y, columnWidth(c), hhh);
                        painter.fillRect(rec, QColor("lightgrey"));
                        painter.setFont(headerFont);
                        painter.setPen(headerPen);
                        painter.drawText(QRect(rec.x()+4, rec.y()+4, rec.width()-8, rec.height()-8), model()->headerData(c, Qt::Horizontal).toString());
                        painter.drawRect(rec);
                        //if (x==vhw &&) painter.drawLine(rec.topLeft(), QPoint(rec.left(), p->pageRect().height()));
                        x=x+columnWidth(c);
                    }

                }
                if (pw%pagesWide==0) {
                    y=0;
                    int x=0;
                    if (ph==0) y=hhh;
                    for (int r=pageRows[ph]; r<pageRows[ph+1]; r++) {
                        QRect rec=QRect(x, (int)y, vhw, rowHeight(r));
                        painter.fillRect(rec, QColor("lightgrey"));
                        painter.setPen(headerPen);
                        painter.setFont(headerFont);
                        painter.drawText(QRect(rec.x()+4, rec.y()+4, rec.width()-8, rec.height()-8), model()->headerData(r, Qt::Vertical).toString());
                        painter.drawRect(rec);
                        //if (x==vhw &&) painter.drawLine(rec.topLeft(), QPoint(rec.left(), p->pageRect().height()));
                        y=y+rowHeight(r);
                    }

                }


                if (p && pw<pagesWide-1) p->newPage();
            }
            pageCnt++;
        }
        if (p && ph<pagesHigh-1) p->newPage();
    }
    painter.restore();
}



QPrinter *QEnhancedTableView::getPrinter(QPrinter *printerIn, bool *localPrinter)
{
    QPrinter* p=printerIn;
    if (p==NULL) {
        p=new QPrinter();
        p->setPrinterName(ProgramOptions::getConfigValue("QEnhancedTableView/lastPrinter", p->printerName()).toString());
        if (localPrinter) *localPrinter=true;
    }

    QPrintDialog *dialog = new QPrintDialog(p, NULL);
    dialog->setWindowTitle(tr("Print Table"));
    if (dialog->exec() != QDialog::Accepted) {
        if (localPrinter && *localPrinter) delete p;
        delete dialog;
        return printerIn;
    }
    p=dialog->printer();
    if (p) ProgramOptions::setConfigValue("QEnhancedTableView/lastPrinter", p->printerName());
    delete dialog;
    return p;
}

