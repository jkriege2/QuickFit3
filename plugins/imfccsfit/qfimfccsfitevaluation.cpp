#include "qfimfccsfitevaluation.h"
#include <QtGui>
#include "qfimfccsfitevaluation_item.h"
#include "qffcstools.h"
#include "optionswidget.h"
#include "qfimfccsrelativeccfdialog.h"
#include "qfrdrimagetoruninterface.h"
#include <stdint.h>
#include "qfmathtools.h"

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
    QFImFCCSRelativeCCFDialog* dlg=new QFImFCCSRelativeCCFDialog(parentWidget);
    if (dlg->exec()) {
        QFRawDataRecord* acf=dlg->getACF();
        QFRDRFCSDataInterface* acfFCS=dynamic_cast<QFRDRFCSDataInterface*>(acf);
        QFRDRImageToRunInterface* acfRUNIMAGE=dynamic_cast<QFRDRImageToRunInterface*>(acf);
        QFRawDataRecord* ccf=dlg->getACF();
        QFRDRFCSDataInterface* ccfFCS=dynamic_cast<QFRDRFCSDataInterface*>(ccf);
        QFRDRImageToRunInterface* ccfRUNIMAGE=dynamic_cast<QFRDRImageToRunInterface*>(ccf);
        if (acfFCS && acfRUNIMAGE && ccfFCS && ccfRUNIMAGE) {
            if (   acfRUNIMAGE->getImageFromRunsWidth()==ccfRUNIMAGE->getImageFromRunsWidth()
                && acfRUNIMAGE->getImageFromRunsHeight()==ccfRUNIMAGE->getImageFromRunsHeight() )
            {
                int w=acfRUNIMAGE->getImageFromRunsWidth();
                int h=acfRUNIMAGE->getImageFromRunsHeight();
                double* rel=(double*)malloc(w*h*sizeof(double));
                double* rel_error=(double*)malloc(w*h*sizeof(double));

                double* acfD=acfFCS->getCorrelation();
                double* acfS=acfFCS->getCorrelationStdDev();
                int acfN=acfFCS->getCorrelationN();
                double* ccfD=ccfFCS->getCorrelation();
                double* ccfS=ccfFCS->getCorrelationStdDev();
                int ccfN=ccfFCS->getCorrelationN();

                uint64_t idxA=0;
                uint64_t idxC=0;
                for (int i=0; i<w*h; i++) {
                    rel[i]=ccfD[idxC]/acfD[idxA];
                    rel_error[i]=0;
                    if (acfS && ccfS) rel_error[i]=sqrt(qfSqr(acfS[idxA]*ccfD[idxC]/qfSqr(acfD[idxA])) + qfSqr(ccfS[idxC]/acfD[idxA]));
                    if (!QFFloatIsOK(rel_error[i])) rel_error[i]=0;
                    idxA=idxA+acfN;
                    idxC=idxC+ccfN;
                }
                QString evalName=QString("CalcRelCCF_ACF%1_CCF%2").arg(acf->getID()).arg(ccf->getID());
                QString rn="relative_ccf_amplitude";
                ccf->resultsSetNumberErrorList(evalName, rn, rel, rel_error, w*h);
                ccf->resultsSetLabel(evalName, rn, tr("relative CCF amplitude"));

                free(rel);
                free(rel_error);
            } else {
                QMessageBox::critical(parentWidget, tr("Calculate Relative CCF"), tr("Can't calculate relative CCFs: The two selected files contain images of different size."));
            }

        } else {
            QMessageBox::critical(parentWidget, tr("Calculate Relative CCF"), tr("No files selected."));
        }
    }
    delete dlg;
}


Q_EXPORT_PLUGIN2(imfccs_fit, QFImFCCSFitEvaluation)
