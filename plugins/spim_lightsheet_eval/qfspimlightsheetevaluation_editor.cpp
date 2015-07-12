/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is qfFree software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the qfFree Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfspimlightsheetevaluation_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfspimlightsheetevaluation_item.h"
#include "ui_qfspimlightsheetevaluation_editor.h"
#include "qfevaluationpropertyeditor.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtCore>
#include "qfespimlightsheetevaluationcopyconfigdialog.h"

QFSPIMLightsheetEvaluationEditor::QFSPIMLightsheetEvaluationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent):
    QFEvaluationEditor(services, propEditor, parent),
    ui(new Ui::QFSPIMLightsheetEvaluationEditor)
{
    updatingData=true;
    
    currentSaveDirectory="";

    lastMousePreviewX=0;
    lastMousePreviewY=0;

    // setup widgets
    ui->setupUi(this);
    ui->cmbModel->updateFitFunctions("lightsheet_");

    ui->cmbModel->setCurrentFitFunction("lightsheet_gaussian");
    ui->cmbAlgorithm->setCurrentAlgorithm("fit_lmfit");

    ui->pltFit->get_plotter()->get_xAxis()->set_axisLabel(tr("position [{\\mu}m]"));
    ui->pltFit->get_plotter()->get_yAxis()->set_axisLabel(tr("intensity [A.U.]"));
    ui->pltFit->get_plotter()->set_plotLabel(tr("image frame cut and fit"));
    ui->pltImage->get_plotter()->set_plotLabel(tr("image frame"));

    ui->pltParam->get_plotter()->get_xAxis()->set_axisLabel(tr("lightsheet position [{\\mu}m]"));
    ui->pltBeamPos->get_plotter()->get_xAxis()->set_axisLabel(tr("lightsheet position [{\\mu}m]"));
    ui->pltBeamPosDifference->get_plotter()->get_xAxis()->set_axisLabel(tr("lightsheet position [{\\mu}m]"));
    ui->pltBeamWidth->get_plotter()->get_xAxis()->set_axisLabel(tr("lightsheet position [{\\mu}m]"));
    ui->pltBeamPos->get_plotter()->get_yAxis()->set_axisLabel(tr("beam center position [{\\mu}m]"));
    ui->pltBeamPosDifference->get_plotter()->get_yAxis()->set_axisLabel(tr("beam center distance [{\\mu}m]"));
    ui->pltBeamWidth->get_plotter()->get_yAxis()->set_axisLabel(tr("lightsheet width [{\\mu}m]"));

    tableFitResults=new QFTableModel(ui->tabFitResults);
    ui->tabFitResults->setModel(tableFitResults);



    prepareImagePlotter(ui->pltImage);

    plteImage=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0, 0, JKQTPMathImageGRAY, ui->pltImage->get_plotter());
    ui->pltImage->addGraph(plteImage);
    c_markDataX=ui->pltImage->getDatastore()->addColumn(markDataX, 2, "mark X");
    c_markDataY=ui->pltImage->getDatastore()->addColumn(markDataY, 2, "mark Y");
    plteMark=new JKQTPxyLineGraph(ui->pltImage->get_plotter());
    QColor c=QColor("red");
    c.setAlphaF(0.5);
    plteMark->set_color(c);
    plteMark->set_lineWidth(3);
    plteMark->set_drawLine(true);
    plteMark->set_symbol(JKQTPnoSymbol);
    plteMark->set_xColumn(c_markDataX);
    plteMark->set_yColumn(c_markDataY);
    ui->pltImage->addGraph(plteMark);




    
    // create progress dialog for evaluation
    dlgEvaluationProgress=new QProgressDialog(this);
    dlgEvaluationProgress->hide();
    dlgEvaluationProgress->setWindowModality(Qt::WindowModal);
    
    // connect widgets 
    //connect(ui->btnEvaluateAll, SIGNAL(clicked()), this, SLOT(evaluateAll()));
    connect(ui->btnEvaluateCurrent, SIGNAL(clicked()), this, SLOT(evaluateCurrent()));

    actSaveImageCutSeries=new QAction(tr("save image cut series"), this);
    connect(actSaveImageCutSeries, SIGNAL(triggered()), this, SLOT(saveImageCutSeries()));
    QMenu* menuTools=propEditor->addMenu("Tools", 0);
    menuTools->addAction(actSaveImageCutSeries);

    
    updatingData=false;

    ui->tabWidget->setCurrentIndex(0);
}

QFSPIMLightsheetEvaluationEditor::~QFSPIMLightsheetEvaluationEditor()
{
    delete ui;
    delete dlgEvaluationProgress;
}

void QFSPIMLightsheetEvaluationEditor::prepareImagePlotter(JKQtPlotter* pltImage) {
    pltImage->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltImage->set_displayMousePosition(true);
    pltImage->set_displayToolbar(false);
    pltImage->get_plotter()->set_maintainAspectRatio(true);
    pltImage->get_plotter()->set_aspectRatio(1);
    pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
    pltImage->get_plotter()->set_axisAspectRatio(1);
    pltImage->setXY(0,0,1,1);
    pltImage->setAbsoluteXY(0,1,0,1);
    pltImage->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltImage->get_plotter()->getYAxis()->set_axisMinWidth(1);
    pltImage->get_plotter()->getXAxis()->set_labelType(JKQTPCALTdefault);
    pltImage->get_plotter()->getYAxis()->set_labelType(JKQTPCALTdefault);
    pltImage->get_plotter()->getXAxis()->set_labelDigits(2);
    pltImage->get_plotter()->getYAxis()->set_labelDigits(2);
    pltImage->get_plotter()->getXAxis()->set_minTicks(5);
    //pltImage->get_plotter()->getXAxis()->addAxisTickLabel(0,"0");
    pltImage->get_plotter()->getYAxis()->set_minTicks(5);
    //pltImage->get_plotter()->getYAxis()->addAxisTickLabel(0,"0");

    pltImage->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltImage->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltImage->get_plotter()->getXAxis()->set_axisLabel("");
    pltImage->get_plotter()->getYAxis()->set_axisLabel("");
    pltImage->get_plotter()->setGrid(false);
    pltImage->get_plotter()->set_useAntiAliasingForSystem(true);
    pltImage->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltImage->set_userActionCompositionMode(QPainter::CompositionMode_Xor);
}

void QFSPIMLightsheetEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    // called when this widget should be connected to a new QFEvaluationItem

    QFSPIMLightsheetEvaluationItem* item=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFSPIMLightsheetEvaluationItem* item_old=qobject_cast<QFSPIMLightsheetEvaluationItem*>(old);

    if (old!=NULL) {
        /* disconnect item_old and clear all widgets here */
        disconnect(item_old, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
    }



    if (item) {
        updatingData=true;

        /* connect widgets and fill with data from item here */
        connect(item, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        
        updatingData=false;
    }


    displayPreview();
    displayEvaluationResults();
}

void QFSPIMLightsheetEvaluationEditor::resultsChanged() {
    /* some other evaluation or the user changed the results stored in the current raw data record,
       so redisplay */
    displayPreview();
    displayEvaluationResults();
}

void QFSPIMLightsheetEvaluationEditor::readSettings() {
    // read widget settings
    if (!settings) return;
    currentSaveDirectory=settings->getQSettings()->value(QString("spim_lightsheet_eval/editor/lastSaveDirectory"), currentSaveDirectory).toString();
};

void QFSPIMLightsheetEvaluationEditor::writeSettings() {
    // write widget settings
    if (!settings) return;
    settings->getQSettings()->setValue(QString("spim_lightsheet_eval/editor/lastSaveDirectory"), currentSaveDirectory);
}

void QFSPIMLightsheetEvaluationEditor::updateStack() {
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    int stack=ui->cmbStack->currentIndex();

    if (data) {
        bool oldUpdt=updatingData;
        updatingData=true;


        ui->cmbChannel->clear();
        for (int i=0; i<data->getImageStackChannels(stack); i++) {
            ui->cmbChannel->addItem(data->getImageStackChannelName(stack, i));
        }
        ui->cmbChannel->setCurrentIndex(0);

        ui->spinStackPos->setRange(0, data->getImageStackFrames(stack)-1);
        ui->horizontalScrollBar->setRange(0, data->getImageStackFrames(stack)-1);
        ui->spinStackPos->setValue(0);

        ui->spinDeltaX->setValue(record->getProperty(eval->getEvaluationResultID(stack)+"_DELTAX", record->getProperty("PIXEL_WIDTH", ui->spinDeltaX->value()).toDouble()).toDouble());
        ui->spinDeltaZ->setValue(record->getProperty(eval->getEvaluationResultID(stack)+"_DELTAZ", record->getProperty("DELTAZ", ui->spinDeltaZ->value()).toDouble()).toDouble());
        ui->chkUseMask->setChecked(record->getProperty(eval->getEvaluationResultID(stack)+"_USEMASK", record->getProperty("USEMASK", ui->chkUseMask->isChecked()).toBool()).toBool());
        ui->chkEstimateInitial->setChecked(record->getProperty("ESTIMATE_INITIAL_FF", record->getProperty("ESTIMATE_INITIAL_FF", ui->chkEstimateInitial->isChecked()).toBool()).toBool());
        ui->chkScaleIntensityFit->setChecked(record->getProperty("FIT_SCALE_INTENSITY", record->getProperty("FIT_SCALE_INTENSITY", ui->chkScaleIntensityFit->isChecked()).toBool()).toBool());
        ui->cmbOrientation->setCurrentIndex(record->getProperty(eval->getEvaluationResultID(stack)+"_ORIENTATION", record->getProperty("ORIENTATION", ui->cmbOrientation->currentIndex()).toInt()).toInt());
        ui->cmbModel->setCurrentFitFunction(record->getProperty(eval->getEvaluationResultID(stack)+"_MODEL", record->getProperty("MODEL", ui->cmbModel->currentFitFunctionID()).toString()).toString());
        ui->cmbAlgorithm->setCurrentAlgorithm(record->getProperty(eval->getEvaluationResultID(stack)+"_ALGORITHM", record->getProperty("ALGORITHM", ui->cmbAlgorithm->currentFitAlgorithmID()).toString()).toString());
        ui->spinWidthRangeMin->setValue(record->getProperty(eval->getEvaluationResultID(stack)+"_WIDTHRANGEMIN", 0).toDouble());
        ui->spinWidthRangeMax->setValue(record->getProperty(eval->getEvaluationResultID(stack)+"_WIDTHRANGEMAX", 5).toDouble());
        ui->spinRangeMin->setValue(record->getProperty(eval->getEvaluationResultID(stack)+"_RANGEMIN", 0).toDouble());
        ui->spinRangeMax->setValue(record->getProperty(eval->getEvaluationResultID(stack)+"_RANGEMAX", 128).toDouble());
        ui->chkErrorsParam->setChecked(record->getProperty(eval->getEvaluationResultID(stack)+"_PARAMERRORS", true).toBool());
        ui->chkErrorsBeamPos->setChecked(record->getProperty(eval->getEvaluationResultID(stack)+"_BEAMPOSERRORS", false).toBool());
        ui->cmbParameter->setCurrentIndex(record->getProperty(eval->getEvaluationResultID(stack)+"_PARAMETER", record->getProperty("PARAMETER", ui->cmbParameter->currentIndex()).toInt()).toInt());

        updatingData=oldUpdt;
    }
    // ensure that data of new highlighted record is displayed
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::paramChanged(int i)
{
    //QList<QVariant> md=ui->cmbParameter->itemData(i).toList();


    //md<<c_X<<cv<<ce;
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_PARAMETER", i, false, false);
    }
    ui->pltParam->getYAxis()->set_axisLabel(ui->cmbParameter->itemData(i).toString());
    //ui->pltParam->get_plotter()->setOnlyGraphVisible(i);
    ui->pltParam->get_plotter()->setOnlyNthGraphsVisible(i, ui->cmbParameter->count());
    ui->pltParam->zoomToFit();
    ui->pltParam->update_plot();
}

void QFSPIMLightsheetEvaluationEditor::on_chkErrorsParam_toggled(bool checked)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_PARAMERRORS", checked, false, false);
    }
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::on_chkErrorsBeamPos_toggled(bool checked)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_BEAMPOSERRORS", checked, false, false);
    }
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::on_chkUseMask_toggled(bool checked)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_USEMASK", checked, false, false);
    }
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::on_chkEstimateInitial_toggled(bool checked)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty("ESTIMATE_INITIAL_FF", checked, false, false);
    }
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::on_chkScaleIntensityFit_toggled(bool checked)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty("FIT_SCALE_INTENSITY", checked, false, false);
    }
    displayEvaluationResults();
    displayPreview();
}


