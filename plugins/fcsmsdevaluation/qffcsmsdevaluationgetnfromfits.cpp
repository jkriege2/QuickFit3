#include "qffcsmsdevaluationgetnfromfits.h"
#include "ui_qffcsmsdevaluationgetnfromfits.h"
#include "qfproject.h"
#include "qfpluginservices.h"
#include "qfevaluationitem.h"

QFFCSMSDEvaluationGetNFromFits::QFFCSMSDEvaluationGetNFromFits(QFRawDataRecord *current, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFCSMSDEvaluationGetNFromFits)
{
    ui->setupUi(this);
    ui->cmbApplyTo->setCurrentIndex(ProgramOptions::getConfigValue("QFFCSMSDEvaluationGetNFromFits/applyto", 0).toInt());
    ui->chkGetN->setChecked(ProgramOptions::getConfigValue("QFFCSMSDEvaluationGetNFromFits/getN", true).toBool());
    ui->chkGetTriplet->setChecked(ProgramOptions::getConfigValue("QFFCSMSDEvaluationGetNFromFits/getTriplet", true).toBool());

    ui->cmbEval->setRDR(current);
}

QFFCSMSDEvaluationGetNFromFits::~QFFCSMSDEvaluationGetNFromFits()
{
    ProgramOptions::setConfigValue("QFFCSMSDEvaluationGetNFromFits/applyto", getApplyTo());
    ProgramOptions::setConfigValue("QFFCSMSDEvaluationGetNFromFits/getN", getN());
    ProgramOptions::setConfigValue("QFFCSMSDEvaluationGetNFromFits/getTriplet", getTriplet());
    delete ui;
}

int QFFCSMSDEvaluationGetNFromFits::getApplyTo() const
{
    return qMax(0, ui->cmbApplyTo->currentIndex());
}

QString QFFCSMSDEvaluationGetNFromFits::getEvalGroup() const
{
    return ui->cmbEval->currentEvaluationGroup();
}

QString QFFCSMSDEvaluationGetNFromFits::getParameterN() const
{
    return "fitparam_n_particle";
}

QString QFFCSMSDEvaluationGetNFromFits::getParameterTripletTheta() const
{
    return "fitparam_nonfl_theta1";
}

QString QFFCSMSDEvaluationGetNFromFits::getParameterTripletTau() const
{
    return "fitparam_nonfl_tau1";
}

bool QFFCSMSDEvaluationGetNFromFits::getN() const
{
    return ui->chkGetN->isChecked();
}

bool QFFCSMSDEvaluationGetNFromFits::getTriplet() const
{
    return ui->chkGetTriplet->isChecked();
}

