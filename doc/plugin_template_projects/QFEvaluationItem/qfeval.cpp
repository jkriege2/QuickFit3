#include "qfeval.h"
#include <QtGui>
#include "qfeval_item.h"

QFEVAL::QFEVAL(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFEVAL::~QFEVAL()
{
    //dtor
}

QFEvaluationItem* QFEVAL::createRecord(QFProject* parent) {
    return new QFEVALItem(parent);
}


void QFEVAL::registerToMenu(QMenu* menu) {
    QAction* actAddEval=new QAction(QIcon(":/target_id/qfeval_small.png"), getName(), parentWidget);
    actAddEval->setStatusTip(getDescription());
    connect(actAddEval, SIGNAL(triggered()), this, SLOT(insertEvaluation()));
    menu->addAction(actAddEval);
}




void QFEVAL::insertEvaluation() {
    if (project) {
        project->addEvaluation(getID(), getName());
    }
}

Q_EXPORT_PLUGIN2(target_id, QFEVAL)
