/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-01-21 11:37:05 +0100 (Mi, 21 Jan 2015) $  (revision $Rev: 3738 $)

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


#include "qfevalbeadscanpsf_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfevalbeadscanpsf_item.h"
#include "ui_qfevalbeadscanpsf_editor.h"
#include "qmoretextobject.h"
#include "cimg.h"
#include "qfrawdatarecordfactory.h"
#include "qffitfunctionplottools.h"
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

QFEvalBeadScanPSFEditor::QFEvalBeadScanPSFEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent):
    QFEvaluationEditor(services, propEditor, parent),
    ui(new Ui::QFEvalBeadScanPSFEditor)
{
    updatingData=true;
    
    currentSaveDirectory="";
    
    // setup widgets
    ui->setupUi(this);


    ui->histogram1->setSpaceSavingMode(true);
    ui->histogram2->setSpaceSavingMode(true);

    // create progress dialog for evaluation
    dlgEvaluationProgress=new QFListProgressDialog(this);
    dlgEvaluationProgress->hide();
    dlgEvaluationProgress->setWindowModality(Qt::WindowModal);
    
    // connect widgets 
    connect(ui->btnEvaluateCurrent, SIGNAL(clicked()), this, SLOT(evaluateCurrent()));
    connect(ui->btnEvaluateAll, SIGNAL(clicked()), this, SLOT(evaluateAll()));
    connect(ui->btnResetCurrent, SIGNAL(clicked()), this, SLOT(resetCurrent()));
    ui->btnPrintReport->setDefaultAction(actPrintReport);
    ui->btnSaveReport->setDefaultAction(actSaveReport);

    updatingData=false;
}

QFEvalBeadScanPSFEditor::~QFEvalBeadScanPSFEditor()
{
    delete ui;
    //delete dlgEvaluationProgress;
}

void QFEvalBeadScanPSFEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    // called when this widget should be connected to a new QFEvaluationItem

    QFEvalBeadScanPSFItem* item=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    QFEvalBeadScanPSFItem* item_old=qobject_cast<QFEvalBeadScanPSFItem*>(old);

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


    displayResults();
}

void QFEvalBeadScanPSFEditor::resultsChanged() {
    /* some other evaluation or the user changed the results stored in the current raw data record,
       so redisplay */
    displayResults();
}

void QFEvalBeadScanPSFEditor::readSettings() {
    // read widget settings
    if (!settings) return;
    currentSaveDirectory=settings->getQSettings()->value(QString("eval_beadscanpsf/editor/lastSaveDirectory"), currentSaveDirectory).toString();
};

void QFEvalBeadScanPSFEditor::writeSettings() {
    // write widget settings
    if (!settings) return;
    settings->getQSettings()->setValue(QString("eval_beadscanpsf/editor/lastSaveDirectory"), currentSaveDirectory);
}

QStringList QFEvalBeadScanPSFEditor::getImageWriterFilterList(QFPluginServices *pluginservices)
{
    QStringList l;
    int i=0;
    QFExporterImageSeries* r=NULL;
    while ((r=getImageWriter(i, pluginservices))!=NULL) {
        l.append(r->filter());
        delete r;
        i++;
    }
    return l;
}

QStringList QFEvalBeadScanPSFEditor::getImageWriterFormatNameList(QFPluginServices *pluginservices)
{
    QStringList l;
    int i=0;
    QFExporterImageSeries* r=NULL;
    while ((r=getImageWriter(i, pluginservices))!=NULL) {
        l.append(r->formatName());
        delete r;
        i++;
    }
    return l;
}

QFExporterImageSeries *QFEvalBeadScanPSFEditor::getImageWriter(int idx, QFPluginServices *pluginservices)
{
    QFExporterImageSeries* r=NULL;

    QStringList imp=pluginservices->getExporterManager()->getExporters<QFExporterImageSeries*>();

    if (idx>=0 && idx<imp.size()) {
        r=dynamic_cast<QFExporterImageSeries*>(pluginservices->getExporterManager()->createExporter(imp[idx]));
    }

    return r;
}

int QFEvalBeadScanPSFEditor::getImageWriterCount(QFPluginServices *pluginservices)
{
    QStringList imp=pluginservices->getExporterManager()->getExporters<QFExporterImageSeries*>();
    return imp.size();
}

QStringList QFEvalBeadScanPSFEditor::getImageWriterIDList(QFPluginServices *pluginservices)
{
    return pluginservices->getExporterManager()->getExporters<QFExporterImageSeries*>();
}

void QFEvalBeadScanPSFEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    // this slot is called when the user selects a new record in the raw data record list on the RHS of this widget in the evaluation dialog
    
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QFRawDataRecord* record=currentRecord; // possibly to a qobject_cast<> to the data type/interface you are working with here:
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(currentRecord);
    //disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData()));

    if (record && data && eval) { // if we have a valid object, update
        //connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData())); // redisplay data, if data changed

        updatingData=true;

        ui->spinA->setValue(record->getProperty(eval->getEvaluationResultID()+"_DELTAX", record->getProperty("PIXEL_WIDTH", ui->spinA->value()).toDouble()).toDouble());
        ui->spinZ->setValue(record->getProperty(eval->getEvaluationResultID()+"_DELTAZ", record->getProperty("DELTAZ", ui->spinZ->value()).toDouble()).toDouble());
        ui->spinROIXY->setValue(record->getProperty(eval->getEvaluationResultID()+"_ROI_XY", record->getProperty("PSF_ROI_XY", ui->spinROIXY->value()).toDouble()).toDouble());
        ui->spinROIZ->setValue(record->getProperty(eval->getEvaluationResultID()+"_ROI_Z", record->getProperty("PSF_ROI_Z", ui->spinROIZ->value()).toDouble()).toDouble());
        ui->spinPixPerFrame->setValue(record->getProperty(eval->getEvaluationResultID()+"_PIX_PER_FRAME", record->getProperty("BEADSEARCH_PIX_PER_FRAME", ui->spinPixPerFrame->value()).toDouble()).toDouble());
        ui->spinPSFWidth->setValue(record->getProperty(eval->getEvaluationResultID()+"_EST_PSF_WIDTH", record->getProperty("EST_PSF_WIDTH", ui->spinPSFWidth->value()).toDouble()).toDouble());
        ui->spinPSFHeight->setValue(record->getProperty(eval->getEvaluationResultID()+"_EST_PSF_HEIGHT", record->getProperty("EST_PSF_HEIGHT", ui->spinPSFHeight->value()).toDouble()).toDouble());
        ui->spinWZFraction->setValue(record->getProperty(eval->getEvaluationResultID()+"_WZ_FRACTION", record->getProperty("WZ_FRACTION", ui->spinWZFraction->value()).toDouble()).toDouble());

        ui->grpFilterBeads->setChecked(record->getProperty(eval->getEvaluationResultID()+"_FilterBeads", eval->getProperty("FilterBeads", ui->grpFilterBeads->isChecked()).toBool()).toBool());
        ui->spinAxRatioMin->setValue(record->getProperty(eval->getEvaluationResultID()+"_FilterBeads_AxRatMin", eval->getProperty("FilterBeads_AxRatMin", ui->spinAxRatioMin->value()).toDouble()).toDouble());
        ui->spinAxRatioMax->setValue(record->getProperty(eval->getEvaluationResultID()+"_FilterBeads_AxRatMax", eval->getProperty("FilterBeads_AxRatMax", ui->spinAxRatioMax->value()).toDouble()).toDouble());
        ui->chkMedianFIlter->setChecked(record->getProperty(eval->getEvaluationResultID()+"_MEDIAN", eval->getProperty("USE_MEDIAN_FILTER", ui->chkMedianFIlter->isChecked()).toBool()).toBool());

        ui->spinXYWidthMin->setValue(record->getProperty(eval->getEvaluationResultID()+"_FilterBeads_WxyMin", eval->getProperty("FilterBeads_WxyMin", ui->spinXYWidthMin->value()).toDouble()).toDouble());
        ui->spinXYWidthMax->setValue(record->getProperty(eval->getEvaluationResultID()+"_FilterBeads_WxyMax", eval->getProperty("FilterBeads_WxyMax", ui->spinXYWidthMax->value()).toDouble()).toDouble());
        ui->spinZWidthMin->setValue(record->getProperty(eval->getEvaluationResultID()+"_FilterBeads_WzMin", eval->getProperty("FilterBeads_WzMin", ui->spinZWidthMin->value()).toDouble()).toDouble());
        ui->spinZWidthMax->setValue(record->getProperty(eval->getEvaluationResultID()+"_FilterBeads_WzMax", eval->getProperty("FilterBeads_WzMax", ui->spinZWidthMax->value()).toDouble()).toDouble());

        ui->chkFilterAxRation->setChecked(record->getProperty(eval->getEvaluationResultID()+"_FilterBeads_Ax", eval->getProperty("FilterBeads_Ax", ui->chkFilterAxRation->isChecked()).toBool()).toBool());
        ui->chkFilterXYWidth->setChecked(record->getProperty(eval->getEvaluationResultID()+"_FilterBeads_Wxy", eval->getProperty("FilterBeads_Wxy", ui->chkFilterXYWidth->isChecked()).toBool()).toBool());
        ui->chkFilterZYWidth->setChecked(record->getProperty(eval->getEvaluationResultID()+"_FilterBeads_Wz", eval->getProperty("FilterBeads_Wz", ui->chkFilterZYWidth->isChecked()).toBool()).toBool());
        updatingData=false;
    }
    
    // ensure that data of new highlighted record is displayed
    displayResults();
}

