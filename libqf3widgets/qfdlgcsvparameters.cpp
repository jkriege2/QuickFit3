#include "qfdlgcsvparameters.h"
#include <QtGui>


QFDlgCSVParameters::QFDlgCSVParameters(QWidget* parent, QString columnSeparator, QString decimalSeparator, QString commentStart, QString headerStart):
    QDialog(parent)
{
    setupUi(this);
    edtColumn->setText(QString(columnSeparator));
    if (QString(columnSeparator).isEmpty()) edtColumn->setEnabled(false);
    edtDecimal->setText(QString(decimalSeparator));
    if (QString(decimalSeparator).isEmpty()) edtDecimal->setEnabled(false);
    edtComment->setText(QString(commentStart));
    if (QString(commentStart).isEmpty()) edtComment->setEnabled(false);
    edtHeader->setText(headerStart);
    if (headerStart.isEmpty()) edtHeader->setEnabled(false);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(checkValues()));
}

QFDlgCSVParameters::~QFDlgCSVParameters()
{
    //dtor
}


void QFDlgCSVParameters::setFileContents(const QString& filename) {
    QString preview="";
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            preview=preview + in.readLine() +"\n";
        }
        file.close();
    }
    txtContents->setPlainText(preview);
}

void QFDlgCSVParameters::checkValues() {
    bool ok1=(!edtColumn->isEnabled()) || ( (edtColumn->text().size()>0) && (edtColumn->text()!=edtComment->text()) && (edtColumn->text()!=edtDecimal->text()) );
    bool ok2=(!edtDecimal->isEnabled()) || ( (edtDecimal->text().size()>0) && (edtDecimal->text()!=edtColumn->text()) && (edtDecimal->text()!=edtComment->text()) );
    bool ok3=(!edtComment->isEnabled()) || ( (edtComment->text().size()>0) && (edtComment->text()!=edtColumn->text()) && (edtComment->text()!=edtDecimal->text()) );
    bool ok4=(!edtHeader->isEnabled()) || ( edtHeader->text().size()>0 );
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