void QFSPIMLightsheetEvaluationEditor::on_cmbOrientation_currentIndexChanged(int index)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_ORIENTATION", index, false, false);
    }
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::on_cmbModel_currentIndexChanged(int /*index*/)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_MODEL", ui->cmbModel->currentFitFunctionID(), false, false);
    }
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::on_cmbAlgorithm_currentIndexChanged(int /*index*/)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_ALGORITHM", ui->cmbAlgorithm->currentFitAlgorithmID(), false, false);
    }
}
void QFSPIMLightsheetEvaluationEditor::on_spinDeltaX_valueChanged(double value) {
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_DELTAX", value, false, false);
    }
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::on_spinDeltaZ_valueChanged(double value) {
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_DELTAZ", value, false, false);
    }
    displayEvaluationResults();
    displayPreview();
}


void QFSPIMLightsheetEvaluationEditor::showImageCut(QFSPIMLightsheetEvaluationItem* eval, QFRawDataRecord* record, QFRDRImageStackInterface* data, int stack, int stack_pos, double x, double y, int chan) {
    int channel=chan;
    if (chan<0) channel=ui->cmbChannel->currentIndex();
    JKQTPdatastore* ds=ui->pltFit->getDatastore();
    double* img=data->getImageStack(stack, stack_pos, channel);
    int w=data->getImageStackWidth(stack);
    int h=data->getImageStackHeight(stack);
    QString resultID=eval->getEvaluationResultID(stack, channel);



    QVector<QColor> cols;
    cols<<QColor("blue")<<QColor("red")<<QColor("green")<<QColor("magenta");


    JKQTPxyLineErrorGraph* plteLineFitData=new JKQTPxyLineErrorGraph(ui->pltFit->get_plotter());
    plteLineFitData->set_title(tr("%1: data").arg(ui->cmbChannel->itemText(channel)));
    plteLineFitData->set_drawLine(true);
    plteLineFitData->set_symbol(JKQTPcross);
    plteLineFitData->set_symbolSize(5);
    plteLineFitData->set_xErrorStyle(JKQTPnoError);
    plteLineFitData->set_yErrorStyle(JKQTPerrorPolygons);
    plteLineFitData->set_color(cols[channel%cols.size()]);
    ui->pltFit->addGraph(plteLineFitData);


    int item=-1;
    bool*mask=getCurrentMask();
    int start_offset=0;
    if(img) {
        if (ui->cmbOrientation->currentIndex()==0) {
            markDataX[0]=0;
            markDataX[1]=w;
            markDataY[0]=markDataY[1]=round(y);
            item=(int)round(y);
            double* data=(double*)qfMalloc(w*sizeof(double));
            double* dataX=(double*)qfMalloc(w*sizeof(double));
            int data_count=0;

            if (mask) {
                for (int yy=0; yy<w; yy++) {
                    if (!mask[item*w+yy]) {
                        data[data_count]=img[item*w+yy];
                        dataX[data_count]=double(yy)*ui->spinDeltaX->value()/1000.0;
                        data_count++;
                    } else {
                        if (data_count<=0) start_offset++;
                    }
                }
            } else {
                for (int yy=0; yy<w; yy++) {
                    data[data_count]=img[item*w+yy];
                    dataX[data_count]=double(yy)*ui->spinDeltaX->value()/1000.0;
                    data_count++;
                }
            }

            if (data_count>0) {
                //int c_x=ds->addLinearColumn(w, 0, double(w)*ui->spinDeltaX->value()/1000.0, "dataX");
                int c_x=ds->addCopiedColumn(dataX, data_count, tr("X: %1").arg(ui->cmbChannel->itemText(channel)));
                int c_y=ds->addCopiedColumn(data, data_count, tr("I: %1").arg(ui->cmbChannel->itemText(channel)));
                plteLineFitData->set_xColumn(c_x);
                plteLineFitData->set_yColumn(c_y);
                plteLineFitData->set_xErrorColumn(-1);
                plteLineFitData->set_yErrorColumn(-1);
            }
            qfFree(data);
            qfFree(dataX);

        } else {
            markDataY[0]=0;
            markDataY[1]=h;
            markDataX[0]=markDataX[1]=round(x);

            item=(int)round(x);
            double* data=(double*)qfMalloc(h*sizeof(double));
            double* dataX=(double*)qfMalloc(h*sizeof(double));

            int data_count=0;
            //int start_offset=0;
            if (mask) {
                for (int yy=0; yy<h; yy++) {
                    if (!mask[item*w+yy]) {
                        data[data_count]=img[yy*w+item];
                        dataX[data_count]=double(yy)*ui->spinDeltaX->value()/1000.0;
                        data_count++;
                    } else {
                        if (data_count<=0) start_offset++;
                    }
                }
            } else {
                for (int yy=0; yy<h; yy++) {
                    data[data_count]=img[yy*w+item];
                    dataX[data_count]=double(yy)*ui->spinDeltaX->value()/1000.0;
                    data_count++;
                }
            }


            //int c_x=ds->addLinearColumn(h, 0, double(h)*ui->spinDeltaX->value()/1000.0, "dataX");
            int c_x=ds->addCopiedColumn(dataX, data_count, tr("X: %1").arg(ui->cmbChannel->itemText(channel)));
            int c_y=ds->addCopiedColumn(data, data_count, tr("I: %1").arg(ui->cmbChannel->itemText(channel)));
            plteLineFitData->set_xColumn(c_x);
            plteLineFitData->set_yColumn(c_y);
            plteLineFitData->set_xErrorColumn(-1);
            plteLineFitData->set_yErrorColumn(-1);
            qfFree(data);
            qfFree(dataX);
        }
    }

    tableFitResults->disableSignals();
    QString param;
    if (item>=0 && record->resultsExists(resultID, param=QString("fitok_frame%1").arg(stack_pos))) {
        bool fitOK=record->resultsGetInBooleanList(resultID, param, item, false);
        QString modelID=record->resultsGetAsString(resultID, param=QString("fit_model"));
        QFFitFunction* model=QFFitFunctionManager::getInstance()->createFunction(modelID);
        if (fitOK && model) {
            QStringList paramIDs=model->getParameterIDs();
            int pcount=model->paramCount();
            QVector<double> params;
            QString parTxt="";
            for (int i=0; i<paramIDs.size(); i++) {
                QFFitFunction::ParameterDescription d=model->getDescription(paramIDs[i]);
                double v=record->resultsGetInNumberList(resultID, param=QString("%2_frame%1").arg(stack_pos).arg(paramIDs[i]), item, d.initialValue);
                double e=record->resultsGetErrorInNumberErrorList(resultID, param=QString("%2_frame%1").arg(stack_pos).arg(paramIDs[i]), item, 0);
                params.append(v);


                tableFitResults->setCellCreate(i, 0, d.label);
                tableFitResults->setCellCreate(i, 1+channel*6+0, v);
                tableFitResults->setCellCreate(i, 1+channel*6+1, e);
                tableFitResults->setCellCreate(i, 1+channel*6+2, d.unitLabel);
                tableFitResults->setColumnTitle(1+channel*6+0, tr("%1: value").arg(ui->cmbChannel->itemText(channel)));
                tableFitResults->setColumnTitle(1+channel*6+1, tr("error"));

                parTxt+=QString("<tr>")+QString("<td><i>%1</i></td><td>%2</td><td>%3</td><td>%4</td>").arg(d.label).arg(v).arg(e).arg(d.unitLabel);
                if (paramIDs[i].toUpper()=="POSITION" || paramIDs[i].toUpper()=="WIDTH") {
                    parTxt+=QString("<td><i>%1</i></td><td>%2</td><td>&mu;m</td>").arg(v*ui->spinDeltaX->value()/1000.0).arg(e*ui->spinDeltaX->value()/1000.0);
                    tableFitResults->setCellCreate(i, 1+channel*6+3, v*ui->spinDeltaX->value()/1000.0);
                    tableFitResults->setCellCreate(i, 1+channel*6+4, e*ui->spinDeltaX->value()/1000.0);
                    tableFitResults->setCellCreate(i, 1+channel*6+5, tr("&mu;m"));
                    tableFitResults->setColumnTitle(1+channel*6+3, tr("value"));
                    tableFitResults->setColumnTitle(1+channel*6+4, tr("error"));
                }
                parTxt+=QString("</tr>");
            }
            JKQTPxQFFitFunctionLineGraph* lg=new JKQTPxQFFitFunctionLineGraph(ui->pltFit->get_plotter());
            lg->set_params(params);
            lg->set_fitFunction(model, true);
            lg->set_drawLine(true);
            lg->set_lineWidth(2);
            lg->set_scaleX(ui->spinDeltaX->value()/1000.0);
            lg->set_title(tr("%1: fit").arg(ui->cmbChannel->itemText(channel)));
            lg->set_color(plteLineFitData->get_color().darker());

            ui->pltFit->addGraph(lg);

            QString fitResults=tr("<ul><li>model: <b>%1</b></li><li>fit OK: <b>%2</b></li></ul>" ).arg(model->name()).arg(boolToQString(fitOK));
            /*<li>parameters:<br><table border=\"1\" cellpadding=\"1\"><tr><th>parameter</th><th>value</th><th>error</th><th></th></tr>%2</table></li></ul>"*/

            ui->labFitResults->setText(fitResults);
        } else {
            if (!model) ui->labFitResults->setText(tr("no fit available"));
            if (!fitOK) ui->labFitResults->setText(tr("fit did not succeed!"));
            if (model) delete model;

        }
    } else {
        ui->labFitResults->setText(tr("no fit available"));
    }
    tableFitResults->enableSignals(true);

}