void QFEvalBeadScanPSFEditor::displayEvaluationBead() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!record)||(!eval)||(!data)) return;

    int stack=0;

    ui->btnSaveAllROI->setEnabled(false);
    ui->btnSaveROI->setEnabled(false);

    if (eval->hasEvaluation(record)) {
        ui->btnSaveAllROI->setEnabled(true);
        ui->btnSaveROI->setEnabled(true);
        QString evalID=eval->getEvaluationResultID();
        int channel=ui->cmbChannel->currentIndex();
        int bead=ui->cmbBead->currentIndex();

        int ROIxy=ui->spinROIXY->value();
        int ROIz=ui->spinROIZ->value();
        int width=data->getImageStackWidth(stack);
        int height=data->getImageStackHeight(stack);
        int size_z=data->getImageStackFrames(stack);
        double deltaXY=ui->spinA->value();
        double deltaZ=ui->spinZ->value();

        int initPosX=record->resultsGetInNumberList(evalID, "beadsearch_initial_positions_x", bead, -1);
        int initPosY=record->resultsGetInNumberList(evalID, "beadsearch_initial_positions_y", bead, -1);
        int initPosZ=record->resultsGetInNumberList(evalID, "beadsearch_initial_positions_z", bead, -1);

        //qDebug()<<channel<<bead<<initPosX<<initPosY<<initPosZ;

        if (initPosX>=0 && initPosY>=0 && initPosZ>=0) {

            cimg_library::CImg<double> image(data->getImageStack(stack, 0, channel), width, height, size_z, true);
            cimg_library::CImg<double> roi=image.get_crop(initPosX-ROIxy/2, initPosY-ROIxy/2, initPosZ-ROIz/2, initPosX+ROIxy/2, initPosY+ROIxy/2, initPosZ+ROIz/2);

            QVector<double> roiXY(roi.width()*roi.height());
            QVector<double> roiXZ(roi.width()*roi.depth());
            QVector<double> roiYZ(roi.height()*roi.depth());
            QVector<double> cutX(roi.width()), cutXX(roi.width());
            QVector<double> cutY(roi.height()), cutYX(roi.height());
            QVector<double> cutZ(roi.depth()), cutZX(roi.depth());

            QVector<double> fitresCutX=record->resultsGetAsDoubleList(evalID, QString("channel%1_bead%2_cutx_fitresult").arg(channel).arg(bead));
            QString ffIDX=record->resultsGetAsString(evalID, QString("cutx_fitfunction"));
            QVector<double> fitresCutZ=record->resultsGetAsDoubleList(evalID, QString("channel%1_bead%2_cutz_fitresult").arg(channel).arg(bead));
            QString ffIDZ=record->resultsGetAsString(evalID, QString("cutz_fitfunction"));
            QVector<double> fitresCutY=record->resultsGetAsDoubleList(evalID, QString("channel%1_bead%2_cuty_fitresult").arg(channel).arg(bead));
            QString ffIDY=record->resultsGetAsString(evalID, QString("cuty_fitfunction"));

            QVector<double> fitresCutZ_Z=record->resultsGetAsDoubleList(evalID, QString("channel%1_bead%2_cutxz_zpos").arg(channel).arg(bead));
            for (int i=0; i<fitresCutZ_Z.size(); i++) fitresCutZ_Z[i]=fitresCutZ_Z[i]/1000.0;
            QVector<double> fitresCutZX=record->resultsGetAsDoubleList(evalID, QString("channel%1_bead%2_cutxz_width").arg(channel).arg(bead));
            QVector<double> fitresCutZXGB=record->resultsGetAsDoubleList(evalID, QString("channel%1_bead%2_cutxz_gaussianbeam_results").arg(channel).arg(bead));
            QString ffIDCutZX=record->resultsGetAsString(evalID, QString("cutxz_gaussianbeam_fitfunc"));

            QVector<double> fitresCutZY=record->resultsGetAsDoubleList(evalID, QString("channel%1_bead%2_cutyz_width").arg(channel).arg(bead));
            QVector<double> fitresCutZYGB=record->resultsGetAsDoubleList(evalID, QString("channel%1_bead%2_cutyz_gaussianbeam_results").arg(channel).arg(bead));
            QString ffIDCutZY=record->resultsGetAsString(evalID, QString("cutyz_gaussianbeam_fitfunc"));

            if (ui->chkLogscale->isChecked()) {
                for (int y=0; y<roi.height(); y++) {
                    for (int x=0; x<roi.width(); x++) {
                        roiXY[y*roi.width()+x]=log10(roi(x,y, ROIz/2));
                    }
                }
                for (int y=0; y<roi.height(); y++) {
                    for (int z=0; z<roi.depth(); z++) {
                        roiXZ[y*roi.depth()+z]=log10(roi(ROIxy/2,y,z));
                    }
                }
                for (int x=0; x<roi.width(); x++) {
                    for (int z=0; z<roi.depth(); z++) {
                        roiYZ[x*roi.depth()+z]=log10(roi(x,ROIxy/2,z));
                    }
                }
            } else {
                for (int y=0; y<roi.height(); y++) {
                    for (int x=0; x<roi.width(); x++) {
                        roiXY[y*roi.width()+x]=roi(x,y, ROIz/2);
                    }
                }
                for (int y=0; y<roi.height(); y++) {
                    for (int z=0; z<roi.depth(); z++) {
                        roiXZ[y*roi.depth()+z]=roi(ROIxy/2,y,z);
                    }
                }
                for (int x=0; x<roi.width(); x++) {
                    for (int z=0; z<roi.depth(); z++) {
                        roiYZ[x*roi.depth()+z]=roi(x,ROIxy/2,z);
                    }
                }
            }
            for (int x=0; x<roi.width(); x++) {
                cutX[x]=roi(x, ROIxy/2, ROIz/2);
                cutXX[x]=double(x-ROIxy/2)*deltaXY;
            }
            for (int y=0; y<roi.height(); y++) {
                cutY[y]=roi(ROIxy/2, y, ROIz/2);
                cutYX[y]=double(y-ROIxy/2)*deltaXY;
            }
            for (int z=0; z<roi.depth(); z++) {
                cutZ[z]=roi(ROIxy/2, ROIxy/2, z);
                cutZX[z]=double(z-ROIz/2)*deltaZ;
            }
            {
                ui->pltXY->set_doDrawing(false);
                JKQTPdatastore* ds=ui->pltXY->getDatastore();
                ui->pltXY->clearGraphs(true);
                ds->clear();
                ui->pltXY->setAbsoluteXY(cutXX.first(), cutXX.last(), cutYX.first(), cutYX.last());
                //ui->pltXY->setXY(cutX.first(), cutX.last(), cutY.first(), cutY.last());
                ui->pltXY->getXAxis()->set_axisLabel(tr("x [nm]"));
                ui->pltXY->getYAxis()->set_axisLabel(tr("y [nm]"));
                ui->pltXY->get_plotter()->set_axisAspectRatio(double(roi.width())/double(roi.height()));
                ui->pltXY->get_plotter()->set_aspectRatio(1);
                ui->pltXY->get_plotter()->set_maintainAspectRatio(true);
                ui->pltXY->get_plotter()->set_maintainAxisAspectRatio(true);
                int col=ds->addCopiedColumn(roiXY.data(), roiXY.size(), QString("ROI_c%1_b%2_cut_XY").arg(channel).arg(bead));
                JKQTPColumnMathImage* image=new JKQTPColumnMathImage(cutXX.first(), cutYX.first(), cutXX.last()-cutXX.first(), cutYX.last()-cutYX.first(), col, roi.width(), roi.height(), JKQTPMathImageMATLAB, ui->pltXY->get_plotter());
                ui->pltXY->addGraph(image);
                ui->pltXY->zoomToFit();
                ui->pltXY->set_doDrawing(true);
                ui->pltXY->update_plot();
            }
            {
                ui->pltXZ->set_doDrawing(false);
                JKQTPdatastore* ds=ui->pltXZ->getDatastore();
                ui->pltXZ->clearGraphs(true);
                ds->clear();
                ui->pltXZ->setAbsoluteXY(cutZX.first(), cutZX.last(), cutXX.first(), cutXX.last());
                ui->pltXZ->getXAxis()->set_axisLabel(tr("z [nm]"));
                ui->pltXZ->get_plotter()->set_axisAspectRatio(double(roi.depth())/double(roi.width()));
                ui->pltXZ->get_plotter()->set_aspectRatio(deltaXY/deltaZ);
                ui->pltXZ->get_plotter()->set_maintainAspectRatio(true);
                ui->pltXZ->get_plotter()->set_maintainAxisAspectRatio(true);
                ui->pltXZ->getYAxis()->set_axisLabel(tr("x [nm]"));
                int col=ds->addCopiedColumn(roiXZ.data(), roiXZ.size(), QString("ROI_c%1_b%2_cut_XZ").arg(channel).arg(bead));
                JKQTPColumnMathImage* image=new JKQTPColumnMathImage(cutZX.first(), cutXX.first(), cutZX.last()-cutZX.first(), cutXX.last()-cutXX.first(), col, roi.depth(), roi.width(), JKQTPMathImageMATLAB, ui->pltXZ->get_plotter());
                ui->pltXZ->addGraph(image);
                ui->pltXZ->zoomToFit();
                ui->pltXZ->set_doDrawing(true);
                ui->pltXZ->update_plot();
            }
            {
                ui->pltYZ->set_doDrawing(false);
                JKQTPdatastore* ds=ui->pltYZ->getDatastore();
                ui->pltYZ->clearGraphs(true);
                ds->clear();
                ui->pltYZ->setAbsoluteXY(cutZX.first(), cutZX.last(), cutYX.first(), cutYX.last());
                ui->pltYZ->getXAxis()->set_axisLabel(tr("z [nm]"));
                ui->pltYZ->get_plotter()->set_axisAspectRatio(double(roi.depth())/double(roi.height()));
                ui->pltYZ->get_plotter()->set_aspectRatio(deltaXY/deltaZ);
                ui->pltYZ->get_plotter()->set_maintainAspectRatio(true);
                ui->pltYZ->get_plotter()->set_maintainAxisAspectRatio(true);
                ui->pltYZ->getYAxis()->set_axisLabel(tr("y [nm]"));
                int col=ds->addCopiedColumn(roiYZ.data(), roiYZ.size(), QString("ROI_c%1_b%2_cut_YZ").arg(channel).arg(bead));
                JKQTPColumnMathImage* image=new JKQTPColumnMathImage(cutZX.first(), cutYX.first(), cutZX.last()-cutZX.first(), cutYX.last()-cutYX.first(), col, roi.depth(), roi.height(), JKQTPMathImageMATLAB, ui->pltYZ->get_plotter());
                ui->pltYZ->addGraph(image);
                ui->pltYZ->zoomToFit();
                ui->pltYZ->set_doDrawing(true);
                ui->pltYZ->update_plot();
            }

            {
                ui->pltFitX->set_doDrawing(false);
                JKQTPdatastore* ds=ui->pltFitX->getDatastore();
                ui->pltFitX->clearGraphs(true);
                ds->clear();
                ui->pltFitX->getXAxis()->set_axisLabel(tr("x [nm]"));
                ui->pltFitX->getYAxis()->set_axisLabel(tr("intensity [ADU]"));
                int colX=ds->addCopiedColumn(cutXX.data(), cutXX.size(), QString("X_c%1_b%2").arg(channel).arg(bead));
                int colY=ds->addCopiedColumn(cutX.data(), cutX.size(), QString("cutX_c%1_b%2").arg(channel).arg(bead));
                JKQTPxyLineGraph* plt=new JKQTPxyLineGraph(ui->pltFitX->get_plotter());
                plt->set_drawLine(false);
                plt->set_symbol(JKQTPplus);
                plt->set_xColumn(colX);
                plt->set_yColumn(colY);
                ui->pltFitX->addGraph(plt);


                JKQTPxQFFitFunctionLineGraph* fit=new JKQTPxQFFitFunctionLineGraph(ui->pltFitX->get_plotter());
                QFFitFunction* ff=NULL;
                fit->set_fitFunction(ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffIDX));
                double amp=1;
                double width=0;
                for (int i=0; i<fitresCutX.size(); i++) {
                    if (ff && ff->getParameterID(i)=="position") fitresCutX[i]=fitresCutX[i]-double(initPosX)*deltaXY;
                    if (ff && ff->getParameterID(i)=="amplitude") amp=fitresCutX[i];
                    if (ff && ff->getParameterID(i)=="width") width=fitresCutX[i];
                }
                fit->set_params(fitresCutX);
                fit->set_color(plt->get_color().darker());
                fit->set_title(tr("fit: w_0=%1nm").arg(width));
                ui->pltFitX->addGraph(fit);

                ui->pltFitX->zoomToFit();
                double maxy=1.2*amp;
                if (ui->pltFitX->getYMax()<maxy) ui->pltFitX->setY(ui->pltFitX->getYMin(), maxy);

                ui->pltFitX->set_doDrawing(true);
                ui->pltFitX->update_plot();
            }


            {
                ui->pltFitY->set_doDrawing(false);
                JKQTPdatastore* ds=ui->pltFitY->getDatastore();
                ui->pltFitY->clearGraphs(true);
                ds->clear();
                ui->pltFitY->getXAxis()->set_axisLabel(tr("y [nm]"));
                ui->pltFitY->getYAxis()->set_axisLabel(tr("intensity [ADU]"));
                int colX=ds->addCopiedColumn(cutYX.data(), cutYX.size(), QString("Y_c%1_b%2").arg(channel).arg(bead));
                int colY=ds->addCopiedColumn(cutY.data(), cutY.size(), QString("cutY_c%1_b%2").arg(channel).arg(bead));
                JKQTPxyLineGraph* plt=new JKQTPxyLineGraph(ui->pltFitY->get_plotter());
                plt->set_drawLine(false);
                plt->set_symbol(JKQTPplus);
                plt->set_xColumn(colX);
                plt->set_yColumn(colY);
                ui->pltFitY->addGraph(plt);


                JKQTPxQFFitFunctionLineGraph* fit=new JKQTPxQFFitFunctionLineGraph(ui->pltFitY->get_plotter());
                QFFitFunction* ff=NULL;
                fit->set_fitFunction(ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffIDY));
                double amp=1, width=0;
                for (int i=0; i<fitresCutY.size(); i++) {
                    if (ff && ff->getParameterID(i)=="position") fitresCutY[i]=fitresCutY[i]-double(initPosY)*deltaXY;
                    if (ff && ff->getParameterID(i)=="amplitude") amp=fitresCutY[i];
                    if (ff && ff->getParameterID(i)=="width") width=fitresCutY[i];
                }
                fit->set_params(fitresCutY);
                fit->set_color(plt->get_color().darker());
                fit->set_title(tr("fit: w_0=%1nm").arg(width));
                ui->pltFitY->addGraph(fit);

                ui->pltFitY->zoomToFit();
                double maxy=1.2*amp;
                if (ui->pltFitY->getYMax()<maxy) ui->pltFitY->setY(ui->pltFitY->getYMin(), maxy);

                ui->pltFitY->set_doDrawing(true);
                ui->pltFitY->update_plot();
            }


            {
                ui->pltFitZ->set_doDrawing(false);
                JKQTPdatastore* ds=ui->pltFitZ->getDatastore();
                ui->pltFitZ->clearGraphs(true);
                ds->clear();
                ui->pltFitZ->getXAxis()->set_axisLabel(tr("z [nm]"));
                ui->pltFitZ->getYAxis()->set_axisLabel(tr("intensity [ADU]"));
                int colX=ds->addCopiedColumn(cutZX.data(), cutZX.size(), QString("Z_c%1_b%2").arg(channel).arg(bead));
                int colY=ds->addCopiedColumn(cutZ.data(), cutZ.size(), QString("cutZ_c%1_b%2").arg(channel).arg(bead));
                JKQTPxyLineGraph* plt=new JKQTPxyLineGraph(ui->pltFitZ->get_plotter());
                plt->set_drawLine(false);
                plt->set_symbol(JKQTPplus);
                plt->set_xColumn(colX);
                plt->set_yColumn(colY);
                ui->pltFitZ->addGraph(plt);


                JKQTPxQFFitFunctionLineGraph* fit=new JKQTPxQFFitFunctionLineGraph(ui->pltFitZ->get_plotter());
                QFFitFunction* ff=NULL;
                fit->set_fitFunction(ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffIDZ));
                double amp=1, width=0;
                for (int i=0; i<fitresCutZ.size(); i++) {
                    if (ff && ff->getParameterID(i)=="position") fitresCutZ[i]=fitresCutZ[i]-double(initPosZ)*deltaZ;
                    if (ff && ff->getParameterID(i)=="amplitude") amp=fitresCutZ[i];
                    if (ff && ff->getParameterID(i)=="width") width=fitresCutZ[i];
                }
                fit->set_params(fitresCutZ);
                fit->set_color(plt->get_color().darker());
                fit->set_title(tr("fit: w_0=%1nm").arg(width));
                ui->pltFitZ->addGraph(fit);

                ui->pltFitZ->zoomToFit();
                double maxy=1.2*amp;
                if (ui->pltFitZ->getYMax()<maxy) ui->pltFitZ->setY(ui->pltFitZ->getYMin(), maxy);
                ui->pltFitZ->set_doDrawing(true);
                ui->pltFitZ->update_plot();
            }


            {
                ui->pltFitWofZ->set_doDrawing(false);
                JKQTPdatastore* ds=ui->pltFitWofZ->getDatastore();
                ui->pltFitWofZ->clearGraphs(true);
                ds->clear();
                ui->pltFitWofZ->getXAxis()->set_axisLabel(tr("z [{\\mu}m]"));
                ui->pltFitWofZ->getYAxis()->set_axisLabel(tr("PSF-width [nm]"));
                int colX=ds->addCopiedColumn(fitresCutZ_Z.data(), fitresCutZ_Z.size(), QString("Z_c%1_b%2").arg(channel).arg(bead));
                int colYX=ds->addCopiedColumn(fitresCutZX.data(), fitresCutZX.size(), QString("cutZX_c%1_b%2_width").arg(channel).arg(bead));
                int colYY=ds->addCopiedColumn(fitresCutZY.data(), fitresCutZY.size(), QString("cutZY_c%1_b%2_width").arg(channel).arg(bead));
                JKQTPxyLineGraph* plt=new JKQTPxyLineGraph(ui->pltFitWofZ->get_plotter());
                plt->set_drawLine(false);
                plt->set_symbol(JKQTPplus);
                plt->set_xColumn(colX);
                plt->set_yColumn(colYX);
                plt->set_title(tr("w_x(z): w_0=%1nm, z_R=%2nm, z_0=%3{\\mu}m").arg(fitresCutZXGB.value(1)).arg(fitresCutZXGB.value(0)).arg(fitresCutZXGB.value(2)/1000.0));
                ui->pltFitWofZ->addGraph(plt);


                JKQTPxQFFitFunctionLineGraph* fit=new JKQTPxQFFitFunctionLineGraph(ui->pltFitWofZ->get_plotter());
                QFFitFunction* ff=NULL;
                fit->set_fitFunction(ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffIDCutZX));
                //qDebug()<<ffIDCutZX<<ff;
                for (int i=0; i<fitresCutZXGB.size(); i++) {
                    if (ff && ff->getParameterID(i)=="position") fitresCutZXGB[i]=fitresCutZXGB[i]/1000.0;
                }
                //if (ff) delete ff;
                fit->set_params(fitresCutZXGB);
                fit->set_color(plt->get_color().darker());
                ui->pltFitWofZ->addGraph(fit);

                plt=new JKQTPxyLineGraph(ui->pltFitWofZ->get_plotter());
                plt->set_drawLine(false);
                plt->set_symbol(JKQTPplus);
                plt->set_xColumn(colX);
                plt->set_yColumn(colYY);
                plt->set_title(tr("w_y(z): w_0=%1nm, z_R=%2nm, z_0=%3{\\mu}m").arg(fitresCutZYGB.value(1)).arg(fitresCutZYGB.value(0)).arg(fitresCutZYGB.value(2)/1000.0));

                ui->pltFitWofZ->addGraph(plt);


                fit=new JKQTPxQFFitFunctionLineGraph(ui->pltFitWofZ->get_plotter());
                ff=NULL;
                fit->set_fitFunction(ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffIDCutZY));
                //qDebug()<<ffIDCutZY<<ff;
                for (int i=0; i<fitresCutZYGB.size(); i++) {
                    if (ff && ff->getParameterID(i)=="position") fitresCutZYGB[i]=fitresCutZYGB[i]/1000.0;
                }
                //if (ff) delete ff;

                fit->set_params(fitresCutZYGB);
                fit->set_color(plt->get_color().darker());
                ui->pltFitWofZ->addGraph(fit);



                ui->pltFitWofZ->zoomToFit();
                double maxy=3.0*qMax(fitresCutZYGB.value(1),fitresCutZXGB.value(1));
                if (ui->pltFitWofZ->getYMax()>maxy && maxy>=qMax(fitresCutZY.value(fitresCutZY.size()/2, 500), fitresCutZX.value(fitresCutZY.size()/2, 500))) ui->pltFitWofZ->setY(0, maxy);
                else ui->pltFitWofZ->setY(0, ui->pltFitWofZ->getYMax());
                ui->pltFitWofZ->set_doDrawing(true);
                ui->pltFitWofZ->update_plot();
            }
        }
    }
}


