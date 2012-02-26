#include "qfpevalimfcsfit.h"
#include <QtGui>
#include "qfimfcsfitevaluation.h"

QFPEvalIMFCSFit::QFPEvalIMFCSFit(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFPEvalIMFCSFit::~QFPEvalIMFCSFit()
{
    //dtor
}

QFEvaluationItem* QFPEvalIMFCSFit::createRecord(QFProject* parent) {
    return new QFImFCSFitEvaluation(parent);
}


void QFPEvalIMFCSFit::registerToMenu(QMenu* menu) {
    QAction* actFCS=new QAction(QIcon(":/imfcs_fit.png"), tr("imagingFCS Curve Fitting"), parentWidget);
    actFCS->setStatusTip(tr("Insert a new imagingFCS least-squares fit evaluation"));
    connect(actFCS, SIGNAL(triggered()), this, SLOT(insertFCSFit()));
    menu->addAction(actFCS);
}




void QFPEvalIMFCSFit::insertFCSFit() {
    if (project) {
        project->addEvaluation(getID(), "imagingFCS Fit");
    }
}

Q_EXPORT_PLUGIN2(imfcsfit, QFPEvalIMFCSFit)