void QFSPIMLightsheetEvaluationEditor::on_pltImage_plotMouseClicked(double x, double y, Qt::KeyboardModifiers /*modifiers*/, Qt::MouseButton /*button*/) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();
    int channel=ui->cmbChannel->currentIndex();
    int stack_pos=ui->spinStackPos->value();
    QString resultID=eval->getEvaluationResultID(stack, channel);

    if ((!record)||(!eval)||(!data)) return;

    lastMousePreviewX=x;
    lastMousePreviewY=y;

    bool updI=ui->pltImage->get_doDrawing();
    bool updF=ui->pltFit->get_doDrawing();
    ui->pltImage->set_doDrawing(false);
    ui->pltFit->set_doDrawing(false);

    JKQTPdatastore* ds=ui->pltFit->getDatastore();
    ui->pltFit->clearGraphs(true);
    ds->clear();
    tableFitResults->setReadonly(false);
    tableFitResults->clear();

    for (int channel=0; channel<ui->cmbChannel->count(); channel++) {
        showImageCut(eval, record, data, stack, stack_pos, x, y, channel);
    }
    tableFitResults->setReadonly(true);
    tableFitResults->enableSignals(true);
    ui->tabFitResults->setModel(tableFitResults);
    ui->tabFitResults->resizeColumnsToContents();


    ui->pltImage->zoomToFit();
    ui->pltFit->zoomToFit();
    if (updI) {
        ui->pltImage->set_doDrawing(true);
        ui->pltImage->update_plot();
    }
    if (updF) {
        ui->pltFit->set_doDrawing(true);
        ui->pltFit->update_plot();
    }
}


void QFSPIMLightsheetEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    // this slot is called when the user selects a new record in the raw data record list on the RHS of this widget in the evaluation dialog
    
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(currentRecord);
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(displayPreview()));

    if (data) { // if we have a valid object, update
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(displayPreview())); // redisplay data, if data changed
        lastMousePreviewX=0;
        lastMousePreviewY=0;

        updatingData=true;
        setUpdatesEnabled(false);
        // assign values to widgets here 
        ui->cmbStack->clear();
        for (int i=0; i<data->getImageStackCount(); i++) {
            ui->cmbStack->addItem(data->getImageStackDescription(i));
        }
        ui->cmbStack->setCurrentIndex(0);
        updateStack();


        setUpdatesEnabled(true);
        updatingData=false;
    }
    
    // ensure that data of new highlighted record is displayed
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::displayEvaluationResults() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if ((!record)||(!eval)||(!data)) return;

    bool upd=updatesEnabled();
    setUpdatesEnabled(false);
    bool updP=ui->pltParam->get_doDrawing();
    bool updB=ui->pltBeamPos->get_doDrawing();
    bool updD=ui->pltBeamPosDifference->get_doDrawing();
    bool updW=ui->pltBeamWidth->get_doDrawing();
    ui->pltParam->set_doDrawing(false);
    ui->pltBeamPos->set_doDrawing(false);
    ui->pltBeamPosDifference->set_doDrawing(false);
    ui->pltBeamWidth->set_doDrawing(false);

    ui->pltParam->clearGraphs();
    ui->pltParam->getDatastore()->clear();
    JKQTPdatastore* ds=ui->pltParam->getDatastore();

    ui->pltBeamPos->clearGraphs();
    ui->pltBeamPos->get_plotter()->useExternalDatastore(ds);
    ui->pltBeamPosDifference->clearGraphs();
    ui->pltBeamPosDifference->get_plotter()->useExternalDatastore(ds);
    ui->pltBeamWidth->clearGraphs();
    ui->pltBeamWidth->get_plotter()->useExternalDatastore(ds);


    int z=data->getImageStackFrames(stack);
    double deltaZ=ui->spinDeltaZ->value()/1000.0;
    double deltaX=ui->spinDeltaX->value()/1000.0;

    int paramItem=ui->cmbParameter->currentIndex();
    disconnect(ui->cmbParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged(int)));
    ui->cmbParameter->clear();


    QString param;
    QList<QVector<double> > posV, posE;
    int c_X=ds->addLinearColumn(z, 0, double(z-1)*deltaZ, "lightsheet_pos");


    ui->spinRangeMin->setRange(0, z*deltaZ);
    ui->spinRangeMin->setValue(0);
    ui->spinRangeMax->setRange(0, z*deltaZ);
    ui->spinRangeMax->setValue(z*deltaZ);

    updateFitResultRanges();


    for (int channel=0; channel<ui->cmbChannel->count(); channel++) {
        QString resultID=eval->getEvaluationResultID(stack, channel);
        //qDebug()<<"checking fit model";
        if (record->resultsExists(resultID, param=QString("fit_model"))) {
            //qDebug()<<"has fit model";
            QString modelID=record->resultsGetAsString(resultID, param);
            QFFitFunction* model=QFFitFunctionManager::getInstance()->createFunction(modelID);
            if (model) {
                QStringList paramIDs=model->getParameterIDs();
                int pcount=model->paramCount();
                int c_WIDTH=-1;
                int c_POSITION=-1;
                int ce_WIDTH=-1;
                int ce_POSITION=-1;
                for (int i=0; i<paramIDs.size(); i++) {
                    QFFitFunction::ParameterDescription d=model->getDescription(paramIDs[i]);



                    QVector<double> v=record->resultsGetAsDoubleList(resultID, param=paramIDs[i]);
                    QVector<double> e=record->resultsGetErrorAsDoubleList(resultID, param=paramIDs[i]);

                    if (v.size()>0) {
                        QString unit=d.unit;
                        if (paramIDs[i].toUpper()=="POSITION" || paramIDs[i].toUpper()=="WIDTH") {
                            for (int vi=0; vi<v.size(); vi++) {
                                v[vi]=v[vi]*deltaX;
                            }
                            for (int vi=0; vi<e.size(); vi++) {
                                e[vi]=e[vi]*deltaX;
                            }
                            unit=tr("micrometers");
                        }

                        int cv=ds->addCopiedColumn(v.data(), v.size(), paramIDs[i]);
                        int ce=ds->addCopiedColumn(e.data(), e.size(), tr("%1_error").arg(paramIDs[i]));

                        if (paramIDs[i].toUpper()=="POSITION") {
                            c_POSITION=cv;
                            ce_POSITION=ce;
                            posV.append(v);
                            posE.append(e);
                        } else if (paramIDs[i].toUpper()=="WIDTH") {
                            c_WIDTH=cv;
                            ce_WIDTH=ce;
                        } else {
                            if (paramItem<0) paramItem=i;
                        }

                        QList<QVariant> md;
                        md<<c_X<<cv<<ce;
                        if (channel==0) {
                            ui->cmbParameter->addItem(tr("%1").arg(d.name), tr("%1 [%2]").arg(d.name).arg(unit));
                        }

                        JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(ui->pltParam->get_plotter());
                        g->set_xColumn(c_X);
                        g->set_yColumn(cv);
                        g->set_xErrorColumn(-1);
                        g->set_drawLine(false);
                        if (ui->chkErrorsParam->isChecked()) {
                            g->set_yErrorColumn(ce);
                            g->set_yErrorStyle(JKQTPerrorBars);
                        } else {
                            g->set_yErrorColumn(-1);
                            g->set_yErrorStyle(JKQTPnoError);
                        }

                        g->set_symbol(JKQTPcross);
                        g->set_xErrorStyle(JKQTPnoError);
                        g->set_visible((i%paramIDs.size())==paramItem);
                        g->set_errorColor(g->get_color().darker());
                        g->set_title(tr("%2, channel %1").arg(channel+1).arg(d.name));
                        ui->pltParam->addGraph(g);
                    }

                }

                JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(ui->pltBeamWidth->get_plotter());
                g->set_xColumn(c_X);
                g->set_yColumn(c_WIDTH);
                g->set_xErrorColumn(-1);
                g->set_yErrorColumn(ce_WIDTH);
                g->set_drawLine(false);
                g->set_yErrorStyle(JKQTPerrorBars);
                g->set_symbol(JKQTPcross);
                g->set_xErrorStyle(JKQTPnoError);
                g->set_visible(true);
                g->set_errorColor(g->get_color().darker());
                g->set_title(tr("channel %1").arg(channel+1));
                ui->pltBeamWidth->addGraph(g);

                g=new JKQTPxyLineErrorGraph(ui->pltBeamPos->get_plotter());
                g->set_xColumn(c_X);
                g->set_yColumn(c_POSITION);
                g->set_xErrorColumn(-1);
                g->set_yErrorColumn(ce_POSITION);
                g->set_drawLine(false);
                g->set_yErrorStyle(JKQTPerrorBars);
                g->set_symbol(JKQTPcross);
                g->set_xErrorStyle(JKQTPnoError);
                g->set_visible(true);
                g->set_errorColor(g->get_color().darker());
                g->set_title(tr("channel %1").arg(channel+1));
                ui->pltBeamPos->addGraph(g);

            }
        }
    }


    if (posV.size()>1) {
        QVector<double> posD, posDE;
        for (int i=0; i<qMin(posV[0].size(), posV[1].size()); i++) {
            posD.append(posV[0].value(i,0)-posV[1].value(i,0));
            posDE.append(sqrt(posE[0].value(i,0)*posE[0].value(i,0)+posE[1].value(i,0)*posE[1].value(i,0)));
        }
        int c_POSITIONDIST=ds->addCopiedColumn(posD.data(), posD.size(), tr("position_difference"));
        int ce_POSITIONDIST=ds->addCopiedColumn(posDE.data(), posDE.size(), tr("position_difference_error"));

        JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(ui->pltBeamPosDifference->get_plotter());
        g->set_xColumn(c_X);
        g->set_yColumn(c_POSITIONDIST);
        g->set_xErrorColumn(-1);
        if (ui->chkErrorsBeamPos->isChecked())
            g->set_yErrorColumn(ce_POSITIONDIST);
        else
            g->set_yErrorColumn(-1);
        g->set_drawLine(false);
        g->set_yErrorStyle(JKQTPerrorBars);
        g->set_symbol(JKQTPcross);
        g->set_errorColor(g->get_color().darker());
        g->set_xErrorStyle(JKQTPnoError);
        g->set_visible(true);
        ui->pltBeamPosDifference->addGraph(g);
    }

    connect(ui->cmbParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged(int)));
    ui->cmbParameter->setCurrentIndex(paramItem);


    ui->pltParam->zoomToFit();
    ui->pltBeamPos->zoomToFit();
    ui->pltBeamPosDifference->zoomToFit();
    ui->pltBeamWidth->zoomToFit();
    if (updP) {
        ui->pltParam->set_doDrawing(true);
        ui->pltParam->update_plot();
    }
    if (updB) {
        ui->pltBeamPos->set_doDrawing(true);
        ui->pltBeamPos->update_plot();
    }
    if (updD) {
        ui->pltBeamPosDifference->set_doDrawing(true);
        ui->pltBeamPosDifference->update_plot();
    }
    if (updW) {
        ui->pltBeamWidth->set_doDrawing(true);
        ui->pltBeamWidth->update_plot();
    }
    setUpdatesEnabled(upd);
}


