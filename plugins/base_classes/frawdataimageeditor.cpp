#include "frawdataimageeditor.h"
#include "qfrdrimagecopydataselectdialog.h"
#include <QDebug>
#include <math.h>
#include "jkqttools.h"
#include "qffitfunction.h"
#include "qffitfunctionmanager.h"
#include "datatable2.h"
#include "jkimage.h"
#include "qmoretextobject.h"
#include "statistics_tools.h"
#include "jkqtpelements.h"
#include "qftools.h"
#include "qfrawdatapropertyeditor.h"
#include "qfselectionlistdialog.h"
#include "statistics_tools.h"
#include "qffcstools.h"
#include "qfrdrimagetoruninterface.h"
#include "csvtools.h"
#define sqr(x) qfSqr(x)

#define CLICK_UPDATE_TIMEOUT 500
//#define DEBUG_TIMIMNG
#undef DEBUG_TIMIMNG

#define OverlayRectanglesAsImageOverlay true

FRawDataImageEditor::FRawDataImageEditor(const QString& configprefix, QFPluginServices *services, QFRawDataPropertyEditor *propEditor, QWidget *parent):
    QFRawDataEditor(services, propEditor, parent)
{
    correlationMaskTools=NULL;
    this->configprefix=configprefix;
    m_fitFunctions=QFPluginServices::getInstance()->getFitFunctionManager()->getModels("", this);
    plteOverviewSelectedData=NULL;
    plteOverviewExcludedData=NULL;
    plteOverviewSize=0;
    //plteImageData=NULL;
    plteImageSize=0;
    lastSavePath="";
    connectImageWidgetsCounter=0;
    connectParameterWidgetsCounter=0;
    //datahist=datahistsel=NULL;
    timUpdateAfterClick=new QTimer(this);
    timUpdateAfterClick->setSingleShot(true);
    timUpdateAfterClick->setInterval(CLICK_UPDATE_TIMEOUT);
    timUpdateAfterClick->stop();
    createWidgets();
    //QTimer::singleShot(500, this, SLOT(debugInfo()));
    connect(timUpdateAfterClick, SIGNAL(timeout()), this, SLOT(updateAfterClick()));
}

FRawDataImageEditor::~FRawDataImageEditor()
{
    if (plteOverviewSelectedData) free(plteOverviewSelectedData);
    plteOverviewSelectedData=NULL;
    if (plteOverviewExcludedData) free(plteOverviewExcludedData);
    plteOverviewExcludedData=NULL;
}

void FRawDataImageEditor::readSettings()
{
    if (!settings) return;
    connectParameterWidgets(false);
    connectImageWidgets(false);
    lastSavePath=settings->getQSettings()->value(QString("imfcsimageeditor/last_save_path"), lastSavePath).toString();
    lastMaskDir=settings->getQSettings()->value(QString("imfcsimageeditor/last_mask_path"), lastMaskDir).toString();
    //plotter->loadSettings(*(settings->getQSettings()), QString("imfcsimageeditor/corrplot"));
    //chkLogTauAxis->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/log_tau_axis"), true).toBool());
    chkOverviewVisible->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/overview_visible"), true).toBool());
    chkParamImage2Visible->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/image2_visible"), true).toBool());
    chkMaskVisible->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/mask_visible"), false).toBool());
    /*chkKeys->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_keys"), false).toBool());
    int defCDM=0;
    if (current && dynamic_cast<QFRDRImagingFCSData*>(current) && dynamic_cast<QFRDRImagingFCSData*>(current)->isFCCS()) defCDM=2;
    cmbSeletionCorrDisplayMode->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/corr_seldisplaymode"), defCDM).toInt());
    if (cmbSeletionCorrDisplayMode->currentIndex()<0) cmbSeletionCorrDisplayMode->setCurrentIndex(defCDM);
    cmbCrosstalkDirection->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/crosstalk_direction"), 0).toInt());
    cmbCrosstalkMode->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/crosstalk_mode"), 0).toInt());
    spinCrosstalk->setValue(settings->getQSettings()->value(QString("imfcsimageeditor/crosstalk"), 0).toDouble());
    spinCrosstalkAvg->setValue(settings->getQSettings()->value(QString("imfcsimageeditor/crosstalk_avg"), 4).toInt());
    chkDisplayResiduals->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_resid"), true).toBool());
    chkDisplayAverage->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_avg"), true).toBool());
    cmbAverageStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/avg_style"), 0).toInt());
    cmbAverageErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/avg_error_style"), JKQTPerrorPolygons).toInt());*/
    cmbColorbarOverview->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/ovrcolorbar"), JKQTPMathImageGRAY).toInt());
    chkDisplayImageOverlay->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/image_overlays"), true).toBool());
    cmbSelectionStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/image_overlay_style"), 0).toBool());
    //cmbRunStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/run_style"), 1).toInt());
    //cmbRunErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/run_error_style"), JKQTPerrorPolygons).toInt());
    loadSplitter(*(settings->getQSettings()), splitterTop, "imfcsimageeditor/splittertopSizes");
    loadSplitter(*(settings->getQSettings()), splitterTopBot, "imfcsimageeditor/splittertopbotSizes");
    loadSplitter(*(settings->getQSettings()), splitterBot, "imfcsimageeditor/splitterbotSizes");
    corrView->readSettings(*(settings->getQSettings()), "imfcsimageeditor/corrView/");
    histogram->readSettings(*(settings->getQSettings()), "imfcsimageeditor/");
    histogram_2->readSettings(*(settings->getQSettings()), "imfcsimageeditor/histogram2/");
    histogram2->readSettings(*(settings->getQSettings()), "imfcsimageeditor/histogramp2");
    histogram2_2->readSettings(*(settings->getQSettings()), "imfcsimageeditor/histogramp2_2/");
    readAdditionalSettings();
    connectParameterWidgets(true);
    rawDataChanged();
    connectImageWidgets(true);
}

void FRawDataImageEditor::writeSettings()
{
    if (!settings) return;
    settings->getQSettings()->setValue(QString("imfcsimageeditor/last_save_path"), lastSavePath);
    settings->getQSettings()->setValue(QString("imfcsimageeditor/last_mask_path"), lastMaskDir);
    /*settings->getQSettings()->setValue(QString("imfcsimageeditor/corr_seldisplaymode"), cmbSeletionCorrDisplayMode->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/crosstalk_direction"), cmbCrosstalkDirection->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/crosstalk_mode"), cmbCrosstalkMode->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/crosstalk"), spinCrosstalk->value());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/crosstalk_avg"), spinCrosstalkAvg->value());



    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_keys"), chkKeys->isChecked());*/
    settings->getQSettings()->setValue(QString("imfcsimageeditor/image_overlays"), chkDisplayImageOverlay->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/image_overlay_style"), cmbSelectionStyle->currentIndex());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/display_resid"), chkDisplayResiduals->isChecked());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/display_avg"), chkDisplayAverage->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/ovrcolorbar"), cmbColorbarOverview->currentIndex());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/avg_style"), cmbAverageStyle->currentIndex());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/avg_error_style"), cmbAverageErrorStyle->currentIndex());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/run_style"), cmbRunStyle->currentIndex());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/run_error_style"), cmbRunErrorStyle->currentIndex());
    settings->getQSettings()->value(QString("imfcsimageeditor/overview_visible"), chkOverviewVisible->isChecked());
    settings->getQSettings()->value(QString("imfcsimageeditor/image2_visible"), chkParamImage2Visible->isChecked());
    settings->getQSettings()->value(QString("imfcsimageeditor/mask_visible"), chkMaskVisible->isChecked());
    saveSplitter(*(settings->getQSettings()), splitterTop, "imfcsimageeditor/splittertopSizes");
    saveSplitter(*(settings->getQSettings()), splitterBot, "imfcsimageeditor/splitterbotSizes");
    saveSplitter(*(settings->getQSettings()), splitterTopBot, "imfcsimageeditor/splittertopbotSizes");
    corrView->writeSettings(*(settings->getQSettings()), "imfcsimageeditor/corrView/");
    histogram->writeSettings(*(settings->getQSettings()), "imfcsimageeditor/");
    histogram_2->writeSettings(*(settings->getQSettings()), "imfcsimageeditor/histogram2/");
    histogram2->writeSettings(*(settings->getQSettings()), "imfcsimageeditor/histogramp2/");
    histogram2_2->writeSettings(*(settings->getQSettings()), "imfcsimageeditor/histogramp2_2/");

}

QString FRawDataImageEditor::currentEvalGroup() const
{
    int grp=cmbResultGroup->currentIndex();
    if (grp<0) return "";
    return cmbResultGroup->itemData(grp).toString();
}

QString FRawDataImageEditor::currentFitParameter() const
{
    if (cmbParameter->currentIndex()<0) return "";
    return cmbParameter->itemData(cmbParameter->currentIndex()).toString();
}

QString FRawDataImageEditor::currentFit2Parameter() const
{
    if (cmbParameter2->currentIndex()<0) return "";
    return cmbParameter2->itemData(cmbParameter2->currentIndex()).toString();
}

FRawDataImageEditor::ImageTransforms FRawDataImageEditor::currentFitParameterTransfrom() const
{
    if (cmbParameterTransform->currentIndex()<0) return FRawDataImageEditor::itNone;
    return (FRawDataImageEditor::ImageTransforms)cmbParameterTransform->currentIndex();

}

FRawDataImageEditor::ImageTransforms FRawDataImageEditor::currentFit2ParameterTransfrom() const
{
    if (cmbParameter2Transform->currentIndex()<0) return FRawDataImageEditor::itNone;
    return (FRawDataImageEditor::ImageTransforms)cmbParameter2Transform->currentIndex();

}

QString FRawDataImageEditor::formatTransformAndParameter(QComboBox *cmbParameter, QComboBox *cmbTransform)
{
    if (cmbTransform->currentIndex()==1) return tr("abs(%1)").arg(cmbParameter->currentText().replace('_', "{\\_}"));
    else if (cmbTransform->currentIndex()==2) return tr("log_{10}\\left({%1}\\right)").arg(cmbParameter->currentText().replace('_', "{\\_}"));
    else if (cmbTransform->currentIndex()==3) return tr("1/\\left({%1}\\right)").arg(cmbParameter->currentText().replace('_', "{\\_}"));
    else if (cmbTransform->currentIndex()==4) return tr("\\sqrt{%1}").arg(cmbParameter->currentText().replace('_', "{\\_}"));
    else return cmbParameter->currentText().replace('_', "{\\_}");
}


