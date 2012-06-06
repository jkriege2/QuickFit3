
#include "qffcsmsdevaluation.h"
#include <QtGui>
#include "qffcsmsdevaluation_item.h"

QFFCSMSDEvaluation::QFFCSMSDEvaluation(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFFCSMSDEvaluation::~QFFCSMSDEvaluation()
{
    //dtor
}

QFEvaluationItem* QFFCSMSDEvaluation::createRecord(QFProject* parent) {
    return new QFFCSMSDEvaluationItem(parent);
}


void QFFCSMSDEvaluation::registerToMenu(QMenu* menu) {
    QAction* actAddEval=new QAction(QIcon(":/fcsmsdevaluation/fcsmsdevaluation_small.png"), getName(), parentWidget);
    actAddEval->setStatusTip(getDescription());
    connect(actAddEval, SIGNAL(triggered()), this, SLOT(insertEvaluation()));
    menu->addAction(actAddEval);
}




void QFFCSMSDEvaluation::insertEvaluation() {
    if (project) {
        project->addEvaluation(getID(), getName());
    }
}

Q_EXPORT_PLUGIN2(QFFCSMaxEntEvaluation, QFFCSMSDEvaluation)
