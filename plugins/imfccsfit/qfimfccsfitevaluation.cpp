#include "qfimfccsfitevaluation.h"
#include <QtGui>
#include "qfimfccsfitevaluation_item.h"
#include "qffcstools.h"
#include "optionswidget.h"
#include "qfimfccsrelativeccfdialog.h"
#include "qfrdrimagetoruninterface.h"
#include <stdint.h>
#include "qfmathtools.h"
#include "qfrawdatarecord.h"

QFImFCCSFitEvaluation::QFImFCCSFitEvaluation(QObject* parent):
    QObject(parent)
{
    dlgRelCCF=NULL;
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

void QFImFCCSFitEvaluation::init()
{
    QMenu* menu=new QMenu(tr("imFCCS"));
    menu->setIcon(QIcon(getIconFilename()));
    QAction* actCCF=new QAction(tr("calculate &relative CCF amplitude"), menu);
    connect(actCCF, SIGNAL(triggered()), this, SLOT(calcRelativeCCF()));
    menu->addAction(actCCF);

    QFPluginServices::getInstance()->getMenu("tools")->addMenu(menu);
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

void QFImFCCSFitEvaluation::calcRelativeCCF()
{
    if (!dlgRelCCF) dlgRelCCF=new QFImFCCSRelativeCCFDialog(parentWidget);
    dlgRelCCF->show();
}


Q_EXPORT_PLUGIN2(imfccs_fit, QFImFCCSFitEvaluation)
