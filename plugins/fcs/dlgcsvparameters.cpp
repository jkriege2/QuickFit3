#include "dlgcsvparameters.h"

dlgCSVParameters::dlgCSVParameters(QWidget* parent, QString columnSeparator, QString decimalSeparator, QString commentStart, QString headerStart):
    QDialog(parent)
{
    setupUi(this);
    edtColumn->setText(QString(columnSeparator));
    edtDecimal->setText(QString(decimalSeparator));
    edtComment->setText(QString(commentStart));
    edtHeader->setText(headerStart);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(checkValues()));
}

dlgCSVParameters::~dlgCSVParameters()
{
    //dtor
}

void dlgCSVParameters::checkValues() {
    bool ok1=(edtColumn->text().size()>0) && (edtColumn->text()!=edtComment->text()) && (edtColumn->text()!=edtDecimal->text());
    bool ok2=(edtDecimal->text().size()>0) && (edtDecimal->text()!=edtColumn->text()) && (edtDecimal->text()!=edtComment->text());
    bool ok3=(edtComment->text().size()>0) && (edtComment->text()!=edtColumn->text()) && (edtComment->text()!=edtDecimal->text());
    bool ok4=edtHeader->text().size()>0;
    if (!(ok1 && ok2 && ok3 && ok4)) {
        (void) QMessageBox::critical(this, tr("CSV Import Properties"), tr("The first three properties (column, decimal and comment separator) may not be set to the same character! Please correct the indicated fields."), QMessageBox::Ok);
        if (!ok1) {
            QPalette p=edtColumn->palette();
            p.setColor(QPalette::Background, QColor("salmon"));
            edtColumn->setPalette(p);
        } else edtColumn->setPalette(QPalette());
        if (!ok2) {
            QPalette p=edtComment->palette();
            p.setColor(QPalette::Background, QColor("salmon"));
            edtDecimal->setPalette(p);
        } else edtDecimal->setPalette(QPalette());
        if (!ok3) {
            QPalette p=edtHeader->palette();
            p.setColor(QPalette::Background, QColor("salmon"));
            edtComment->setPalette(p);
        } else edtComment->setPalette(QPalette());
        if (!ok4) {
            QPalette p=edtHeader->palette();
            p.setColor(QPalette::Background, QColor("salmon"));
            edtHeader->setPalette(p);
        } else edtHeader->setPalette(QPalette());
    } else {
        QString s=edtColumn->text();
        column_separator=(s.size()>0)?s[0].toAscii():',';
        s=edtDecimal->text();
        decimal_separator=(s.size()>0)?s[0].toAscii():'.';
        s=edtComment->text();
        comment_start=(s.size()>0)?s[0].toAscii():'#';
        header_start=edtHeader->text();

        accept();
    }
}
