#include "dlgcsvparameters.h"
#include <QtGui>
#include <QtDebug>

dlgCSVParameters::dlgCSVParameters(QWidget* parent, QString startswith, QString columnSeparator, QString commentStart, double timefactor, int startInLine):
    QDialog(parent)
{
    setupUi(this);
    edtStartswith->setText(startswith);
    QDoubleValidator* validator=new QDoubleValidator(1e-100,1e100,9,edtTimefactor);
    validator->setNotation(QDoubleValidator::ScientificNotation);
    //validator->setDecimals(9);
    edtTimefactor->setValidator(validator);
    edtTimefactor->setText(QString::number(timefactor));
    edtColumn->setText(QString(columnSeparator));
    edtComment->setText(QString(commentStart));
    spinStartInLine->setValue(startInLine);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(checkValues()));
}

dlgCSVParameters::~dlgCSVParameters()
{
    //dtor
}


void dlgCSVParameters::setFileContents(const QString& filename) {
    QString preview="";
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly/* | QIODevice::Text*/)) {
        /*QString data1=file.readAll();
        for (int i=0; i<qMin(100, data1.size()); i++) {
            int x=data1[i].toLatin1();
            qDebug()<<QString("%1 ").arg(x, 2, 16);
        }
        qDebug()<<"\n\n";*/

        file.seek(0);
        QString data=file.readAll().replace("\n\r", "\n").replace("\r\n", "\n").replace("\r", "\n").replace("\n\n", "\n").replace(QChar(0x0D), "\n").trimmed();
        /*for (int i=0; i<qMin(100, data.size()); i++) {
            int x=data[i].toLatin1();
            qDebug()<<QString("%1 ").arg(x, 2, 16);
        }
        qDebug()<<"\n\n";*/

        QTextStream in(&data);
        while (!in.atEnd()) {
            preview=preview + in.readLine() +"\n";
        }
        file.close();
    }
    //txtContents->setPlainText(preview);
    txtContents->getEditor()->setPlainText(preview);
}

void dlgCSVParameters::checkValues() {
    bool ok1=(edtColumn->text().size()>0) && (edtColumn->text()!=edtComment->text());
    bool ok3=(edtComment->text().size()>0) && (edtComment->text()!=edtColumn->text());
    if (!(ok1 && ok3)) {
        (void) QMessageBox::critical(this, tr("CSV Import Properties"), tr("The first two properties (column and comment separator) may not be set to the same character! Please correct the indicated fields."), QMessageBox::Ok);
        if (!ok1) {
            QPalette p=edtColumn->palette();
            p.setColor(QPalette::Background, QColor("salmon"));
            edtColumn->setPalette(p);
        } else edtColumn->setPalette(QPalette());
        if (!ok3) {
            QPalette p=edtComment->palette();
            p.setColor(QPalette::Background, QColor("salmon"));
            edtComment->setPalette(p);
        } else edtComment->setPalette(QPalette());
    } else {
        QString s=edtColumn->text();
        column_separator=(s.size()>0)?s[0].toAscii():',';
        s=edtComment->text();
        comment_start=(s.size()>0)?s[0].toAscii():'#';
        startswith=edtStartswith->text();
        timefactor=edtTimefactor->text().toDouble();
        firstline=spinStartInLine->value();

        accept();
    }
}
