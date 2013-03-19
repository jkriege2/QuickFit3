#include "qfimfccsfitevaluation.h"
#include <QtGui>
#include "qfimfccsfitevaluation_item.h"
#include "qffcstools.h"
#include "optionswidget.h"

QFImFCCSFitEvaluation::QFImFCCSFitEvaluation(QObject* parent):
    QObject(parent)
{
    QFPluginServices::getInstance()->registerSettingsPane(this);
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

QString QFImFCCSFitEvaluation::pluginOptionsName() const
{
    return getName();
}

QIcon QFImFCCSFitEvaluation::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFImFCCSFitEvaluation::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}




void QFImFCCSFitEvaluation::insertEvaluation() {
    if (project) {
        project->addEvaluation(getID(), getName());
    }
}


Q_EXPORT_PLUGIN2(imfccs_fit, QFImFCCSFitEvaluation)