QVector<double> QFEvalBeadScanPSFEditor::getBeadsData(const QString& paramName, int paramIdx,  int channel)
{
    QVector<double> dat;

    if (!current) return dat;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return dat;

    QString evalID=eval->getEvaluationResultID();

    QVector<int> beadIDs=getFilteresBeadsID(channel);


    for (int bid=0; bid<beadIDs.size(); bid++) {
        int b=beadIDs[bid];
        QString param=QString("channel%1_bead%2").arg(channel).arg(b);
        QString p1=param+paramName;

        bool ok=true;
        QVector<double> p=record->resultsGetAsDoubleList(evalID, p1, &ok);
        if (ok && paramIdx<p.size()) {
            dat<<p[paramIdx];
        }
    }

    return dat;
}

QVector<int> QFEvalBeadScanPSFEditor::getFilteresBeadsID(int channel)
{
    QVector<int> dat;

    if (!current) return dat;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return dat;

    QString evalID=eval->getEvaluationResultID();
    int beads=record->resultsGetAsInteger(evalID, "channel0_beads");

    QStringList params3D=record->resultsGetAsStringList(evalID, "fit3d_fitfunction_parameternames");
    int wid1ID3D=params3D.indexOf("width1");
    int wid2ID3D=params3D.indexOf("width2");
    int wid3ID3D=params3D.indexOf("width3");

    for (int b=0; b<beads; b++) {
        QString param=QString("channel%1_bead%2").arg(channel).arg(b);

        bool useBead=true;
        if (ui->grpFilterBeads->isChecked()) {
            bool ok=true;
            QVector<double> ar=record->resultsGetAsDoubleList(evalID, param+"_fits_axialratios_3d", &ok);
            if (ok && ar.size()>=2 && ui->chkFilterAxRation->isChecked()) {
                if (ar[0]<ui->spinAxRatioMin->value() || ar[0]> ui->spinAxRatioMax->value() ||
                    ar[1]<ui->spinAxRatioMin->value() || ar[1]> ui->spinAxRatioMax->value()) {
                    useBead=false;
                }
            }
            if (useBead && ui->chkFilterAxRation->isChecked()) {
                ar=record->resultsGetAsDoubleList(evalID, param+"_fits_axialratios", &ok);
                if (ok && ar.size()>=2) {
                    if (ar[0]<ui->spinAxRatioMin->value() || ar[0]> ui->spinAxRatioMax->value() ||
                        ar[1]<ui->spinAxRatioMin->value() || ar[1]> ui->spinAxRatioMax->value()) {
                        useBead=false;
                    }
                }
            }
            if (useBead && (ui->chkFilterXYWidth->isChecked() || ui->chkFilterZYWidth->isChecked())) {
                ar=record->resultsGetAsDoubleList(evalID, param+"_fit3d_results", &ok);
                if (useBead && ok && ui->chkFilterXYWidth->isChecked() && wid1ID3D<ar.size()) {
                    if (ar[wid1ID3D]<ui->spinXYWidthMin->value() || ar[wid1ID3D]> ui->spinXYWidthMax->value()) {
                        useBead=false;
                    }
                }
                if (useBead && ok && ui->chkFilterXYWidth->isChecked() && wid2ID3D<ar.size()) {
                    if (ar[wid2ID3D]<ui->spinXYWidthMin->value() || ar[wid2ID3D]> ui->spinXYWidthMax->value()) {
                        useBead=false;
                    }
                }
                if (useBead && ok && ui->chkFilterZYWidth->isChecked() && wid3ID3D<ar.size()) {
                    if (ar[wid3ID3D]<ui->spinZWidthMin->value() || ar[wid3ID3D]> ui->spinZWidthMax->value()) {
                        useBead=false;
                    }
                }

            }
            // GO ON HERE!!!
        }
        if (useBead) {
            dat<<b;
        }
    }

    return dat;
}


