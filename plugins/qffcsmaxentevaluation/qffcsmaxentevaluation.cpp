
#include "qffcsmaxentevaluation.h"
#include <QtGui>
#include "qffcsmaxentevaluation_item.h"

QFFCSMaxEntEvaluation::QFFCSMaxEntEvaluation(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFFCSMaxEntEvaluation::~QFFCSMaxEntEvaluation()
{
    //dtor
}

QFEvaluationItem* QFFCSMaxEntEvaluation::createRecord(QFProject* parent) {
    return new QFFCSMaxEntEvaluationItem(parent);
}


void QFFCSMaxEntEvaluation::registerToMenu(QMenu* menu) {
    QAction* actAddEval=new QAction(QIcon(":/fcs_maxent/qffcsmaxentevaluation_small.png"), getName(), parentWidget);
    actAddEval->setStatusTip(getDescription());
    connect(actAddEval, SIGNAL(triggered()), this, SLOT(insertEvaluation()));
    menu->addAction(actAddEval);
}




void QFFCSMaxEntEvaluation::insertEvaluation() {
    if (project) {
        project->addEvaluation(getID(), getName());
    }
}

Q_EXPORT_PLUGIN2(fcs_maxent, QFFCSMaxEntEvaluation)