void QFSPIMLightsheetEvaluationEditor::updateFitResultRanges()
{
    if (updatingData) return;
    ui->pltParam->setAbsoluteX(ui->spinRangeMin->value(), ui->spinRangeMax->value());
    ui->pltParam->get_plotter()->setGraphsDataRange(ui->spinRangeMin->value()/(ui->spinDeltaZ->value()/1000.0), ui->spinRangeMax->value()/(ui->spinDeltaZ->value()/1000.0));
    ui->pltParam->zoomToFit();
    ui->pltParam->update_plot();

    ui->pltBeamPos->setAbsoluteX(ui->spinRangeMin->value(), ui->spinRangeMax->value());
    ui->pltBeamPos->get_plotter()->setGraphsDataRange(ui->spinRangeMin->value()/(ui->spinDeltaZ->value()/1000.0), ui->spinRangeMax->value()/(ui->spinDeltaZ->value()/1000.0));
    ui->pltBeamPos->zoomToFit();
    ui->pltBeamPos->update_plot();

    ui->pltBeamPosDifference->setAbsoluteX(ui->spinRangeMin->value(), ui->spinRangeMax->value());
    ui->pltBeamPosDifference->get_plotter()->setGraphsDataRange(ui->spinRangeMin->value()/(ui->spinDeltaZ->value()/1000.0), ui->spinRangeMax->value()/(ui->spinDeltaZ->value()/1000.0));
    ui->pltBeamPosDifference->zoomToFit();
    ui->pltBeamPosDifference->update_plot();

    ui->pltBeamWidth->setAbsoluteX(ui->spinRangeMin->value(), ui->spinRangeMax->value());
    ui->pltBeamWidth->setAbsoluteY(ui->spinWidthRangeMin->value(), ui->spinWidthRangeMax->value());
    ui->pltBeamWidth->get_plotter()->setGraphsDataRange(ui->spinRangeMin->value()/(ui->spinDeltaZ->value()/1000.0), ui->spinRangeMax->value()/(ui->spinDeltaZ->value()/1000.0));
    ui->pltBeamWidth->zoomToFit();
    ui->pltBeamWidth->update_plot();

    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_WIDTHRANGEMIN", ui->spinWidthRangeMin->value(), false, false);
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_WIDTHRANGEMAX", ui->spinWidthRangeMax->value(), false, false);
    }
}