void FRawDataImageEditor::readParameterImage(double *image, uint16_t width, uint16_t height, QString evalGroupIn, QString fitParam, FRawDataImageEditor::ImageTransforms tranFitParam, bool thisRDR, const QString &otherRDRRole, const QString &otherRDRevalGroup)
{
    uint16_t arraysize=width*height;
    //qDebug()<<"readParameterImage("<<image<<gof_image<<width<<height<<evalGroup;
    if (image) {
        for (register uint16_t i=0; i<arraysize; i++) {
            image[i]=NAN;
        }
    }
    QString evalGroup=evalGroupIn;

    QFRawDataRecord* rdr=current;
    if (!thisRDR && rdr && rdr->getGroup()>=0) {
        QFRawDataRecord* mm=getRDRForParameter2(otherRDRRole);
        if (mm) {
            rdr=mm;
            evalGroup=otherRDRevalGroup;
        }
    }

    QFRDRImageToRunInterface* m=qobject_cast<QFRDRImageToRunInterface*>(rdr);

    if ( (!m) || evalGroup.isEmpty() || fitParam.isEmpty() ) return;

#ifdef DEBUG_TIMING
    QElapsedTimer time;
    time.start();
#endif

    QStringList evals=rdr->resultsCalcEvaluationsInGroup(evalGroup);
    //qDebug()<<"evals.size() = "<<evals.size()<<"\n   "<<evals;
    bool readImage=false;
    if (evals.size()>0  && evals.size()<=2) {
        QString usedEval="";
        for (int i=0; i<evals.size(); i++) {
            if (rdr->resultsExists(evals[i], fitParam)) {
                QFRawDataRecord::evaluationResultType typ=rdr->resultsGetType(evals[i], fitParam);

                switch (typ) {
                    case QFRawDataRecord::qfrdreNumberVector:
                    case QFRawDataRecord::qfrdreNumberMatrix:
                    case QFRawDataRecord::qfrdreNumberErrorVector:
                    case QFRawDataRecord::qfrdreNumberErrorMatrix:
                    case QFRawDataRecord::qfrdreIntegerVector:
                    case QFRawDataRecord::qfrdreIntegerMatrix:
                    case QFRawDataRecord::qfrdreBooleanVector:
                    case QFRawDataRecord::qfrdreBooleanMatrix:
                        usedEval=evals[i];
                        break;
                    default:
                        break;
                }
            }
        }
        if (!usedEval.isEmpty()) {
            readImage=true;
            QVector<double> dvec=rdr->resultsGetAsDoubleList(usedEval, fitParam);
            for (uint32_t i=0; i<qMin(dvec.size(), width*height); i++) {
                int x=m->runToX(i);
                int y=m->runToY(i);
                image[y*width+x]=dvec[i];
            }
        }
    }

    if (!readImage) {
        readImage=true;
        for (register int i=0; i<evals.size(); i++) {
            const QString& en=evals[i];
            int grpIdx=rdr->resultsGetEvaluationGroupIndex(en);
            int x=m->runToX(grpIdx);
            int y=m->runToY(grpIdx);
            if (x>=0 && x<width && y>=0 && y<height) {
                if (rdr->resultsExists(en, fitParam)) {
                    image[y*width+x]=rdr->resultsGetAsDouble(en, fitParam);
                }
            }
        }
    }
    transformImage(image, width, height, tranFitParam);
#ifdef DEBUG_TIMING
    //qDebug()<<"QFRDRImagingFCSImageEditor::readParameterImage("<<evalGroup<<fitParam<<") finished after "<<time.elapsed()<<"ms";
#endif
}

void FRawDataImageEditor::transformImage(double *image, uint16_t width, uint16_t height, FRawDataImageEditor::ImageTransforms tranFitParam)
{
    switch(tranFitParam) {
        case FRawDataImageEditor::itAbs: {
                for (int32_t i=0; i<width*height; i++) {
                    image[i]=fabs(image[i]);
                }
            } break;
        case FRawDataImageEditor::itLog: {
                for (int32_t i=0; i<width*height; i++) {
                    if (image[i]>0) image[i]=log(image[i])/log(10.0);
                    else  image[i]=NAN;
                }
            } break;
        case FRawDataImageEditor::itReciprocal: {
                for (int32_t i=0; i<width*height; i++) {
                    if (image[i]!=0) image[i]=1.0/image[i];
                    else  image[i]=NAN;
                }
            } break;
        case FRawDataImageEditor::itSqrt: {
                for (int32_t i=0; i<width*height; i++) {
                    if (image[i]>=0) image[i]=sqrt(image[i]);
                    else  image[i]=NAN;
                }
            } break;
        default: break;
    }
}

bool FRawDataImageEditor::evaluateFitFunction(QFRawDataRecord *current, const double *tau, double *fit, uint32_t N, QStringList &names, QStringList &namelabels, QList<double> &values, QList<double> &errors, QList<bool> &fix, QStringList &units, QStringList &unitlabels, QString evaluation, int index)
{
    QString fitfunc="";
    bool isMatrixResults=false;
    //qDebug()<<evaluation<<fitfunc<<m_fitFunctions.size();
    //qDebug()<<"idx="<<index<<"  eval="<<evaluation;
    if (index<0) {
        fitfunc=current->resultsGetAsString(evaluation, "fit_model_name");
    } else {
        //qDebug()<<current->resultsExists(evaluation, "fit_model_name")<<"   "<<evaluationResultType2String(current->resultsGetType(evaluation, "fit_model_name"));
        if (current->resultsExists(evaluation, "fit_model_name")) {
            switch (current->resultsGetType(evaluation, "fit_model_name")) {
                case QFRawDataRecord::qfrdreStringVector:
                case QFRawDataRecord::qfrdreStringMatrix:
                    fitfunc=current->resultsGetInStringList(evaluation, "fit_model_name", index);
                    isMatrixResults=true;
                    if (fitfunc.isEmpty()) {
                        bool ok=false;
                        QStringList sl=current->resultsGetAsStringList(evaluation, "fit_model_name", &ok);
                        if (ok) {
                            for (int i=0; i<sl.size(); i++) {
                                if (!sl[i].isEmpty()) {
                                    fitfunc=sl[i];
                                    break;
                                }
                            }
                        }
                    }
                    break;
                case QFRawDataRecord::qfrdreString:
                    fitfunc=current->resultsGetAsString(evaluation, "fit_model_name");

                    break;
                default:
                    break;
            }

        }
    }
    //qDebug()<<"evaluateFitFunction()  "<<evaluation<<index<<"  => "<<fitfunc<<isMatrixResults;
    QFFitFunction* ff=m_fitFunctions.value(fitfunc, NULL);
    //qDebug()<<evaluation<<fitfunc<<m_fitFunctions.size()<<ff;
    if (!ff) return false;
    double* params=(double*)calloc(ff->paramCount(),sizeof(double));
    double* errs=(double*)calloc(ff->paramCount(),sizeof(double));
    bool* fixs=(bool*)calloc(ff->paramCount(),sizeof(bool));
    names.clear();
    namelabels.clear();
    values.clear();
    errors.clear();
    fix.clear();
    units.clear();
    unitlabels.clear();

    QStringList pids;
    bool recheckedmatrix=false;
    //qDebug()<<"ff->paramCount()="<<ff->paramCount();
    for (int i=0; i<ff->paramCount(); i++) {
        QString id=ff->getParameterID(i);
        pids.append(id);
        errs[i]=0;
        fixs[i]=ff->getDescription(id).initialFix;
        params[i]=ff->getDescription(id).initialValue;
        /*double value=0;
        if (overrideFitFunctionPreset(id, value)) params[i]=value;*/

        if (!isMatrixResults) {
            if (current->resultsExists(evaluation, "fitparam_"+id)) {
                params[i]=current->resultsGetAsDouble(evaluation, "fitparam_"+id);
                errs[i]=current->resultsGetErrorAsDouble(evaluation, "fitparam_"+id);
            }
            if (current->resultsExists(evaluation, "fitparam_"+id+"_fix")) {
                fixs[i]=current->resultsGetAsBoolean(evaluation, "fitparam_"+id+"_fix");
            }
            if (!recheckedmatrix && current->resultsGet(evaluation, "fitparam_"+id).getVectorMatrixItems()>1) {
                isMatrixResults=true;
                recheckedmatrix=recheckedmatrix;
            }
        }
        if (isMatrixResults){
            if (current->resultsExists(evaluation, "fitparam_"+id)) {
                params[i]=current->resultsGetInNumberList(evaluation, "fitparam_"+id, index);
                errs[i]=current->resultsGetErrorInNumberErrorList(evaluation, "fitparam_"+id, index);
            }
            if (current->resultsExists(evaluation, "fitparam_"+id+"_fix")) {
                fixs[i]=current->resultsGetInBooleanList(evaluation, "fitparam_"+id+"_fix", index);
            }
        }
    }
    for (int i=0; i<ff->paramCount(); i++) {
        QString id=ff->getParameterID(i);
        if (ff->isParameterVisible(i, params)) {
            namelabels.append(ff->getDescription(id).label);
            unitlabels.append(ff->getDescription(id).unitLabel);
            names.append(ff->getDescription(id).name);
            units.append(ff->getDescription(id).unit);
            values.append(params[i]);
            errors.append(errs[i]);
            fix.append(fixs[i]);
        }
    }

    if (tau && fit) {
        for (unsigned int i=0; i<N; i++) {
            fit[i]=ff->evaluate(tau[i], params);
        }
    }

    free(params);
    free(errs);
    free(fixs);
    return true;
}

void FRawDataImageEditor::fillParameterComboBox(QComboBox *cmbParameter, QFRawDataRecord *m, const QString &egroup, const QStringList &param1Default, const QString &otherParameter, const QString &indexPropertyName, const QString &parameterDefault, int idxDefault)
{
    cmbParameter->setEnabled(true);
    QList<QPair<QString, QString> > params=m->resultsCalcNamesAndLabels("", "fit results", egroup);
    QList<QPair<QString, QString> > params1=m->resultsCalcNamesAndLabels("", "results", egroup);
    QList<QPair<QString, QString> > params2=m->resultsCalcNamesAndLabels("", "evaluation results", egroup);
    params.append(params1);
    params.append(params2);
    cmbParameter->clear();
    for (int i=0; i<params.size(); i++) {
        if (!params[i].second.endsWith("_fix")) {
            cmbParameter->addItem(params[i].first, params[i].second);
            //qDebug()<<params[i].second;
        }
    }
    for (int i=0; i<params.size(); i++) {
        if (params[i].second.endsWith("_fix")) {
            cmbParameter->addItem(params[i].first, params[i].second);
        }
    }

    params=m->resultsCalcNamesAndLabels("", "fit properties", egroup);
    for (int i=0; i<params.size(); i++) {
        if (!params[i].second.endsWith("_fix")) {
            cmbParameter->addItem(params[i].first, params[i].second);
        }
    }
    for (int i=0; i<params.size(); i++) {
        if (params[i].second.endsWith("_fix")) {
            cmbParameter->addItem(params[i].first, params[i].second);
        }
    }
    int d=cmbParameter->findData(current->getProperty(indexPropertyName, param1Default.value(0, parameterDefault)));
    for (int i=1; i<param1Default.size(); i++) {
        if (d<0 || param1Default[i]==otherParameter) d=cmbParameter->findData(param1Default[i]);
    }
    if (d>=0) cmbParameter->setCurrentIndex(d);
    else cmbParameter->setCurrentIndex(idxDefault);
    //int d1=cmbParameter->currentIndex();

}

