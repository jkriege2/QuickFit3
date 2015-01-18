/*
    Copyright (c) 2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2014-09-02 17:43:02 +0200 (Di, 02 Sep 2014) $  (revision $Rev: 3433 $)

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


#include "qfevalcameracalibration_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfevalcameracalibration_item.h"
#include "ui_qfevalcameracalibration_editor.h"
#include "qmoretextobject.h"
#include "qfrdrimagestackinterface.h"
#include "qfrdrimagemask.h"
#include "qftools.h"
#include "statistics_tools.h"
#include <typeinfo>
#include "qftabledelegate.h"
#include <QtGui>
#include <QtCore>
QFEvalCameraCalibrationEditor::QFEvalCameraCalibrationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent):
    QFEvaluationEditor(services, propEditor, parent),
    ui(new Ui::QFEvalCameraCalibrationEditor)
{
    updatingData=true;
    
    currentSaveDirectory="";
    table=new QFTableModel(this);
    
    // setup widgets
    ui->setupUi(this);
    ui->tabResults->setModel(table);
    ui->tabResults->setItemDelegate(new QFTableDelegate(this));
    
    // create progress dialog for evaluation
    dlgEvaluationProgress=new QProgressDialog(NULL);
    dlgEvaluationProgress->hide();
    dlgEvaluationProgress->setWindowModality(Qt::WindowModal);
    
    // connect widgets 
    connect(ui->btnEvaluateCurrent, SIGNAL(clicked()), this, SLOT(evaluateCurrent()));
    ui->btnPrintReport->setDefaultAction(actPrintReport);
    ui->btnSaveReport->setDefaultAction(actSaveReport);

    ui->plotterSNR->get_plotter()->useExternalDatastore(ui->plotter->get_plotter()->getDatastore());

    updatingData=false;
}

QFEvalCameraCalibrationEditor::~QFEvalCameraCalibrationEditor()
{
    delete ui;
    delete dlgEvaluationProgress;
}

void QFEvalCameraCalibrationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    // called when this widget should be connected to a new QFEvaluationItem

    QFEvalCameraCalibrationItem* item=qobject_cast<QFEvalCameraCalibrationItem*>(current);
    QFEvalCameraCalibrationItem* item_old=qobject_cast<QFEvalCameraCalibrationItem*>(old);

    if (old!=NULL) {
        /* disconnect item_old and clear all widgets here */
        disconnect(item_old, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        disconnect(ui->cmbBackground, SIGNAL(currentIndexChanged(int)), this, SLOT(saveProperties()));
        disconnect(ui->chkBackgroundCorrection, SIGNAL(toggled(bool)), this, SLOT(saveProperties()));
        disconnect(ui->chkLogXPlot, SIGNAL(toggled(bool)), this, SLOT(saveProperties()));
        disconnect(ui->chkLogYPlot, SIGNAL(toggled(bool)), this, SLOT(saveProperties()));
        disconnect(ui->chkLogXPlot, SIGNAL(toggled(bool)), this, SLOT(resultsChanged()));
        disconnect(ui->chkLogYPlot, SIGNAL(toggled(bool)), this, SLOT(resultsChanged()));
        disconnect(ui->cmbMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveProperties()));
        disconnect(ui->spinExcessNoise, SIGNAL(valueChanged(double)), this, SLOT(saveProperties()));
        disconnect(ui->chkBackgroundRDR, SIGNAL(toggled(bool)), this, SLOT(saveProperties()));
        disconnect(ui->spinBackground, SIGNAL(valueChanged(double)), this, SLOT(saveProperties()));
    }



    if (item) {
        updatingData=true;

        /* connect widgets and fill with data from item here */
        connect(item, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        
        ui->cmbBackground->init(current->getProject(), new QFMatchRDRFunctorSelectApplicable(current), true);
        ui->cmbBackground->setCurrentRDRID(current->getQFProperty("EVAL_BACK_FRAMERDR", -1).toInt());
        ui->chkBackgroundCorrection->setChecked(current->getQFProperty("EVAL_BACK_CORRECTION", true).toBool());
        ui->chkLogXPlot->setChecked(current->getQFProperty("PLOT_LOGX", true).toBool());
        ui->chkLogYPlot->setChecked(current->getQFProperty("PLOT_LOGY", true).toBool());
        ui->spinExcessNoise->setValue(current->getQFProperty("EVAL_EXCESSNOISE", 1).toDouble());
        ui->cmbMode->setCurrentIndex(current->getQFProperty("EVAL_MODE", 0).toInt());
        ui->chkBackgroundRDR->setChecked(current->getQFProperty("EVAL_BACK_FRAMERDR_AVAILABLE", true).toBool());
        ui->spinBackground->setValue(current->getQFProperty("EVAL_BACKGROUND_OFFSET", 0).toDouble());


        connect(ui->cmbBackground, SIGNAL(currentIndexChanged(int)), this, SLOT(saveProperties()));
        connect(ui->chkBackgroundCorrection, SIGNAL(toggled(bool)), this, SLOT(saveProperties()));
        connect(ui->cmbMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveProperties()));
        connect(ui->chkLogXPlot, SIGNAL(toggled(bool)), this, SLOT(saveProperties()));
        connect(ui->chkLogYPlot, SIGNAL(toggled(bool)), this, SLOT(saveProperties()));
        connect(ui->chkLogXPlot, SIGNAL(toggled(bool)), this, SLOT(resultsChanged()));
        connect(ui->chkLogYPlot, SIGNAL(toggled(bool)), this, SLOT(resultsChanged()));
        connect(ui->spinExcessNoise, SIGNAL(valueChanged(double)), this, SLOT(saveProperties()));
        connect(ui->chkBackgroundRDR, SIGNAL(toggled(bool)), this, SLOT(saveProperties()));
        connect(ui->spinBackground, SIGNAL(valueChanged(double)), this, SLOT(saveProperties()));

        updatingData=false;
    }


    displayData();
    displayEvaluation();
}

