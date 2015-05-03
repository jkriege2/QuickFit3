/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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
#include <QTextStream>
#include "libtiff_tools.h"
#include "qfimfcssetparamfromfiledialog.h"
#include "ui_qfimfcssetparamfromfiledialog.h"
#include "qfrdrimagetoruninterface.h"
#include "qftools.h"
#include "csvtools.h"

QFImFCSSetParamFromFileDialog::QFImFCSSetParamFromFileDialog(int width, int height, QFImFCSFitEvaluation* eval, QStringList parameters, QStringList parameterIDs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFImFCSSetParamFromFileDialog)
{
    this->parameterIDs=parameterIDs;
    this->desiredwidth=width;
    this->desiredheight=height;
    plt=NULL;
    ui->setupUi(this);
    ui->cmbRDR->init(eval->getProject(), eval->getMatchFunctor());
    ui->cmbResultGroup->setRDR(ui->cmbRDR->currentRDR());
    ui->cmbResult->setRDR(ui->cmbRDR->currentRDR());
    ui->cmbResult->setEvaluationGroup(ui->cmbResultGroup->currentEvaluationGroup());
    ui->cmbParameter->clear();
    ui->cmbParameter->addItems(parameters);
    ui->cmbParameter->setCurrentIndex(0);
    QStringList filt;
    filt<<"fit results"<<"results"<<"evaluation results";
    ui->cmbResult->setResultGroupFilters(filt);
    ui->cmbRDR->setCurrentIndex(0);

    loadWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "QFImFCSSetParamFromFileDialog");
    loadSplitter(*(ProgramOptions::getInstance()->getQSettings()), ui->splitter, "QFImFCSSetParamFromFileDialog/splitter");
    ui->cmbParameter->setCurrentIndex(parameterIDs.indexOf(ProgramOptions::getConfigValue("QFImFCSSetParamFromFileDialog/param", parameterIDs.value(0, "")).toString()));
    ui->cmbRDR->setCurrentIndex(ProgramOptions::getConfigValue("QFImFCSSetParamFromFileDialog/rdr",0).toInt());
    ui->cmbResultGroup->setCurrentEvaluationGroup(ProgramOptions::getConfigValue("QFImFCSSetParamFromFileDialog/resultgroup",ui->cmbResultGroup->currentEvaluationGroup()).toString());
    ui->cmbResult->setCurrentResult(ProgramOptions::getConfigValue("QFImFCSSetParamFromFileDialog/result",ui->cmbResult->currentResult()).toString());
    ui->radRDR->setChecked(ProgramOptions::getConfigValue("QFImFCSSetParamFromFileDialog/radRDR", true).toBool());


    connect(ui->cmbRDR, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResultGroup, SLOT(setRDR(QFRawDataRecord*)));
    connect(ui->cmbRDR, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResult, SLOT(setRDR(QFRawDataRecord*)));
    connect(ui->cmbResultGroup, SIGNAL(currentEvaluationGroupChanged(QString)), ui->cmbResult, SLOT(setEvaluationGroup(QString)));
    connect(ui->imageTransfrom, SIGNAL(propertiesChanged()), this, SLOT(replotOvr()));
    updateDataFromRDR();

    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}


QFImFCSSetParamFromFileDialog::~QFImFCSSetParamFromFileDialog()
{
    saveWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "QFImFCSSetParamFromFileDialog");
    saveSplitter(*(ProgramOptions::getInstance()->getQSettings()), ui->splitter, "QFImFCSSetParamFromFileDialog/splitter");
    ProgramOptions::setConfigValue("QFImFCSSetParamFromFileDialog/param", parameterIDs.value(ui->cmbParameter->currentIndex(), ""));
    ProgramOptions::setConfigValue("QFImFCSSetParamFromFileDialog/rdr",ui->cmbRDR->currentIndex());
    ProgramOptions::setConfigValue("QFImFCSSetParamFromFileDialog/resultgroup",ui->cmbResultGroup->currentEvaluationGroup());
    ProgramOptions::setConfigValue("QFImFCSSetParamFromFileDialog/result",ui->cmbResult->currentResult());
    ProgramOptions::setConfigValue("QFImFCSSetParamFromFileDialog/radRDR", ui->radRDR->isChecked());
    delete ui;
}