QFRawDataRecord *FRawDataImageEditor::getRDRForParameter2(const QString &otherRDRRole) const
{
    QFRDRImageToRunInterface* m=qobject_cast<QFRDRImageToRunInterface*>(current);
    QFRawDataRecord *out=current;

    if (chkOtherFileP2->isChecked() && current && current->getGroup()>=0) {
        //QString otherRDRRole=cmbOtherFileRole->itemData(cmbOtherFileRole->currentIndex()).toString();
        //evalGroup=cmbOtherFilesResultGroup->itemData(cmbOtherFilesResultGroup->currentIndex()).toString();
        QList<QFRawDataRecord*> rdrs=current->getProject()->getRDRGroupMembers(current->getGroup());
        int width=m->getImageFromRunsWidth();
        int height=m->getImageFromRunsHeight();
        for (int i=0; i<rdrs.size(); i++) {
            QFRDRImageToRunInterface* mm=qobject_cast<QFRDRImageToRunInterface*>(rdrs[i]);
            if (mm && rdrs[i]->getRole().toUpper()==otherRDRRole.toUpper()) {
                if (mm->getImageFromRunsWidth()==width && mm->getImageFromRunsHeight()==height) {
                    m=mm;
                    out=rdrs[i];
                    break;
                }
            }
            mm=NULL;
        }
    }

    return out;
}

QString FRawDataImageEditor::selectionToString()
{
    if (!current) return "";
    QFRDRImageToRunInterface* m=qobject_cast<QFRDRImageToRunInterface*>(current);
    if (!m) return "";

    QString data="";
    QTextStream str(&data);
    QSet<int32_t>::iterator i = selected.begin();
    while (i != selected.end()) {
        int x=m->runToX(*i);
        int y=m->runToY(*i);
        str<<x<<", "<<y<<"\n";
        ++i;
    }
    return data;
}

void FRawDataImageEditor::loadSelectionFromString(QString sel)
{
    if (!current) return;
    QFRDRImageToRunInterface* m=qobject_cast<QFRDRImageToRunInterface*>(current);
    if (!m) return;
    QTextStream str(&sel);
    selected.clear();
    while (!str.atEnd())  {
        QVector<double> d=csvReadline(str, ',', '#', -1);
        if (d.size()==2) {
            int idx=m->xyToRun(d[0], d[1]);
            if (idx>=0 && idx<m->getImageFromRunsWidth()*m->getImageFromRunsHeight()) selectedInsert(idx);
        }
    }
}

void FRawDataImageEditor::showHidePlots()
{
    bool changed=  (splitterTop->widget(1)->isVisible() != chkParamImage2Visible->isChecked())
                 ||(splitterTop->widget(2)->isVisible() != chkMaskVisible->isChecked())
                 ||(splitterTop->widget(3)->isVisible() != chkOverviewVisible->isChecked());

    if (changed) {
        int oldcnt=1;
        if (splitterTop->widget(1)->isVisible()) oldcnt++;
        if (splitterTop->widget(2)->isVisible()) oldcnt++;
        if (splitterTop->widget(3)->isVisible()) oldcnt++;

        int cnt=1;
        if (chkParamImage2Visible->isChecked()) cnt++;
        if (chkMaskVisible->isChecked()) cnt++;
        if (chkOverviewVisible->isChecked()) cnt++;

        splitterTop->widget(1)->setVisible(chkParamImage2Visible->isChecked());
        splitterTop->widget(2)->setVisible(chkMaskVisible->isChecked());
        splitterTop->widget(3)->setVisible(chkOverviewVisible->isChecked());

        if (cnt!=oldcnt) {
            QList<int> list;
            int corrWidth=splitterTop->width()-(cnt-1)*splitterTop->handleWidth();
            for (int i=0; i<splitterTop->count(); i++) {
                if (splitterTop->widget(i)->isVisible()) list<<corrWidth/cnt;
                else list<<0;
            }
            splitterTop->setSizes(list);
        }
    }
}

void FRawDataImageEditor::moveColorbarsAuto()
{

}

void FRawDataImageEditor::rawDataChanged()
{

}

void FRawDataImageEditor::maskChanged()
{

}

void FRawDataImageEditor::updateAfterClick()
{

}

void FRawDataImageEditor::connectWidgets(QFRawDataRecord *current, QFRawDataRecord *old)
{

}

void FRawDataImageEditor::replotHistogram()
{

}

void FRawDataImageEditor::updateHistogram()
{

}

void FRawDataImageEditor::updateHistogram(QFHistogramView *histogram, QFRawDataRecord *m, double *plteImageData, int32_t plteImageSize, bool excludeExcluded, bool dv, bool selHistogram)
{

}

void FRawDataImageEditor::updateCorrelation(QFParameterCorrelationView *corrView, QFRawDataRecord *m, double *data1, double *data2, int32_t plteImageSize, bool excludeExcluded, bool dv, bool selHistogram, int mode, int channel, const QString &label1, const QString label2, int width, int height)
{

}

void FRawDataImageEditor::updateSelectionHistogram(bool replot)
{

}

void FRawDataImageEditor::replotData()
{

}

void FRawDataImageEditor::replotImage()
{

}

void FRawDataImageEditor::replotSelection(bool replot)
{

}

void FRawDataImageEditor::updateSelectionArrays()
{

}

void FRawDataImageEditor::replotOverview()
{

}

void FRawDataImageEditor::replotMask()
{

}

void FRawDataImageEditor::excludeRuns()
{

}

void FRawDataImageEditor::includeRuns()
{

}

void FRawDataImageEditor::excludeByIntensity()
{

}

void FRawDataImageEditor::excludeByParamIntensity()
{

}

void FRawDataImageEditor::excludeByParan2Intensity()
{

}

void FRawDataImageEditor::saveSelection()
{

}

void FRawDataImageEditor::loadSelection()
{

}

void FRawDataImageEditor::copySelection()
{

}

void FRawDataImageEditor::pasteSelection()
{

}

void FRawDataImageEditor::includeAll()
{

}

void FRawDataImageEditor::invertMask()
{

}

void FRawDataImageEditor::slidersChanged(int userMin, int userMax, int min, int max)
{

}

void FRawDataImageEditor::imageClicked(double x, double y, Qt::KeyboardModifiers modifiers)
{

}

void FRawDataImageEditor::imageScribbled(double x, double y, Qt::KeyboardModifiers modifiers, bool first, bool last)
{

}

void FRawDataImageEditor::imageMouseMoved(double x, double y)
{

}

void FRawDataImageEditor::imageRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers)
{

}

void FRawDataImageEditor::imageLineFinished(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers)
{

}

void FRawDataImageEditor::imageCircleFinished(double x, double y, double radius, Qt::KeyboardModifiers modifiers)
{

}

void FRawDataImageEditor::imageEllipseFinished(double x, double y, double radiusX, double radiusY, Qt::KeyboardModifiers modifiers)
{

}

void FRawDataImageEditor::setImageEditMode()
{

}

void FRawDataImageEditor::parameterSetChanged()
{

}

void FRawDataImageEditor::parameterChanged()
{

}

void FRawDataImageEditor::transformChanged()
{

}

void FRawDataImageEditor::clearImage()
{

}

void FRawDataImageEditor::fillParameterSet()
{

}

void FRawDataImageEditor::connectParameterWidgets(bool connectTo)
{

}

void FRawDataImageEditor::connectImageWidgets(bool connectTo)
{

}

void FRawDataImageEditor::ovrPaletteChanged()
{

}

void FRawDataImageEditor::histogramSettingsChanged()
{

}

void FRawDataImageEditor::updateOverlaySettings()
{

}

void FRawDataImageEditor::saveImageSettings()
{

}

void FRawDataImageEditor::loadImageSettings()
{

}

void FRawDataImageEditor::imageZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter *sender)
{

}

void FRawDataImageEditor::resultsChanged(const QString &evalName, const QString &resultName, bool deleted)
{

}

void FRawDataImageEditor::debugInfo()
{

}

void FRawDataImageEditor::saveReport()
{

}

void FRawDataImageEditor::saveData()
{

}

void FRawDataImageEditor::copyToMatlab()
{

}

void FRawDataImageEditor::copyDataAsColumns()
{

}

void FRawDataImageEditor::printReport()
{

}

void FRawDataImageEditor::setCopyableData()
{

}

void FRawDataImageEditor::updateSelectionCombobox()
{

}

void FRawDataImageEditor::selectedSelectionInCombo(int index)
{

}

void FRawDataImageEditor::storeSelection()
{

}

void FRawDataImageEditor::deleteSelection()
{

}

void FRawDataImageEditor::selectionEdited()
{

}

void FRawDataImageEditor::dualviewChanged(int mode)
{

}

void FRawDataImageEditor::copyFitResultStatistics()
{

}

void FRawDataImageEditor::copyMaskToAll()
{

}



void FRawDataImageEditor::createReportDoc(QTextDocument *document)
{

}







