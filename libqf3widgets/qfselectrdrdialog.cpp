/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qfselectrdrdialog.h"
#include "ui_qfselectrdrdialog.h"
#include "qfpluginservices.h"
#include <QDebug>
#include "qfcompleterfromfile.h"
#include "qfstyledbutton.h"

QFSelectRDRDialog::QFSelectRDRDialog(QFMatchRDRFunctor *matchFunctor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFSelectRDRDialog)
{
    ui->setupUi(this);
    ui->lineEditFilter->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, ui->lineEditFilter, ui->lineEditFilter));
    ui->lineEditFilter->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter, ui->lineEditFilter, ui->lineEditFilter));
    QFCompleterFromFile* cc=new QFCompleterFromFile(ui->lineEditFilter);
    cc->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfselectrdrdialog_filters.txt");
    ui->lineEditFilter->setCompleter(cc);

    this->matchFunctor=matchFunctor;
    this->project=project;
    functorPrivate=false;
    ui->btnHelp->setVisible(false);
    setAllowCreateNew(true);
    setAllowMultiSelect(false);
    setDescription("");
    setProject(NULL);
    setOnlineHelp(QFPluginServices::getInstance()->getMainHelpDirectory()+"qfselectrdrdialog.html");
}

QFSelectRDRDialog::QFSelectRDRDialog(QFMatchRDRFunctor *matchFunctor, bool functorPrivate, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFSelectRDRDialog)
{
    ui->setupUi(this);
    ui->lineEditFilter->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, ui->lineEditFilter, ui->lineEditFilter));
    ui->lineEditFilter->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter, ui->lineEditFilter, ui->lineEditFilter));
    QFCompleterFromFile* cc=new QFCompleterFromFile(ui->lineEditFilter);
    cc->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfselectrdrdialog_filters.txt");
    ui->lineEditFilter->setCompleter(cc);

    this->matchFunctor=matchFunctor;
    this->project=project;
    this->functorPrivate=functorPrivate;
    ui->btnHelp->setVisible(false);
    setAllowCreateNew(true);
    setAllowMultiSelect(false);
    setDescription("");
    setProject(NULL);
    setOnlineHelp(QFPluginServices::getInstance()->getMainHelpDirectory()+"qfselectrdrdialog.html");
}

QFSelectRDRDialog::QFSelectRDRDialog(QWidget *parent):
    QDialog(parent),
    ui(new Ui::QFSelectRDRDialog)
{
    ui->setupUi(this);
    ui->lineEditFilter->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, ui->lineEditFilter, ui->lineEditFilter));
    ui->lineEditFilter->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter, ui->lineEditFilter, ui->lineEditFilter));
    QFCompleterFromFile* cc=new QFCompleterFromFile(ui->lineEditFilter);
    cc->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfselectrdrdialog_filters.txt");
    ui->lineEditFilter->setCompleter(cc);

    this->matchFunctor=new QFMatchRDRFunctorSelectAll();
    functorPrivate=true;
    this->project=project;
    ui->btnHelp->setVisible(false);
    setAllowCreateNew(true);
    setAllowMultiSelect(false);
    setDescription("");
    setProject(NULL);
    setOnlineHelp(QFPluginServices::getInstance()->getMainHelpDirectory()+"qfselectrdrdialog.html");
}

QFSelectRDRDialog::~QFSelectRDRDialog()
{
    delete ui;
    if (functorPrivate && matchFunctor) delete matchFunctor;
    matchFunctor=NULL;
}

void QFSelectRDRDialog::addWidget(const QString &label, QWidget *widget)
{
    ui->formLayout->addRow(label, widget);
}

void QFSelectRDRDialog::addWidget(QLabel *label, QWidget *widget)
{
    ui->formLayout->addRow(label, widget);
}

void QFSelectRDRDialog::setAllowCreateNew(bool allowNew)
{
    if (allowNew) {
        ui->radNew->setEnabled(true);
        ui->radSelect->setEnabled(true);
    } else {
        ui->radNew->setEnabled(false);
        ui->radSelect->setEnabled(false);
        ui->radSelect->setChecked(false);
        ui->radSelect->setChecked(true);
    }
    ui->horizontalWidget->setEnabled(ui->radSelect->isChecked() || (!ui->radSelect->isEnabled() && !ui->radNew->isEnabled()));
}

