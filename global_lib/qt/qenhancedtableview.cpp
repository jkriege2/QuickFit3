#include "qenhancedtableview.h"

#include <QSet>
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QtGlobal>
#include <QDebug>
#include <QPrintDialog>
#include <QPainter>
#include <QAction>
#include <QHeaderView>
#include <QPicture>
#include <QStyleOptionViewItem>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include "qenhancedprintpreviewdialog.h"

QEnhancedTableView::QEnhancedTableView(QWidget* parent):
    QTableView(parent)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* act;

    act=new QAction(tr("Copy Selection to Clipboard (for Excel ...)"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(copySelectionToExcel()));
    addAction(act);
    act=new QAction(tr("Print Table"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(print()));
    addAction(act);
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
        lay->addWidget(chk1Wide, 0, 1);
        lay->addWidget(chk1High, 1, 1);

        QDialogButtonBox* btns=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,Qt::Horizontal,dlg);
        connect(btns, SIGNAL(accepted()), dlg, SLOT(accept()));
        connect(btns, SIGNAL(rejected()), dlg, SLOT(reject()));
        lay->addWidget(btns, 2, 0,1,1);

        if (dlg->exec()) {
            print(tablePrinter, chk1Wide->isChecked(), chk1High->isChecked());
        }
        delete dlg;
        delete tablePrinter;
    }
}



void QEnhancedTableView::print(QPrinter *printer, bool onePageWide, bool onePageHigh)
{
    QPrinter* p=printer;


     //p->setPageMargins(10,10,10,10,QPrinter::Millimeter);

     if (width()>height()) {
         p->setOrientation(QPrinter::Landscape);
     } else {
         p->setOrientation(QPrinter::Portrait);
     }

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

     QPainter painter(p);
     paint(painter, scale, -1, hhh, vhw, pageCols, pageRows, p);
     painter.end();




     /// PRINT DONE //////////////////////////////////////////////////////////////////////////////////
}

void QEnhancedTableView::paint(QPainter &painter, QRect pageRec)
{
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


void QEnhancedTableView::keyPressEvent(QKeyEvent* event) {
    if (event->matches(QKeySequence::Copy)) {
        copySelectionToExcel();
        event->accept();
    } else if (event->matches(QKeySequence::Print)) {
        print();
        event->accept();
    } else QTableView::keyPressEvent(event);
}

bool QEnhancedTableView::event(QEvent* event) {
    /*qDebug() << "tableView event:   ";
    switch(event->type()) {
        case QEvent::FocusIn: //qDebug() << "QEvent::FocusIn\n"; break;
        case QEvent::FocusOut: //qDebug() << "QEvent::FocusOut\n"; break;
        case QEvent::MouseButtonDblClick: //qDebug() << "QEvent::MouseButtonDblClick\n"; break;
        case QEvent::MouseButtonPress: //qDebug() << "QEvent::MouseButtonPress\n"; break;
        case QEvent::MouseButtonRelease: //qDebug() << "QEvent::MouseButtonRelease\n"; break;
        case QEvent::MouseMove: //qDebug() << "QEvent::MouseMove\n"; break;
        case QEvent::NonClientAreaMouseButtonDblClick: //qDebug() << "QEvent::NonClientAreaMouseButtonDblClick\n"; break;
        default: //qDebug() << (int)event->type() << "\n"; break;
    }*/
    return QTableView::event(event);
}

QPrinter *QEnhancedTableView::getPrinter(QPrinter *printerIn, bool *localPrinter)
{
    QPrinter* p=printerIn;
    if (p==NULL) {
        p=new QPrinter();
        if (localPrinter) *localPrinter=true;
    }

    QPrintDialog *dialog = new QPrintDialog(p, NULL);
    dialog->setWindowTitle(tr("Print Table"));
    if (dialog->exec() != QDialog::Accepted) {
        if (localPrinter && *localPrinter) delete p;
        delete dialog;
        return printerIn;
    }
    delete dialog;

    return p;
}
