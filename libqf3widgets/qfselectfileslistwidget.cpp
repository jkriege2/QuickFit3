/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "qfselectfileslistwidget.h"
#include "ui_qfselectfileslistwidget.h"
#include "programoptions.h"
#include "qftools.h"

QFSelectFilesListWidget::QFSelectFilesListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFSelectFilesListWidget)
{
    onlyOneFormat=false;
    ui->setupUi(this);
    filters<<tr("All files (*.*)");
    lastDirID="QFSelectFilesListWidget/lastDir";
    lastFilterID="QFSelectFilesListWidget/lastFilter";
}

QFSelectFilesListWidget::~QFSelectFilesListWidget()
{
    delete ui;
}

void QFSelectFilesListWidget::setFilters(const QStringList &filters)
{
    this->filters=filters;
    this->filterids=filters;
}

void QFSelectFilesListWidget::setFilters(const QStringList &filters, const QStringList &filterids)
{
    this->filterids=filterids;

    this->filters=filters;
}

void QFSelectFilesListWidget::setSettingsIDs(const QString &lastDirID, const QString &lastFilterID)
{
    this->lastDirID=lastDirID;
    this->lastFilterID=lastFilterID;
}

int QFSelectFilesListWidget::filesCount() const
{
    return ui->files->count();
}

QStringList QFSelectFilesListWidget::files() const
{
    QStringList sl;
    for (int i=0; i<ui->files->count(); i++) {
        sl<<ui->files->item(i)->data(Qt::UserRole+1).toString();
    }
    return sl;
}

QStringList QFSelectFilesListWidget::fileFilters() const
{
    QStringList sl;
    for (int i=0; i<ui->files->count(); i++) {
        sl<<ui->files->item(i)->data(Qt::UserRole+2).toString();
    }
    return sl;
}

QStringList QFSelectFilesListWidget::fileFilterIDs() const
{
    QStringList sl;
    for (int i=0; i<ui->files->count(); i++) {
        int idx=filters.indexOf(ui->files->item(i)->data(Qt::UserRole+2).toString());
        sl<<filterids.value(idx, filters.value(idx, ""));
    }
    return sl;
}

QList<int> QFSelectFilesListWidget::fileFilterNums() const
{
    QList<int> sl;
    for (int i=0; i<ui->files->count(); i++) {
        sl<<filters.indexOf(ui->files->item(i)->data(Qt::UserRole+2).toString());
    }
    return sl;
}

void QFSelectFilesListWidget::setOnlyOneFormatAllowed(bool allowed)
{
    onlyOneFormat=allowed;
}

void QFSelectFilesListWidget::addFiles()
{
    on_btnAdd_clicked();
}

void QFSelectFilesListWidget::on_btnAdd_clicked()
{
    QString currentFCSFileFormatFilter=ProgramOptions::getConfigValue(lastFilterID, filters.value(0)).toString();
    QString dir=ProgramOptions::getConfigValue(lastDirID, "").toString();
    QStringList filterss=filters;
    if (onlyOneFormat && ui->files->count()>1 ) {
        filterss.clear();
        filterss<<currentFCSFileFormatFilter;
    }
    QStringList files = qfGetOpenFileNames(this,
                          tr("Select File(s) ..."),
                          dir,
                          filterss.join(";;"), &currentFCSFileFormatFilter);
    if (files.size()>0) {
        ProgramOptions::setConfigValue(lastFilterID, currentFCSFileFormatFilter);
        ProgramOptions::setConfigValue(lastDirID, QFileInfo(files[0]).absolutePath());
        for (int i=0; i<files.size(); i++) {
            QListWidgetItem* itm= new QListWidgetItem();
            itm->setText(QFileInfo(files[i]).fileName());
            itm->setData(Qt::UserRole+1, QFileInfo(files[i]).absoluteFilePath());
            itm->setData(Qt::UserRole+2, currentFCSFileFormatFilter);
            ui->files->addItem(itm);
        }
    }
}

void QFSelectFilesListWidget::on_btnRemove_clicked()
{
    QList<QListWidgetItem*> sels=ui->files->selectedItems();
    for (int i=0; i<sels.size(); i++) {
        ui->files->removeItemWidget(sels[i]);
        delete sels[i];
    }
}

void QFSelectFilesListWidget::on_btnUp_clicked()
{
    QList<QListWidgetItem*> sels=ui->files->selectedItems();
    for (int i=0; i<sels.size(); i++) {
        int currentIndex=ui->files->row(sels[i]);
        QListWidgetItem *currentItem = ui->files->takeItem(currentIndex);
        ui->files->insertItem(currentIndex-1, currentItem);
        ui->files->setCurrentRow(currentIndex-1);
    }
}

void QFSelectFilesListWidget::on_btnDown_clicked()
{
    QList<QListWidgetItem*> sels=ui->files->selectedItems();
    for (int i=0; i<sels.size(); i++) {
        int currentIndex=ui->files->row(sels[i]);
        QListWidgetItem *currentItem = ui->files->takeItem(currentIndex);
        ui->files->insertItem(currentIndex+1, currentItem);
        ui->files->setCurrentRow(currentIndex+1);
    }
}


QFSelectFilesWizardPage::QFSelectFilesWizardPage(const QString &title, QWidget *parent):
    QFWizardPage(title, parent)
{
    m_addStartup=false;
    m_needsfiles=true;
    sel = new QFSelectFilesListWidget(this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(sel);
    formLay=new QFormLayout();
    layout->addLayout(formLay);
    setLayout(layout);
}

void QFSelectFilesWizardPage::addRow(const QString &label, QWidget *widget)
{
    formLay->addRow(label, widget);
}

void QFSelectFilesWizardPage::addRow(const QString &label, QLayout *layout)
{
    formLay->addRow(label, layout);
}

void QFSelectFilesWizardPage::setFilters(const QStringList &filters)
{
    sel->setFilters(filters);
}

void QFSelectFilesWizardPage::setFilters(const QStringList &filters, const QStringList &filterids)
{
    sel->setFilters(filters, filterids);
}

void QFSelectFilesWizardPage::setSettingsIDs(const QString &lastDirID, const QString &lastFilterID)
{
    sel->setSettingsIDs(lastDirID, lastFilterID);
}

int QFSelectFilesWizardPage::filesCount() const
{
    return sel->filesCount();
}

QStringList QFSelectFilesWizardPage::files() const
{
    return sel->files();
}

QStringList QFSelectFilesWizardPage::fileFilters() const
{
    return sel->fileFilters();
}

QStringList QFSelectFilesWizardPage::fileFilterIDs() const
{
    return sel->fileFilterIDs();
}

QList<int> QFSelectFilesWizardPage::fileFilterNums() const
{
    return sel->fileFilterNums();
}

void QFSelectFilesWizardPage::setAddOnStartup(bool add)
{
    m_addStartup=add;
}

void QFSelectFilesWizardPage::setFilesRequired(bool add)
{
    m_needsfiles=add;
}

void QFSelectFilesWizardPage::setOnlyOneFormatAllowed(bool allowed)
{
    sel->setOnlyOneFormatAllowed(allowed);
}

void QFSelectFilesWizardPage::initializePage()
{
    QFWizardPage::initializePage();
    if (m_addStartup) sel->addFiles();
}

bool QFSelectFilesWizardPage::validatePage()
{
    if (m_needsfiles && sel->filesCount()<=0) {
        QMessageBox::critical(this, title(), tr("You have to add files to the list!"), QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }
    return QFWizardPage::validatePage();
}
