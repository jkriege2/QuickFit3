/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfdlgcsvparameters.h"
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "ui_qfdlg_csvparameters.h"
#include "programoptions.h"
#include "qftablemodel.h"
#include "csvtools.h"

QFDlgCSVParameters::QFDlgCSVParameters(QWidget* parent, QString columnSeparator, QString decimalSeparator, QString commentStart, QString headerStart):
    QDialog(parent), ui(new Ui::QFDlgCSVParameters)
{
    colselcombos=0;
    tabmodel=new QFTableModel(this);
    ui->setupUi(this);
    ui->widConfig->registerWidget("column_separator", ui->edtColumn);
    ui->widConfig->registerWidget("comment_start", ui->edtComment);
    ui->widConfig->registerWidget("decimal_separator", ui->edtDecimal);
    ui->widConfig->registerWidget("header_start", ui->edtHeader);
    ui->widConfig->setFilename(QFPluginServices::getInstance()->getConfigFileDirectory()+QString("/csvdlg_csvconfig.ini"));
    ui->edtColumn->setText(QString(columnSeparator));
    //if (QString(columnSeparator).isEmpty()) ui->edtColumn->setEnabled(false);
    ui->edtDecimal->setText(QString(decimalSeparator));
    //if (QString(decimalSeparator).isEmpty()) ui->edtDecimal->setEnabled(false);
    ui->edtComment->setText(QString(commentStart));
    //if (QString(commentStart).isEmpty()) ui->edtComment->setEnabled(false);
    ui->edtHeader->setText(headerStart);
    //if (headerStart.isEmpty()) ui->edtHeader->setEnabled(false);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(checkValues()));
    ui->tableView->setModel(tabmodel);

    ProgramOptions::getConfigWindowGeometry(this, "QFDlgCSVParameters/window/");
    ProgramOptions::getConfigQSplitter(ui->splitter, "QFDlgCSVParameters/splitter/");
    ui->widConfig->setCurrentConfig(ProgramOptions::getConfigValue("QFDlgCSVParameters/config/", "").toString());
    setShowColumnsSelection(false);
    ui->lstColumns->setModel(&colslistCheckable);
    guessParameters();
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

QFDlgCSVParameters::~QFDlgCSVParameters()
{
    ProgramOptions::setConfigWindowGeometry(this, "QFDlgCSVParameters/window/");
    ProgramOptions::setConfigQSplitter(ui->splitter, "QFDlgCSVParameters/splitter/");
    ProgramOptions::setConfigValue("QFDlgCSVParameters/config/", ui->widConfig->currentConfigName());
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
    file_contents=preview;
    this->filename=filename;
    ui->txtContents->setPlainText(preview);

    guessParameters();

    reloadCSV();

    for (int i=0; i<colCmb.size(); i++) {
        colCmb[i]->setCurrentIndex(i);
    }
}

void QFDlgCSVParameters::setShowColumnsSelection(bool en)
{
    show_sel_columns=en;
    ui->lstColumns->setVisible(en);
    ui->labColumns->setVisible(en);
}

void QFDlgCSVParameters::addColSelComboBox(const QString &name)
{
    colselcombos++;
    colCmbNames<<name;
    QComboBox* cmb=new QComboBox(this);
    cmb->setModel(&colslist);
    ui->mainFormLayout->addRow(name, cmb);
    colCmb<<cmb;
}

int QFDlgCSVParameters::getColSelComboBoxCurrentIndex(int i)
{
    if (i>=0 && i<colCmb.size()) {
        return colCmb[i]->currentIndex();
    }
    return -1;
}

void QFDlgCSVParameters::checkValues(bool doAccept) {
    bool ok1=(!ui->edtColumn->isEnabled()) || ( (ui->edtColumn->text().size()>0) && (ui->edtColumn->text()!=ui->edtComment->text()) && (ui->edtColumn->text()!=ui->edtDecimal->text()) );
    bool ok2=(!ui->edtDecimal->isEnabled()) || ( (ui->edtDecimal->text().size()>0) && (ui->edtDecimal->text()!=ui->edtColumn->text()) && (ui->edtDecimal->text()!=ui->edtComment->text()) );
    bool ok3=(!ui->edtComment->isEnabled()) || ( (ui->edtComment->text().size()>0) && (ui->edtComment->text()!=ui->edtColumn->text()) && (ui->edtComment->text()!=ui->edtDecimal->text()) );
    bool ok4=(!ui->edtHeader->isEnabled()) || ( ui->edtHeader->text().size()>0 );
    if (!(ok1 && ok2 && ok3 && ok4)) {
        (void) QMessageBox::critical(this, tr("CSV Import Properties"), tr("The first three properties (column, decimal and comment separator) may not be set to the same character! Please correct the indicated fields."), QMessageBox::Ok);
        if (!ok1) {
            QPalette p=ui->edtColumn->palette();
            p.setColor(QPalette::Background, QColor("salmon"));
            ui->edtColumn->setPalette(p);
        } else ui->edtColumn->setPalette(QPalette());
        if (!ok2) {
            QPalette p=ui->edtComment->palette();
            p.setColor(QPalette::Background, QColor("salmon"));
            ui->edtDecimal->setPalette(p);
        } else ui->edtDecimal->setPalette(QPalette());
        if (!ok3) {
            QPalette p=ui->edtHeader->palette();
            p.setColor(QPalette::Background, QColor("salmon"));
            ui->edtComment->setPalette(p);
        } else ui->edtComment->setPalette(QPalette());
        if (!ok4) {
            QPalette p=ui->edtHeader->palette();
            p.setColor(QPalette::Background, QColor("salmon"));
            ui->edtHeader->setPalette(p);
        } else ui->edtHeader->setPalette(QPalette());
    } else {
        QString s=ui->edtColumn->text();
        column_separator=(s.size()>0)?s[0].toLatin1():',';
        s=ui->edtDecimal->text();
        decimal_separator=(s.size()>0)?s[0].toLatin1():'.';
        s=ui->edtComment->text();
        comment_start=(s.size()>0)?s[0].toLatin1():'#';
        header_start=ui->edtHeader->text();

        if (doAccept) accept();
    }
}

void QFDlgCSVParameters::on_btnTab_clicked()
{
    ui->edtColumn->setText("\n");
}

void QFDlgCSVParameters::reloadCSV()
{
    checkValues(false);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    tabmodel->setReadonly(false);
    tabmodel->clear();
    bool ex=false;
    if ((ex=QFile::exists(filename))) {
        tabmodel->readCSV(filename, get_column_separator(), get_decimal_separator(), get_header_start(), get_comment_start());
    }
    ui->tableView->setVisible(ex);
    ui->lstColumns->setModel(NULL);
    QList<int> oldidx;
    for (int i=0; i<colCmb.size(); i++) {
        oldidx<<colCmb[i]->currentIndex();
        colCmb[i]->setModel(NULL);
    }
    colslistCheckable.setEditable(true);
    colslistCheckable.clear();
    colslist.setEditable(true);
    colslist.clear();
    for (int i=0; i<tabmodel->columnCount(); i++) {
        colslistCheckable.addItem(tabmodel->columnTitle(i), true);
        colslist.addItem(tabmodel->columnTitle(i));
    }
    colslist.setEditable(false);
    colslistCheckable.setEditable(false);
    for (int i=0; i<colCmb.size(); i++) {
        colCmb[i]->setModel(&colslist);
        colCmb[i]->setCurrentIndex(oldidx.value(i, i));
    }
    ui->lstColumns->setModel(&colslistCheckable);
    tabmodel->setReadonly(true);
    QApplication::restoreOverrideCursor();
}

void QFDlgCSVParameters::guessParameters()
{
    char sep, dec, comment;
    QString headercomment;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (guessCSVParameters(file_contents, &sep, &dec, &comment, &headercomment)) {

        disconnect(ui->edtColumn, SIGNAL(textChanged(QString)), this, SLOT(reloadCSV()));
        disconnect(ui->edtDecimal, SIGNAL(textChanged(QString)), this, SLOT(reloadCSV()));
        disconnect(ui->edtComment, SIGNAL(textChanged(QString)), this, SLOT(reloadCSV()));
        disconnect(ui->edtHeader, SIGNAL(textChanged(QString)), this, SLOT(reloadCSV()));

        ui->edtColumn->setText(QString(QChar(sep)));
        ui->edtComment->setText(QString(QChar(comment)));
        ui->edtDecimal->setText(QString(QChar(dec)));
        ui->edtHeader->setText(headercomment);

        connect(ui->edtColumn, SIGNAL(textChanged(QString)), this, SLOT(reloadCSV()));
        connect(ui->edtDecimal, SIGNAL(textChanged(QString)), this, SLOT(reloadCSV()));
        connect(ui->edtComment, SIGNAL(textChanged(QString)), this, SLOT(reloadCSV()));
        connect(ui->edtHeader, SIGNAL(textChanged(QString)), this, SLOT(reloadCSV()));

    }
    reloadCSV();
    QApplication::restoreOverrideCursor();
}
