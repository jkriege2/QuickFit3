#include "qfimfccsrelativeintensitydialog.h"
#include "ui_qfimfccsrelativeintensitydialog.h"
#include "qfmatchrdrfunctor.h"
#include "qfimfccsfitevaluation_item.h"
#include "qfpluginservices.h"
#include <QtGui>
#include "qfimfccsfitevaluation_item.h"
#include "qffcstools.h"
#include "optionswidget.h"
#include "qfrdrimagetoruninterface.h"
#include <stdint.h>
#include "qfmathtools.h"
#include "programoptions.h"
#include "qftools.h"
#include "jkautooutputtimer.h"

QFImFCCSRelativeIntensityDialog::QFImFCCSRelativeIntensityDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFImFCCSRelativeIntensityDialog)
{

    plt=NULL;
    matchFunctor=new QFImFCCSMatchRDRFunctor();
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();
    QList<QPointer<QFRawDataRecord> > lst=matchFunctor->getFilteredList(project);
    ui->setupUi(this);
    ui->cmbCCF->init(project, matchFunctor);
    ui->widOverviewCCF->setRDR(NULL);

    bool okCCF=false;
    for (int i=0; i<lst.size(); i++) {
        if (lst[i]) {
            if (!okCCF && isCCF(lst[i])) {
                ui->cmbCCF->setCurrentRDR(lst[i]);
                okCCF=true;
            }
        }
        if (okCCF) break;
    }

    loadWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "ImFCSCalibrationWizard");
    connect(ui->cmbCCF, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbCCF_currentIndexChanged(int)));

    //qDebug()<<10;

}

QFImFCCSRelativeIntensityDialog::~QFImFCCSRelativeIntensityDialog()
{
    saveWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "ImFCSCalibrationWizard");
    delete ui;
    delete matchFunctor;
}


QFRawDataRecord *QFImFCCSRelativeIntensityDialog::getCCF() const
{
    return ui->cmbCCF->currentRDR();
}

bool QFImFCCSRelativeIntensityDialog::calculateRelIntensity(QFRawDataRecord *ccf, double **rel_out, int&w, int& h, bool showErrorMessage)
{
    //JKAutoOutputTimer tim(QString("calculateRelCCF"));
    if (!ccf) return false;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImageToRunInterface* ccfRUNIMAGE=dynamic_cast<QFRDRImageToRunInterface*>(ccf);
    if (ccfRUNIMAGE) {

            //tim.write("1");
            w=ccfRUNIMAGE->getImageFromRunsWidth();
            h=ccfRUNIMAGE->getImageFromRunsHeight();

            double* ch0=ccfRUNIMAGE->getImageFromRunsPreview(0);
            double* ch1=ccfRUNIMAGE->getImageFromRunsPreview(1);
            //tim.write(QString("1: acfN=%1, ccfN=%2 w=%3 h=%4").arg(acfN).arg(ccfN).arg(w).arg(h));

            if (w>0 && h>0 && ch0 && ch1) {
                //tim.write("2");
                double* rel=(double*)malloc(w*h*sizeof(double));
                for (int i=0; i<w*h; i++) {

                    rel[i]=ch1[i]/ch0[i];
                }
                if (rel_out) *rel_out=rel;
                else if (rel) free(rel);

                QApplication::restoreOverrideCursor();
                return true;
            } else {
                if (showErrorMessage) QMessageBox::critical(NULL, tr("Calculate Relative Intensities"), tr("Can't Calculate Relative Intensitiess: Memory access error."));
                QApplication::restoreOverrideCursor();
                return false;
            }

    } else {
        if (showErrorMessage) QMessageBox::critical(NULL, tr("Calculate Relative Intensities"), tr("No files selected."));
        QApplication::restoreOverrideCursor();
        return false;
    }
    QApplication::restoreOverrideCursor();
    return true;
}

void QFImFCCSRelativeIntensityDialog::cmbCCF_currentIndexChanged(int index)
{
    replotImages();
}