void QFEvalCameraCalibrationEditor::resultsChanged() {
    /* some other evaluation or the user changed the results stored in the current raw data record,
       so redisplay */
    displayData();
    displayEvaluation();    
}

void QFEvalCameraCalibrationEditor::readSettings() {
    // read widget settings
    if (!settings) return;
    currentSaveDirectory=settings->getQSettings()->value(QString("eval_cameracalibration/editor/lastSaveDirectory"), currentSaveDirectory).toString();
}

void QFEvalCameraCalibrationEditor::writeSettings() {
    // write widget settings
    if (!settings) return;
    settings->getQSettings()->setValue(QString("eval_cameracalibration/editor/lastSaveDirectory"), currentSaveDirectory);
}

void QFEvalCameraCalibrationEditor::saveProperties()
{
    if (current) {
        current->setQFProperty("EVAL_BACK_FRAMERDR", ui->cmbBackground->currentRDRID());
        current->setQFProperty("EVAL_BACK_CORRECTION", ui->chkBackgroundCorrection->isChecked());
        current->setQFProperty("PLOT_LOGX", ui->chkLogXPlot->isChecked());
        current->setQFProperty("PLOT_LOGY", ui->chkLogYPlot->isChecked());
        current->setQFProperty("EVAL_MODE", ui->cmbMode->currentIndex());
        current->setQFProperty("EVAL_BACK_FRAMERDR_AVAILABLE", ui->chkBackgroundRDR->isChecked());
        current->setQFProperty("EVAL_BACKGROUND_OFFSET", ui->spinBackground->value());
        current->setQFProperty("EVAL_EXCESSNOISE", ui->spinExcessNoise->value());
    }
}

void QFEvalCameraCalibrationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    // this slot is called when the user selects a new record in the raw data record list on the RHS of this widget in the evaluation dialog
    
    QFEvalCameraCalibrationItem* eval=qobject_cast<QFEvalCameraCalibrationItem*>(current);
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QFRawDataRecord* data=currentRecord; // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(currentRecord);
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData()));

    if (data) { // if we have a valid object, update
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData())); // redisplay data, if data changed

        updatingData=true;
        // assign values to widgets here 
        updatingData=false;
    }
    
    // ensure that data of new highlighted record is displayed
    displayEvaluation();
    displayData();
}

void QFEvalCameraCalibrationEditor::displayEvaluation() {
    if (!current) return;
    QFEvalCameraCalibrationItem* eval=qobject_cast<QFEvalCameraCalibrationItem*>(current);
    if ((!eval)/*||(!data)*/) return;

    QFRawDataRecord* record=NULL;
    QList<QPointer<QFRawDataRecord> > rdrs_in=eval->getSelectedRecords();
    QList<QFRDRImageStackInterface*> rdrs;
    QFRDRImageStackInterface* rdr_back=NULL;
    for (int i=0; i<rdrs_in.size(); i++) {
        QFRDRImageStackInterface* intf=dynamic_cast<QFRDRImageStackInterface*>(rdrs_in[i].data());
        if (intf) {
            rdrs<<intf;
        }
    }
    // write back fit results to record!
    for (int i=0; i<rdrs.size(); i++) {
        QFRawDataRecord* rec=dynamic_cast<QFRawDataRecord*>(rdrs[i]);
        if (rec && eval->hasEvaluation(rec)) {
            record=rec;
            break;
        }
    }



    if (record && eval->hasEvaluation(record)) {
        ui->labResults->setText(tr("<b>Results:</b>"));

        JKQTPdatastore* ds=ui->plotter->getDatastore();
        ui->plotter->set_doDrawing(false);
        ui->plotter->clearGraphs();
        ui->plotterSNR->set_doDrawing(false);
        ui->plotterSNR->clearGraphs();

        ds->clear();

        QString evalID=eval->getEvaluationResultID();

        QVector<double> avgI=record->resultsGetAsDoubleList(evalID, "intensity_avg");
        QVector<double> varI=record->resultsGetAsDoubleList(evalID, "intensity_var");
        QVector<double> avgIE, varIE, avgIElectrons, snr, snrErr, avgIElectronsErr;
        bool hasErrors=false;
        if (record->resultsExists(evalID, "intensity_avg_error") && record->resultsExists(evalID, "intensity_var_error")) {
            avgIE=record->resultsGetAsDoubleList(evalID, "intensity_avg_error");
            varIE=record->resultsGetAsDoubleList(evalID, "intensity_var_error");
            hasErrors=true;
        }
        if (record->resultsExists(evalID, "snr")) {
            snr=record->resultsGetAsDoubleList(evalID, "snr");
        }
        if (record->resultsExists(evalID, "snr_error")) {
            snrErr=record->resultsGetAsDoubleList(evalID, "snr_error");
        }
        if (record->resultsExists(evalID, "intensity_avg_elec")) {
            avgIElectrons=record->resultsGetAsDoubleList(evalID, "intensity_avg_elec");
        }
        if (record->resultsExists(evalID, "intensity_avg_elec_error")) {
            avgIElectronsErr=record->resultsGetAsDoubleList(evalID, "intensity_avg_elec_error");
        }

        table->disableSignals();
        table->setReadonly(false);
        table->clear();

        QStringList evals=record->resultsGetResultNames(evalID);
        int row=0;
        table->setColumnTitleCreate(0, tr("Property"));
        table->setColumnTitleCreate(1, tr("Value"));
        table->setColumnTitleCreate(2, tr("Unit"));
        QStringList excludedRes;
        excludedRes<<"average_intensity"<<"variance_intensity"<<"intensity_avg_elec_error"<<"average_intensity_error"<<"variance_intensity_error"<<"background_avg_image"<<"background_var_image"<<"intensity_avg_elec"<<"snr"<<"snr_error";
        for (int i=0; i<evals.size(); i++) {
            if (!excludedRes.contains( evals[i])) {
                QFRawDataRecord::evaluationResult res=record->resultsGet(evalID, evals[i]);
                if (res.type==QFRawDataRecord::qfrdreNumber || res.type==QFRawDataRecord::qfrdreBoolean
                         || res.type==QFRawDataRecord::qfrdreInteger || res.type==QFRawDataRecord::qfrdreString
                        || res.type==QFRawDataRecord::qfrdreNumberError) {
                    QString lab=res.label;
                    if (lab.isEmpty()) lab=evals[i];
                    table->setCellCreate(row, 0, lab);
                    table->setCellCreate(row, 1, res.getAsVariant());
                    table->setCellCreate(row, 2, res.unit);
                    row++;
                }
            }
        }
        table->setReadonly(true);
        table->enableSignals(true);
        ui->tabResults->resizeColumnsToContents();


        JKQTPxyLineErrorGraph* g_dat=new JKQTPxyLineErrorGraph(ui->plotter->get_plotter());
        size_t c_avgI=ds->addCopiedColumn(avgI, tr("average_intensity"));
        size_t c_varI=ds->addCopiedColumn(varI, tr("variance_intensity"));
        g_dat->set_xColumn(c_avgI);
        g_dat->set_yColumn(c_varI);

        if (hasErrors) {
            size_t c_avgIE=ds->addCopiedColumn(avgIE, tr("average_intensity_error"));
            size_t c_varIE=ds->addCopiedColumn(varIE, tr("variance_intensity_error"));
            g_dat->set_xErrorColumn(c_avgIE);
            g_dat->set_yErrorColumn(c_varIE);
            g_dat->set_xErrorStyle(JKQTPerrorBars);
            g_dat->set_yErrorStyle(JKQTPerrorBars);
        } else {
            g_dat->set_xErrorStyle(JKQTPnoError);
            g_dat->set_yErrorStyle(JKQTPnoError);
        }
        g_dat->set_drawLine(false);
        g_dat->set_symbol(JKQTPcross);
        g_dat->set_color(QColor("red"));
        g_dat->set_title("data");
        g_dat->set_errorColor(g_dat->get_color().darker());
        g_dat->set_symbolSize(6);
        ui->plotter->addGraph(g_dat);

        QVector<double> params;
        JKQTPxFunctionLineGraph* g_fitfull=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
        g_fitfull->setSpecialFunction(JKQTPxFunctionLineGraph::Polynomial);
        params.clear();
        params<<record->resultsGetAsDouble(evalID, "fullfit_offset")<<record->resultsGetAsDouble(evalID, "fullfit_slope");
        g_fitfull->set_params(params);
        g_fitfull->set_title(tr("full fit"));
        g_fitfull->set_color(QColor("darkblue"));
        g_fitfull->set_lineWidth(1);
        g_fitfull->set_style(Qt::DashLine);
        ui->plotter->addGraph(g_fitfull);

        JKQTPxFunctionLineGraph* g_fitstart=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
        g_fitstart->setSpecialFunction(JKQTPxFunctionLineGraph::Polynomial);
        params.clear();
        params<<record->resultsGetAsDouble(evalID, "startfit_offset")<<record->resultsGetAsDouble(evalID, "startfit_slope");
        g_fitstart->set_params(params);
        g_fitstart->set_title(tr("fit low-intensity datapoints"));
        g_fitstart->set_color(QColor("blue"));
        g_fitstart->set_lineWidth(2);
        g_fitstart->set_style(Qt::SolidLine);
        ui->plotter->addGraph(g_fitstart);





        JKQTPxyLineErrorGraph* g_snr=new JKQTPxyLineErrorGraph(ui->plotterSNR->get_plotter());
        size_t c_avgIElec=ds->addCopiedColumn(avgIElectrons, tr("intensity_avg_electrons"));
        size_t c_snr=ds->addCopiedColumn(snr, tr("snr"));
        g_snr->set_xColumn(c_avgIElec);
        g_snr->set_yColumn(c_snr);
        g_snr->set_drawLine(false);
        g_snr->set_symbol(JKQTPcross);
        g_snr->set_color(QColor("red"));
        g_snr->set_title("data");
        g_snr->set_symbolSize(6);
        if (snrErr.size()>0 && avgIElectronsErr.size()>0) {
            size_t c_avgIElE=ds->addCopiedColumn(avgIElectronsErr, tr("intensity_avg_electrons_err"));
            size_t c_snrErr=ds->addCopiedColumn(snrErr, tr("SNR_error"));
            g_snr->set_xErrorColumn(c_avgIElE);
            g_snr->set_yErrorColumn(c_snrErr);
            g_snr->set_xErrorStyle(JKQTPerrorSimpleBars);
            g_snr->set_yErrorStyle(JKQTPerrorSimpleBars);
            g_snr->set_errorColor(g_snr->get_color().darker());
        }
        ui->plotterSNR->addGraph(g_snr);

        JKQTPxFunctionLineGraph* g_snrideal=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
        g_snrideal->setSpecialFunction(JKQTPxFunctionLineGraph::PowerLaw);
        params.clear();
        params<<0<<1<<0.5;
        g_snrideal->set_params(params);
        g_snrideal->set_title(tr("ideal sensor, \\eta_{phot}=1, \\mathcal{F}^2=1"));
        g_snrideal->set_color(QColor("blue"));
        g_snrideal->set_lineWidth(2);
        g_snrideal->set_style(Qt::SolidLine);
        ui->plotterSNR->addGraph(g_snrideal);

        if (ui->spinExcessNoise->value()!=1) {
            JKQTPxFunctionLineGraph* g_snrideal2=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
            g_snrideal2->setSpecialFunction(JKQTPxFunctionLineGraph::PowerLaw);
            params.clear();
            params<<0<<(1.0/sqrt(ui->spinExcessNoise->value()))<<0.5;
            g_snrideal2->set_params(params);
            g_snrideal2->set_title(tr("ideal sensor, \\eta_{phot}=1, \\mathcal{F}^2=%1").arg(ui->spinExcessNoise->value()));
            g_snrideal2->set_color(QColor("darkblue"));
            g_snrideal2->set_lineWidth(2);
            g_snrideal2->set_style(Qt::DashLine);
            ui->plotterSNR->addGraph(g_snrideal2);
        }




        ui->plotter->get_plotter()->set_keyPosition(JKQTPkeyInsideTopLeft);
        ui->plotter->getXAxis()->set_axisLabel("image intensity $\\langle I\\rangle-\\langle B\\rangle$");
        ui->plotter->getYAxis()->set_axisLabel("image variance $\\langle\\sigma_I^2\\rangle$");

        ui->plotter->getXAxis()->set_logAxis(ui->chkLogXPlot->isChecked());
        ui->plotter->getYAxis()->set_logAxis(ui->chkLogYPlot->isChecked());

        ui->plotter->zoomToFit();
        ui->plotter->set_doDrawing(true);
        ui->plotter->update_plot();


        ui->plotterSNR->get_plotter()->set_keyPosition(JKQTPkeyInsideTopLeft);
        ui->plotterSNR->getXAxis()->set_axisLabel("image intensity $\\langle I\\rangle-\\langle B\\rangle$ [electrons]");
        ui->plotterSNR->getYAxis()->set_axisLabel("signal-to-noise ratio SNR");

        ui->plotterSNR->getXAxis()->set_logAxis(true);
        ui->plotterSNR->getYAxis()->set_logAxis(true);

        ui->plotterSNR->zoomToFit();
        ui->plotterSNR->set_doDrawing(true);
        ui->plotterSNR->update_plot();

    } else {
        ui->labResults->setText(tr("<b>NO EVALUATION DONE YET</b>"));    
        JKQTPdatastore* ds=ui->plotter->getDatastore();
        ui->plotter->set_doDrawing(false);
        ui->plotter->clearGraphs();
        ds->clear();

        ui->plotter->set_doDrawing(true);
        ui->plotter->update_plot();
        ui->plotterSNR->set_doDrawing(false);
        ui->plotterSNR->clearGraphs();

        ui->plotterSNR->set_doDrawing(true);
        ui->plotterSNR->update_plot();
    }
}

