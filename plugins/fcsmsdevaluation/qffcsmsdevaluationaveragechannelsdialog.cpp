#include "qffcsmsdevaluationaveragechannelsdialog.h"
#include "ui_qffcsmsdevaluationaveragechannelsdialog.h"
#include "programoptions.h"

QFFCSMSDEvaluationAverageChannelsDialog::QFFCSMSDEvaluationAverageChannelsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFCSMSDEvaluationAverageChannelsDialog)
{
    ui->setupUi(this);
    ui->cmbApplyTo->setCurrentIndex(ProgramOptions::getConfigValue("QFFCSMSDEvaluationAverageChannelsDialog/applyto", 0).toInt());
    ui->spinPoints->setValue(ProgramOptions::getConfigValue("QFFCSMSDEvaluationAverageChannelsDialog/points", 5).toInt());
}

QFFCSMSDEvaluationAverageChannelsDialog::~QFFCSMSDEvaluationAverageChannelsDialog()
{
    ProgramOptions::setConfigValue("QFFCSMSDEvaluationAverageChannelsDialog/applyto", getApplyTo());
    ProgramOptions::setConfigValue("QFFCSMSDEvaluationAverageChannelsDialog/points", getPoints());
    delete ui;
}

void QFFCSMSDEvaluationAverageChannelsDialog::init(int pointsToAvg, int rangeMin, int rangeMax)
{
    ui->spinPoints->setRange(rangeMin, rangeMax);
    ui->spinPoints->setValue(pointsToAvg);
}

int QFFCSMSDEvaluationAverageChannelsDialog::getPoints() const
{
    return ui->spinPoints->value();
}

int QFFCSMSDEvaluationAverageChannelsDialog::getApplyTo() const
{
    return qMax(0, ui->cmbApplyTo->currentIndex());
}
