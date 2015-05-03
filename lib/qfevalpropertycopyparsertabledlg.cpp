#include "qfevalpropertycopyparsertabledlg.h"
#include "ui_qfevalpropertycopyparsertabledlg.h"
#include "programoptions.h"

QFEvalPropertyCopyParsertableDlg::QFEvalPropertyCopyParsertableDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFEvalPropertyCopyParsertableDlg)
{
    ui->setupUi(this);
    ProgramOptions::getConfigQCheckBox(ui->chkCopyErrors, "QFEvalPropertyCopyParsertableDlg/chkCopyErrors");
    ProgramOptions::getConfigQComboBox(ui->cmbDataStat, "QFEvalPropertyCopyParsertableDlg/cmbDataStat");
    ProgramOptions::getConfigQComboBox(ui->cmbErrorStat, "QFEvalPropertyCopyParsertableDlg/cmbErrorStat");
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

QFEvalPropertyCopyParsertableDlg::~QFEvalPropertyCopyParsertableDlg()
{
    ProgramOptions::setConfigQCheckBox(ui->chkCopyErrors, "QFEvalPropertyCopyParsertableDlg/chkCopyErrors");
    ProgramOptions::setConfigQComboBox(ui->cmbDataStat, "QFEvalPropertyCopyParsertableDlg/cmbDataStat");
    ProgramOptions::setConfigQComboBox(ui->cmbErrorStat, "QFEvalPropertyCopyParsertableDlg/cmbErrorStat");
    delete ui;
}

bool QFEvalPropertyCopyParsertableDlg::getCopyErrors() const
{
    return ui->chkCopyErrors->isChecked();
}

int QFEvalPropertyCopyParsertableDlg::dataStatistics() const
{
    return ui->cmbDataStat->currentIndex();
}

int QFEvalPropertyCopyParsertableDlg::errorStatistics() const
{
    return ui->cmbErrorStat->currentIndex();
}
