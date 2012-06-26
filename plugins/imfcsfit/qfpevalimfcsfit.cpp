#include "qfpevalimfcsfit.h"
#include <QtGui>
#include "qfimfcsfitevaluation.h"
#include "imfcscalibrationdialog.h"
#include "optionswidget.h"

QFPEvalIMFCSFit::QFPEvalIMFCSFit(QObject* parent):
    QObject(parent)
{
    //ctor
    QFPluginServices::getInstance()->registerSettingsPane(this);
}

QFPEvalIMFCSFit::~QFPEvalIMFCSFit()
{
    //dtor
}

QFEvaluationItem* QFPEvalIMFCSFit::createRecord(QFProject* parent) {
    return new QFImFCSFitEvaluation(parent);
}


void QFPEvalIMFCSFit::registerToMenu(QMenu* menu) {
    QAction* actFCS=new QAction(QIcon(":/imfcs_fit.png"), tr("imagingFCS Curve Fitting"), parentWidget);
    actFCS->setStatusTip(tr("Insert a new imagingFCS least-squares fit evaluation"));
    connect(actFCS, SIGNAL(triggered()), this, SLOT(insertFCSFit()));
    menu->addAction(actFCS);

    QAction* actFCSCalib=new QAction(QIcon(":/imfcsfit/imfcs_fitcalib.png"), tr("imagingFCS Calibration"), parentWidget);
    actFCSCalib->setStatusTip(tr("Insert a set of imagingFCS least-squares fit evaluations for a SPIM calibration"));
    connect(actFCSCalib, SIGNAL(triggered()), this, SLOT(insertFCSFitForCalibration()));
    menu->addAction(actFCSCalib);


}

QString QFPEvalIMFCSFit::pluginOptionsName() const
{
    return getName();
}

QIcon QFPEvalIMFCSFit::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFPEvalIMFCSFit::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}



void QFPEvalIMFCSFit::insertFCSFit() {
    if (project) {
        project->addEvaluation(getID(), "imagingFCS Fit");
    }
}

void QFPEvalIMFCSFit::insertFCSFitForCalibration() {
    if (project) {

        ImFCSCalibrationDialog* dlg=new ImFCSCalibrationDialog(NULL);
        if (dlg->exec()) {
            QList<double> vals=dlg->getValues();
            for (int i=0; i<vals.size(); i++) {
                QFEvaluationItem* e=project->addEvaluation(getID(), "imagingFCS Fit");
                e->setQFProperty("PRESET_FOCUS_WIDTH", vals[i], false, false);
                e->setName(tr("wxy=%1 nm").arg(vals[i]));
            }
         }
        delete dlg;
    }
}
Q_EXPORT_PLUGIN2(imfcsfit, QFPEvalIMFCSFit)
