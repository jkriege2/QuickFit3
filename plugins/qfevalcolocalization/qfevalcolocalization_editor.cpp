/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-02-05 19:29:55 +0100 (Do, 05 Feb 2015) $  (revision $Rev: 3794 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qfevalcolocalization_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfevalcolocalization_item.h"
#include "ui_qfevalcolocalization_editor.h"
#include "qmoretextobject.h"
#include "qfrdrimagestackinterface.h"
#include "qfrdrimagemask.h"
#include "qftools.h"
#include "statistics_tools.h"
#include <typeinfo>
#include "qftabledelegate.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "qfevaluationpropertyeditor.h"

#include <QtCore>

QFEValColocalizationEditor::QFEValColocalizationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent):
    QFEvaluationEditor(services, propEditor, parent),
    ui(new Ui::QFEValColocalizationEditor)
{
    updatingData=true;
    

    // setup widgets
    ui->setupUi(this);

    ui->plotterCorrelation->setRangeSelectionMode(true);
    connect(ui->plotterCorrelation, SIGNAL(rangeSelected(double,double,double,double)), this, SLOT(userRangeChanged(double,double,double,double)));


    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,1);
    ui->splitter2->setStretchFactor(0,2);
    ui->splitter2->setStretchFactor(1,1);

    ui->tableView->setModel(&modelresults);

    // create progress dialog for evaluation
    dlgEvaluationProgress=new QProgressDialog(this);
    dlgEvaluationProgress->hide();
    dlgEvaluationProgress->setWindowModality(Qt::WindowModal);
    
    // connect widgets 
    connect(ui->btnEvaluateAll, SIGNAL(clicked()), this, SLOT(evaluateAll()));
    connect(ui->btnEvaluateCurrent, SIGNAL(clicked()), this, SLOT(evaluateCurrent()));
    ui->btnPrintReport->setDefaultAction(actPrintReport);
    ui->btnSaveReport->setDefaultAction(actSaveReport);

    ui->btnEvaluateAll->setVisible(false);
    ui->btnEvaluateCurrent->setVisible(false);
    updatingData=false;
    loadSplitter(*ProgramOptions::getInstance()->getQSettings(), ui->splitter, QString("QFEValColocalizationEditor/splitter/"));
    loadSplitter(*ProgramOptions::getInstance()->getQSettings(), ui->splitter2, QString("QFEValColocalizationEditor/splitter2/"));

    QMenu* menuResults= propEditor->addOrFindMenu(tr("Results"));
    menuResults->addAction(actSaveReport);
    menuResults->addAction(actPrintReport);

}

QFEValColocalizationEditor::~QFEValColocalizationEditor()
{
    saveSplitter(*ProgramOptions::getInstance()->getQSettings(), ui->splitter, QString("QFEValColocalizationEditor/splitter"));
    saveSplitter(*ProgramOptions::getInstance()->getQSettings(), ui->splitter2, QString("QFEValColocalizationEditor/splitter2"));
    delete ui;
    delete dlgEvaluationProgress;
}

void QFEValColocalizationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    // called when this widget should be connected to a new QFEvaluationItem

    QFEValColocalizationItem* item=qobject_cast<QFEValColocalizationItem*>(current);
    QFEValColocalizationItem* item_old=qobject_cast<QFEValColocalizationItem*>(old);

    if (old!=NULL) {
        /* disconnect item_old and clear all widgets here */
        disconnect(item_old, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        saveSplitter(*ProgramOptions::getInstance()->getQSettings(), ui->splitter, QString("QFEValColocalizationEditor/splitter"));
        saveSplitter(*ProgramOptions::getInstance()->getQSettings(), ui->splitter2, QString("QFEValColocalizationEditor/splitter2"));
    }



    if (item) {
        updatingData=true;
        loadSplitter(*ProgramOptions::getInstance()->getQSettings(), ui->splitter, QString("QFEValColocalizationEditor/splitter"));
        loadSplitter(*ProgramOptions::getInstance()->getQSettings(), ui->splitter2, QString("QFEValColocalizationEditor/splitter2"));


        /* connect widgets and fill with data from item here */

        connect(item, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        updatingData=false;
    }


    displayData();
    displayEvaluation();
}

void QFEValColocalizationEditor::resultsChanged() {
    /* some other evaluation or the user changed the results stored in the current raw data record,
       so redisplay */
    displayData();
    displayEvaluation();    
}

void QFEValColocalizationEditor::readSettings() {
    // read widget settings
    if (!settings) return;
};

void QFEValColocalizationEditor::writeSettings() {
    // write widget settings
    if (!settings) return;
}

void QFEValColocalizationEditor::userRangeChanged(double /*xmin*/, double /*xmax*/, double /*ymin*/, double /*ymax*/)
{
    displayData();
}

void QFEValColocalizationEditor::on_btnEstimateBackground_clicked()
{
    if (!current) return;

    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
    //qDebug()<<"displayData "<<record<<data<<eval;
    if ((!record)||(!eval)||(!data)) return;

    int stack=0;

    int ch1=ui->spinChannel1->value();
    int ch2=ui->spinChannel2->value();
    int dframe=ui->spinDisplayFrame->value();
    int64_t width=data->getImageStackWidth(stack);
    int64_t height=data->getImageStackHeight(stack);
    double* datar_ch1=data->getImageStack(stack, dframe, ch1);
    double* datar_ch2=data->getImageStack(stack, dframe, ch2);
    double back1=statisticsQuantile(datar_ch1, width*height, 0.05);
    double back2=statisticsQuantile(datar_ch2, width*height, 0.05);
    disconnect(ui->spinBackground, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
    disconnect(ui->spinBackground2, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));

    ui->spinBackground->setValue(back1);
    ui->spinBackground2->setValue(back2);

    connect(ui->spinBackground, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
    connect(ui->spinBackground2, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));

    displayData();
}

void QFEValColocalizationEditor::writeWidgetValues(QFRawDataRecord *formerRecord)
{
    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
    if (!eval) return;
    QString resultID=eval->getEvaluationResultID(ui->cmbStack->currentIndex(), ui->spinChannel1->value(), ui->spinChannel2->value());
    ui->plotterCorrelation->writeQFProperties(formerRecord, resultID+"_CORRPLOT/", "", "");
    formerRecord->setQFProperty(resultID+"_RANGE1_MIN", ui->plotterCorrelation->getCurrentRangeSelectionXMin(), false, false);
    formerRecord->setQFProperty(resultID+"_RANGE1_MAX", ui->plotterCorrelation->getCurrentRangeSelectionXMax(), false, false);
    formerRecord->setQFProperty(resultID+"_RANGE2_MIN", ui->plotterCorrelation->getCurrentRangeSelectionYMin(), false, false);
    formerRecord->setQFProperty(resultID+"_RANGE2_MAX", ui->plotterCorrelation->getCurrentRangeSelectionYMax(), false, false);
    formerRecord->setQFProperty(resultID+"_evalframe", ui->spinFrame->value(), false, false);
    formerRecord->setQFProperty(resultID+"_displayframe", ui->spinDisplayFrame->value(), false, false);
    formerRecord->setQFProperty(resultID+"_backthreshold", ui->spinBackgroundThreshold->value(), false, false);
    formerRecord->setQFProperty(resultID+"_back1", ui->spinBackground->value(), false, false);
    formerRecord->setQFProperty(resultID+"_back2", ui->spinBackground2->value(), false, false);
    resultID=eval->getEvaluationResultID(-1,-1,-1);
    formerRecord->setQFProperty(resultID+"_channel1", ui->spinChannel1->value(), false, false);
    formerRecord->setQFProperty(resultID+"_channel2", ui->spinChannel2->value(), false, false);
    if (formerRecord->getType()=="imaging_fcs") {
        formerRecord->setQFProperty(resultID+"_stack_imfcs", ui->cmbStack->currentIndex(), false, false);
        eval->setQFProperty(resultID+"_stack_imfcs", ui->cmbStack->currentIndex(), false, false);
    } else {
        formerRecord->setQFProperty(resultID+"_stack", ui->cmbStack->currentIndex(), false, false);
        eval->setQFProperty(resultID+"_stack", ui->cmbStack->currentIndex(), false, false);
    }


    eval->setQFProperty(resultID+"_channel1", ui->spinChannel1->value(), false, false);
    eval->setQFProperty(resultID+"_channel2", ui->spinChannel2->value(), false, false);
    eval->setQFProperty(resultID+"_evalframe", ui->spinFrame->value(), false, false);
    eval->setQFProperty(resultID+"_displayframe", ui->spinDisplayFrame->value(), false, false);
    eval->setQFProperty(resultID+"_backthreshold", ui->spinBackgroundThreshold->value(), false, false);

}

void QFEValColocalizationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    // this slot is called when the user selects a new record in the raw data record list on the RHS of this widget in the evaluation dialog
    
    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);

    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(currentRecord);
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData()));
    if (formerRecord) {
        disconnect(ui->cmbStack, SIGNAL(currentIndexChanged(int)), this, SLOT(resultsChanged()));
        disconnect(ui->chkAllFrames, SIGNAL(toggled(bool)), this, SLOT(resultsChanged()));
        disconnect(ui->spinBackgroundThreshold, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        disconnect(ui->spinBackground, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        disconnect(ui->spinBackground2, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        disconnect(ui->spinFrame, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        disconnect(ui->spinChannel1, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        disconnect(ui->spinChannel2, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        disconnect(ui->spinDisplayFrame, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        writeWidgetValues(formerRecord);

    }

    if (data) { // if we have a valid object, update
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData())); // redisplay data, if data changed
        int stack=0;

        updatingData=true;
        // assign values to widgets here 

        QString resultID=eval->getEvaluationResultID(-1,-1,-1);
        ui->cmbStack->clear();
        for (int i=0; i<data->getImageStackCount(); i++) {
            ui->cmbStack->addItem(data->getImageStackDescription(i));
        }
        if (currentRecord->getType()=="imaging_fcs") {
            ui->cmbStack->setCurrentIndex(qBound(0, currentRecord->getQFProperty(resultID+"_stack_imfcs", eval->getQFProperty(resultID+"_stack_imfcs", 2).toInt()).toInt(), ui->cmbStack->count()-1));
        } else {
            ui->cmbStack->setCurrentIndex(qBound(0, currentRecord->getQFProperty(resultID+"_stack", eval->getQFProperty(resultID+"_stack", 0).toInt()).toInt(), ui->cmbStack->count()-1));
        }

        ui->spinChannel1->setRange(0, data->getImageStackChannels(stack));
        ui->spinChannel2->setRange(0, data->getImageStackChannels(stack));                
        ui->spinChannel1->setValue(currentRecord->getQFProperty(resultID+"_channel1", eval->getQFProperty(resultID+"_channel1", 0).toInt()).toInt());
        ui->spinChannel2->setValue(currentRecord->getQFProperty(resultID+"_channel2", eval->getQFProperty(resultID+"_channel2", 1).toInt()).toInt());
        resultID=eval->getEvaluationResultID(ui->cmbStack->currentIndex(), ui->spinChannel1->value(), ui->spinChannel2->value());
        ui->spinDisplayFrame->setRange(0, data->getImageStackFrames(stack));
        ui->spinFrame->setRange(0, data->getImageStackFrames(stack));
        ui->chkAllFrames->setChecked(currentRecord->getQFProperty(resultID+"_allframes", eval->getQFProperty(resultID+"_allframes", true).toBool()).toBool());
        ui->spinFrame->setValue(currentRecord->getQFProperty(resultID+"_evalframe", data->getImageStackFrames(stack)/2).toInt());
        ui->spinDisplayFrame->setValue(currentRecord->getQFProperty(resultID+"_displayframe", eval->getQFProperty(resultID+"_displayframe", data->getImageStackFrames(stack)/2).toInt()).toInt());
        ui->spinBackgroundThreshold->setValue(currentRecord->getQFProperty(resultID+"_backthreshold", eval->getQFProperty(resultID+"_backthreshold", -1000).toDouble()).toDouble()); //statisticsQuantile(data->getImageStack(stack, ui->spinFrame->value(), ui->spinChannel1->value()), data->getImageStackWidth(stack)*data->getImageStackHeight(stack), 0.05)*1.5).toDouble());
        ui->spinBackground->setValue(currentRecord->getQFProperty(resultID+"_back1", 0).toDouble()); //statisticsQuantile(data->getImageStack(stack, ui->spinFrame->value(), ui->spinChannel1->value()), data->getImageStackWidth(stack)*data->getImageStackHeight(stack), 0.05)*1.5).toDouble());
        ui->spinBackground2->setValue(currentRecord->getQFProperty(resultID+"_back2", 0).toDouble()); //statisticsQuantile(data->getImageStack(stack, ui->spinFrame->value(), ui->spinChannel1->value()), data->getImageStackWidth(stack)*data->getImageStackHeight(stack), 0.05)*1.5).toDouble());
        ui->plotterCorrelation->readQFProperties(currentRecord, resultID+"_CORRPLOT/", "", "");
        ui->plotterCorrelation->setrangeSelection(currentRecord->getQFProperty(resultID+"_RANGE1_MIN", 0).toDouble(), currentRecord->getQFProperty(resultID+"_RANGE1_MAX", 0).toDouble(),
                                                  currentRecord->getQFProperty(resultID+"_RANGE2_MIN", 0).toDouble(), currentRecord->getQFProperty(resultID+"_RANGE2_MAX", 0).toDouble());

        connect(ui->cmbStack, SIGNAL(currentIndexChanged(int)), this, SLOT(resultsChanged()));
        connect(ui->spinBackground2, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        connect(ui->spinBackground, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        connect(ui->spinBackgroundThreshold, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        connect(ui->spinFrame, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        connect(ui->spinChannel1, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        connect(ui->spinChannel2, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        connect(ui->spinDisplayFrame, SIGNAL(editingFinished()), this, SLOT(resultsChanged()));
        connect(ui->chkAllFrames, SIGNAL(toggled(bool)), this, SLOT(resultsChanged()));
        updatingData=false;
    }
    
    // ensure that data of new highlighted record is displayed
    displayEvaluation();
    displayData();
}

void QFEValColocalizationEditor::displayEvaluation() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
    if ((!record)||(!eval)||(!data)) return;

    /*if (eval->hasEvaluation(record)) {
        if (record->resultsExists(eval->getEvaluationResultID(), "evaluation_completed")) {
            ui->labResults->setText(tr("<b>Results:</b><br>evaluation_completed = %1").arg(record->resultsGetAsString(eval->getEvaluationResultID(), "evaluation_completed")));
        } else {
            ui->labResults->setText(tr("<b>NO RESULTS STORED</b>"));
        }
    } else {
        ui->labResults->setText(tr("<b>NO EVALUATION DONE YET</b>"));    
    }*/
}

void QFEValColocalizationEditor::displayData() {
    if (!current) return;

    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
    //qDebug()<<"displayData "<<record<<data<<eval;
    if ((!record)||(!eval)||(!data)||(ui->cmbStack->currentIndex()<0)) return;


    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    writeWidgetValues(record);
    record->disableEmitResultsChanged();
    QString resultID=eval->getEvaluationResultID(ui->cmbStack->currentIndex(), ui->spinChannel1->value(), ui->spinChannel2->value());
    record->resultsClear(resultID);
    modelresults.setReadonly(false);
    modelresults.clear();
    modelresults.setColumnTitleCreate(0, tr("Parameter"));
    modelresults.setColumnTitleCreate(1, tr("value"));
    int trow=0;

    ui->plotterCorrelation->clear();
    int stack=ui->cmbStack->currentIndex();
    int frame=ui->spinFrame->value();
    int frames=1;
    if (ui->chkAllFrames->isChecked()) {
        frame=0;
        frames=data->getImageStackFrames(stack);
    }
    int ch1=ui->spinChannel1->value();
    int ch2=ui->spinChannel2->value();
    int dframe=ui->spinDisplayFrame->value();
    int width=data->getImageStackWidth(stack);
    int height=data->getImageStackHeight(stack);
    int pixel_count=frames*width*height;
    double thresh=ui->spinBackgroundThreshold->value();
    double back=ui->spinBackground->value();
    double back2=ui->spinBackground2->value();
    constDouble* data_ch1=data->getImageStack(stack, frame, ch1);
    constDouble* data_ch2=data->getImageStack(stack, frame, ch2);
    constDouble* datar_ch1=data->getImageStack(stack, dframe, ch1);
    constDouble* datar_ch2=data->getImageStack(stack, dframe, ch2);
    QVector<double> dch1, dch2;
    for (long long int  i=0; i<frames*width*height; i++) {
        if (data_ch1[i]-back>=thresh && data_ch2[i]-back2>=thresh) {
            dch1<<(data_ch1[i]-back);
            dch2<<(data_ch2[i]-back2);
        }
    }
    if (dch1.size()>0) {
        data_ch1=dch1.data();
        data_ch2=dch2.data();
        pixel_count=dch1.size();
    }

    ui->plotterCorrelation->setColorCodingMode(QFParameterCorrelationView::ccmXYGR);
    ui->plotterCorrelation->addCopiedCorrelation(QString("channels %1--%2").arg(ch1).arg(ch2), data_ch1, data_ch2, pixel_count);
    ui->plotterCorrelation->updateCorrelation(true);

    ui->plotter->set_doDrawing(false);
    JKQTPdatastore* ds=ui->plotter->getDatastore();
    ui->plotter->clearGraphs();
    ds->clear();
    JKQTPColumnRGBMathImage* img=new JKQTPColumnRGBMathImage(ui->plotter->get_plotter());
    int rch1=ds->addCopiedColumn(data->getImageStack(stack, dframe, ch1), width*height, tr("channel %1").arg(ch1));
    int rch2=ds->addCopiedColumn(data->getImageStack(stack, dframe, ch2), width*height, tr("channel %1").arg(ch2));
    ds->getColumn(rch1).subtract(back);
    ds->getColumn(rch2).subtract(back2);
    img->set_imageGColumn(rch1);
    img->set_imageRColumn(rch2);
    img->set_autoImageRange(true);
    img->set_width(width);
    img->set_height(height);
    img->set_Nx(width);
    img->set_Ny(height);
    ui->plotter->addGraph(img);


    QVector<bool> overlayImage;
    overlayImage.resize(width*height);
    double ch1min=ui->plotterCorrelation->getCurrentRangeSelectionXMin();
    double ch2min=ui->plotterCorrelation->getCurrentRangeSelectionYMin();
    double ch1max=ui->plotterCorrelation->getCurrentRangeSelectionXMax();
    double ch2max=ui->plotterCorrelation->getCurrentRangeSelectionYMax();
    record->setQFProperty(resultID+"_RANGE1_MIN", ui->plotterCorrelation->getCurrentRangeSelectionXMin(), false, false);
    record->setQFProperty(resultID+"_RANGE1_MAX", ui->plotterCorrelation->getCurrentRangeSelectionXMax(), false, false);
    record->setQFProperty(resultID+"_RANGE2_MIN", ui->plotterCorrelation->getCurrentRangeSelectionYMin(), false, false);
    record->setQFProperty(resultID+"_RANGE2_MAX", ui->plotterCorrelation->getCurrentRangeSelectionYMax(), false, false);
    //qDebug()<<"plot: "<<ch1min<<ch1max<<ch2min<<ch2max;
    if (ch1min!=ch1max && ch2min!=ch2max) {
        QVector<double> ch1_ranged, ch2_ranged;
        for (long long i=0; i<width*height; i++) {
            overlayImage[i]=(datar_ch1[i]-back>thresh) && (datar_ch2[i]-back>thresh) && (ch1min<=datar_ch1[i]-back)&&(datar_ch1[i]-back<=ch1max) && (ch2min<=datar_ch2[i]-back2)&&(datar_ch2[i]-back2<=ch2max);
        }

        long f=frame;
        for (long j=0; j<frames; j++) {
            double* datarr_ch1=data->getImageStack(stack, f, ch1);
            double* datarr_ch2=data->getImageStack(stack, f, ch2);
            for (long long i=0; i<width*height; i++) {
                if ((datarr_ch1[i]-back>=thresh)&&(datarr_ch2[i]-back2>=thresh)&&(ch1min<=datarr_ch1[i]-back)&&(datarr_ch1[i]-back<=ch1max) && (ch2min<=datarr_ch2[i]-back2)&&(datarr_ch2[i]-back2<=ch2max)) {
                    ch1_ranged<<(datarr_ch1[i]-back);
                    ch2_ranged<<(datarr_ch2[i]-back2);
                }
            }
            f++;
        }

        record->resultsSetNumber(resultID, "range_ch1_min", ch1min);
        record->resultsSetNumber(resultID, "range_ch1_max", ch1max);
        record->resultsSetNumber(resultID, "range_ch2_min", ch2min);
        record->resultsSetNumber(resultID, "range_ch2_max", ch2max);
        record->resultsSetInteger(resultID, "range_pixels", ch1_ranged.size());
        double p;
        record->resultsSetNumber(resultID, "range_pearson", p=statisticsCorrelationCoefficient(ch1_ranged.data(), ch2_ranged.data(), ch1_ranged.size()));
        modelresults.setCellCreate(trow, 0, tr("Selection: Pearson Correlation Coefficient"));
        modelresults.setCellCreate(trow, 1, p);
        trow++;
        record->resultsSetNumber(resultID, "range_mandersoverlap", p=statisticsMandersOverlapCoefficient(ch1_ranged.data(), ch2_ranged.data(), ch1_ranged.size()));
        modelresults.setCellCreate(trow, 0, tr("Selection: Manders Overlap Coefficient"));
        modelresults.setCellCreate(trow, 1, p);
        trow++;
    }
    int rmask=ds->addCopiedColumn(overlayImage.data(), overlayImage.size(), tr("range selection mask "));

    record->resultsSetNumber(resultID, "background_intensity_ch1", back);
    record->resultsSetNumber(resultID, "background_intensity_ch2", back2);
    record->resultsSetInteger(resultID, "stack", stack);
    record->resultsSetInteger(resultID, "ch1", ch1);
    record->resultsSetInteger(resultID, "ch2", ch2);
    record->resultsSetInteger(resultID, "pixels", dch2.size());
    record->resultsSetInteger(resultID, "allframes", ui->chkAllFrames->isChecked());
    if (ui->chkAllFrames->isChecked()) record->resultsSetInteger(resultID, "frame", ui->spinFrame->value());
    record->resultsSetNumber(resultID, "intensity_threshold", thresh);
    double p;
    record->resultsSetNumber(resultID, "pearson", p=statisticsCorrelationCoefficient(dch1.data(), dch2.data(), dch1.size()));
    modelresults.setCellCreate(trow, 0, tr("All: Pearson Correlation Coefficient"));
    modelresults.setCellCreate(trow, 1, p);
    trow++;
    record->resultsSetNumber(resultID, "mandersoverlap", p=statisticsMandersOverlapCoefficient(dch1.data(), dch2.data(), dch1.size()));
    modelresults.setCellCreate(trow, 0, tr("All: Manders Overlap Coefficient"));
    modelresults.setCellCreate(trow, 1, p);
    trow++;

    JKQTPColumnOverlayImageEnhanced* plteOverlay=new JKQTPColumnOverlayImageEnhanced(ui->plotter->get_plotter());
    plteOverlay->set_drawAsRectangles(true);
    plteOverlay->set_rectanglesAsImageOverlay(true);
    QColor col("blue");
    col.setAlphaF(0.5);
    plteOverlay->set_width(width);
    plteOverlay->set_height(height);
    plteOverlay->set_Nx(width);
    plteOverlay->set_Ny(height);
    plteOverlay->set_trueColor(col);
    plteOverlay->set_falseColor(Qt::transparent);
    plteOverlay->set_imageColumn(rmask);
    ui->plotter->addGraph(plteOverlay);

    ui->plotter->getXAxis()->set_axisLabel(tr("x [pixel]"));
    ui->plotter->getYAxis()->set_axisLabel(tr("y [pixel]"));
    ui->plotter->setAbsoluteXY(0,width,0,height);
    ui->plotter->get_plotter()->set_maintainAspectRatio(true);
    ui->plotter->get_plotter()->set_maintainAxisAspectRatio(true);
    ui->plotter->get_plotter()->set_aspectRatio(double(width)/double(height));
    ui->plotter->get_plotter()->set_axisAspectRatio(double(width)/double(height));

    ui->plotter->zoomToFit();
    ui->plotter->set_doDrawing(true);
    ui->plotter->update_plot();
    modelresults.setReadonly(true);
    ui->tableView->resizeColumnsToContents();
    record->enableEmitResultsChanged(true);

    QApplication::restoreOverrideCursor();
}






void QFEValColocalizationEditor::evaluateCurrent() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    
    dlgEvaluationProgress->reset();
    dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));    
    dlgEvaluationProgress->setRange(0,100);
    dlgEvaluationProgress->setValue(50);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // here we call doEvaluation to execute our evaluation for the current record only
    //eval->doEvaluation(record, dlgEvaluationProgress);

    displayEvaluation();
    displayData();
    dlgEvaluationProgress->setValue(100);
	dlgEvaluationProgress->close();

    QApplication::restoreOverrideCursor();
	resultsChanged();
}


void QFEValColocalizationEditor::evaluateAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
	dlgEvaluationProgress->reset();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i]; 
        // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
        QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
        if ((record)/*&&(data)*/) {
            dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));
            // here we call doEvaluation to execute our evaluation for the current record only
            //doEvaluation(record);
        }
        dlgEvaluationProgress->setValue(i);
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dlgEvaluationProgress->setValue(recs.size());
	dlgEvaluationProgress->close();
    displayEvaluation();
    displayData();
    QApplication::restoreOverrideCursor();
}



void QFEValColocalizationEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    
    // we use this QTextCursor to write the document
    QTextCursor cursor(document);
    
    // here we define some generic formats
    QTextCharFormat fText=cursor.charFormat();
    fText.setFontPointSize(8);
    QTextCharFormat fTextSmall=fText;
    fTextSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fTextBold=fText;
    fTextBold.setFontWeight(QFont::Bold);
    QTextCharFormat fTextBoldSmall=fTextBold;
    fTextBoldSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fHeading1=fText;
    QTextBlockFormat bfLeft;
    bfLeft.setAlignment(Qt::AlignLeft);
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);

    fHeading1.setFontPointSize(2*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);

    
    // insert heading
    cursor.insertText(tr("Colocalization Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);

    // insert table with some data
    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(5, 4, tableFormat);
    table->mergeCells(0,1,1,3);
    int row=0;
    int col=0;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("raw data:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(record->getName(), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("ID:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(record->getID()), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("display frame:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(boolToQString(ui->spinDisplayFrame->value()), fText);
    row++;

	row=1;
    col=2;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("evaluate all frames:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(boolToQString(ui->chkAllFrames->isChecked()), fText);
    row++;
    if (ui->chkAllFrames->isChecked()) {
        table->cellAt(row, col+0).firstCursorPosition().insertText(tr("evaluate frame:"), fTextBold);
        table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(ui->spinFrame->value()), fText);
        row++;
    }
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("channel 1:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(boolToQString(ui->spinChannel1->value()), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("channel 2:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(boolToQString(ui->spinChannel2->value()), fText);
    row++;

    cursor.movePosition(QTextCursor::End);
	
	
	
    ui->plotterCorrelation->writeReport(cursor, document);
	

	int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);

	
    QTextTable* tablePic = cursor.insertTable(1,2, tableFormat);
    {
        // insert a plot from ui->plotter
        QTextCursor tabCursor=tablePic->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        ui->plotter->get_plotter()->draw(*painter, QRect(0,0,ui->plotter->width(),ui->plotter->height()));
        delete painter;
        double scale=0.7*document->textWidth()/double(pic.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("Overview image:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.7);
        QApplication::processEvents();


        QApplication::processEvents();
    }
    {
        // insert a plot from ui->plotter
        QTextCursor tabCursor=tablePic->cellAt(0, 1).firstCursorPosition();
        QPicture pic;
        {
            QPainter painter(&pic);
            ui->tableView->paint(painter, QRect(0,0,0.28*document->textWidth(), 5.0*0.28*document->textWidth()));
        }

        double scale=0.28*document->textWidth()/double(pic.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("Results:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.28);
        QApplication::processEvents();


        QApplication::processEvents();
    }

}