void QFEvalBeadScanPSFEditor::displayEvaluationHistograms() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;

    if (!updatingData) {
        record->setQFProperty(eval->getEvaluationResultID()+"_FilterBeads", ui->grpFilterBeads->isChecked(), false, false);
        record->setQFProperty(eval->getEvaluationResultID()+"_FilterBeads_AxRatMin", ui->spinAxRatioMin->value(), false, false);
        record->setQFProperty(eval->getEvaluationResultID()+"_FilterBeads_AxRatMax", ui->spinAxRatioMax->value(), false, false);
        record->setQFProperty(eval->getEvaluationResultID()+"_FilterBeads_WxyMin", ui->spinXYWidthMin->value(), false, false);
        record->setQFProperty(eval->getEvaluationResultID()+"_FilterBeads_WxyMax", ui->spinXYWidthMax->value(), false, false);
        record->setQFProperty(eval->getEvaluationResultID()+"_FilterBeads_WzMin", ui->spinZWidthMin->value(), false, false);
        record->setQFProperty(eval->getEvaluationResultID()+"_FilterBeads_WzMax", ui->spinZWidthMax->value(), false, false);
        record->setQFProperty(eval->getEvaluationResultID()+"_FilterBeads_Ax", ui->chkFilterAxRation->isChecked(), false, false);
        record->setQFProperty(eval->getEvaluationResultID()+"_FilterBeads_Wxy", ui->chkFilterXYWidth->isChecked(), false, false);
        record->setQFProperty(eval->getEvaluationResultID()+"_FilterBeads_Wz", ui->chkFilterZYWidth->isChecked(), false, false);

        eval->setQFProperty("FilterBeads", ui->grpFilterBeads->isChecked(), false, false);
        eval->setQFProperty("FilterBeads_AxRatMin", ui->spinAxRatioMin->value(), false, false);
        eval->setQFProperty("FilterBeads_AxRatMax", ui->spinAxRatioMax->value(), false, false);
        eval->setQFProperty("FilterBeads_WxyMin", ui->spinXYWidthMin->value(), false, false);
        eval->setQFProperty("FilterBeads_WxyMax", ui->spinXYWidthMax->value(), false, false);
        eval->setQFProperty("FilterBeads_WzMin", ui->spinZWidthMin->value(), false, false);
        eval->setQFProperty("FilterBeads_WzMax", ui->spinZWidthMax->value(), false, false);
        eval->setQFProperty("FilterBeads_Ax", ui->chkFilterAxRation->isChecked(), false, false);
        eval->setQFProperty("FilterBeads_Wxy", ui->chkFilterXYWidth->isChecked(), false, false);
        eval->setQFProperty("FilterBeads_Wz", ui->chkFilterZYWidth->isChecked(), false, false);
    }

    if (eval->hasEvaluation(record)) {
        QString evalID=eval->getEvaluationResultID();
        QStringList sl1=ui->cmbParam1->itemData(ui->cmbParam1->currentIndex()).toStringList();
        QStringList sl2=ui->cmbParam2->itemData(ui->cmbParam2->currentIndex()).toStringList();
        int channels=record->resultsGetAsInteger(evalID, "channels");
        int beads=record->resultsGetAsInteger(evalID, "channel0_beads");
        if (sl1.size()>=2) {
            ui->histogram1->clear();
            ui->histogram1->setDefaultColor(0, QColor("darkgreen"));
            ui->histogram1->setDefaultColor(1, QColor("red"));
            ui->histogram1->setDefaultColor(2, QColor("blue"));
            for (int c=0; c<channels; c++) {
                QVector<double> dat=getBeadsData(sl1.value(0), sl1.value(1).toInt(), c);
                /*for (int b=0; b<beads; b++) {
                    QString param=QString("channel%1_bead%2").arg(c).arg(b);
                    QString p1=param+sl1.value(0);
                    int p1i=sl1.value(1).toInt();
                    //qDebug()<<p1<<p1i;
                    bool ok=true;
                    QVector<double> p=record->resultsGetAsDoubleList(evalID, p1, &ok);
                    if (ok && p1i<p.size()) {
                        dat<<p[p1i];
                    }
                }*/
                if (dat.size()>0) ui->histogram1->addCopiedHistogram(tr("ch %1: %2").arg(c).arg(ui->cmbParam1->currentText()), dat.data(), dat.size());
            }
            ui->histogram1->updateHistogram(true);
        }
        if (sl2.size()>=2) {
            ui->histogram2->clear();
            ui->histogram2->setDefaultColor(0, QColor("darkgreen"));
            ui->histogram2->setDefaultColor(1, QColor("red"));
            ui->histogram2->setDefaultColor(2, QColor("blue"));
            for (int c=0; c<channels; c++) {
                QVector<double> dat=getBeadsData(sl2.value(0), sl2.value(1).toInt(), c);
                /*for (int b=0; b<beads; b++) {
                    QString param=QString("channel%1_bead%2").arg(c).arg(b);
                    QString p1=param+sl2.value(0);
                    int p1i=sl2.value(1).toInt();
                    bool ok=true;
                    QVector<double> p=record->resultsGetAsDoubleList(evalID, p1, &ok);
                    if (ok && p1i<p.size()) {
                        dat<<p[p1i];
                    }
                }*/
                if (dat.size()>0) ui->histogram2->addCopiedHistogram(tr("ch %1: %2").arg(c).arg(ui->cmbParam2->currentText()), dat.data(), dat.size());
            }
            ui->histogram2->updateHistogram(true);
        }
        ui->histogramD1->clear();
        ui->histogramD2->clear();
        if (channels>0) {
            QString evalID=eval->getEvaluationResultID();
            QVector<double> datDX=getBeadsData("_fit3d_distc0", 0, 1);
            QVector<double> datDY=getBeadsData("_fit3d_distc0", 1, 1);
            QVector<double> datDZ=getBeadsData("_fit3d_distc0", 2, 1);
            QVector<double> datD=getBeadsData("_fit3d_distc0", 3, 1);

            /*QVector<double> datDX, datDY, datDZ, datD;
            for (int b=0; b<beads; b++) {
                QString param=QString("channel%1_bead%2").arg(1).arg(b);
                bool ok=true;
                QVector<double> d=record->resultsGetAsDoubleList(evalID, param+"_fit3d_distc0", &ok);
                if (ok) {
                    datDX<<d[0];
                    datDY<<d[1];
                    datDZ<<d[2];
                    datD<<d[3];
                }
            }*/
            if (datDX.size()>0) {
                ui->histogramD1->addCopiedHistogram(tr("distance: x"), datDX.data(), datDX.size());
                if (ui->cmbParamD2->currentIndex()==0) ui->histogramD2->addCopiedHistogram(tr("distance: x"), datDX.data(), datDX.size());
            }
            if (datDY.size()>0) {
                ui->histogramD1->addCopiedHistogram(tr("distance: y"), datDY.data(), datDY.size());
                if (ui->cmbParamD2->currentIndex()==1) ui->histogramD2->addCopiedHistogram(tr("distance: y"), datDY.data(), datDY.size());
            }
            if (datDZ.size()>0) {
                ui->histogramD1->addCopiedHistogram(tr("distance: z"), datDZ.data(), datDZ.size());
                if (ui->cmbParamD2->currentIndex()==2) ui->histogramD2->addCopiedHistogram(tr("distance: z"), datDZ.data(), datDZ.size());
            }
            if (datD.size()>0) {
                if (ui->cmbParamD2->currentIndex()==2) ui->histogramD2->addCopiedHistogram(tr("abs. distance"), datD.data(), datD.size());
            }

        }
        ui->histogramD1->updateHistogram(true);
        ui->histogramD2->updateHistogram(true);
    }
}

