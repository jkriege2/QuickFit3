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

QString QFSelectFilesListWidget::getLastDirectory() const
{
    return ProgramOptions::getConfigValue(lastDirID, "").toString();
}


QString QFSelectFilesListWidget::getLastFilter() const
{
    return ProgramOptions::getConfigValue(lastFilterID, filters.value(0)).toString();
}

QStringList QFSelectFilesListWidget::getLastFilters() const
{
    QStringList filterss=filters;
    if (onlyOneFormat && ui->files->count()>1 ) {
        filterss.clear();
        filterss<<getLastFilter();
    }
    return filterss;
}

QString QFSelectFilesListWidget::showAdditionalFileDialog(QString *filter)
{
    QString currentFCSFileFormatFilter=ProgramOptions::getConfigValue(lastFilterID, filters.value(0)).toString();
    QString dir=ProgramOptions::getConfigValue(lastDirID, "").toString();
    QStringList filterss=filters;
    if (onlyOneFormat && ui->files->count()>1 ) {
        filterss.clear();
        filterss<<currentFCSFileFormatFilter;
    }
    QString files = qfGetOpenFileName(this,
                          tr("Select File ..."),
                          dir,
                          filterss.join(";;"), &currentFCSFileFormatFilter);
    if (files.size()>0) {
        ProgramOptions::setConfigValue(lastFilterID, currentFCSFileFormatFilter);
        ProgramOptions::setConfigValue(lastDirID, QFileInfo(files).absolutePath());
        if (filter) *filter=currentFCSFileFormatFilter;

    }
    return files;
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


QFSelectFilesListWizardPage::QFSelectFilesListWizardPage(const QString &title, QWidget *parent):
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

void QFSelectFilesListWizardPage::addRow(const QString &label, QWidget *widget)
{
    formLay->addRow(label, widget);
}

void QFSelectFilesListWizardPage::addRow(const QString &label, QLayout *layout)
{
    formLay->addRow(label, layout);
}

void QFSelectFilesListWizardPage::setFilters(const QStringList &filters)
{
    sel->setFilters(filters);
}

void QFSelectFilesListWizardPage::setFilters(const QStringList &filters, const QStringList &filterids)
{
    sel->setFilters(filters, filterids);
}

void QFSelectFilesListWizardPage::setSettingsIDs(const QString &lastDirID, const QString &lastFilterID)
{
    sel->setSettingsIDs(lastDirID, lastFilterID);
}

int QFSelectFilesListWizardPage::filesCount() const
{
    return sel->filesCount();
}

QStringList QFSelectFilesListWizardPage::files() const
{
    return sel->files();
}

QStringList QFSelectFilesListWizardPage::fileFilters() const
{
    return sel->fileFilters();
}

QStringList QFSelectFilesListWizardPage::fileFilterIDs() const
{
    return sel->fileFilterIDs();
}

QList<int> QFSelectFilesListWizardPage::fileFilterNums() const
{
    return sel->fileFilterNums();
}

void QFSelectFilesListWizardPage::setAddOnStartup(bool add)
{
    m_addStartup=add;
}

void QFSelectFilesListWizardPage::setFilesRequired(bool add)
{
    m_needsfiles=add;
}

void QFSelectFilesListWizardPage::setOnlyOneFormatAllowed(bool allowed)
{
    sel->setOnlyOneFormatAllowed(allowed);
}

void QFSelectFilesListWizardPage::initializePage()
{
    QFWizardPage::initializePage();
    if (m_addStartup) sel->addFiles();
}

bool QFSelectFilesListWizardPage::validatePage()
{
    if (m_needsfiles && sel->filesCount()<=0) {
        QMessageBox::critical(this, title(), tr("You have to add files to the list!"), QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }
    return QFWizardPage::validatePage();
}


QFSelectFilesWizardPage::QFSelectFilesWizardPage(const QString &title, QWidget *parent):
    QFWizardPage(title, parent)
{
    formLay=new QFormLayout();
    setLayout(formLay);
    onlyOneFormat=true;
}

void QFSelectFilesWizardPage::addRow(const QString &label, QWidget *widget)
{
    formLay->addRow(label, widget);
}

void QFSelectFilesWizardPage::addRow(const QString &label, QLayout *layout)
{
    formLay->addRow(label, layout);
}

void QFSelectFilesWizardPage::addFileSelection(const QString &label, const QStringList &filters, bool required)
{
   addFileSelection(label, filters, filters, required);
}

void QFSelectFilesWizardPage::addFileSelection(const QString &label, const QStringList &filters, const QStringList &filterids, bool required)
{
    QFSelectFilesWizardPage::FileData fd;
    fd.label=new QLabel(label, this);
    fd.edit=new QEnhancedLineEdit(this);
    fd.edit->addButton(fd.button=new QFStyledButton(QFStyledButton::None, fd.edit, fd.edit));
    fd.button->setDefaultIcon(QFStyledButton::SelectFile);
    connect(fd.button, SIGNAL(clickedWithButton(QFStyledButton*)), this, SLOT(buttonClicked(QFStyledButton*)));
    formLay->addRow(fd.label, fd.edit);
    fd.filters=filters;
    fd.filterids=filterids;
    fd.required=required;
    m_files.append(fd);
}

void QFSelectFilesWizardPage::setSettingsIDs(const QString &lastDirID, const QString &lastFilterID)
{
    this->lastDirID=lastDirID;
    this->lastFilterID=lastFilterID;
}

int QFSelectFilesWizardPage::filesCount() const
{
    return m_files.size();
}

QStringList QFSelectFilesWizardPage::files() const
{
    QStringList sl;
    for (int i=0; i<m_files.size();i++) {
        sl<<m_files[i].edit->text();
    }
    return sl;
}

QStringList QFSelectFilesWizardPage::fileFilters() const
{
    QStringList sl;
    for (int i=0; i<m_files.size();i++) {
        sl<<m_files[i].format;
    }
    return sl;
}

QStringList QFSelectFilesWizardPage::fileFilterIDs() const
{
    QStringList sl;
    for (int i=0; i<m_files.size();i++) {
        sl<<m_files[i].filterids.value(m_files[i].filters.indexOf(m_files[i].format), m_files[i].format);
    }
    return sl;
}

QList<int> QFSelectFilesWizardPage::fileFilterNums() const
{
    QList<int> sl;
    for (int i=0; i<m_files.size();i++) {
        sl<<m_files[i].filters.indexOf(m_files[i].format);
    }
    return sl;
}

void QFSelectFilesWizardPage::setOnlyOneFormatAllowed(bool allowed)
{
    onlyOneFormat=allowed;
}

void QFSelectFilesWizardPage::initializePage()
{
    lastFormat="";
    QFWizardPage::initializePage();
}

bool QFSelectFilesWizardPage::validatePage()
{
    bool ok=true;
    for (int i=0; i<m_files.size(); i++) {
        bool thisok=(!m_files[i].required || (m_files[i].required && QFile::exists(m_files[i].edit->text())));
        ok=ok&&thisok;
        if (!thisok) {
            QPalette p=m_files[i].edit->palette();
            p.setColor(QPalette::Base, QColor("salmon"));
            m_files[i].edit->setPalette(p);
        }
    }
    if (!ok) {
        QMessageBox::critical(this, title(), tr("You have to select an existing file for the widgets marked in red!"), QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }
    return QFWizardPage::validatePage();
}

void QFSelectFilesWizardPage::buttonClicked(QFStyledButton *btn)
{
    int idx=-1;
    for (int i=0; i<m_files.size(); i++) {
        if (m_files[i].button==btn) {
            idx=i;
            break;
        }
    }
    //qDebug()<<btn<<idx;
    if (idx>=0) {
        QString currentFCSFileFormatFilter=ProgramOptions::getConfigValue(lastFilterID, m_files[idx].filters.value(0)).toString();
        QString dir=ProgramOptions::getConfigValue(lastDirID, "").toString();
        if (m_files[idx].edit->text().size()>0) dir=QFileInfo(m_files[idx].edit->text()).absoluteFilePath();
        QStringList filterss=m_files[idx].filters;
        if (onlyOneFormat && !lastFormat.isEmpty() ) {
            filterss.clear();
            filterss<<lastFormat;
        }
        QString newfile = qfGetOpenFileName(this,
                                                  tr("Select File ..."),
                                                  dir,
                                                  filterss.join(";;"), &currentFCSFileFormatFilter);
        if (QFile::exists(newfile)) {
            ProgramOptions::setConfigValue(lastFilterID, currentFCSFileFormatFilter);
            ProgramOptions::setConfigValue(lastDirID, QFileInfo(newfile).absolutePath());
            m_files[idx].edit->setText(QFileInfo(newfile).absoluteFilePath());
            m_files[idx].format=currentFCSFileFormatFilter;
            lastFormat=currentFCSFileFormatFilter;
        }
    }
}
