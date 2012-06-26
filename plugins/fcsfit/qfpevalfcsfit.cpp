#include "qfpevalfcsfit.h"
#include "optionswidget.h"
#include <QtGui>
#include "qffcsfitevaluation.h"

QFPEvalFCSFit::QFPEvalFCSFit(QObject* parent):
    QObject(parent)
{
    //ctor
    QFPluginServices::getInstance()->registerSettingsPane(this);
}

QFPEvalFCSFit::~QFPEvalFCSFit()
{
    //dtor
}

QFEvaluationItem* QFPEvalFCSFit::createRecord(QFProject* parent) {
    return new QFFCSFitEvaluation(parent);
}


void QFPEvalFCSFit::registerToMenu(QMenu* menu) {
    QAction* actFCS=new QAction(QIcon(":/fcs_fit.png"), tr("FCS Curve Fitting"), parentWidget);
    actFCS->setStatusTip(tr("Insert a new FCS least-squares fit evaluation"));
    connect(actFCS, SIGNAL(triggered()), this, SLOT(insertFCSFit()));
    menu->addAction(actFCS);
}



QString QFPEvalFCSFit::pluginOptionsName() const
{
    return getName();
}

QIcon QFPEvalFCSFit::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFPEvalFCSFit::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}

void QFPEvalFCSFit::insertFCSFit() {
    if (project) {
        project->addEvaluation(getID(), "FCS Fit");
    }
}

Q_EXPORT_PLUGIN2(qfevalfcsfit, QFPEvalFCSFit)