void QFEvalBeadScanPSFEditor::displayEvaluationCorrPlot()
{
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;

    if (eval->hasEvaluation(record)) {
        QString evalID=eval->getEvaluationResultID();
        QStringList sl1=ui->cmbParamC1->itemData(ui->cmbParamC1->currentIndex()).toStringList();
        QStringList sl2=ui->cmbParamC2->itemData(ui->cmbParamC2->currentIndex()).toStringList();
        //int channels=record->resultsGetAsInteger(evalID, "channels");
        int beads=record->resultsGetAsInteger(evalID, "channel0_beads");
        if (sl1.size()>=3 && sl2.size()>=3) {
            ui->corrPlot->clear();

            int c=sl1.value(2).toInt();

            QVector<double> dat=getBeadsData(sl1.value(0), sl1.value(1).toInt(), c);
            QVector<double> dat2=getBeadsData(sl2.value(0), sl2.value(1).toInt(), c);
            /*QVector<double> dat;
            for (int b=0; b<beads; b++) {
                QString param=QString("channel%1_bead%2").arg(c).arg(b);
                QString p1=param+sl1.value(0);
                int p1i=sl1.value(1).toInt();
                //qDebug()<<p1<<p1i;
                bool ok=true;
                QVector<double> p=record->resultsGetAsDoubleList(evalID, p1, &ok);
                if (ok && p1i<p.size()) {
                    dat<<p[p1i];
                }
            }

            c=sl2.value(2).toInt();
            QVector<double> dat2;
            for (int b=0; b<beads; b++) {
                QString param=QString("channel%1_bead%2").arg(c).arg(b);
                QString p1=param+sl2.value(0);
                int p1i=sl2.value(1).toInt();
                bool ok=true;
                QVector<double> p=record->resultsGetAsDoubleList(evalID, p1, &ok);
                if (ok && p1i<p.size()) {
                    dat2<<p[p1i];
                }
            }*/


            if (dat.size()>0 && dat2.size()>0) ui->corrPlot->addCopiedCorrelation(tr("%1 -- %2").arg(ui->cmbParamC1->currentText()).arg(ui->cmbParamC2->currentText()), dat.data(), dat2.data(), dat.size());

            ui->corrPlot->updateCorrelation(true);
        }

    }
}

void QFEvalBeadScanPSFEditor::displayResults()
{
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    //disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData()));
    bool hasRes=false;

    disconnect(ui->cmbChannel, SIGNAL(currentIndexChanged(int)), this, SLOT(displayEvaluationBead()));
    disconnect(ui->cmbBead, SIGNAL(currentIndexChanged(int)), this, SLOT(displayEvaluationBead()));
    disconnect(ui->cmbParam1, SIGNAL(currentIndexChanged(int)), this, SLOT(displayEvaluationHistograms()));
    disconnect(ui->cmbParam2, SIGNAL(currentIndexChanged(int)), this, SLOT(displayEvaluationHistograms()));
    disconnect(ui->cmbParamC1, SIGNAL(currentIndexChanged(int)), this, SLOT(displayEvaluationCorrPlot()));
    disconnect(ui->cmbParamC2, SIGNAL(currentIndexChanged(int)), this, SLOT(displayEvaluationCorrPlot()));
    ui->cmbChannel->clear();
    if (data && record && eval) {
        hasRes=eval->hasEvaluation(record);
        QString evalID=eval->getEvaluationResultID();
        bool ok=true;
        int ch=record->resultsGetAsInteger(evalID, "channels", &ok);
        if (!ok) ch=0;
        for (int i=0; i<ch; i++) {
            ui->cmbChannel->addItem(tr("channel %1").arg(i+1));
        }

        int beads=record->resultsGetAsInteger(evalID, "channel0_beads", &ok);
        if (!ok) beads=0;
        for (int i=0; i<beads; i++) {
            ui->cmbBead->addItem(tr("bead %1").arg(i+1));
        }

        QString s1;
        QStringList sl1;
        ui->cmbParam1->clear();
        ui->cmbParam2->clear();
        ui->cmbParamC2->clear();
        ui->cmbParamC1->clear();
        ui->cmbParam1->addItem(s1=tr("cut X: offset"), sl1=constructQStringListFromItems("_cutx_fitresult", "0")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut X: amplitude"), sl1=constructQStringListFromItems("_cutx_fitresult", "1")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut X: position"), sl1=constructQStringListFromItems("_cutx_fitresult", "2")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut X: width"), sl1=constructQStringListFromItems("_cutx_fitresult", "3")); ui->cmbParam2->addItem(s1, sl1);

        ui->cmbParam1->addItem(s1=tr("cut Y: offset"), sl1=constructQStringListFromItems("_cuty_fitresult", "0")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut Y: amplitude"), sl1=constructQStringListFromItems("_cuty_fitresult", "1")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut Y: position"), sl1=constructQStringListFromItems("_cuty_fitresult", "2")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut Y: width"), sl1=constructQStringListFromItems("_cuty_fitresult", "3")); ui->cmbParam2->addItem(s1, sl1);

        ui->cmbParam1->addItem(s1=tr("cut Z: offset"), sl1=constructQStringListFromItems("_cutz_fitresult", "0")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut Z: amplitude"), sl1=constructQStringListFromItems("_cutz_fitresult", "1")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut Z: position"), sl1=constructQStringListFromItems("_cutz_fitresult", "2")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut Z: width"), sl1=constructQStringListFromItems("_cutz_fitresult", "3")); ui->cmbParam2->addItem(s1, sl1);

        ui->cmbParam1->addItem(s1=tr("cut X+Z: axial ratio wz/wx"), sl1=constructQStringListFromItems("_fits_axialratios", "0")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut Y+Z: axial ratio wz/wy"), sl1=constructQStringListFromItems("_fits_axialratios", "1")); ui->cmbParam2->addItem(s1, sl1);

        ui->cmbParam1->addItem(s1=tr("cut XZ: rayleigh length"), sl1=constructQStringListFromItems("_cutxz_gaussianbeam_results", "0")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut XZ: beam width"), sl1=constructQStringListFromItems("_cutxz_gaussianbeam_results", "1")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut XZ: position"), sl1=constructQStringListFromItems("_cutxz_gaussianbeam_results", "2")); ui->cmbParam2->addItem(s1, sl1);

        ui->cmbParam1->addItem(s1=tr("cut YZ: rayleigh length"), sl1=constructQStringListFromItems("_cutyz_gaussianbeam_results", "0")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut YZ: beam width"), sl1=constructQStringListFromItems("_cutyz_gaussianbeam_results", "1")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("cut YZ: position"), sl1=constructQStringListFromItems("_cutyz_gaussianbeam_results", "2")); ui->cmbParam2->addItem(s1, sl1);

        ui->cmbParam1->addItem(s1=tr("3D fit: offset"), sl1=constructQStringListFromItems("_fit3d_results", "0")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("3D fit: amplitude"), sl1=constructQStringListFromItems("_fit3d_results", "1")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("3D fit: position x"), sl1=constructQStringListFromItems("_fit3d_results", "2")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("3D fit: position y"), sl1=constructQStringListFromItems("_fit3d_results", "3")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("3D fit: position z"), sl1=constructQStringListFromItems("_fit3d_results", "4")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("3D fit: width 1"), sl1=constructQStringListFromItems("_fit3d_results", "5")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("3D fit: width 2"), sl1=constructQStringListFromItems("_fit3d_results", "6")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("3D fit: width 3"), sl1=constructQStringListFromItems("_fit3d_results", "7")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("3D fit: theta"), sl1=constructQStringListFromItems("_fit3d_results", "8")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("3D fit: phi"), sl1=constructQStringListFromItems("_fit3d_results", "9")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("3D fit: alpha"), sl1=constructQStringListFromItems("_fit3d_results", "10")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("3D fit: axial ratio w3/w1"), sl1=constructQStringListFromItems("_fits_axialratios_3d", "0")); ui->cmbParam2->addItem(s1, sl1);
        ui->cmbParam1->addItem(s1=tr("3D fit: axial ratio w3/w2"), sl1=constructQStringListFromItems("_fits_axialratios_3d", "1")); ui->cmbParam2->addItem(s1, sl1);

        if (ch>1) {
            ui->cmbParam1->addItem(s1=tr("3D fit: distance x"), sl1=constructQStringListFromItems("_fit3d_distc0", "0")); ui->cmbParam2->addItem(s1, sl1);
            ui->cmbParam1->addItem(s1=tr("3D fit: distance y"), sl1=constructQStringListFromItems("_fit3d_distc0", "1")); ui->cmbParam2->addItem(s1, sl1);
            ui->cmbParam1->addItem(s1=tr("3D fit: distance z"), sl1=constructQStringListFromItems("_fit3d_distc0", "2")); ui->cmbParam2->addItem(s1, sl1);
            ui->cmbParam1->addItem(s1=tr("3D fit: abs. distance"), sl1=constructQStringListFromItems("_fit3d_distc0", "3")); ui->cmbParam2->addItem(s1, sl1);
        }

        ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->tabHistD), ch>1);

        for (int c=0; c<ch; c++) {
            for (int i=0; i<ui->cmbParam1->count(); i++) {
                QString n=ui->cmbParam1->itemText(i);
                QStringList sl=ui->cmbParam1->itemData(i).toStringList();
                sl<<QLocale::c().toString(c);
                ui->cmbParamC1->addItem(tr("ch %1: %2").arg(c).arg(n), sl);
                ui->cmbParamC2->addItem(tr("ch %1: %2").arg(c).arg(n), sl);
            }
        }
    }
    ui->cmbParam1->setCurrentIndex(ui->cmbParam1->findText(tr("3D fit: width 1")));
    ui->cmbParam2->setCurrentIndex(ui->cmbParam2->findText(tr("3D fit: width 3")));
    ui->cmbParamC1->setCurrentIndex(ui->cmbParamC1->findText(tr("ch 0: 3D fit: position z")));
    ui->cmbParamC2->setCurrentIndex(ui->cmbParamC2->findText(tr("ch 0: 3D fit: width 3")));

    ui->spinA->setEnabled(!hasRes);
    ui->spinZ->setEnabled(!hasRes);
    ui->spinPixPerFrame->setEnabled(!hasRes);
    ui->spinROIXY->setEnabled(!hasRes);
    ui->spinROIZ->setEnabled(!hasRes);
    ui->spinPSFWidth->setEnabled(!hasRes);
    ui->spinPSFHeight->setEnabled(!hasRes);
    ui->spinWZFraction->setEnabled(!hasRes);

    connect(ui->cmbChannel, SIGNAL(currentIndexChanged(int)), this, SLOT(displayEvaluationBead()));
    connect(ui->cmbBead, SIGNAL(currentIndexChanged(int)), this, SLOT(displayEvaluationBead()));
    connect(ui->cmbParam1, SIGNAL(currentIndexChanged(int)), this, SLOT(displayEvaluationHistograms()));
    connect(ui->cmbParam2, SIGNAL(currentIndexChanged(int)), this, SLOT(displayEvaluationHistograms()));
    connect(ui->cmbParamC1, SIGNAL(currentIndexChanged(int)), this, SLOT(displayEvaluationCorrPlot()));
    connect(ui->cmbParamC2, SIGNAL(currentIndexChanged(int)), this, SLOT(displayEvaluationCorrPlot()));

    displayEvaluationHistograms();
    displayEvaluationCorrPlot();
    displayEvaluationBead();
}