QFRawDataRecord *QFImFCSSetParamFromFileDialog::getRDR() const
{
    return ui->cmbResult->getRDR();
}

QString QFImFCSSetParamFromFileDialog::getEvalID() const
{
    return ui->cmbResult->currentEvaluationID();
}

QString QFImFCSSetParamFromFileDialog::getResultID() const
{
    return ui->cmbResult->currentResult();
}

QString QFImFCSSetParamFromFileDialog::getParameter() const
{
    return parameterIDs.value(ui->cmbParameter->currentIndex(), "");
}

QVector<double> QFImFCSSetParamFromFileDialog::getData(bool *ok, int *width, int *height)
{
    QVector<double> d=data;
    if (ok) *ok=(data.size()>=desiredwidth*desiredheight);
    if (ui->chkImageTransform->isChecked()) {
        QVector<double> dat(datawidth*dataheight, 0.0);
        for (int i=0; i<datawidth*dataheight; i++) {
            dat[i]=data.value(i);
        }
        int idatawidth=datawidth;
        int idataheight=dataheight;
        int w=idatawidth,h=idataheight;
        bool o=ui->imageTransfrom->transform(dat, idatawidth, idataheight, d, w, h);
        if (width) *width=w;
        if (height) *height=h;
        if (ok) *ok=(*ok) && o && (d.size()>=desiredwidth*desiredheight);
    }
    return d;
}

void QFImFCSSetParamFromFileDialog::replotOvr()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    ui->pltData->set_doDrawing(false);
    ui->pltData->clearGraphs();
    JKQTPdatastore* ds=ui->pltData->getDatastore();
    ds->clear();

    QFRawDataRecord* rdr=getRDR();
    //QFRDRImageToRunInterface* dataImg=dynamic_cast<QFRDRImageToRunInterface*>(rdr);

    //if (dataImg) {

        bool ok=false;
        int w=datawidth;
        int h=dataheight;
        QVector<double> d=getData(&ok, &w, &h);
        int dcopysize=d.size();

        ui->labInfo->setText(tr("desired image size: %1 x %2 = %3         output data size: %4 (%5x%6)").arg(desiredwidth).arg(desiredheight).arg(desiredwidth*desiredheight).arg(dcopysize).arg(w).arg(h));

        //qDebug()<<w<<h<<d.size()<<dcopysize;
        if (ui->chkMapDesired->isChecked()) {
            w=desiredwidth;
            h=desiredheight;
            if (d.size()>desiredwidth*desiredheight) {
                dcopysize=desiredwidth*desiredheight;
            }
            if (d.size()<desiredwidth*desiredheight) {
                while (d.size()<desiredwidth*desiredheight) d.append(0.0);
                dcopysize=desiredwidth*desiredheight;
            }
        }
        //qDebug()<<w<<h<<d.size()<<dcopysize;

        //qDebug()<<"PLOT!!!";
        //while (d.size()<w*h) d.append(0);


        //if (ok) {
            size_t col=ds->addCopiedColumn(d.data(), dcopysize, getResultID());

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
            plt->set_title(getResultID());
            ui->pltData->get_plotter()->set_showKey(false);
            ui->pltData->get_plotter()->set_axisAspectRatio(double(w)/double(h));
            ui->pltData->get_plotter()->set_aspectRatio(double(w)/double(h));
            ui->pltData->get_plotter()->set_maintainAspectRatio(true);
            ui->pltData->get_plotter()->set_maintainAxisAspectRatio(true);
            ui->pltData->addGraph(plt);
            ui->pltData->setAbsoluteXY(0,w,0,h);
            ui->pltData->setXY(0,w,0,h);
        //}
    //}

    //ui->pltData->zoomToFit();
    ui->pltData->set_doDrawing(true);
    ui->pltData->update_plot();
    QApplication::restoreOverrideCursor();
}

void QFImFCSSetParamFromFileDialog::on_cmbParamter_currentIndexChanged(int /*index*/)
{
    /*ui->cmbResult->setCurrentResult(QString("fitparam_")+parameterIDs.value(index, ""));
    if (ui->cmbResult->currentIndex()<0) ui->cmbResult->setCurrentResult(parameterIDs.value(index, ""));*/
}