void FRawDataImageEditor::createWidgets()
{


    if (!correlationMaskTools) {
        correlationMaskTools=new QFRDRImageMaskEditTools(this);
        connect(correlationMaskTools, SIGNAL(rawDataChanged()), this, SLOT(maskChanged()));
    }

    setUpdatesEnabled(false);


    QLabel* l;
    QGridLayout* lb=new QGridLayout(this);
    setLayout(lb);


    QColor ovlSelCol=QColor("red");
    ovlSelCol.setAlphaF(0.5);
    selectionColor=ovlSelCol;
    QColor ovlExCol=QColor("blue");
    ovlExCol.setAlphaF(0.5);
    excludedColor=ovlExCol;



    ///////////////////////////////////////////////////////////////
    // GROUPBOX: parameter selection group box
    ///////////////////////////////////////////////////////////////

    QGroupBox* grpTop=new QGroupBox(tr(" Parameter Image Plot Settings "), this);
    grpTop->setFlat(true);
    QGridLayout* topgrid=new QGridLayout(this);
    grpTop->setLayout(topgrid);

    int row=0;
    cmbResultGroup=new QComboBox(this);
    cmbResultGroup->setMaximumWidth(700);
    cmbResultGroup->view()->setTextElideMode(Qt::ElideMiddle);
    cmbResultGroup->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    topgrid->addWidget((l=new QLabel(tr("&result set:"))), row, 0);
    l->setBuddy(cmbResultGroup);
    topgrid->addWidget(cmbResultGroup, row, 1);

    cmbParameter=new QComboBox(this);
    cmbParameter->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    topgrid->addWidget((labParameter=new QLabel(tr("&parameter:"))), row, 3);
    labParameter->setBuddy(cmbParameter);
    topgrid->addWidget(cmbParameter, row, 4);
    cmbParameterTransform=new QComboBox(this);
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/none.png"), tr("none"));
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/abs.png"), tr("abs"));
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/log.png"), tr("log"));
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/reci.png"), tr("reciprocal"));
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/sqrt.png"), tr("sqrt"));
    topgrid->addWidget((labParameterTransform=new QLabel(tr("    &transform:"))), row, 5);
    labParameterTransform->setBuddy(cmbParameterTransform);
    topgrid->addWidget(cmbParameterTransform, row, 6);

    row++;
    QHBoxLayout* hblp2=new QHBoxLayout(this);
    topgrid->addLayout(hblp2, row, 3, 1, 4);
    chkOtherFileP2=new QCheckBox(tr("other RDR for parameter 2, role:"), this);
    hblp2->addWidget(chkOtherFileP2);
    cmbOtherFileRole=new QComboBox(this);
    cmbOtherFileRole->setEnabled(false);
    hblp2->addWidget(cmbOtherFileRole);
    hblp2->addWidget(new QLabel(tr("result set:")));
    cmbOtherFilesResultGroup=new QComboBox(this);
    cmbOtherFilesResultGroup->setMaximumWidth(700);
    cmbOtherFilesResultGroup->view()->setTextElideMode(Qt::ElideMiddle);
    cmbOtherFilesResultGroup->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    cmbOtherFilesResultGroup->setEnabled(false);
    hblp2->addWidget(cmbOtherFilesResultGroup);
    connect(chkOtherFileP2, SIGNAL(toggled(bool)), cmbOtherFileRole, SLOT(setEnabled(bool)));
    connect(chkOtherFileP2, SIGNAL(toggled(bool)), cmbOtherFilesResultGroup, SLOT(setEnabled(bool)));


    row++;
    cmbParameter2=new QComboBox(this);
    cmbParameter2->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    topgrid->addWidget((labParameter2=new QLabel(tr("paramater &2:"))), row, 3);
    labParameter2->setBuddy(cmbParameter2);
    topgrid->addWidget(cmbParameter2, row, 4);
    cmbParameter2Transform=new QComboBox(this);
    cmbParameter2Transform->addItem(QIcon(":/imaging_fcs/none.png"), tr("none"));
    cmbParameter2Transform->addItem(QIcon(":/imaging_fcs/abs.png"), tr("abs"));
    cmbParameter2Transform->addItem(QIcon(":/imaging_fcs/log.png"), tr("log"));
    cmbParameter2Transform->addItem(QIcon(":/imaging_fcs/reci.png"), tr("reciprocal"));
    cmbParameter2Transform->addItem(QIcon(":/imaging_fcs/sqrt.png"), tr("sqrt"));
    topgrid->addWidget((labParameter2Transform=new QLabel(tr("    tr&ansform:"))), row, 5);
    labParameter2Transform->setBuddy(cmbParameter2Transform);
    topgrid->addWidget(cmbParameter2Transform, row, 6);



    topgrid->addWidget(new QWidget(), 0, 2);
    topgrid->addWidget(new QWidget(), 0, 5);
    topgrid->addWidget(new QWidget(), 0, 7);
    topgrid->setColumnStretch(0, 0);
    topgrid->setColumnStretch(1, 0);
    topgrid->setColumnStretch(2, 1);
    topgrid->setColumnStretch(3, 0);
    topgrid->setColumnStretch(4, 0);
    topgrid->setColumnStretch(5, 0);
    topgrid->setColumnStretch(6, 0);
    topgrid->setColumnStretch(7, 1);


    JKVerticalScrollArea* area=new JKVerticalScrollArea(this);
    QWidget* w=new QWidget(this);
    w->setMinimumWidth(300);
    area->setWidgetResizable(true);
    area->setWidget(w);
    QVBoxLayout* vbl=new QVBoxLayout();
    w->setLayout(vbl);
    QString stylesheetGroupBox=
            QString("QGroupBox {"
                    //"  background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 %2);"
                    "  border-top: 2px solid gray;"
                    "  border-left: 2px solid gray;"
                    "  border-radius: 2px;"
                    "  margin-top: 1ex; "
                    "  margin-left: 1ex; "
                    "  font-weight: bold;"
                    "} "
                    "QGroupBox::title {"
                    "  subcontrol-origin: margin;"
                    "  subcontrol-position: top left; "
                    "  padding: 0 8px;"
                    //"  border: 1px solid gray;"
                    "}").arg(palette().color(QPalette::Window).name()).arg(palette().color(QPalette::Window).lighter(120).name());
    w->setStyleSheet(w->styleSheet()+QString("\n\n")+stylesheetGroupBox);







    ///////////////////////////////////////////////////////////////
    // GROUPBOX: visible plots
    ///////////////////////////////////////////////////////////////
    QGroupBox* grpVisiblePlots=new QGroupBox(tr(" general options "), this);
    grpVisiblePlots->setFlat(true);
    QGridLayout* glVisPlots=new QGridLayout(this);
    grpVisiblePlots->setLayout(glVisPlots);
    chkOverviewVisible=new QCheckBox(tr("overview"), grpVisiblePlots);
    chkOverviewVisible->setChecked(true);
    chkParamImage2Visible=new QCheckBox(tr("parameter 2"), grpVisiblePlots);
    chkParamImage2Visible->setChecked(false);
    chkMaskVisible=new QCheckBox(tr("mask"), grpVisiblePlots);
    chkMaskVisible->setChecked(false);
    glVisPlots->addWidget(chkOverviewVisible, 0,0);
    glVisPlots->addWidget(chkParamImage2Visible, 0,1);
    glVisPlots->addWidget(chkMaskVisible, 0,2);

    cmbDualView=new QComboBox(this);
    cmbDualView->addItem(QIcon(":/imaging_fcs/dvnone.png"), tr("none"));
    cmbDualView->addItem(QIcon(":/imaging_fcs/dvhor.png"), tr("split horizontal"));
    cmbDualView->addItem(QIcon(":/imaging_fcs/dvver.png"), tr("split vertical"));
    glVisPlots->addWidget(new QLabel("DualView mode:"), 1, 0);
    glVisPlots->addWidget(cmbDualView, 1, 1, 1, 2);
    vbl->addWidget(grpVisiblePlots);



    ///////////////////////////////////////////////////////////////
    // GROUPBOX: mask options
    ///////////////////////////////////////////////////////////////
    QGroupBox* wmask=new QGroupBox(tr(" mask options "), this);
    wmask->setFlat(true);
    //wmask->setStyleSheet(wmask->styleSheet()+QString("\n\n")+ stylesheetGroupBox);
    vbl->addWidget(wmask);
    QGridLayout* glmask=new QGridLayout(this);
    glmask->setHorizontalSpacing(2);
    glmask->setVerticalSpacing(2);
    wmask->setLayout(glmask);

    int mskgrpRow=0;
    btnDontUse=createButtonAndActionShowText(actDontUse, QIcon(":/imaging_fcs/mask.png"), tr("&mask sel."), w);
    actDontUse->setToolTip(tr("add the selected pixels to the current mask (so don't use it's data)\nand recalculate the average correlation curve accordingly"));
    connect(actDontUse, SIGNAL(triggered()), this, SLOT(excludeRuns()));
    glmask->addWidget(btnDontUse, mskgrpRow, 0);
    btnUse=createButtonAndActionShowText(actUse, QIcon(":/imaging_fcs/unmask.png"), tr("&unmask sel."), w);
    actUse->setToolTip(tr("remove the selected pixels from the current mask (so use it's data)\nand recalculate the average correlation curve accordingly"));
    connect(actUse, SIGNAL(triggered()), this, SLOT(includeRuns()));
    glmask->addWidget(btnUse, mskgrpRow, 1);

    btnUseAll=createButtonAndActionShowText(actClearMask, QIcon(":/imaging_fcs/clearmask.png"), tr("&clear "), w);
    actClearMask->setToolTip(tr("clear the mask and recalculate the average correlation curve accordingly"));
    glmask->addWidget(btnUseAll, mskgrpRow, 2);
    connect(actClearMask, SIGNAL(triggered()), this, SLOT(includeAll()));
    btnInvertMask=createButtonAndActionShowText(actInvertMask, QIcon(":/imaging_fcs/invertmask.png"), tr("&invert mask"), w);
    actInvertMask->setToolTip(tr("invert the current mask (all masked pixel are unmasked and vice versa)\nand recalculate the average correlation curve accordingly"));
    glmask->addWidget(btnInvertMask, mskgrpRow, 3);
    connect(actInvertMask, SIGNAL(triggered()), this, SLOT(invertMask()));
    mskgrpRow++;


    btnSaveMask=createButtonForActionShowText(correlationMaskTools->get_actSaveMask(), w);
    glmask->addWidget(btnSaveMask, mskgrpRow, 0);
    btnLoadMask=createButtonForActionShowText(correlationMaskTools->get_actLoadMask(), w);
    glmask->addWidget(btnLoadMask, mskgrpRow, 1);
    btnCopyMask=createButtonForActionShowText(correlationMaskTools->get_actCopyMask(), w);
    glmask->addWidget(btnCopyMask, mskgrpRow, 2);
    btnPasteMask=createButtonForActionShowText(correlationMaskTools->get_actPasteMask(), w);
    glmask->addWidget(btnPasteMask, mskgrpRow, 3);


    mskgrpRow++;
    QFrame* frame=new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    glmask->addWidget(frame, mskgrpRow, 0, 1, 4);

    mskgrpRow++;
    btnMaskByIntensity=createButtonAndActionShowText(actMaskByIntensity, tr("m. by &overview"), w);
    actMaskByIntensity->setToolTip(tr("create a mask according to the <b>overview image</b>:\n"
                                      "A dialog will open up, which allows to mask some pixels\n"
                                      "according to a given threshold. The mask created by this\n"
                                      "is combined with the current mask using the set <i>mask edit mode</i>"));
    glmask->addWidget(btnMaskByIntensity, mskgrpRow, 0);
    connect(actMaskByIntensity, SIGNAL(triggered()), this, SLOT(excludeByIntensity()));
    btnMaskByParam2Intensity=createButtonAndActionShowText(actMaskByParam2Intensity, tr("m. by param &2"), w);
    actMaskByParam2Intensity->setToolTip(tr("create a mask according to the <b>parameter image 2</b>:\n"
                                      "A dialog will open up, which allows to mask some pixels\n"
                                      "according to a given threshold. The mask created by this\n"
                                      "is combined with the current mask using the set <i>mask edit mode</i>"));
    glmask->addWidget(btnMaskByParam2Intensity, mskgrpRow, 1);
    connect(actMaskByParam2Intensity, SIGNAL(triggered()), this, SLOT(excludeByParan2Intensity()));
    btnMaskByParamIntensity=createButtonAndActionShowText(actMaskByParamIntensity, tr("m. by &param"), w);
    actMaskByParamIntensity->setToolTip(tr("create a mask according to the <b>parameter image</b>:\n"
                                      "A dialog will open up, which allows to mask some pixels\n"
                                      "according to a given threshold. The mask created by this\n"
                                      "is combined with the current mask using the set <i>mask edit mode</i>"));
    glmask->addWidget(btnMaskByParamIntensity, mskgrpRow, 2);
    connect(actMaskByParamIntensity, SIGNAL(triggered()), this, SLOT(excludeByParamIntensity()));

    actCopyMaskToAll=new QAction(tr("copy mask to files"), this);
    connect(actCopyMaskToAll, SIGNAL(triggered()), this, SLOT(copyMaskToAll()));


    QGroupBox* wsel=new QGroupBox(tr(" selection options "), this);
    wsel->setFlat(true);
    //wsel->setStyleSheet(wsel->styleSheet()+QString("\n\n")+ stylesheetGroupBox);
    vbl->addWidget(wsel);
    QGridLayout* glsel=new QGridLayout(this);
    glsel->setHorizontalSpacing(2);
    glsel->setVerticalSpacing(2);
    wsel->setLayout(glsel);

    int selgrpRow=0;

    btnSaveSelection=createButtonAndActionShowText(actSaveSelection, QIcon(":/imaging_fcs/saveselection.png"), tr("&save sel."), w);
    actSaveSelection->setToolTip(tr("save the selection to harddisk"));
    glsel->addWidget(btnSaveSelection, selgrpRow, 0);
    connect(actSaveSelection, SIGNAL(triggered()), this, SLOT(saveSelection()));
    btnLoadSelection=createButtonAndActionShowText(actLoadSelection, QIcon(":/imaging_fcs/loadselection.png"), tr("&load sel."), w);
    actLoadSelection->setToolTip(tr("load a selection from harddisk"));
    glsel->addWidget(btnLoadSelection, selgrpRow, 1);
    connect(actLoadSelection, SIGNAL(triggered()), this, SLOT(loadSelection()));


    btnCopySelection=createButtonAndActionShowText(actCopySelection, QIcon(":/imaging_fcs/copyselection.png"), tr("&copy sel."), w);
    actCopySelection->setToolTip(tr("copy the selection to clipboard"));
    glsel->addWidget(btnCopySelection, selgrpRow, 2);
    connect(actCopySelection, SIGNAL(triggered()), this, SLOT(copySelection()));
    btnPasteSelection=createButtonAndActionShowText(actPasteSelection, QIcon(":/imaging_fcs/pasteselection.png"), tr("&paste sel."), w);
    actPasteSelection->setToolTip(tr("paste a selection from clipboard"));
    glsel->addWidget(btnPasteSelection, selgrpRow, 3);
    connect(actPasteSelection, SIGNAL(triggered()), this, SLOT(pasteSelection()));
    selgrpRow++;
    frame=new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    glsel->addWidget(frame, selgrpRow, 0, 1, 4);
    selgrpRow++;

    glsel->addWidget(new QLabel(tr("stored selections:")), selgrpRow, 0);
    QWidget* ssel=new QWidget(this);
    QHBoxLayout* ssell=new QHBoxLayout(this);
    ssell->setContentsMargins(0,0,0,0);
    ssel->setLayout(ssell);
    glsel->addWidget(ssel, selgrpRow, 1,1,3);
    selgrpRow++;
    cmbStoredSelections=new QComboBox(this);
    ssell->addWidget(cmbStoredSelections, 1);
    btnSaveSelectionToStored=createButtonAndActionShowText(actSaveSelectionToStored, QIcon(":/imaging_fcs/storeselection.png"), tr("store selection "), w);
    btnSaveSelectionToStored->setToolButtonStyle(Qt::ToolButtonIconOnly);
    actSaveSelectionToStored->setToolTip(tr("save the current selection under a new name to the drop-down field above (record)"));
    ssell->addWidget(btnSaveSelectionToStored);
    btnDeleteStoredSelection=createButtonAndActionShowText(actDeleteStoredSelection, QIcon(":/imaging_fcs/deleteselection.png"), tr("delete selection "), w);
    btnDeleteStoredSelection->setToolButtonStyle(Qt::ToolButtonIconOnly);
    actDeleteStoredSelection->setToolTip(tr("delete the current named selection from the drop-down field above"));
    ssell->addWidget(btnDeleteStoredSelection);
    connect(actDeleteStoredSelection, SIGNAL(triggered()), this, SLOT(deleteSelection()));
    connect(actSaveSelectionToStored, SIGNAL(triggered()), this, SLOT(storeSelection()));
    connect(cmbStoredSelections, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedSelectionInCombo(int)));




    QFormLayout* gli;
    QHBoxLayout* coll;


    ///////////////////////////////////////////////////////////////
    // GROUPBOX: parameter image style
    ///////////////////////////////////////////////////////////////
    grpImage=new QFImageParameterGroupBox(tr(" parameter image style "), this);
    vbl->addWidget(grpImage);
    //grpImage->setStyleSheet(grpImage->styleSheet()+QString("\n\n")+ stylesheetGroupBox);


    QGroupBox* wsels=new QGroupBox(tr(" selection style "), this);
    gli=new QFormLayout(this);
    wsels->setLayout(gli);
    chkDisplayImageOverlay=new QCheckBox(wsels);
    gli->addRow(tr("&enabled:"), chkDisplayImageOverlay);
    wsels->setFlat(true);
    vbl->addWidget(wsels);
    cmbSelectionStyle=new QFOverlayStyleCombobox(wsels);
    gli->addRow(tr("&style:"), cmbSelectionStyle);

    ///////////////////////////////////////////////////////////////
    // GROUPBOX: parameter image style
    ///////////////////////////////////////////////////////////////
    grpImage2=new QFImageParameterGroupBox(tr(" parameter 2 image style "), this);
    vbl->addWidget(grpImage2);
    //connect(chkParamImage2Visible, SIGNAL(toggled(bool)), grpImage2, SLOT(setVisible(bool)));
    //grpImage2->setVisible(chkParamImage2Visible->isChecked());


    ///////////////////////////////////////////////////////////////
    // GROUPBOX: overview image style
    ///////////////////////////////////////////////////////////////
    QGroupBox* wovr=new QGroupBox(tr(" overview image style "), this);
    wovr->setFlat(true);
    vbl->addWidget(wovr);
    gli=new QFormLayout(this);
    wovr->setLayout(gli);

    cmbColorbarOverview=new JKQTPMathImageColorPaletteComboBox(wovr);
    gli->addRow(tr("color &palette:"), cmbColorbarOverview);

    chkAutorangeOverview=new QCheckBox("auto", wovr);
    chkAutorangeOverview->setChecked(true);
    gli->addRow(tr("channel 1 &range:"), chkAutorangeOverview);

    edtOvrMin=new QFDoubleEdit(wovr);
    edtOvrMin->setCheckBounds(false, false);
    edtOvrMax=new QFDoubleEdit(wovr);
    edtOvrMax->setCheckBounds(false, false);
    coll=new QHBoxLayout();
    coll->addWidget(edtOvrMin,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtOvrMax,1);
    coll->setContentsMargins(0,0,0,0);
    gli->addRow(QString(""), coll);

    edtOvr2Min=new QFDoubleEdit(wovr);
    edtOvr2Min->setCheckBounds(false, false);
    edtOvr2Max=new QFDoubleEdit(wovr);
    edtOvr2Max->setCheckBounds(false, false);
    coll=new QHBoxLayout();
    coll->addWidget(edtOvr2Min,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtOvr2Max,1);
    coll->setContentsMargins(0,0,0,0);
    gli->addRow(QString("channel 2 range:"), coll);




    ///////////////////////////////////////////////////////////////
    // GROUPBOX: correlation plot styles
    ///////////////////////////////////////////////////////////////
    /*QGroupBox* wcp=new QGroupBox(tr(" correlation plot styles "), this);
    wcp->setFlat(true);
    //wcp->setStyleSheet(wcp->styleSheet()+QString("\n\n")+ stylesheetGroupBox);
    vbl->addWidget(wcp);
    QFormLayout* gl=new QFormLayout(this);
    gl->setSpacing(2);

    wcp->setLayout(gl);

    QHBoxLayout* cpsHBox=new QHBoxLayout(this);
    cpsHBox->setContentsMargins(0,0,0,0);
    //chkLogTauAxis=new QCheckBox("log tau-axis", w);

    chkDisplayAverage=new QCheckBox(w);
    chkDisplayAverage->setChecked(true);
    cpsHBox->addWidget(chkDisplayAverage);
    cpsHBox->addWidget(chkLogTauAxis);
    l->setTextFormat(Qt::RichText);
    l->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    l->setBuddy(chkLogTauAxis);

    gl->addRow(tr("display average:"), cpsHBox);

    cmbAverageStyle=new JKQTPLinePlotStyleComboBox(w);

    cmbAverageErrorStyle=new JKQTPerrorPlotstyleComboBox(w);
    cpsHBox=new QHBoxLayout(this);
    cpsHBox->setContentsMargins(0,0,0,0);
    cpsHBox->addWidget(cmbAverageStyle);
    cpsHBox->addWidget(cmbAverageErrorStyle);
    gl->addRow((l=new QLabel(tr("average options:"))), cpsHBox);
    l->setBuddy(cmbAverageStyle);
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), cmbAverageStyle, SLOT(setEnabled(bool)));
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), cmbAverageErrorStyle, SLOT(setEnabled(bool)));
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), l, SLOT(setEnabled(bool)));

    cmbRunStyle=new JKQTPLinePlotStyleComboBox(w);

    cmbRunErrorStyle=new JKQTPerrorPlotstyleComboBox(w);
    cpsHBox=new QHBoxLayout(this);
    cpsHBox->setContentsMargins(0,0,0,0);
    cpsHBox->addWidget(cmbRunStyle);
    cpsHBox->addWidget(cmbRunErrorStyle);
    gl->addRow((labRunOptions=new QLabel(tr("pixel options:"))), cpsHBox);
    labRunOptions->setBuddy(cmbRunStyle);


    cpsHBox=new QHBoxLayout(this);
    cpsHBox->setContentsMargins(0,0,0,0);
    chkDisplayResiduals=new QCheckBox(w);
    chkDisplayResiduals->setChecked(true);

    chkKeys=new QCheckBox(tr("display &keys"), w);
    chkKeys->setChecked(true);

    cpsHBox->addWidget(chkDisplayResiduals);
    cpsHBox->addWidget(chkKeys);
    l->setTextFormat(Qt::RichText);
    l->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    l->setBuddy(chkKeys);
    gl->addRow(tr("display residuals:"), cpsHBox);

    cmbSeletionCorrDisplayMode=new QComboBox(this);
    cmbSeletionCorrDisplayMode->addItem(tr("display average"));
    cmbSeletionCorrDisplayMode->addItem(tr("display all"));
    cmbSeletionCorrDisplayMode->addItem(tr("FCCS display"));
    gl->addRow(tr("selection display:"), cmbSeletionCorrDisplayMode);

    QWidget* wcrosstalk=new QWidget(this);
    QGridLayout* wclayout=new QGridLayout(wcrosstalk);
    wcrosstalk->setLayout(wclayout);
    wclayout->setContentsMargins(0,0,0,0);
    spinCrosstalk=new QDoubleSpinBox(wcrosstalk);
    spinCrosstalk->setRange(0,1000);
    spinCrosstalk->setSuffix(tr(" %"));
    spinCrosstalk->setValue(0);
    wclayout->addWidget(spinCrosstalk,0,0,1,2);
    cmbCrosstalkDirection=new QComboBox(wcrosstalk);
    cmbCrosstalkDirection->addItem(tr("0 -> 1"));
    cmbCrosstalkDirection->addItem(tr("1 -> 0"));
    cmbCrosstalkDirection->addItem(tr("none"));
    cmbCrosstalkDirection->setCurrentIndex(0);
    wclayout->addWidget(new QLabel(tr("dir.: "), wcrosstalk),0,2);
    wclayout->addWidget(cmbCrosstalkDirection,0,3);
    spinCrosstalkAvg=new QSpinBox(wcrosstalk);
    spinCrosstalkAvg->setRange(0,1000);
    spinCrosstalkAvg->setValue(4);
    wclayout->addWidget(new QLabel(tr("avg.: "), wcrosstalk),1,0);
    wclayout->addWidget(spinCrosstalkAvg,1,1);
    cmbCrosstalkMode=new QComboBox(wcrosstalk);
    cmbCrosstalkMode->addItem(tr("corrected"));
    cmbCrosstalkMode->addItem(tr("explained"));
    cmbCrosstalkMode->setCurrentIndex(0);
    wclayout->addWidget(new QLabel(tr("mode: "), wcrosstalk),1,2);
    wclayout->addWidget(cmbCrosstalkMode,1,3);
    gl->addRow(tr("crosstalk:"), wcrosstalk);


    gl->addRow(tr("rel. ccf amplitude:"), labRelCCF=new QLabel(wcrosstalk));
    labRelCCF->setTextInteractionFlags(Qt::TextBrowserInteraction);*/

    connectImageWidgets(true);






    ///////////////////////////////////////////////////////////////
    // GROUPBOX: overview image plot
    ///////////////////////////////////////////////////////////////
    QWidget* wpltOverview=new QWidget(this);
    QVBoxLayout* lpltOverview=new QVBoxLayout();
    lpltOverview->setSpacing(2);
    wpltOverview->setLayout(lpltOverview);
    pltOverview=new QFPlotter(wpltOverview);
    pltOverview->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltOverview->setObjectName("pltOverview");
    //lpltOverview->addWidget(new QLabel(tr("Overview:")));
    lpltOverview->addWidget(pltOverview, 1);
    pltOverview->get_plotter()->set_plotLabel(tr("\\textbf{Overview}"));
    pltOverview->get_plotter()->set_plotLabelFontSize(10);
    pltOverview->set_displayMousePosition(false);
    pltOverview->set_displayToolbar(true);
    pltOverview->get_plotter()->set_maintainAspectRatio(true);
    pltOverview->get_plotter()->set_aspectRatio(1);
    pltOverview->get_plotter()->set_maintainAxisAspectRatio(true);
    pltOverview->get_plotter()->set_axisAspectRatio(1);
    pltOverview->setXY(0,0,1,1);
    pltOverview->setAbsoluteXY(0,1,0,1);
    pltOverview->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltOverview->get_plotter()->getYAxis()->set_axisMinWidth(1);

    pltOverview->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltOverview->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltOverview->get_plotter()->getXAxis()->set_axisLabel("");
    pltOverview->get_plotter()->getYAxis()->set_axisLabel("");
    pltOverview->get_plotter()->setGrid(false);
    pltOverview->get_plotter()->set_useAntiAliasingForSystem(true);
    pltOverview->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltOverview->set_userActionCompositionMode(QPainter::CompositionMode_Xor);
    connect(pltOverview, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltOverview, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    connect(pltOverview, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    connect(pltOverview, SIGNAL(plotMouseMove(double,double)), this, SLOT(imageMouseMoved(double,double)));
    connect(pltOverview, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltOverview, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    connect(pltOverview, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltOverview, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));

    plteOverview=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::UInt16Array, NULL, 0,0, JKQTPMathImageGRAY, pltOverview->get_plotter());
    plteOverview->set_visible(true);
    pltOverview->addGraph(plteOverview);
    plteOverviewRGB=new JKQTPRGBMathImage(0,0,1,1,JKQTPMathImageBase::UInt16Array, NULL, 0,0, pltOverview->get_plotter());
    plteOverviewRGB->set_visible(false);
    pltOverview->addGraph(plteOverviewRGB);

    plteOverviewSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltOverview->get_plotter());
    plteOverviewSelected->set_rectanglesAsImageOverlay(OverlayRectanglesAsImageOverlay);
    pltOverview->addGraph(plteOverviewSelected);


    plteOverviewExcluded=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlExCol, pltOverview->get_plotter());
    plteOverviewExcluded->set_rectanglesAsImageOverlay(OverlayRectanglesAsImageOverlay);
    pltOverview->addGraph(plteOverviewExcluded);

    //plteImageData=NULL;






    ///////////////////////////////////////////////////////////////
    // WIDGET: mask image plot
    ///////////////////////////////////////////////////////////////
    QWidget* wpltMask=new QWidget(this);
    wpltMask->setVisible(false);
    QVBoxLayout* lpltMask=new QVBoxLayout();
    wpltMask->setLayout(lpltMask);
    pltMask=new QFPlotter(wpltMask);
    pltMask->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltMask->setObjectName("pltMask");
    pltMask->get_plotter()->set_plotLabel(tr("\\textbf{Mask/Selected}"));
    pltMask->get_plotter()->set_plotLabelFontSize(10);

    //lpltMask->addWidget(new QLabel(tr("Mask/Selected:")));
    lpltMask->addWidget(pltMask, 1);

    pltMask->set_displayToolbar(true);
    pltMask->get_plotter()->set_maintainAspectRatio(true);
    pltMask->get_plotter()->set_aspectRatio(1);
    pltMask->get_plotter()->set_maintainAxisAspectRatio(true);
    pltMask->get_plotter()->set_axisAspectRatio(1);
    pltMask->setXY(0,0,1,1);
    pltMask->setAbsoluteXY(0,1,0,1);
    pltMask->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltMask->get_plotter()->getYAxis()->set_axisMinWidth(1);

    pltMask->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltMask->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltMask->get_plotter()->getXAxis()->set_axisLabel("");
    pltMask->get_plotter()->getYAxis()->set_axisLabel("");
    pltMask->get_plotter()->setGrid(false);
    pltMask->get_plotter()->set_useAntiAliasingForSystem(true);
    pltMask->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltMask->set_userActionCompositionMode(QPainter::CompositionMode_Xor);
    connect(pltMask, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltMask, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    connect(pltMask, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    connect(pltMask, SIGNAL(plotMouseMove(double,double)), this, SLOT(imageMouseMoved(double,double)));
    connect(pltMask, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltMask, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    connect(pltMask, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltMask, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));

    plteMask=new JKQTPOverlayImage(0,0,1,1,NULL, 0,0, QColor("black"), pltMask->get_plotter());
    plteMask->set_falseColor(QColor("white"));
    pltMask->addGraph(plteMask);

    plteMaskSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltMask->get_plotter());
    plteMaskSelected->set_rectanglesAsImageOverlay(OverlayRectanglesAsImageOverlay);

    pltMask->addGraph(plteMaskSelected);










    ///////////////////////////////////////////////////////////////
    // WIDGET: parameter image plot
    ///////////////////////////////////////////////////////////////

    QWidget* wpltImage=new QWidget(this);
    QVBoxLayout* lpltImage=new QVBoxLayout();
    wpltImage->setLayout(lpltImage);
    pltImage=new QFImagePlotter(wpltImage);
    pltImage->setObjectName("pltImage");
    pltImage->get_plotter()->set_plotLabel(tr("\\textbf{Parameter Image}"));
    pltImage->connectTo(grpImage, cmbSelectionStyle);

    //lpltImage->addWidget((labParamImage=new QLabel(tr("Parameter Image:"))));
    lpltImage->addWidget(pltImage, 1);


    connect(pltImage, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltImage, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    connect(pltImage, SIGNAL(plotMouseMove(double,double)), this, SLOT(imageMouseMoved(double,double)));
    connect(pltImage, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(saveImageSettings()), this, SLOT(saveImageSettings()));







    ///////////////////////////////////////////////////////////////
    // WIDGET: parameter image 2 plot
    ///////////////////////////////////////////////////////////////

    QWidget* wpltImage2=new QWidget(this);
    QVBoxLayout* lpltImage2=new QVBoxLayout();
    wpltImage2->setLayout(lpltImage2);
    pltParamImage2=new QFImagePlotter(wpltImage2);
    pltParamImage2->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltParamImage2->setObjectName("pltImage2");
    pltParamImage2->get_plotter()->set_plotLabel(tr("\\textbf{\"Parameter Image 2\" Image}"));
    pltParamImage2->connectTo(grpImage2, cmbSelectionStyle);
    lpltImage2->addWidget(pltParamImage2, 1);


    connect(pltParamImage2, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltParamImage2, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    connect(pltParamImage2, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    connect(pltParamImage2, SIGNAL(plotMouseMove(double,double)), this, SLOT(imageMouseMoved(double,double)));
    connect(pltParamImage2, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltParamImage2, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    connect(pltParamImage2, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltParamImage2, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltParamImage2, SIGNAL(saveImageSettings()), this, SLOT(saveImageSettings()));



    connect(pltOverview, SIGNAL(beforePlotScalingRecalculate()), this, SLOT(moveColorbarsAuto()));






    ///////////////////////////////////////////////////////////////
    // WIDGET: correlation&residuals plot
    ///////////////////////////////////////////////////////////////

    /*plotter = new QFPlotter(true, this);
    plotter->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    plotter->setObjectName("plotter");

    plotter->get_plotter()->set_showKey(true);
    plotterResid = new QFPlotter(true, this);
    plotterResid->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    plotterResid->setObjectName("plotterResid");

    plotterResid->get_plotter()->set_showKey(true);


    plotter->get_plotter()->set_gridPrinting(true);
    plotter->get_plotter()->addGridPrintingPlotter(0,1,plotterResid->get_plotter());
    plotter->set_displayToolbar(true);
    plotterResid->set_displayToolbar(false);


    plotterResid->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    plotterResid->getXAxis()->set_labelFontSize(11);
    plotterResid->getXAxis()->set_tickLabelFontSize(10);
    plotterResid->getYAxis()->set_axisLabel(tr("residuals"));
    plotterResid->getYAxis()->set_labelFontSize(11);
    plotterResid->getYAxis()->set_tickLabelFontSize(10);
    plotterResid->getYAxis()->set_minTicks(5);
    plotter->getXAxis()->set_axisLabel("");
    plotter->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    plotter->getYAxis()->set_labelFontSize(11);
    plotter->getYAxis()->set_tickLabelFontSize(10);
    plotter->getXAxis()->set_drawMode1(JKQTPCADMticks);
    plotter->getXAxis()->set_tickLabelFontSize(10);
    plotter->getYAxis()->set_minTicks(5);
    plotterResid->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    plotterResid->get_plotter()->setBorder(1,1,1,1);
    plotter->get_plotter()->setBorder(1,1,1,1);
    plotterResid->synchronizeToMaster(plotter, true, false);
    plotter->get_plotter()->set_useAntiAliasingForSystem(true);
    plotter->get_plotter()->set_useAntiAliasingForGraphs(true);
    plotterResid->get_plotter()->set_useAntiAliasingForSystem(true);
    plotterResid->get_plotter()->set_useAntiAliasingForGraphs(true);
    plotter->set_displayMousePosition(false);
    plotterResid->set_displayMousePosition(false);
    plotter->get_plotter()->set_keyFontSize(9);
    plotterResid->get_plotter()->set_keyFontSize(9);
    plotterResid->useExternalDatastore(plotter->getDatastore());
    connect(plotter, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(acfZoomChangedLocally(double,double,double,double,JKQtPlotter*)));



    QWidget* wp=new QWidget(this);
    QVBoxLayout* lp=new QVBoxLayout(this);
    lp->setContentsMargins(0,0,0,0);
    wp->setLayout(lp);
    lp->addWidget(plotterResid,1);
    sliders=new DataCutSliders(this);
    connect(sliders, SIGNAL(slidersChanged(int , int , int, int)), this, SLOT(slidersChanged(int, int, int, int)));
    lp->addWidget(sliders);


    tvParams=new QEnhancedTableView();
    tvParams->setItemDelegate(new QFHTMLDelegate(tvParams));
    tvParams->setAlternatingRowColors(true);
    QFontMetrics tvfm(font());
    tvParams->verticalHeader()->setDefaultSectionSize((int)round((double)tvfm.height()*1.5));
    tabFitvals=new QFTableModel(tvParams);
    tabFitvals->setReadonly(true);*/



    splitterTopBot=new QVisibleHandleSplitter(Qt::Vertical, this);
    splitterTop=new QVisibleHandleSplitter(Qt::Horizontal, this);
    splitterTop->addWidget(wpltImage);
    splitterTop->addWidget(wpltImage2);
    splitterTop->addWidget(wpltMask);
    splitterTop->addWidget(wpltOverview);
    splitterTopBot->addWidget(splitterTop);
    QVBoxLayout* layPlotsBot=new QVBoxLayout(this);
    QWidget* wPlotsBot=new QWidget(this);
    wPlotsBot->setLayout(layPlotsBot);
    layPlotsBot->setContentsMargins(0,0,0,0);
    //layPlotsBot->addWidget(plotter,3);
    //layPlotsBot->addWidget(wp,2);
    splitterBot=new QVisibleHandleSplitter(Qt::Horizontal, this);
    //splitterBot->addWidget(splitterBotPlots);
    splitterBot->addWidget(wPlotsBot);
    splitterBot->addWidget(tvParams);
    splitterTopBot->addWidget(splitterBot);

    vbl->addWidget(new QWidget(this), 1);








    ///////////////////////////////////////////////////////////////
    // GROUPBOX: buttons for data/report saving
    ///////////////////////////////////////////////////////////////

    QGridLayout* grdTop=new QGridLayout(this);
    btnPrintReport = createButtonAndActionShowText(actPrintReport, QIcon(":/imaging_fcs/report_print.png"), tr("&Print report"), this);
    actPrintReport->setToolTip(tr("print a report which contains all data on the current screen:<br><ul>"
                                  "<li>all images (parameter, mask, parameter 2, overview</li>"
                                  "<li>correlation curves and fit parameters</li>"
                                  "<li>histpgram and statistics</li>"
                                  "<li>additional data (files, description configuration ...)</li>"
                                  "</ul>"));

    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));
    btnSaveReport = createButtonAndActionShowText(actSaveReport, QIcon(":/imaging_fcs/report_save.png"), tr("&Save report"), this);
    actSaveReport->setToolTip(tr("save a report which contains all data on the current screen as PDF or PostScript file:<br><ul>"
                                  "<li>all images (parameter, mask, parameter 2, overview)</li>"
                                  "<li>correlation curves and fit parameters</li>"
                                  "<li>histpgram and statistics</li>"
                                  "<li>additional data (files, description configuration ...)</li>"
                                  "</ul>"));
    connect(actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    btnSaveData = createButtonAndActionShowText(actSaveData, QIcon(":/imaging_fcs/preview_savedata.png"), tr("Save &data"), this);
    actSaveData->setToolTip(tr("save the currently displayed images (parameter, mask, parameter 2, overview)\nas image files (e.g. TIFF), so they can be processed in other programs."));
    connect(actSaveData, SIGNAL(triggered()), this, SLOT(saveData()));

    actInsertSelectedCorrelationsAsFCSRDR=new QAction(tr("Insert Correlation Curves as new RDR into Project"), this);
    connect(actInsertSelectedCorrelationsAsFCSRDR, SIGNAL(triggered()), this, SLOT(insertSelectedCorrelationsAsFCSRDR()));

    btnCopyDataToMatlab = createButtonAndActionShowText(actCopyDataToMatlab, QIcon(":/imaging_fcs/copydatatomatlab.png"), tr("Copy Images to &Matlab"), this);
    actCopyDataToMatlab->setToolTip(tr("copy the currently dispalyed images (parameter, mask, parameter 2, overview) as a Matlab script."));
    connect(actCopyDataToMatlab, SIGNAL(triggered()), this, SLOT(copyToMatlab()));
    btnCopyDataAsColumns = createButtonAndActionShowText(actCopyDataAsColumns, QIcon(":/imaging_fcs/copydata.png"), tr("Copy Images as &Columns"), this);
    actCopyDataAsColumns->setToolTip(tr("copy the currently dispalyed images (parameter, mask, parameter 2, overview) as columns of data to the clipboard. The data may be pasted e.g. into a spreadsheet program like Excel"));
    connect(actCopyDataAsColumns, SIGNAL(triggered()), this, SLOT(copyDataAsColumns()));

    actCopyFitResultStatistics=new QAction(tr("copy fit result statistics for Excel/Origin..."), this);
    connect(actCopyFitResultStatistics, SIGNAL(triggered()), this, SLOT(copyFitResultStatistics()));


    grdTop->addWidget(grpTop, 0, 2, 3, 1);
    grdTop->addWidget(btnSaveData, 0, 0);
    grdTop->addWidget(btnCopyDataToMatlab, 1, 0);
    grdTop->addWidget(btnCopyDataAsColumns, 2, 0);
    grdTop->addWidget(btnSaveReport, 0, 1);
    grdTop->addWidget(btnPrintReport, 1, 1);
    grdTop->setColumnStretch(1,0);
    grdTop->setColumnStretch(0,0);









    ///////////////////////////////////////////////////////////////
    // TOOLBAR & ACTIONS: edit image plots
    ///////////////////////////////////////////////////////////////
    actImagesZoom=new QAction(QIcon(":/imaging_fcs/zoom.png"), tr("zoom"), this);
    actImagesZoom->setToolTip(tr("in this mode the user may zoom into a plot by drawing a rectangle (draging with the left mouse key)\nA click toggles the current selection/mask position."));
    actImagesZoom->setCheckable(true);
    actImagesDrawRectangle=new QAction(QIcon(":/imaging_fcs/draw_rectangle.png"), tr("rectangular selection"), this);
    actImagesDrawRectangle->setToolTip(tr("in this mode the user may draw a rectangle.<br>"
                                 "All pixels inside the rectangle will be selected<br>"
                                 "when the user releases the left mouse key. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawRectangle->setCheckable(true);

    actImagesDrawPoints=new QAction(QIcon(":/imaging_fcs/draw_point.png"), tr("point-wise selection"), this);
    actImagesDrawPoints->setToolTip(tr("in this mode the user may click on single points.<br>"
                                 "to add and remove them to/from the mask/selection. A click<br>"
                                 "will toggle the state of the clicked pixel<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawPoints->setCheckable(true);

    actImagesDrawCircle=new QAction(QIcon(":/imaging_fcs/draw_circle.png"), tr("circular selection"), this);
    actImagesDrawCircle->setToolTip(tr("in this mode the user may draw a circle.<br>"
                                 "All pixels inside the cirle will be selected<br>"
                                 "when the user releases the left mouse key. The point of first<br>"
                                 "click will be the center of the circle. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawCircle->setCheckable(true);
    actImagesDrawEllipse=new QAction(QIcon(":/imaging_fcs/draw_ellipse.png"), tr("elliptical selection"), this);
    actImagesDrawEllipse->setToolTip(tr("in this mode the user may draw a ellipse.<br>"
                                 "All pixels inside the ellipse will be selected<br>"
                                "when the user releases the left mouse key. The point of first<br>"
                                "click will be the center of the ellipse. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawEllipse->setCheckable(true);
    actImagesDrawLine=new QAction(QIcon(":/imaging_fcs/draw_line.png"), tr("line selection"), this);
    actImagesDrawLine->setToolTip(tr("in this mode the user may draw a line.<br>"
                                 "All pixels on the line will be selected<br>"
                                 "when the user releases the left mouse key. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawLine->setCheckable(true);

    actImagesScribble=new QAction(QIcon(":/imaging_fcs/draw_scribble.png"), tr("scribble selection"), this);
    actImagesScribble->setToolTip(tr("in this mode the user may select/deselect pixels by.<br>"
                                 "keeping the left mouse button pressed and moving the mouse<br>"
                                 "over the image. Depending on the key pressed on the keyboard,<br>"
                                 "different actions are executed:<ul>"
                                 "<li>CTRL: the old selection/mask is not deleted, so the result is appended to it.</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesScribble->setCheckable(true);
    agImageSelectionActions=new QActionGroup(this);
    agImageSelectionActions->setExclusive(true);
    agImageSelectionActions->addAction(actImagesZoom);
    agImageSelectionActions->addAction(actImagesDrawPoints);
    agImageSelectionActions->addAction(actImagesDrawRectangle);
    agImageSelectionActions->addAction(actImagesScribble);
    agImageSelectionActions->addAction(actImagesDrawLine);
    agImageSelectionActions->addAction(actImagesDrawCircle);
    agImageSelectionActions->addAction(actImagesDrawEllipse);
    tbParameterImage=new QToolBar(this);
    tbParameterImage->addAction(pltImage->get_plotter()->get_actZoomAll());
    tbParameterImage->addAction(pltImage->get_plotter()->get_actZoomIn());
    tbParameterImage->addAction(pltImage->get_plotter()->get_actZoomOut());
    tbParameterImage->addSeparator();
    tbParameterImage->addWidget(new QLabel(tr("    edit "), this));
    cmbMaskEditMode=new QComboBox(this);
    cmbMaskEditMode->addItem("selection");
    cmbMaskEditMode->addItem("mask");
    QWidget* spacer=new QWidget(this);
    spacer->setMinimumWidth(20);
    tbParameterImage->addWidget(cmbMaskEditMode);
    tbParameterImage->addWidget(spacer);
    tbParameterImage->addAction(actImagesZoom);
    tbParameterImage->addAction(actImagesDrawPoints);
    tbParameterImage->addAction(actImagesDrawRectangle);
    tbParameterImage->addAction(actImagesScribble);
    tbParameterImage->addAction(actImagesDrawLine);
    tbParameterImage->addAction(actImagesDrawCircle);
    tbParameterImage->addAction(actImagesDrawEllipse);
    labImagePositionDisplay=new QLabel(this);
    labImageAvg=new QLabel(this);
    tbParameterImage->addSeparator();
    spacer=new QWidget(this);
    spacer->setMinimumWidth(20);
    tbParameterImage->addWidget(spacer);
    tbParameterImage->addWidget(labImagePositionDisplay);
    tbParameterImage->addSeparator();
    tbParameterImage->addWidget(labImageAvg);
    connect(agImageSelectionActions, SIGNAL(triggered(QAction*)), this, SLOT(setImageEditMode()));
    actImagesDrawPoints->setChecked(true);
    actImagesZoom->setChecked(true);
    actImagesDrawPoints->setChecked(true);
    setImageEditMode();



    //////////////////////////////////////////////////////////////////////////////////////////
    // CORRELATION FUNCTION TAB
    //////////////////////////////////////////////////////////////////////////////////////////
    QWidget* widACFs=new QWidget(this);
    QGridLayout* layACFs=new QGridLayout();
    widACFs->setLayout(layACFs);
    layACFs->addWidget(tbParameterImage, 0, 0, 1, 2);
    layACFs->addWidget(splitterTopBot, 1, 0);
    layACFs->addWidget(area, 1, 1); // !!!
    layACFs->setColumnStretch(0,10);
    layACFs->setRowStretch(1,0);
    layACFs->setContentsMargins(0,0,0,0);



    //////////////////////////////////////////////////////////////////////////////////////////
    // HISTOGRAM 1 TAB
    //////////////////////////////////////////////////////////////////////////////////////////
    histogram=new QFHistogramView(this);
    histogram->setMinimumHeight(200);
    histogram_2=new QFHistogramView(this);
    histogram_2->setMinimumHeight(200);
    chkExcludeExcludedRunsFromHistogram=new QCheckBox("", this);
    chkExcludeExcludedRunsFromHistogram->setToolTip(tr("if this option is activated the histograms are only calculated for those pixels that are not excluded."));
    chkExcludeExcludedRunsFromHistogram_2=new QCheckBox("", this);
    chkExcludeExcludedRunsFromHistogram_2->setToolTip(tr("if this option is activated the histograms are only calculated for those pixels that are not excluded."));
    histogram->addSettingsWidget(tr("w/o excluded:"), chkExcludeExcludedRunsFromHistogram);
    histogram_2->addSettingsWidget(tr("w/o excluded:"), chkExcludeExcludedRunsFromHistogram_2);
    histogram_2->setVisible(false);

    QWidget* widHist=new QWidget(this); //=histogram;
    histLay=new QGridLayout(this);
    widHist->setLayout(histLay);
    histLay->addWidget(histogram,0,0);
    histLay->addWidget(histogram_2,0,1);


    //////////////////////////////////////////////////////////////////////////////////////////
    // HISTOGRAM 2 TAB
    //////////////////////////////////////////////////////////////////////////////////////////
    histogram2=new QFHistogramView(this);
    histogram2->setMinimumHeight(200);
    histogram2_2=new QFHistogramView(this);
    histogram2_2->setMinimumHeight(200);
    chkExcludeExcludedRunsFromHistogram2=new QCheckBox("", this);
    chkExcludeExcludedRunsFromHistogram2->setToolTip(tr("if this option is activated the histograms are only calculated for those pixels that are not excluded."));
    chkExcludeExcludedRunsFromHistogram2_2=new QCheckBox("", this);
    chkExcludeExcludedRunsFromHistogram2_2->setToolTip(tr("if this option is activated the histograms are only calculated for those pixels that are not excluded."));
    histogram2->addSettingsWidget(tr("w/o excluded:"), chkExcludeExcludedRunsFromHistogram2);
    histogram2_2->addSettingsWidget(tr("w/o excluded:"), chkExcludeExcludedRunsFromHistogram2_2);
    histogram2_2->setVisible(false);

    histLay->addWidget(histogram2,1,0);
    histLay->addWidget(histogram2_2,1,1);
    //histLay->addWidget(new QWidget(this),2,0);

    histLay->setColumnStretch(0,1);
    histLay->setColumnStretch(1,0);
    histLay->setRowStretch(0,1);
    histLay->setRowStretch(1,1);
    //histLay->setRowStretch(2,3);



    //////////////////////////////////////////////////////////////////////////////////////////
    // CORRELATION TAB
    //////////////////////////////////////////////////////////////////////////////////////////
    corrView=new QFParameterCorrelationView(this);
    cmbCorrelationDisplayMode=new QComboBox(this);
    cmbCorrelationDisplayMode->addItem(tr("parameter 1 vs. 2"));
    cmbCorrelationDisplayMode->addItem(tr("parameter 1 vs. image"));
    cmbCorrelationDisplayMode->addItem(tr("parameter 2 vs. image"));
    cmbCorrelationDisplayMode->addItem(tr("parameter 1 vs. x-coordinate"));
    cmbCorrelationDisplayMode->addItem(tr("parameter 2 vs. x-coordinate"));
    cmbCorrelationDisplayMode->addItem(tr("parameter 1 vs. y-coordinate"));
    cmbCorrelationDisplayMode->addItem(tr("parameter 2 vs. y-coordinate"));
    cmbCorrelationDisplayMode->addItem(tr("parameter 1 vs. center distance"));
    cmbCorrelationDisplayMode->addItem(tr("parameter 2 vs. center distance"));
    spinCorrelationChannel=new QSpinBox(this);
    spinCorrelationChannel->setRange(0,0);
    spinCorrelationChannel->setValue(0);

    QWidget* widCorr=new QWidget(this);
    QVBoxLayout* corrLay=new QVBoxLayout(this);
    QHBoxLayout* layCorrCombo=new QHBoxLayout(this);
    corrLay->addLayout(layCorrCombo);
    layCorrCombo->addStretch();
    layCorrCombo->addWidget(new QLabel(tr("correlation plot mode:"), this));
    layCorrCombo->addWidget(cmbCorrelationDisplayMode);
    layCorrCombo->addWidget(new QLabel(tr("  channel:"), this));
    layCorrCombo->addWidget(spinCorrelationChannel);
    layCorrCombo->addStretch();
    widCorr->setLayout(corrLay);
    corrLay->addWidget(corrView,0,0);

    //////////////////////////////////////////////////////////////////////////////////////////
    // PUT EVERYTHING TOGETHER
    //////////////////////////////////////////////////////////////////////////////////////////
    tabDisplay=new QTabWidget(this);
    tabDisplay->addTab(widACFs, tr("&Images/Data"));
    tabDisplay->addTab(widHist, tr("&Histograms"));
    tabDisplay->addTab(widCorr, tr("&Correlations"));


    //lb->addWidget(grpTop, 1, 0, 1, 2);
    lb->addLayout(grdTop, 0,0);
    lb->addWidget(tabDisplay, 1,0);



    connect(chkOverviewVisible, SIGNAL(toggled(bool)), this, SLOT(showHidePlots()));
    connect(chkParamImage2Visible, SIGNAL(toggled(bool)), this, SLOT(showHidePlots()));
    connect(chkMaskVisible, SIGNAL(toggled(bool)), this, SLOT(showHidePlots()));



    connectParameterWidgets();





    menuData=propertyEditor->addMenu("&Data", 0);
    menuData->addAction(actSaveData);
    menuData->addAction(actCopyDataToMatlab);
    menuData->addAction(actCopyDataAsColumns);
    menuData->addAction(actInsertSelectedCorrelationsAsFCSRDR);
    menuData->addAction(actCopyFitResultStatistics);
    menuData->addSeparator();
    menuData->addAction(actSaveReport);
    menuData->addAction(actPrintReport);
    menuData->addSeparator();
    QMenu* m;
    m=menuData->addMenu(tr("parameter image plot"));
    m->addAction(pltImage->get_plotter()->get_actShowPlotData());
    m->addSeparator();
    m->addAction(pltImage->get_actCopyToTable());
    m->addSeparator();
    m->addAction(pltImage->get_plotter()->get_actSaveData());
    m->addAction(pltImage->get_plotter()->get_actCopyData());
    m->addAction(pltImage->get_plotter()->get_actCopyMatlab());
    m->addAction(pltImage->get_plotter()->get_actCopyPixelImage());
    m->addAction(pltImage->get_plotter()->get_actSavePlot());
    m->addAction(pltImage->get_plotter()->get_actPrint());
    m=menuData->addMenu(tr("overview image plot"));
    m->addAction(pltOverview->get_plotter()->get_actShowPlotData());
    m->addSeparator();
    m->addAction(pltOverview->get_actCopyToTable());
    m->addSeparator();
    m->addAction(pltOverview->get_plotter()->get_actSaveData());
    m->addAction(pltOverview->get_plotter()->get_actCopyData());
    m->addAction(pltOverview->get_plotter()->get_actCopyMatlab());
    m->addAction(pltOverview->get_plotter()->get_actCopyPixelImage());
    m->addAction(pltOverview->get_plotter()->get_actSavePlot());
    m->addAction(pltOverview->get_plotter()->get_actPrint());
    m=menuData->addMenu(tr("parameter image 2 plot"));
    m->addAction(pltParamImage2->get_plotter()->get_actShowPlotData());
    m->addSeparator();
    m->addAction(pltParamImage2->get_actCopyToTable());
    m->addSeparator();
    m->addAction(pltParamImage2->get_plotter()->get_actSaveData());
    m->addAction(pltParamImage2->get_plotter()->get_actCopyData());
    m->addAction(pltParamImage2->get_plotter()->get_actCopyMatlab());
    m->addAction(pltParamImage2->get_plotter()->get_actCopyPixelImage());
    m->addAction(pltParamImage2->get_plotter()->get_actSavePlot());
    m->addAction(pltParamImage2->get_plotter()->get_actPrint());
    /*m=menuData->addMenu(tr("correlation function/residual plot"));
    m->addAction(plotter->get_plotter()->get_actShowPlotData());
    m->addSeparator();
    m->addAction(plotter->get_actCopyToTable());
    m->addSeparator();
    m->addAction(plotter->get_plotter()->get_actSaveData());
    m->addAction(plotter->get_plotter()->get_actCopyData());
    m->addAction(plotter->get_plotter()->get_actCopyMatlab());
    m->addAction(plotter->get_plotter()->get_actCopyPixelImage());
    m->addAction(plotter->get_plotter()->get_actSavePlot());
    m->addAction(plotter->get_plotter()->get_actPrint());*/

    menuMask=propertyEditor->addMenu("&Mask", 0);
    correlationMaskTools->registerMaskToolsToMenu(menuMask);
    menuMask->addSeparator();
    menuMask->addAction(actCopyMaskToAll);
    menuMask->addSeparator();
    menuMask->addAction(actUse);
    menuMask->addAction(actDontUse);
    menuMask->addAction(actClearMask);
    menuMask->addAction(actInvertMask);
    menuMask->addAction(actMaskByIntensity);
    menuMask->addAction(actMaskByParam2Intensity);
    menuMask->addAction(actMaskByParamIntensity);
    //correlationMaskTools->registerCorrelationToolsToMenu(menuMask);


    menuSelection=propertyEditor->addMenu("&Selection", 0);
    menuSelection->addAction(actSaveSelection);
    menuSelection->addAction(actLoadSelection);
    menuSelection->addSeparator();
    menuSelection->addAction(actCopySelection);
    menuSelection->addAction(actPasteSelection);

    createAdditionalWidgets();

    setUpdatesEnabled(true);
}

























bool FRawDataImageEditor::indexIsDualView2(int32_t sel)
{
    /*QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return false;
    return m->indexIsDualView2(sel);*/
    return false;

}

void FRawDataImageEditor::selectedInsert(int idx)
{

}

void FRawDataImageEditor::selectedRemove(int idx)
{

}

void FRawDataImageEditor::excludeByImage(double *imageIn)
{

}

void FRawDataImageEditor::getCurrentResultNamesAndLabels(QStringList &names, QStringList &labels)
{

}