void QFEvalBeadScanPSFEditor::on_chkMedianFIlter_toggled(bool value)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);


    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID()+"_MEDIAN", value, false, false);
    }
}



void QFEvalBeadScanPSFEditor::on_spinA_valueChanged(double value) {
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);


    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID()+"_DELTAX", value, false, false);
    }
}

void QFEvalBeadScanPSFEditor::on_spinPSFHeight_valueChanged(double value)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);


    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID()+"_EST_PSF_WIDTH", value, false, false);
    }
}

void QFEvalBeadScanPSFEditor::on_spinPSFWidth_valueChanged(double value)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);


    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID()+"_EST_PSF_HEIGHT", value, false, false);
    }
}

void QFEvalBeadScanPSFEditor::on_spinROIZ_valueChanged(int value)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);


    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID()+"_ROI_Z", value, false, false);
    }
}

void QFEvalBeadScanPSFEditor::on_spinPixPerFrame_valueChanged(int value)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);


    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID()+"_PIX_PER_FRAME", value, false, false);
    }
}

void QFEvalBeadScanPSFEditor::on_spinWZFraction_valueChanged(double value)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);


    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID()+"_WZ_FRACTION", value, false, false);
    }
}



void QFEvalBeadScanPSFEditor::saveROI(const QString &filename, const QString& format, QFRawDataRecord *rdr, int bead, int channel)
{
    if (!current) return;
    QFRawDataRecord* record=rdr;
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!record)||(!eval)||(!data)) return;

    if (eval->hasEvaluation(record)) {
        QString evalID=eval->getEvaluationResultID();

        int ROIxy=ui->spinROIXY->value();
        int ROIz=ui->spinROIZ->value();
        int stack=0;
        int width=data->getImageStackWidth(stack);
        int height=data->getImageStackHeight(stack);
        int size_z=data->getImageStackFrames(stack);

        int initPosX=record->resultsGetInNumberList(evalID, "beadsearch_initial_positions_x", bead, -1);
        int initPosY=record->resultsGetInNumberList(evalID, "beadsearch_initial_positions_y", bead, -1);
        int initPosZ=record->resultsGetInNumberList(evalID, "beadsearch_initial_positions_z", bead, -1);

        //qDebug()<<channel<<bead<<initPosX<<initPosY<<initPosZ;

        if (initPosX>=0 && initPosY>=0 && initPosZ>=0) {
            cimg_library::CImg<double> image(data->getImageStack(stack, 0, channel), width, height, size_z, true);
            cimg_library::CImg<double> roi=image.get_crop(initPosX-ROIxy/2, initPosY-ROIxy/2, initPosZ-ROIz/2, initPosX+ROIxy/2, initPosY+ROIxy/2, initPosZ+ROIz/2);
            if (getImageWriterIDList(QFPluginServices::getInstance()).contains(format) && !filename.isEmpty()) {
                QFExporterImageSeries* exp=dynamic_cast<QFExporterImageSeries*>(QFPluginServices::getInstance()->getExporterManager()->createExporter(format));
                if (exp) {
                    exp->setFileComment(record->getName());
                    uint32_t w=roi.width();
                    uint32_t h=roi.height();
                    uint32_t f=roi.depth();
                    uint32_t ch=roi.spectrum();
                    exp->setFrameSize(w,h,ch);
                    const double* ro=roi.data();
                    if (exp->open(filename)){
                        for (uint32_t i=0; i<f; i++) {
                            exp->writeFrameDouble(&(ro[i*w*h]));
                        }
                    }
                    exp->close();
                    delete exp;
                    exp=NULL;
                }
            }
        }
    }
}
void QFEvalBeadScanPSFEditor::on_btnSaveAllROI_clicked()
{
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;

    if (eval->hasEvaluation(record)) {
        int channel=ui->cmbChannel->currentIndex();
        int bead=ui->cmbBead->currentIndex();

        QStringList ids=getImageWriterIDList(QFPluginServices::getInstance());
        QStringList filters=getImageWriterFilterList(QFPluginServices::getInstance());
        if (ids.size()>0 && filters.size()>0) {
            QString filter=filters[0];
            QString filename=qfGetSaveFileNameSet("QFEvalBeadScanPSFEditor/saveroi/", this, tr("Save all ROIs"), QString(), filters.join(";;"), &filter);
            QString format=ids.value(filters.indexOf(filter), "");
            if (filename.size()>0) {
                QProgressDialog dialog;
                dialog.setLabelText("exporting ROIs ...");
                dialog.setRange(0,ui->cmbChannel->count()*ui->cmbBead->count());
                dialog.show();
                int i=0;
                for (channel=0; channel<ui->cmbChannel->count(); channel++) {
                    for (bead=0; bead<ui->cmbBead->count(); bead++) {
                        dialog.setLabelText(tr("exporting ROI %1, channel %2 ...").arg(bead+1).arg(channel+1));
                        dialog.setValue(i);
                        i++;
                        if (dialog.wasCanceled()) break;
                        QApplication::processEvents();

                        saveROI(QString("%1_ch%2_ch%3.%4").arg(QFileInfo(filename).dir().absolutePath()+"/"+QFileInfo(filename).baseName()).arg(channel,2,10,QLatin1Char('0')).arg(bead,5,10,QLatin1Char('0')).arg(QFileInfo(filename).completeSuffix()), format, record, bead, channel);

                    }
                    if (dialog.wasCanceled()) break;
                }

            }

        }
    }
}

