#include "dlgfccsfitallconfiguration.h"
#include "ui_dlgfccsfitallconfiguration.h"

DlgFCCSFitAllConfiguration::DlgFCCSFitAllConfiguration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgFCCSFitAllConfiguration)
{
    ui->setupUi(this);
}

DlgFCCSFitAllConfiguration::~DlgFCCSFitAllConfiguration()
{
    delete ui;
}

bool DlgFCCSFitAllConfiguration::getFitGuessed() const
{
    return ui->chkFitGuessed->isChecked();
}


bool DlgFCCSFitAllConfiguration::getFitAllRuns() const
{
    return ui->cmbRuns->currentIndex()==0;
}


bool DlgFCCSFitAllConfiguration::getFitFitted() const
{
    return ui->chkFitFitted->isChecked();
}
