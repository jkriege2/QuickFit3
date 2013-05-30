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
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();
    int idx=0;
    for (int i=0; i<project->getEvaluationCount(); i++) {
        QFEvaluationItem* it=project->getEvaluationByNum(i);
        if (it->getType().toLower()=="fcs_fit") {
            ui->cmbEval->addItem(it->getSmallIcon(), it->getName(), it->getID());
            /*if (current) {
                if (current->resultsExistsFromEvaluation())
            }*/
        }
    }
    ui->cmbEval->setCurrentIndex(idx);
}

QFFCSMSDEvaluationGetNFromFits::~QFFCSMSDEvaluationGetNFromFits()
{
    ProgramOptions::setConfigValue("QFFCSMSDEvaluationGetNFromFits/applyto", getApplyTo());
    delete ui;
}

int QFFCSMSDEvaluationGetNFromFits::getApplyTo() const
{
    return qMax(0, ui->cmbApplyTo->currentIndex());
}

QFEvaluationItem *QFFCSMSDEvaluationGetNFromFits::getEval() const
{
    if (ui->cmbEval->currentIndex()<0) return NULL;
    return QFPluginServices::getInstance()->getCurrentProject()->getEvaluationByID(ui->cmbEval->itemData(ui->cmbEval->currentIndex()).toInt());
}