void QFSPIMLightsheetEvaluationEditor::saveImageCutSeries()
{
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if ((!record)||(!eval)||(!data)) return;

    QList<QVector<double> > dataOut;
    QStringList headers;
    int w=data->getImageStackWidth(stack);
    int h=data->getImageStackHeight(stack);


    QFESPIMLightSheetEvaluationCopyConfigDialog* dlg=new QFESPIMLightSheetEvaluationCopyConfigDialog(this);
    if (dlg->exec()) {


        for (int z=0; z<(int64_t)data->getImageStackFrames(stack); z++) {
            for (int c=0; c<data->getImageStackChannels(stack); c++) {
                QString resultID=eval->getEvaluationResultID(stack, c);
                double* img=data->getImageStack(stack, z, c);
                int item=lastMousePreviewX;
                if (ui->cmbOrientation->currentIndex()==0) {
                    item=lastMousePreviewY;
                }
                QString param;
                if (item>=0 && record->resultsExists(resultID, param=QString("fitok_frame%1").arg(z))) {
                    bool fitOK=record->resultsGetInBooleanList(resultID, param, item, false);
                    QString modelID=record->resultsGetAsString(resultID, param=QString("fit_model"));
                    QFFitFunction* model=QFFitFunctionManager::getInstance()->createFunction(modelID);
                    double position=0;
                    double amplitude=1;
                    QVector<double> params;
                    if (fitOK && model) {
                        QStringList paramIDs=model->getParameterIDs();
                        for (int i=0; i<paramIDs.size(); i++) {
                            QFFitFunction::ParameterDescription d=model->getDescription(paramIDs[i]);
                            double v=record->resultsGetInNumberList(resultID, param=QString("%2_frame%1").arg(z).arg(paramIDs[i]), item, d.initialValue);
                            double e=record->resultsGetErrorInNumberErrorList(resultID, param=QString("%2_frame%1").arg(z).arg(paramIDs[i]), item, 0);
                            params.append(v);
                            if (paramIDs[i].toUpper()=="POSITION") {
                                position=v;
                            }
                            if (paramIDs[i].toUpper()=="AMPLITUDE") {
                                amplitude=v;
                            }
                        }
                    }
                    double dx=ui->spinDeltaX->value()/1000.0;
                    double dz=ui->spinDeltaZ->value()/1000.0;
                    if (dlg->getUnits()==1) {
                        dx=1;
                        dz=1;
                    }
                    if (!dlg->getShiftCenter()) {
                        position=0;
                    }
                    //qDebug()<<z<<c<<position<<amplitude;
                    QVector<double> zz, xx, ff, ff1, fit, fit1;
                    if (ui->cmbOrientation->currentIndex()==0) {
                        // x=...
                        for (int x=0; x<w; x++) {
                            xx.append((double(x)-position)*dx);
                            ff.append(img[item*w+x]);
                            ff1.append(img[item*w+x]/amplitude);
                            zz.append(z*dz);
                        }
                    } else {
                        // y=...
                        for (int y=0; y<h; y++) {
                            xx.append((double(y)-position)*dx);
                            ff.append(img[y*w+item]);
                            ff1.append(img[y*w+item]/amplitude);
                            zz.append(z*dz);
                        }
                    }
                    if (model && params.size()>=model->paramCount()) {
                        for (int i=0; i<xx.size(); i++) {
                            fit.append(model->evaluate(xx[i]/dx+position, params.data()));
                            fit1.append(fit.last()/amplitude);
                        }
                    }
                    dataOut.append(zz);
                    dataOut.append(xx);
                    dataOut.append(ff);
                    if (dlg->getSaveNormalized()) dataOut.append(ff1);
                    if (dlg->getSaveFit() && model) {
                        dataOut.append(fit);
                        if (dlg->getSaveNormalized()) dataOut.append(fit1);
                    }
                    headers<<tr("z=%1, c=%2: Z").arg(z).arg(c);
                    headers<<tr("z=%1, c=%2: X").arg(z).arg(c);
                    headers<<tr("z=%1, c=%2: LS").arg(z).arg(c);
                    if (dlg->getSaveNormalized()) headers<<tr("z=%1, c=%2: LS/amplitude").arg(z).arg(c);
                    if (dlg->getSaveFit() && model) {
                        headers<<tr("z=%1, c=%2: fit").arg(z).arg(c);
                        if (dlg->getSaveNormalized()) headers<<tr("z=%1, c=%2: fit/amplitude").arg(z).arg(c);
                    }
                }
            }
        }

        if (dlg->getOutput()==0) QFDataExportHandler::save(dataOut, headers);
        else if (dlg->getOutput()==1) QFDataExportHandler::copyCSV(dataOut, headers);
        else if (dlg->getOutput()==2) QFDataExportHandler::copyMatlab(dataOut);
    }
    delete dlg;

    //QString filename=qfGetOpenFileName(this, tr("save image cut series ..."), "", "CSV file")
}