void QFImFCSSetParamFromFileDialog::updateDataFromRDR()
{
    if (ui->radRDR->isChecked()) {
        QFRawDataRecord* rdr=getRDR();
        if (rdr) {
            data=rdr->resultsGetAsDoubleList(getEvalID(), getResultID());
            datawidth=desiredwidth;
            dataheight=desiredheight;
            QFRDRImageToRunInterface* runs=dynamic_cast<QFRDRImageToRunInterface*>(rdr);
            if (runs) {
                datawidth=runs->getImageFromRunsWidth();
                dataheight=runs->getImageFromRunsHeight();
            }
        } else data=QVector<double>();
    } else if (ui->radOverview->isChecked()) {
        QFRawDataRecord* rdr=getRDR();
        if (rdr) {
            QFRDRImageToRunInterface* runs=dynamic_cast<QFRDRImageToRunInterface*>(rdr);
            if (runs) {
                datawidth=runs->getImageFromRunsWidth();
                dataheight=runs->getImageFromRunsHeight();
                data=arrayToVector(runs->getImageFromRunsPreview(), datawidth*dataheight);
            } else data=QVector<double>();
        } else data=QVector<double>();
    }
    replotOvr();
}

void QFImFCSSetParamFromFileDialog::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("imfcs_fit")+"/setparamfromfile.html");
}

void QFImFCSSetParamFromFileDialog::on_btnLoadFile_clicked()
{
    QString lastDir=ProgramOptions::getConfigValue("QFImFCSSetParamFromFileDialog/lastdir", "").toString();
    QStringList filters;
    filters<<tr("TIFF image (*.tif)")<<tr("Column from Commma-separated values (*.dat *.csv *.txt)");
    QString selFilter=ProgramOptions::getConfigValue("QFImFCSSetParamFromFileDialog/lastfilter", filters.first()).toString();
    QString filename= qfGetOpenFileName(this, tr("select image ..."), lastDir, filters.join(";;"), &selFilter);
    if (QFile::exists(filename)) {

        int idx=filters.indexOf(selFilter);

        QVector<double> newData;

        if (idx==0) {
            double* data=NULL;
            int width, height;
            if (TIFFReadFrame<double>(filename.toLocal8Bit().data(), &data, &width, &height)) {
                if (data && width>0 && height>0) {
                    newData=arrayToVector(data, width*height);
                    qfFree(data);
                    datawidth=width;
                    dataheight=height;
                }
            }
        } else if (idx==1) {
            QStringList csvCols;
            QList<QVector<double> > csv=importCSVAskUser(filename, "QFImFCSSetParamFromFileDialog/csv/", &csvCols);
            if (csv.size()==1) {
                newData=csv[0];
            } else if (csv.size()>0) {
                QStringList items;
                for (int i=0; i<csv.size(); i++) {
                    items<<tr("column %1").arg(i+1);
                    if (i<csvCols.size() && !csvCols[i].isEmpty()) items.last().append(" '"+csvCols[i]+"'");
                }
                bool ok=true;
                int col=items.indexOf(QInputDialog::getItem(this, tr("load data from CSV file"), tr("Please selectthe column to import:"), items, 0, false, &ok));
                if (ok && col>=0 && col<csv.size()) {
                    newData=csv[col];
                }
            } else {
                QMessageBox::critical(this, tr("load file"), tr("Could not read any data from CSV file '%1'!").arg(filename));
            }
        }
        if (newData.size()>0) {
            if (newData.size()<desiredwidth*desiredheight) {
                if (QMessageBox::question(this, tr("load file"), tr("The size of read data is smaller than the required data size (new data size: %4,  required size: %1x%2 = %3).\n  Import data nevertheless?").arg(desiredwidth).arg(desiredheight).arg(desiredwidth*desiredheight).arg(newData.size()), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
                    data=newData;
                    datawidth=desiredwidth;
                    dataheight=desiredheight;
                }
            } else {
                data=newData;
            }
            replotOvr();
        }
        ProgramOptions::setConfigValue("QFImFCSSetParamFromFileDialog/lastdir", lastDir);
        ProgramOptions::setConfigValue("QFImFCSSetParamFromFileDialog/lastfilter", selFilter);
    }
}