void QFSelectRDRDialog::setAllowMultiSelect(bool allowMulti)
{
    if (allowMulti) {
        ui->listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    } else {
        ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    }
}

void QFSelectRDRDialog::selectRDR(QFRawDataRecord *rdr)
{
    int idx=-1;
    for (int i=0; i<rdrList.size(); i++) {
        if (rdrList[i]==rdr) {
            idx=i;
            break;
        }
    }
    if (idx>=0) {
        ui->listWidget->setCurrentRow(idx);
    }
}

void QFSelectRDRDialog::setDescription(const QString &text)
{
    ui->labDescription->setText(text);
    ui->labDescription->setVisible(!text.isEmpty());
}

void QFSelectRDRDialog::setProject(QFProject *project)
{
    this->project=project;
    updateRdrList();
    ui->listWidget->selectAll();
}

QList<QPointer<QFRawDataRecord> > QFSelectRDRDialog::getAllRDRs() const
{
    return rdrList;
}

QList<QPointer<QFRawDataRecord> > QFSelectRDRDialog::getSelectedRDRs() const
{
    QList<QPointer<QFRawDataRecord> > list;
    QList<QListWidgetItem *> l=ui->listWidget->selectedItems();
    for (int i=0; i<l.size(); i++) {
        QFRawDataRecord* r=rdrList.value(l[i]->data(Qt::UserRole).toInt(), NULL);
        if (r) list<<r;
    }
    return list;
}

QList<QFRawDataRecord* > QFSelectRDRDialog::getSelectedRDRsp() const
{
    QList<QFRawDataRecord* > list;
    QList<QListWidgetItem *> l=ui->listWidget->selectedItems();
    for (int i=0; i<l.size(); i++) {
        QFRawDataRecord* r=rdrList.value(l[i]->data(Qt::UserRole).toInt(), NULL);
        if (r) list<<r;
    }
    return list;
}

QPointer<QFRawDataRecord> QFSelectRDRDialog::getSelectedRDR() const
{
    return rdrList.value(ui->listWidget->currentItem()->data(Qt::UserRole).toInt(), NULL);
}

bool QFSelectRDRDialog::doCreateNew() const
{
    return ui->radNew->isChecked();
}

void QFSelectRDRDialog::setOnlineHelp(const QString &helpfile)
{
    this->helpfile=helpfile;
    ui->btnHelp->setVisible(!helpfile.isEmpty());
}

void QFSelectRDRDialog::updateRdrList()
{
    QFProject* project=this->project;
    if (!project) project=QFPluginServices::getInstance()->getCurrentProject();

    if (matchFunctor) rdrList=matchFunctor->getFilteredList(project);
    else rdrList.clear();

    QString sel="";
    if (ui->listWidget->currentItem()) ui->listWidget->currentItem()->text();
    int selI=0;
    ui->listWidget->clear();
    QRegExp rx(ui->lineEditFilter->text(), Qt::CaseInsensitive, QRegExp::Wildcard);
    for (int i=0; i<rdrList.size(); i++) {
        if (rdrList[i]) {
            if (rx.pattern().isEmpty() || rx.indexIn(rdrList[i]->getName())>=0) {
                QListWidgetItem* item=new QListWidgetItem(rdrList[i]->getSmallIcon(), rdrList[i]->getName(), ui->listWidget);
                item->setData(Qt::UserRole, i);
                ui->listWidget->addItem(item);
                if (item->text()==sel) selI=ui->listWidget->count()-1;
            }
        }
    }
    ui->listWidget->setCurrentRow(selI);

    if (ui->listWidget->count()<=0 && ui->radNew->isEnabled()) {
        ui->radNew->setChecked(true);
        ui->radSelect->setEnabled(false);
    }



}

void QFSelectRDRDialog::selectAll()
{
    on_btnAll_clicked();
}

void QFSelectRDRDialog::selectNone()
{
    on_btnNone_clicked();
}

void QFSelectRDRDialog::on_btnAll_clicked()
{
    ui->listWidget->selectAll();
}

void QFSelectRDRDialog::on_btnNone_clicked()
{
    ui->listWidget->clearSelection();
}

void QFSelectRDRDialog::on_lineEditFilter_textChanged(const QString &text)
{
    updateRdrList();
    ui->listWidget->selectAll();
}

void QFSelectRDRDialog::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(helpfile);
}