void QFEvalBeadScanPSFEditor::on_btnSaveROI_clicked()
{

    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;

    if (eval->hasEvaluation(record)) {
        int channel=ui->cmbChannel->currentIndex();
        int bead=ui->cmbBead->currentIndex();

        QStringList ids=getImageWriterIDList(QFPluginServices::getInstance());
        QStringList filters=getImageWriterFilterList(QFPluginServices::getInstance());
        if (ids.size()>0 && filters.size()>0) {
            QString filter=filters[0];
            QString filename=qfGetSaveFileNameSet("QFEvalBeadScanPSFEditor/saveroi/", this, tr("Save current ROI"), QString(), filters.join(";;"), &filter);
            QString format=ids.value(filters.indexOf(filter), "");
            if (filename.size()>0) {
                saveROI(filename, format, record, bead, channel);
            }

        }
    }
}

void QFEvalBeadScanPSFEditor::on_btnSaveAvgROI_clicked()
{
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!record)||(!eval)||(!data)) return;


    if (eval->hasEvaluation(record)) {

        QStringList ids=getImageWriterIDList(QFPluginServices::getInstance());
        QStringList filters=getImageWriterFilterList(QFPluginServices::getInstance());
        QStringList files;
        if (ids.size()>0 && filters.size()>0) {
            QString filter=filters[0];
            QString filename=qfGetSaveFileNameSet("QFEvalBeadScanPSFEditor/saveroi/", this, tr("Save averaged ROI"), QFileInfo(record->getFileName(0)).absolutePath(), filters.join(";;"), &filter);
            QString format=ids.value(filters.indexOf(filter), "");
            if (filename.size()>0) {
                bool ok=true;
                int supersampling=QInputDialog::getInt(this, tr("Save averaged ROI"), tr("supersamlping factor:"), 4, 2, 20,1,&ok);
                int channels=ui->cmbChannel->count();
                QString evalID=eval->getEvaluationResultID();
                int ROIxy=ui->spinROIXY->value();
                int ROIz=ui->spinROIZ->value();
                double deltaXY=ui->spinA->value();
                double deltaZ=ui->spinZ->value();
                int stack=0;
                int width=data->getImageStackWidth(stack);
                int height=data->getImageStackHeight(stack);
                int size_z=data->getImageStackFrames(stack);

                if (ok) {
                    QVector<int> beads=getFilteresBeadsID(0);
                    QProgressDialog dialog;
                    dialog.setLabelText("exporting ROIs ...");
                    dialog.setRange(0,channels*beads.size());
                    dialog.show();
                    int pcnt=0;
                    for (int c=0; c<channels; c++) {
                        cimg_library::CImg<double> image(data->getImageStack(stack, 0, c), width, height, size_z, true);
                        cimg_library::CImg<float> roi_out((ROIxy/2+ROIxy/2+1)*supersampling, (ROIxy/2+ROIxy/2+1)*supersampling, (ROIz/2+ROIz/2+1)*supersampling);

                        QStringList params3D=record->resultsGetAsStringList(evalID, "fit3d_fitfunction_parameternames");
                        int posxID=params3D.indexOf("position_x");
                        int posyID=params3D.indexOf("position_y");
                        int poszID=params3D.indexOf("position_z");

                        float normSum=0;

                        for (int bi=0; bi<beads.size(); bi++) {
                            int bead=beads[bi];
                            dialog.setValue(pcnt);
                            pcnt++;
                            QApplication::processEvents();
                            if (dialog.wasCanceled()) break;

                            int initPosX=record->resultsGetInNumberList(evalID, "beadsearch_initial_positions_x", bead, -1);
                            int initPosY=record->resultsGetInNumberList(evalID, "beadsearch_initial_positions_y", bead, -1);
                            int initPosZ=record->resultsGetInNumberList(evalID, "beadsearch_initial_positions_z", bead, -1);
                            float posX=record->resultsGetInNumberList(evalID, QString("channel%1_bead%2_fit3d_results").arg(c).arg(bead), posxID, -1)/deltaXY-float(initPosX)+float(ROIxy/2);
                            float posY=record->resultsGetInNumberList(evalID, QString("channel%1_bead%2_fit3d_results").arg(c).arg(bead), posyID, -1)/deltaXY-float(initPosY)+float(ROIxy/2);
                            float posZ=record->resultsGetInNumberList(evalID, QString("channel%1_bead%2_fit3d_results").arg(c).arg(bead), poszID, -1)/deltaZ-float(initPosZ)+float(ROIz/2);


                            if (initPosX>=0 && initPosY>=0 && initPosZ>=0 && posxID>=0 && posyID>=0 && poszID>=0) {
                                cimg_library::CImg<double> roi=image.get_crop(initPosX-ROIxy/2, initPosY-ROIxy/2, initPosZ-ROIz/2, initPosX+ROIxy/2, initPosY+ROIxy/2, initPosZ+ROIz/2);
                                qDebug()<<initPosX<<initPosY<<initPosZ<<": "<<posX<<"/"<<roi.width()<<posY<<"/"<<roi.height()<<posZ<<"/"<<roi.depth();

                                for (int z=0; z<roi_out.depth(); z++) {
                                    for (int y=0; y<roi_out.height(); y++) {
                                        for (int x=0; x<roi_out.width(); x++) {
                                            float xx=posX+(float(x)-float(roi_out.width()/2))/float(supersampling);
                                            float yy=posY+(float(y)-float(roi_out.height()/2))/float(supersampling);
                                            float zz=posZ+(float(z)-float(roi_out.depth()/2))/float(supersampling);
                                            roi_out(x,y,z)=roi_out(x,y,z)+roi.linear_atXYZ(xx,yy,zz);
                                        }
                                    }
                                }
                                normSum++;

                            }
                        }
                        roi_out/=normSum;
                        if (getImageWriterIDList(QFPluginServices::getInstance()).contains(format) && !filename.isEmpty()) {
                            QFExporterImageSeries* exp=dynamic_cast<QFExporterImageSeries*>(QFPluginServices::getInstance()->getExporterManager()->createExporter(format));
                            if (exp) {
                                exp->setFileComment(tr("averaged PSF: %1, channel=%2").arg(record->getName()).arg(c));
                                uint32_t w=roi_out.width();
                                uint32_t h=roi_out.height();
                                uint32_t f=roi_out.depth();
                                uint32_t ch=roi_out.spectrum();
                                exp->setFrameSize(w,h,ch);
                                exp->setResolution(deltaXY/float(supersampling), deltaXY/float(supersampling), deltaZ/float(supersampling), tr("nm"));
                                files<<(QFileInfo(filename).absoluteDir().absoluteFilePath(QFileInfo(filename).baseName()+QString("_ch%1").arg(c)+"."+QFileInfo(filename).completeSuffix()));
                                qDebug()<<files.last();
                                if (exp->open(files.last())) {
                                    exp->writeFramesFloat(roi_out.data(), f);
                                }
                                exp->close();
                                delete exp;
                                exp=NULL;
                            }
                        }

                        QApplication::processEvents();
                        if (dialog.wasCanceled()) break;
                    }
                    qDebug()<<files<<format<<QFPluginServices::getInstance()->getRawDataRecordFactory()->contains("image_stack");
                    if (!dialog.wasCanceled() && files.size()>0 && format.toLower().contains("tiff") && QFPluginServices::getInstance()->getRawDataRecordFactory()->contains("image_stack")) {
                        if (QMessageBox::question(this, tr("Save averaged ROI"), tr("Do you want to load the result as an image stack into the current project?"), QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes)==QMessageBox::Yes) {
                            QFProject* project=eval->getProject();
                            if (project) {
                                // here we store some initial parameters
                                QMap<QString, QVariant> initParams;
                                // set whatever you want (FILETYPE is just an example)!
                                initParams["FILETYPE"]="imageimporter_libtiff";
                                initParams["STACKTYPE"]="ONEFILEPERCHANNEL";

                                // add all properties in initParams that will be readonly
                                QStringList paramsReadonly;
                                paramsReadonly<<"FILETYPE"<<"STACKTYPE";
                                QStringList types;
                                for (int i=0; i<files.size(); i++) {
                                    types.append("image");
                                }

                                QStringList filenames;
                                for (int i=0; i<files.size(); i++) {
                                    filenames<<QFileInfo(files[i]).fileName();
                                }
                                QString name=filenames.value(0, "");
                                name=tr("averaged PSF stack (%1 ...)").arg(qfGetLargestCommonStart(filenames, name));
                                QFRawDataRecord* e=project->addRawData("image_stack", name, files, initParams, paramsReadonly, types);
                                if (e&&e->error()) { // when an error occured: remove record and output an error message
                                    project->deleteRawData(e->getID());
                                    QMessageBox::critical(this, tr("PSF Analysis"), tr("Error while importing image stack:\n%1").arg(e->errorDescription()));
                                    services->log_error(tr("Error while importing image stack:\n    %1\n").arg(e->errorDescription()));
                                }
                            }
                        }
                    }
                }                
            }
        }
    }
}

void QFEvalBeadScanPSFEditor::on_spinROIXY_valueChanged(int value)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);


    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID()+"_ROI_XY", value, false, false);
    }
}

void QFEvalBeadScanPSFEditor::on_spinZ_valueChanged(double value) {
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID()+"_DELTAZ", value, false, false);
    }
}

void QFEvalBeadScanPSFEditor::evaluateAll() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->reset();
    dlgEvaluationProgress->setMinorProgressRange(0,100);
    dlgEvaluationProgress->setMinorProgress(50);
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->setMajorProgressEnabled(true);
    dlgEvaluationProgress->setMinorProgressEnabled(true);
    dlgEvaluationProgress->setUserClose(false);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
        if ((record)/*&&(data)*/) {
            dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));
            // here we call doEvaluation to execute our evaluation for the current record only
            eval->doEvaluation(record, ui->spinA->value(), ui->spinZ->value(), ui->spinROIXY->value(), ui->spinROIZ->value(), ui->spinPixPerFrame->value(), ui->spinPSFWidth->value(), ui->spinPSFHeight->value(), ui->spinWZFraction->value()/100.0, ui->chkMedianFIlter->isChecked(), dlgEvaluationProgress);
        }
        dlgEvaluationProgress->setValue(i);
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dlgEvaluationProgress->setValue(recs.size());
    dlgEvaluationProgress->close();
    displayResults();
    dlgEvaluationProgress->close();

    QApplication::restoreOverrideCursor();
    resultsChanged();
}


