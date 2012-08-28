#include "qfspimlightsheetevaluation.h"
#include <QtGui>
#include "qfspimlightsheetevaluation_item.h"

QFSPIMLightsheetEvaluation::QFSPIMLightsheetEvaluation(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFSPIMLightsheetEvaluation::~QFSPIMLightsheetEvaluation()
{
    //dtor
}

QFEvaluationItem* QFSPIMLightsheetEvaluation::createRecord(QFProject* parent) {
    return new QFSPIMLightsheetEvaluationItem(parent);
}


void QFSPIMLightsheetEvaluation::registerToMenu(QMenu* menu) {
    QAction* actAddEval=new QAction(QIcon(":/spim_lightsheet_eval/qfspimlightsheetevaluation_small.png"), getName(), parentWidget);
    actAddEval->setStatusTip(getDescription());
    connect(actAddEval, SIGNAL(triggered()), this, SLOT(insertEvaluation()));
    menu->addAction(actAddEval);
}




void QFSPIMLightsheetEvaluation::insertEvaluation() {
    if (project) {
        project->addEvaluation(getID(), getName());
    }
}

Q_EXPORT_PLUGIN2(spim_lightsheet_eval, QFSPIMLightsheetEvaluation)