void QFSPIMLightsheetEvaluationEditor::displayPreview() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if ((!record)||(!eval)||(!data)) return;

    bool updI=ui->pltImage->get_doDrawing();
    bool updF=ui->pltFit->get_doDrawing();
    ui->pltImage->set_doDrawing(false);
    ui->pltFit->set_doDrawing(false);



    double* img=data->getImageStack(stack, ui->spinStackPos->value(), ui->cmbChannel->currentIndex());
    int w=data->getImageStackWidth(stack);
    int h=data->getImageStackHeight(stack);

    plteImage->set_data(img, w,h);
    plteImage->set_width(w);
    plteImage->set_height(h);
    ui->pltImage->setAbsoluteXY(0,w,0,h);
    ui->pltImage->get_plotter()->set_maintainAspectRatio(true);
    if (h>0) ui->pltImage->get_plotter()->set_aspectRatio(double(w)/double(h));
    else ui->pltImage->get_plotter()->set_aspectRatio(1);
    ui->pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
    if (h>0) ui->pltImage->get_plotter()->set_axisAspectRatio(double(w)/double(h));
    else ui->pltImage->get_plotter()->set_axisAspectRatio(1);


    on_pltImage_plotMouseClicked(lastMousePreviewX, lastMousePreviewY, Qt::NoModifier, Qt::LeftButton);


    ui->pltImage->zoomToFit();
    ui->pltFit->zoomToFit();
    if (updI) {
        ui->pltImage->set_doDrawing(true);
        ui->pltImage->update_plot();
    }
    if (updF) {
        ui->pltFit->set_doDrawing(true);
        ui->pltFit->update_plot();
    }
}






void QFSPIMLightsheetEvaluationEditor::evaluateCurrent() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    if (dlgEvaluationProgress) {
        delete dlgEvaluationProgress;
    }
    dlgEvaluationProgress=new QProgressDialog(this);
    
    dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));
    
    dlgEvaluationProgress->setRange(0,100);
    dlgEvaluationProgress->setValue(50);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // here we call doEvaluation to execute our evaluation for the current record only
    doEvaluation(record);

    dlgEvaluationProgress->setLabelText(tr("updating user interface ..."));
    displayEvaluationResults();
    displayPreview();
    dlgEvaluationProgress->setValue(100);
    dlgEvaluationProgress->close();

    QApplication::restoreOverrideCursor();
}


void QFSPIMLightsheetEvaluationEditor::evaluateAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i]; 
        // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
        QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
        if ((record)/*&&(data)*/) {
            dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));
            // here we call doEvaluation to execute our evaluation for the current record only
            doEvaluation(record);
        }
        dlgEvaluationProgress->setValue(i);
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dlgEvaluationProgress->setLabelText(tr("updating user interface ..."));
    displayEvaluationResults();
    displayPreview();
    dlgEvaluationProgress->setValue(recs.size());
    dlgEvaluationProgress->close();
    QApplication::restoreOverrideCursor();
}



void QFSPIMLightsheetEvaluationEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
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
    cursor.insertText(tr("Evaluation Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);

    // insert table with some data
    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(2, 2, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText(tr("raw data:"), fTextBold);
    table->cellAt(0, 1).firstCursorPosition().insertText(record->getName(), fText);
    table->cellAt(1, 0).firstCursorPosition().insertText(tr("ID:"), fTextBold);
    table->cellAt(1, 1).firstCursorPosition().insertText(QString::number(record->getID()));
    cursor.movePosition(QTextCursor::End);

}

bool *QFSPIMLightsheetEvaluationEditor::getCurrentMask()
{
    if (!current) return NULL;
    bool* res=NULL;
    QFRDRImageMaskInterface* dataM=qobject_cast<QFRDRImageMaskInterface*>(current->getHighlightedRecord());
    if (dataM) return dataM->maskGet();
    return res;
}













void QFSPIMLightsheetEvaluationEditor::doEvaluation(QFRawDataRecord *record) {
    if (!current) return;
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if ((!record)||(!eval)||(!data)) return;

    dlgEvaluationProgress->setRange(0,data->getImageStackChannels(stack)*data->getImageStackFrames(stack));
    dlgEvaluationProgress->setLabelText(tr("evaluating '%1' ... ").arg(record->getName()));
    dlgEvaluationProgress->setValue(0);

    QFFitAlgorithm* alg=ui->cmbAlgorithm->createCurrentInstance();
    QFFitFunction* model=ui->cmbModel->createCurrentInstance();

    for (int channel=0; channel<data->getImageStackChannels(stack); channel++) {
        QFSPIMLightsheetEvaluationItem::Orientation o=QFSPIMLightsheetEvaluationItem::fitRows;
        if (ui->cmbOrientation->currentIndex()==1) o=QFSPIMLightsheetEvaluationItem::fitColumns;
        ui->cmbChannel->setCurrentIndex(channel);
        for (int stackpos=0; stackpos<(int64_t)data->getImageStackFrames(stack); stackpos++) {
            if (dlgEvaluationProgress) {
                dlgEvaluationProgress->setLabelText(tr("evaluating '%1', ch.%2, z=%3 ... ").arg(record->getName()).arg(channel).arg(stackpos));
                dlgEvaluationProgress->setValue(channel*data->getImageStackFrames(stack)+stackpos);
                QApplication::processEvents();
            }
            eval->doEvaluation(record, stack, stackpos, channel, ui->spinDeltaX->value(), ui->spinDeltaZ->value(), model, alg, o, ui->chkUseMask->isChecked());
            if (dlgEvaluationProgress) {
                QApplication::processEvents();
                if (dlgEvaluationProgress->wasCanceled()) break;
            }
            lastMousePreviewX=data->getImageStackWidth(stack)/2;
            lastMousePreviewY=data->getImageStackHeight(stack)/2;
            ui->spinStackPos->setValue(stackpos);
            //displayPreview();
            if (stackpos%(data->getImageStackFrames(stack)/10)==0) displayEvaluationResults();
        }
        displayEvaluationResults();
        if (dlgEvaluationProgress) {
            if (dlgEvaluationProgress->wasCanceled()) break;
        }
    }

    delete alg;
    delete model;

}
