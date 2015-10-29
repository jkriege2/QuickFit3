#include "dlgfccsresetsetparameter.h"
#include "ui_dlgfccsresetsetparameter.h"
#include "qfhtmldelegate.h"
#include "qfpluginservices.h"

DlgFCCSResetSetParameter::DlgFCCSResetSetParameter(QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::DlgFCCSResetSetParameter)
{
    ui->setupUi(this);
    ui->cmbParam->setItemDelegate(new QFHTMLDelegate(ui->cmbParam));
    setHelpfile("");
}

DlgFCCSResetSetParameter::~DlgFCCSResetSetParameter()
{
    delete ui;
}

void DlgFCCSResetSetParameter::addParameter(const DlgFCCSResetSetParameter::Parameter &p)
{
    params.append(p);
    updateWidgets();
}

void DlgFCCSResetSetParameter::setParameters(const QList<DlgFCCSResetSetParameter::Parameter> &p)
{
    params=p;
    updateWidgets();
}

void DlgFCCSResetSetParameter::clearParams()
{
    params.clear();
    updateWidgets();
}

bool DlgFCCSResetSetParameter::setInAll() const
{
    return ui->chkChangeAll->isChecked();
}

bool DlgFCCSResetSetParameter::setInInitial() const
{
    return ui->chkInitial->isChecked();
}

DlgFCCSResetSetParameter::Operation DlgFCCSResetSetParameter::operationValue() const
{
    if (ui->radValueDontChange->isChecked()) return DlgFCCSResetSetParameter::opNothing;
    if (ui->radValueSet->isChecked()) return DlgFCCSResetSetParameter::opSet;
    if (ui->radValueReset->isChecked()) return DlgFCCSResetSetParameter::opReset;

    return DlgFCCSResetSetParameter::opNothing;
}

DlgFCCSResetSetParameter::Operation DlgFCCSResetSetParameter::operationError() const
{
    if (ui->radErrorDontChange->isChecked()) return DlgFCCSResetSetParameter::opNothing;
    if (ui->radErrorSet->isChecked()) return DlgFCCSResetSetParameter::opSet;
    if (ui->radErrorReset->isChecked()) return DlgFCCSResetSetParameter::opReset;

    return DlgFCCSResetSetParameter::opNothing;

}

DlgFCCSResetSetParameter::Operation DlgFCCSResetSetParameter::operationFix() const
{
    if (ui->radFixDontChange->isChecked()) return DlgFCCSResetSetParameter::opNothing;
    if (ui->radFixSet->isChecked()) return DlgFCCSResetSetParameter::opSet;
    if (ui->radFixReset->isChecked()) return DlgFCCSResetSetParameter::opReset;

    return DlgFCCSResetSetParameter::opNothing;

}

double DlgFCCSResetSetParameter::getValue() const
{
    return ui->edtValue->value();
}

double DlgFCCSResetSetParameter::getError() const
{
    return ui->edtError->value();
}

bool DlgFCCSResetSetParameter::getFix() const
{
    return ui->chkFixed->isChecked();
}

bool DlgFCCSResetSetParameter::setAllIndices()
{
    return ui->chkResetAllIndices->isChecked();
}

QString DlgFCCSResetSetParameter::getParameter() const
{
    int idx=ui->cmbParam->currentIndex();
    if (idx>=0 && idx<params.size()) {
        return params[idx].id;
    }
    return QString();
}

void DlgFCCSResetSetParameter::setHelpfile(const QString help)
{
    this->help=help;
    ui->btnHelp->setVisible(!help.isEmpty());
}

void DlgFCCSResetSetParameter::updateWidgets()
{
    int idx=ui->cmbParam->currentIndex();

    ui->cmbParam->clear();
    QStringList labs;
    for (int i=0; i<params.size(); i++) {
        labs<<params[i].label;
    }
    ui->cmbParam->addItems(labs);
    ui->cmbParam->setCurrentIndex(idx);
    on_cmbParam_currentIndexChanged();
}

void DlgFCCSResetSetParameter::on_cmbParam_currentIndexChanged()
{
    int idx=ui->cmbParam->currentIndex();
    if (idx>=0 && idx<params.size()) {
        ui->edtError->setValue(params[idx].error);
        ui->edtValue->setValue(params[idx].value);
        ui->chkFixed->setChecked(params[idx].fix);
    }
}

void DlgFCCSResetSetParameter::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(help);
}

void DlgFCCSResetSetParameter::dataEdited()
{
    int idx=ui->cmbParam->currentIndex();
    if (idx>=0 && idx<params.size()) {
        params[idx].error=ui->edtError->value();
        params[idx].value=ui->edtValue->value();
        params[idx].fix=ui->chkFixed->isChecked();
    }
}
