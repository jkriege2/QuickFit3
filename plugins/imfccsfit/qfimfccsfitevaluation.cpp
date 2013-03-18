#include "qfimfccsfitevaluation.h"
#include <QtGui>
#include "qfimfccsfitevaluation_item.h"

QFImFCCSFitEvaluation::QFImFCCSFitEvaluation(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFImFCCSFitEvaluation::~QFImFCCSFitEvaluation()
{
    //dtor
}

QFEvaluationItem* QFImFCCSFitEvaluation::createRecord(QFProject* parent) {
    return new QFImFCCSFitEvaluationItem(parent);
}


void QFImFCCSFitEvaluation::registerToMenu(QMenu* menu) {
    QAction* actAddEval=new QAction(QIcon(getIconFilename()), tr("imFCCS Curve Fitting"), parentWidget);
    actAddEval->setStatusTip(getDescription());
    connect(actAddEval, SIGNAL(triggered()), this, SLOT(insertEvaluation()));
    menu->addAction(actAddEval);
}




void QFImFCCSFitEvaluation::insertEvaluation() {
    if (project) {
        project->addEvaluation(getID(), getName());
    }
}

Q_EXPORT_PLUGIN2(imfccs_fit, QFImFCCSFitEvaluation)
