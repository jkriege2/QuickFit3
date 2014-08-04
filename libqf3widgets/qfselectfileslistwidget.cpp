#include "qfselectfileslistwidget.h"
#include "ui_qfselectfileslistwidget.h"
#include "programoptions.h"
#include "qftools.h"

QFSelectFilesListWidget::QFSelectFilesListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFSelectFilesListWidget)
{
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
        ui->files->item(i)->data(Qt::UserRole+1).toString();
    }
    return sl;
}

QStringList QFSelectFilesListWidget::fileFilters() const
{
    QStringList sl;
    for (int i=0; i<ui->files->count(); i++) {
        ui->files->item(i)->data(Qt::UserRole+2).toString();
    }
    return sl;
}

void QFSelectFilesListWidget::on_btnAdd_clicked()
{
    QString currentFCSFileFormatFilter=ProgramOptions::getConfigValue(lastFilterID, filters.value(0)).toString();
    QString dir=ProgramOptions::getConfigValue(lastDirID, "").toString();
    QStringList files = qfGetOpenFileNames(this,
                          tr("Select File(s) ..."),
                          dir,
                          filters.join(";;"), &currentFCSFileFormatFilter);
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
    QFSelectFilesListWidget *sel = new QFSelectFilesListWidget(this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(sel);
    setLayout(layout);

}

void QFSelectFilesWizardPage::setFilters(const QStringList &filters)
{
    sel->setFilters(filters);
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
