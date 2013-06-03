#include "qffccsfitevaluation.h"
#include <QtGui>
#include "qffccsfitevaluation_item.h"
#include "qffcstools.h"
#include "optionswidget.h"
#include "qfrdrimagetoruninterface.h"
#include <stdint.h>
#include "qfmathtools.h"
#include "qfrawdatarecord.h"

QFFCCSFitEvaluation::QFFCCSFitEvaluation(QObject* parent):
    QObject(parent)
{
    QFPluginServices::getInstance()->registerSettingsPane(this);
}

QFFCCSFitEvaluation::~QFFCCSFitEvaluation()
{
    //dtor
}

QFEvaluationItem* QFFCCSFitEvaluation::createRecord(QFProject* parent) {
    return new QFFCCSFitEvaluationItem(parent);
}


void QFFCCSFitEvaluation::registerToMenu(QMenu* menu) {
    QAction* actAddEval=new QAction(QIcon(getIconFilename()), tr("FCCS Curve Fitting"), parentWidget);
    actAddEval->setStatusTip(getDescription());
    connect(actAddEval, SIGNAL(triggered()), this, SLOT(insertEvaluation()));
    menu->addAction(actAddEval);
}

void QFFCCSFitEvaluation::init()
{
}

QString QFFCCSFitEvaluation::pluginOptionsName() const
{
    return getName();
}

QIcon QFFCCSFitEvaluation::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFFCCSFitEvaluation::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}




void QFFCCSFitEvaluation::insertEvaluation() {
    if (project) {
        project->addEvaluation(getID(), getName());
    }
}



Q_EXPORT_PLUGIN2(fccs_fit, QFFCCSFitEvaluation)
