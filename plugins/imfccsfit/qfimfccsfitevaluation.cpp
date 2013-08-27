#include "qfimfccsfitevaluation.h"
#include <QtGui>
#include "qfimfccsfitevaluation_item.h"
#include "qffcstools.h"
#include "optionswidget.h"
#include "qfimfccsrelativeccfdialog.h"
#include "qfimfccsrelativeintensitydialog.h"
#include "qfrdrimagetoruninterface.h"
#include <stdint.h>
#include "qfmathtools.h"
#include "qfrawdatarecord.h"
#include "qfimfccsrelativeccfcrosstalkdialog.h"
#include "qfimfccsamplitudefitdialog.h"

QFImFCCSFitEvaluation::QFImFCCSFitEvaluation(QObject* parent):
    QObject(parent)
{
    dlgRelCCF=NULL;
    dlgRelCCFCrosstalk=NULL;
    dlgRelInten=NULL;
    dlgAmpFit=NULL;
    QFPluginServices::getInstance()->registerSettingsPane(this);
}

QFImFCCSFitEvaluation::~QFImFCCSFitEvaluation()
{
    if (dlgRelCCF) {
        dlgRelCCF->close();
        dlgRelCCF->deleteLater();
    }

    if (dlgRelCCFCrosstalk) {
        dlgRelCCFCrosstalk->close();
        dlgRelCCFCrosstalk->deleteLater();
    }

    if (dlgRelInten) {
        dlgRelInten->close();
        dlgRelInten->deleteLater();
    }

    if (dlgAmpFit) {
        dlgAmpFit->close();
        dlgAmpFit->deleteLater();
    }
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
    QAction* actI=new QAction(tr("calculate &relative intensity"), menu);
    connect(actI, SIGNAL(triggered()), this, SLOT(calcRelativeIntensity()));
    menu->addAction(actI);
    menu->addSeparator();
    QAction* actCCF=new QAction(tr("calculate &rel. CCF amplitude"), menu);
    connect(actCCF, SIGNAL(triggered()), this, SLOT(calcRelativeCCF()));
    menu->addAction(actCCF);
    QAction* actCCFCT=new QAction(tr("calculate &rel. CCF amplitude, crosstalk correction"), menu);
    connect(actCCFCT, SIGNAL(triggered()), this, SLOT(calcRelativeCCFCrosstalk()));
    menu->addAction(actCCFCT);
    menu->addSeparator();
    QAction* actAmpFit=new QAction(tr("imFCCS amplitude fit"), menu);
    connect(actAmpFit, SIGNAL(triggered()), this, SLOT(calcAmplitudeFit()));
    menu->addAction(actAmpFit);
    actAmpFit->setEnabled(false);

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
    //qDebug()<<"calcRelativeCCF";
    if (!dlgRelCCF) dlgRelCCF=new QFImFCCSRelativeCCFDialog(NULL);
    //qDebug()<<"calcRelativeCCF1";
    dlgRelCCF->show();
    //qDebug()<<"calcRelativeCCF2";
}

void QFImFCCSFitEvaluation::calcRelativeCCFCrosstalk()
{
    //qDebug()<<"calcRelativeCCF";
    if (!dlgRelCCFCrosstalk) dlgRelCCFCrosstalk=new QFImFCCSRelativeCCFCrosstalkDialog(NULL);
    //qDebug()<<"calcRelativeCCF1";
    dlgRelCCFCrosstalk->show();
    //qDebug()<<"calcRelativeCCF2";

}

void QFImFCCSFitEvaluation::calcRelativeIntensity()
{
    if (!dlgRelInten) dlgRelInten=new QFImFCCSRelativeIntensityDialog(NULL);
    dlgRelInten->show();

}

void QFImFCCSFitEvaluation::calcAmplitudeFit()
{
    if (!dlgAmpFit) dlgAmpFit=new QFImFCCSAmplitudeFitDialog(NULL);
    dlgAmpFit->show();
}


Q_EXPORT_PLUGIN2(imfccs_fit, QFImFCCSFitEvaluation)