void QFImFCCSRelativeIntensityDialog::replotImages()
{
    //JKAutoOutputTimer tim(QString("replotImages"));
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->widOverviewCCF->setRDR(ui->cmbCCF->currentRDR());

    ui->pltData->set_doDrawing(false);
    ui->pltData->clearGraphs();
    JKQTPdatastore* ds=ui->pltData->getDatastore();
    ds->clear();
    double* rel=NULL;
    int w=0, h=0;
    if (calculateRelIntensity(ui->cmbCCF->currentRDR(), &rel, w, h, false)) {
        size_t col=ds->addCopiedColumn(rel, w*h, tr("rel. intensity"));
        plt=new JKQTPColumnMathImage(ui->pltData->get_plotter());
        plt->set_palette(JKQTPMathImageMATLAB);
        plt->set_imageColumn(col);
        plt->set_Nx(w);
        plt->set_Ny(h);
        plt->set_x(0);
        plt->set_y(0);
        plt->set_width(w);
        plt->set_height(h);
        plt->set_showColorBar(true);
        plt->set_autoImageRange(true);
        bool showTop=w>h;
        plt->set_colorBarTopVisible(showTop);
        plt->set_colorBarRightVisible(!showTop);
        plt->set_title("");
        ui->pltData->get_plotter()->set_showKey(false);
        ui->pltData->get_plotter()->set_axisAspectRatio(double(w)/double(h));
        ui->pltData->get_plotter()->set_aspectRatio(double(w)/double(h));
        ui->pltData->get_plotter()->set_maintainAspectRatio(true);
        ui->pltData->get_plotter()->set_maintainAxisAspectRatio(true);
        ui->pltData->addGraph(plt);
        ui->pltData->setAbsoluteXY(0,w,0,h);
        ui->pltData->setXY(0,w,0,h);
    }
    if (rel) free(rel);
    ui->pltData->set_doDrawing(true);
    ui->pltData->update_plot();
    QApplication::restoreOverrideCursor();
}

void QFImFCCSRelativeIntensityDialog::addResult()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    double* rel;
    int w=0, h=0;
    QFRawDataRecord* ccf=getCCF();
    if (ccf&&calculateRelIntensity(ccf, &rel, w, h, true)) {

        QString evalName=QString("CalcRelIntensities_CCF%1").arg(ccf->getID());
        QString group="results";
        QString egroup=evalName;
        QString ccfName=ccf->getRole();
        if (ccfName.isEmpty()) ccfName=ccf->getName();
        QString egrouplabel=tr("rel. intensities of CCF: '%1'").arg(ccfName);
        QString rn="relative_intensity";

        ccf->resultsSetEvaluationGroup(evalName, egroup);
        ccf->resultsSetEvaluationGroupLabel(egroup, egrouplabel);
        ccf->resultsSetEvaluationGroupIndex(evalName, -1);
        ccf->resultsSetNumberList(evalName, rn, rel, w*h);
        ccf->resultsSetLabel(evalName, rn, tr("relative intensities"));
        ccf->resultsSetGroup(evalName, rn, group);
        ccf->resultsSetInteger(evalName, "ccf_file_id", ccf->getID());
        ccf->resultsSetString(evalName, "ccf_file_role", ccf->getRole());
        ccf->resultsSetString(evalName, "ccf_file_name", ccf->getName());

        if (rel) free(rel);
    }
    QApplication::restoreOverrideCursor();
}

void QFImFCCSRelativeIntensityDialog::on_btnNextACF_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    nextACF();
    QApplication::restoreOverrideCursor();

}


bool QFImFCCSRelativeIntensityDialog::isCCF(const QFRawDataRecord *rec)
{
    if (!rec) return false;
    return rec->getRole().toUpper().contains("FCCS") || rec->getRole().toUpper().contains("DCCF")|| (rec->getRole().isEmpty() && (rec->getName().toLower().contains("fccs") || rec->getName().toLower().contains("dccf")) );
}


bool QFImFCCSRelativeIntensityDialog::nextACF()
{
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();
    if (!(matchFunctor && project&&getCCF()) ) return false;
    QList<QPointer<QFRawDataRecord> > lst=matchFunctor->getFilteredList(project);
    int current=lst.indexOf(getCCF());
    if (current>=0 && current+1<lst.size()) {
        QFRawDataRecord* guess1=NULL;

        for (int i=current+1; i<lst.size(); i++) {
            if (isCCF(lst[i])) {
                guess1=lst[i];
            }
        }
        if (guess1) { ui->cmbCCF->setCurrentRDR(guess1); return true; }
    }
    return false;


}




void QFImFCCSRelativeIntensityDialog::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayPluginHelpWindow("imfccs_fit", "calc_rel_intensity.html");
}






void QFImFCCSRelativeIntensityDialog::on_btnStoreDataAll_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QModernProgressDialog progress(tr("calculating relative CCF amplitudes ..."), tr("&Cancel"), this);
    progress.show();
    QFRawDataRecord* rdr=ui->cmbCCF->currentRDR();
    QString role="";
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();
    if (rdr&&project) {
        role=rdr->getRole().toUpper();
        QList<QPointer<QFRawDataRecord> > lst=matchFunctor->getFilteredList(project);
        for (int i=0; i<lst.size(); i++) {
            if (lst[i] && isCCF(lst[i])) {
                ui->cmbCCF->setCurrentRDR(lst[i]);
                break;
            }
        }

    }
    QApplication::processEvents();
    cmbCCF_currentIndexChanged(ui->cmbCCF->currentIndex());
    addResult();
    while (nextACF()) {
        cmbCCF_currentIndexChanged(ui->cmbCCF->currentIndex());
        //QApplication::processEvents();
        addResult();
        if (progress.wasCanceled()) break;
    }
    QApplication::restoreOverrideCursor();
}

