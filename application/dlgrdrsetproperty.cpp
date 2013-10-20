#include "dlgrdrsetproperty.h"
#include "ui_dlgrdrsetproperty.h"

dlgRDRSetProperty::dlgRDRSetProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgRDRSetProperty)
{
    ui->setupUi(this);
}

dlgRDRSetProperty::~dlgRDRSetProperty() {
    QSettings* settings=ProgramOptions::getInstance()->getQSettings();
    QString prefix="QuickFit/dlgRDRSetProperty/";

    settings->setValue(prefix+"name", ui->edtName->text());
    settings->setValue(prefix+"value", ui->edtValue->text());
    settings->setValue(prefix+"type", ui->cmbType->currentIndex());
    settings->setValue(prefix+"overwrite", ui->chkOverwrite->isChecked());
    settings->setValue(prefix+"create", ui->chkCreate->isChecked());

    delete ui;
}

void dlgRDRSetProperty::init(QFProject *project)
{
    this->project=project;
    rdrs.clear();
    for (int i=0; i<project->getRawDataCount(); i++) {
        QPointer<QFRawDataRecord> r=project->getRawDataByNum(i);
        rdrs.append(r);
        QListWidgetItem* item=new QListWidgetItem(r->getName(), ui->lstRDR);
        item->setCheckState(Qt::Checked);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        item->setData(Qt::UserRole, r->getID());
        item->setIcon(r->getSmallIcon());
        ui->lstRDR->addItem(item);
    }

    QSettings* settings=ProgramOptions::getInstance()->getQSettings();
    QString prefix="QuickFit/dlgRDRSetProperty/";

    ui->edtName->setText(settings->value(prefix+"name", QString("new_property_name")).toString());
    ui->edtValue->setText(settings->value(prefix+"value", QString("")).toString());
    ui->cmbType->setCurrentIndex(settings->value(prefix+"type", 0).toInt());
    ui->chkCreate->setChecked(settings->value(prefix+"create", true).toBool());
    ui->chkOverwrite->setChecked(settings->value(prefix+"overwrite", true).toBool());
}

bool dlgRDRSetProperty::doOverwrite() const
{
    return ui->chkOverwrite->isChecked();
}

bool dlgRDRSetProperty::doCreateNew() const
{
    return ui->chkCreate->isChecked();
}

QList<QPointer<QFRawDataRecord> > dlgRDRSetProperty::getSelectedRDRs() const
{
    QList<QPointer<QFRawDataRecord> > l;
    for (int i=0; i<ui->lstRDR->count(); i++)  {
        if (ui->lstRDR->item(i)->checkState()==Qt::Checked) {
            l<<rdrs[i];
        }
    }
    return l;
}

void dlgRDRSetProperty::on_btnSelectNone_clicked()
{
    for (int i=0; i<ui->lstRDR->count(); i++)  {
        ui->lstRDR->item(i)->setCheckState(Qt::Unchecked);
    }
}

void dlgRDRSetProperty::on_btnSelectAll_clicked()
{
    for (int i=0; i<ui->lstRDR->count(); i++)  {
        ui->lstRDR->item(i)->setCheckState(Qt::Checked);
    }
}

void dlgRDRSetProperty::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("dlgrdrsetproperty.html");
}


QStringList dlgRDRSetProperty::getNewPropNames() const
{
    QStringList sl;
    sl<<ui->edtName->text();
    return sl;
}

QList<QVariant> dlgRDRSetProperty::getNewPropValues() const
{
    QList<QVariant> l;
    if (ui->cmbType->currentIndex()==0) {
        l<<ui->edtValue->text();
    } else if (ui->cmbType->currentIndex()==1) {
        l<<QStringToDouble( ui->edtValue->text());
    } else if (ui->cmbType->currentIndex()==2) {
        l<<ui->edtValue->text().toInt();
    } else if (ui->cmbType->currentIndex()==3) {
        l<<QStringToBool(ui->edtValue->text());
    } else {
        l<<QString("");
    }
    return l;
}