void QFEvalCameraCalibrationEditor::displayData() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEvalCameraCalibrationItem* eval=qobject_cast<QFEvalCameraCalibrationItem*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;


}






void QFEvalCameraCalibrationEditor::evaluateCurrent() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEvalCameraCalibrationItem* eval=qobject_cast<QFEvalCameraCalibrationItem*>(current);
    if ((!eval)) return;

    
    dlgEvaluationProgress->reset();
    dlgEvaluationProgress->setLabelText(tr("evaluate ..."));
    
    dlgEvaluationProgress->setRange(0,100);
    dlgEvaluationProgress->setValue(50);
    dlgEvaluationProgress->open();    

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // here we call doEvaluation to execute our evaluation for the current record only
    eval->doEvaluation(dlgEvaluationProgress);

    displayEvaluation();
    displayData();
    dlgEvaluationProgress->setValue(dlgEvaluationProgress->maximum());
    dlgEvaluationProgress->close();

    QApplication::restoreOverrideCursor();
    resultsChanged();
}






void QFEvalCameraCalibrationEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEvalCameraCalibrationItem* eval=qobject_cast<QFEvalCameraCalibrationItem*>(current);
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
    cursor.insertText(tr("Camera Calibration Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();

    // insert table with some data
    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();

    int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);

    QTextTable* table = cursor.insertTable(2,2, tableFormat);
    {
        ui->tabWidget->setCurrentIndex(0);
        QApplication::processEvents();
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic, pic2;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        ui->plotter->get_plotter()->draw(*painter, QRect(0,0,ui->plotter->width(),ui->plotter->height()));
        delete painter;
        double scale=0.45*document->textWidth()/double(pic.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("variance vs. average intensity plot:\n"), fTextBold);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
        QApplication::processEvents();

        ui->tabWidget->setCurrentIndex(1);
        tabCursor=table->cellAt(0,1).firstCursorPosition();
        QApplication::processEvents();
        painter=new JKQTPEnhancedPainter(&pic2);
        ui->plotterSNR->get_plotter()->draw(*painter, QRect(0,0,ui->plotterSNR->width(),ui->plotterSNR->height()));
        delete painter;
        scale=0.45*document->textWidth()/double(pic2.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("signal to noise ration (SNR) plot:\n"), fTextBold);
        insertQPicture(tabCursor, PicTextFormat, pic2, QSizeF(pic2.boundingRect().width(), pic2.boundingRect().height())*scale);
        QApplication::processEvents();

        tabCursor=table->cellAt(1,0).firstCursorPosition();
        tabCursor.insertText(tr("\n"), fTextBoldSmall);
        QPicture picT;
        painter=new JKQTPEnhancedPainter(&picT);
        ui->tabResults->paint(*painter);
        delete painter;
        scale=0.45*document->textWidth()/double(picT.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("fit results table:\n"), fTextBold);
        insertQPicture(tabCursor, PicTextFormat, picT, QSizeF(picT.boundingRect().width(), picT.boundingRect().height())*scale);
        QApplication::processEvents();
        ui->tabWidget->setCurrentIndex(0);
    }
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();


}