void QFEvalBeadScanPSFEditor::evaluateCurrent() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    
    
    dlgEvaluationProgress->reset();
    dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));
    dlgEvaluationProgress->setMinorProgressRange(0,100);
    dlgEvaluationProgress->setMinorProgress(50);
    dlgEvaluationProgress->setMajorProgressEnabled(false);
    dlgEvaluationProgress->setMinorProgressEnabled(true);
    dlgEvaluationProgress->setUserClose(false);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // here we call doEvaluation to execute our evaluation for the current record only
    eval->doEvaluation(record, ui->spinA->value(), ui->spinZ->value(), ui->spinROIXY->value(), ui->spinROIZ->value(), ui->spinPixPerFrame->value(), ui->spinPSFWidth->value(), ui->spinPSFHeight->value(), ui->spinWZFraction->value()/100.0, ui->chkMedianFIlter->isChecked(), dlgEvaluationProgress);

    displayResults();
    dlgEvaluationProgress->close();

    QApplication::restoreOverrideCursor();
    resultsChanged();
}

void QFEvalBeadScanPSFEditor::resetCurrent()
{
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    record->resultsClear(eval->getEvaluationResultID());

    displayResults();
}





void QFEvalBeadScanPSFEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;    
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEvalBeadScanPSFItem* eval=qobject_cast<QFEvalBeadScanPSFItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    int lastIds=ui->tabWidget->currentIndex();
    for (int i=0; i<ui->tabWidget->count(); i++ ){
        ui->tabWidget->setCurrentIndex(i);
        QApplication::processEvents();
    }
    ui->tabWidget->setCurrentIndex(lastIds);

    QString evalID=eval->getEvaluationResultID();
    int channels=record->resultsGetAsInteger(evalID, "channels");

    
    // we use this QTextCursor to write the document
    QTextCursor cursor(document);
    
    // here we define some generic formats
    QTextCharFormat fText=cursor.charFormat();
    fText.setFontPointSize(7);
    QTextCharFormat fTextSmall=fText;
    fTextSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fTextBold=fText;
    fTextBold.setFontWeight(QFont::Bold);
    QTextCharFormat fTextBoldSmall=fTextBold;
    fTextBoldSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fHeading1=fText;
    QTextCharFormat fHeading2=fText;
    QTextBlockFormat bfLeft;
    bfLeft.setAlignment(Qt::AlignLeft);
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);

    fHeading1.setFontPointSize(2*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);
    fHeading2.setFontPointSize(1.5*fText.fontPointSize());
    fHeading2.setFontWeight(QFont::Bold);

    
    // insert heading
    cursor.insertText(tr("Evaluation Report: PSF\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);

    // insert table with some data
    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(7, 4, tableFormat);
    table->mergeCells(0,1,1,3);
    int row=0;
    int col=0;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("raw data:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(record->getName(), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("pixel size:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(ui->spinA->value())+tr(" nm"), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("step size:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(ui->spinZ->value())+tr(" nm"), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("ROI lateral size (xy):"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(ui->spinROIXY->value())+tr(" pixel"), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("ROI longitudinal size (z):"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(ui->spinROIZ->value())+tr(" pixel"), fText);
    row++;
    if (ui->grpFilterBeads->isChecked()) {
        table->cellAt(row, col+0).firstCursorPosition().insertText(tr("axial ratio filter:"), fTextBold);
        table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(ui->spinAxRatioMin->value())+QString(" ... ")+QString::number(ui->spinAxRatioMax->value()), fTextBold);
        row++;
    }

    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("found beads (per channel):"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(record->resultsGetAsInteger(evalID, "channel0_beads")), fTextBold);
    row++;
    row=1;
    col=2;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("estimated beads per frame:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(ui->spinPixPerFrame->value())+tr(" pixel"), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertHtml(tr("<b>w<sub>x</sub>(z)/w<sub>x</sub>(z)-fit z-range fraction:</b>"));
    table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(ui->spinWZFraction->value())+tr(" %"), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("estimated PSF width:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(ui->spinPSFWidth->value())+tr(" nm"), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("estimated PSF height:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(ui->spinPSFHeight->value())+tr(" nm"), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("median-filtering:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(boolToQString(ui->chkMedianFIlter->isChecked()), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("channels:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(channels), fText);
    row++;
    cursor.movePosition(QTextCursor::End);
	
	
	
	int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);

	

    cursor.movePosition(QTextCursor::End);
    ui->histogram1->writeReport(cursor, document);
    ui->histogram2->writeReport(cursor, document);
    if (channels>1) {
        ui->histogramD1->writeReport(cursor, document);
        ui->histogramD2->writeReport(cursor, document);
    }
    ui->corrPlot->writeReport(cursor, document);

    QList<int> beadsl;
    beadsl<<ui->cmbBead->currentIndex();
    if (!beadsl.contains(0)) beadsl<<0;
    if (!beadsl.contains(1)) beadsl<<1;
    if (!beadsl.contains(2)) beadsl<<2;
    for (int b=0; b<beadsl.size(); b++){
        ui->cmbBead->setCurrentIndex(beadsl[b]);
        for (int c=0; c<channels; c++) {
            ui->cmbChannel->setCurrentIndex(c);
            QApplication::processEvents();
            displayEvaluationBead();
            cursor.insertText(tr("Example PSF, bead %1, channel %2\n\n").arg(ui->cmbBead->currentIndex()+1).arg(ui->cmbChannel->currentIndex()), fHeading1);
            cursor.movePosition(QTextCursor::End);

            QTextTable* tablePic = cursor.insertTable(3,3, tableFormat);
            tablePic->mergeCells(2,0,1,2);
            {
                // insert a plot from ui->plotter
                QTextCursor tabCursor=tablePic->cellAt(0, 0).firstCursorPosition();
                QPicture pic;
                JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
                ui->pltXZ->get_plotter()->draw(*painter, QRect(0,0,ui->pltXZ->width(),ui->pltXZ->height()));
                delete painter;
                double scale=0.3*document->textWidth()/double(pic.boundingRect().width());
                if (scale<=0) scale=1;
                tabCursor.insertText(tr("XZ-cut:\n"), fTextBoldSmall);
                insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.3);
                QApplication::processEvents();

            }
            {
                // insert a plot from ui->plotter
                QTextCursor tabCursor=tablePic->cellAt(0, 1).firstCursorPosition();
                QPicture pic;
                JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
                ui->pltYZ->get_plotter()->draw(*painter, QRect(0,0,ui->pltYZ->width(),ui->pltYZ->height()));
                delete painter;
                double scale=0.3*document->textWidth()/double(pic.boundingRect().width());
                if (scale<=0) scale=1;
                tabCursor.insertText(tr("YZ-cut:\n"), fTextBoldSmall);
                insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.3);
                QApplication::processEvents();

            }
            {
                // insert a plot from ui->plotter
                QTextCursor tabCursor=tablePic->cellAt(0, 2).firstCursorPosition();
                QPicture pic;
                JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
                ui->pltXY->get_plotter()->draw(*painter, QRect(0,0,ui->pltXY->width(),ui->pltXY->height()));
                delete painter;
                double scale=0.3*document->textWidth()/double(pic.boundingRect().width());
                if (scale<=0) scale=1;
                tabCursor.insertText(tr("XY-cut:\n"), fTextBoldSmall);
                insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.3);
                QApplication::processEvents();

            }
            {
                // insert a plot from ui->plotter
                QTextCursor tabCursor=tablePic->cellAt(1, 0).firstCursorPosition();
                QPicture pic;
                JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
                ui->pltFitX->get_plotter()->draw(*painter, QRect(0,0,ui->pltFitX->width(),ui->pltFitX->height()));
                delete painter;
                double scale=0.3*document->textWidth()/double(pic.boundingRect().width());
                if (scale<=0) scale=1;
                tabCursor.insertText(tr("X-cut:\n"), fTextBoldSmall);
                insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.3);
                QApplication::processEvents();

            }
            {
                // insert a plot from ui->plotter
                QTextCursor tabCursor=tablePic->cellAt(1, 1).firstCursorPosition();
                QPicture pic;
                JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
                ui->pltFitY->get_plotter()->draw(*painter, QRect(0,0,ui->pltFitY->width(),ui->pltFitY->height()));
                delete painter;
                double scale=0.3*document->textWidth()/double(pic.boundingRect().width());
                if (scale<=0) scale=1;
                tabCursor.insertText(tr("Y-cut:\n"), fTextBoldSmall);
                insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.3);
                QApplication::processEvents();

            }
            {
                // insert a plot from ui->plotter
                QTextCursor tabCursor=tablePic->cellAt(1, 2).firstCursorPosition();
                QPicture pic;
                JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
                ui->pltFitZ->get_plotter()->draw(*painter, QRect(0,0,ui->pltFitZ->width(),ui->pltFitZ->height()));
                delete painter;
                double scale=0.3*document->textWidth()/double(pic.boundingRect().width());
                if (scale<=0) scale=1;
                tabCursor.insertText(tr("Z-cut:\n"), fTextBoldSmall);
                insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.3);
                QApplication::processEvents();

            }
            {
                // insert a plot from ui->plotter
                QTextCursor tabCursor=tablePic->cellAt(2, 0).firstCursorPosition();
                QPicture pic;
                JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
                ui->pltFitWofZ->get_plotter()->draw(*painter, QRect(0,0,ui->pltFitWofZ->width(),ui->pltFitWofZ->height()));
                delete painter;
                double scale=0.63*document->textWidth()/double(pic.boundingRect().width());
                if (scale<=0) scale=1;
                tabCursor.insertText(tr("X/Y(Z)-cuts:\n"), fTextBoldSmall);
                insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.63);
                QApplication::processEvents();

            }
        }
    }


}


