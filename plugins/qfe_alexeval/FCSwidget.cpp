/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#include "FCSwidget.h"
#include "ui_FCSwidget.h"
#include "gsl/gsl_histogram2d.h"
#include <QDebug>
#include "qfproject.h"
#include "qfrawdatarecord.h"
#include "qfpluginservices.h"

FCSwidget::FCSwidget(QWidget *parent, AlexData* data, FCSdata *fd) :
    QWidget(parent),
    ui(new Ui::FCSwidget)
{
    ui->setupUi(this);
    if(data==NULL) qFatal("FCSwidget: data not initialized");
    ad=data;
    if(fd==NULL) qFatal("FCSwidget: FCS data not initialized");
    this->fd=fd;

    ui->chkSelect->setChecked(true);

    ui->textBrowserInfoFCS->setText(ad->FCSparams.toString());

    connect(ui->pushButtonSave,SIGNAL(clicked()),this,SLOT(save()));
    connect(ui->pushButtonSaveToProject,SIGNAL(clicked()),this,SLOT(saveToProject()));
    connect(ui->pushButtonLoadAndRunFCS,SIGNAL(clicked()),this,SLOT(loadAndRunFCS()));
    connect(ui->widgetPlotAlex,SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)),this,SLOT(selectRangeRect(double,double,double,double)));
    connect(ui->pushButtonFCS,SIGNAL(clicked()),this,SLOT(runFCS()));

    ui->comboBoxFCSMethod->addItem("TTTR");
    ui->comboBoxFCSMethod->addItem("stepwise MultiTau");

    ui->spinBoxMin->setMinimum(1);
    ui->spinBoxMax->setMinimum(1);
    ui->spinBoxMax->setMaximum(500);


    connect(ui->spinBoxMin,SIGNAL(editingFinished()),this,SLOT(plotFCS()));
    connect(ui->spinBoxMax,SIGNAL(editingFinished()),this,SLOT(plotFCS()));
    connect(ui->spinBoxMax,SIGNAL(valueChanged(int)),this,SLOT(spinBoxMinSetMax(int)));
}

FCSwidget::~FCSwidget()
{
    delete ui;
}

void FCSwidget::storeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+"comboBoxPalette",ui->comboBoxPalette->currentIndex());
    settings.setValue(prefix+"comboBoxFCSMethod",ui->comboBoxFCSMethod->currentIndex());
    settings.setValue(prefix+"doubleSpinBoxMaxLag",ui->doubleSpinBoxMaxLag->value());
    settings.setValue(prefix+"alphaSelectionRectangle",alphaSelectionRectangle);
    settings.setValue(prefix+"splitterGeometry",ui->splitter->saveState());
    settings.setValue(prefix+"splitterGeometry2",ui->splitter_2->saveState());
    settings.setValue(prefix+"nbins",nbins);
}

void FCSwidget::loadSettings(QSettings &settings, const QString &prefix)
{
    ui->comboBoxPalette->setCurrentIndex(settings.value(prefix+"comboBoxPalette",0).toInt());
    ui->comboBoxFCSMethod->setCurrentIndex(settings.value(prefix+"comboBoxFCSMethod",0).toInt());
    ui->doubleSpinBoxMaxLag->setValue(settings.value(prefix+"doubleSpinBoxMaxLag",0).toDouble());
    alphaSelectionRectangle=settings.value(prefix+"alphaSelectionRectangle",0.2).toDouble();
    ui->splitter->restoreState(settings.value(prefix+"splitterGeometry").toByteArray());
    ui->splitter_2->restoreState(settings.value(prefix+"splitterGeometry2").toByteArray());
    nbins=settings.value(prefix+"nbins",40).toInt();
}

void FCSwidget::save() const
{
    QString prefix=ad->currentDir+"/"+ad->name+"_";
    AlexEvalLog::text("save FCS data " + prefix);
    if(fd->isEmpty()) AlexEvalLog::warning("no FCS data to save");
    else {
        fd->saveCorr(prefix+FILENAME_FCS,ad->FCSparams);
        SaveBurstsDual(fd->subpopulation,prefix+FILENAME_FCS_SUBPOPULATION);
    }
}

void FCSwidget::saveToProject() const
{
    QString prefix=ad->currentDir+"/"+ad->name+"_";
    AlexEvalLog::text("save FCS data " + prefix);
    if(fd->isEmpty()) AlexEvalLog::warning("no FCS data to save");
    else {
        QString ACF_Dem_Dex=fd->getCorrCSV(0,ad->FCSparams);
        QString ACF_Aem_Dex=fd->getCorrCSV(1,ad->FCSparams);
        QString CCF_Dex=fd->getCorrCSV(2,ad->FCSparams);
        QString ACF_sumAD_Dex=fd->getCorrCSV(3,ad->FCSparams);

        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p) {
            QString group=ad->name;
            QFRawDataRecord* rdr;
            QString role;
            QString name;
            QMap<QString, QVariant> initParams;
            QStringList initParamsRO;
            initParamsRO<<"INTERNAL_CSVMODE"<<"INTERNAL_CSV"<<"FILETYPE";

            initParams["INTERNAL_CSVMODE"]=0;
            initParams["FILETYPE"]="INTERNAL";


            role="ACF0";
            name=ad->name+" - ACF_Dem_Dex";
            initParams["INTERNAL_CSV"]=ACF_Dem_Dex;
            rdr=p->addRawData("fcs", name, role, QStringList(), initParams, initParamsRO);
            if (rdr) {
                rdr->setGroup(p->addOrFindRDRGroup(group));
                rdr->setFolder(group);
            }


            role="ACF1";
            name=ad->name+" - ACF_Aem_Dex";
            initParams["INTERNAL_CSV"]=ACF_Aem_Dex;
            rdr=p->addRawData("fcs", name, role, QStringList(), initParams, initParamsRO);
            if (rdr) {
                rdr->setGroup(p->addOrFindRDRGroup(group));
                rdr->setFolder(group);
            }


            role="FCCS01";
            name=ad->name+" - CCF_Dex";
            initParams["INTERNAL_CSV"]=CCF_Dex;
            rdr=p->addRawData("fcs", name, role, QStringList(), initParams, initParamsRO);
            if (rdr) {
                rdr->setGroup(p->addOrFindRDRGroup(group));
                rdr->setFolder(group);
            }

            role="ACF";
            name=ad->name+" - ACF_sumAD_Dex";
            initParams["INTERNAL_CSV"]=ACF_sumAD_Dex;
            rdr=p->addRawData("fcs", name, role, QStringList(), initParams, initParamsRO);
            if (rdr) {
                rdr->setGroup(p->addOrFindRDRGroup(group));
                rdr->setFolder(group);
            }
        }

    }
}


void FCSwidget::runFCS()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    getParameters();
    if(ui->checkBoxBurstwise->isChecked()) {
        if(analysisFCSburstwise(fd,ad)) AlexEvalLog::warning("error in runFCS burstwise");
        else {
            ui->spinBoxMin->setValue(qMin(fd->tau.size()-1,ui->spinBoxMin->value()));
            ui->spinBoxMax->setMaximum(fd->tau.size());
            ui->spinBoxMax->setValue(fd->tau.size());
            plotFCS();
        }
    } else {
        if(analysisFCStotal(fd,ad)) AlexEvalLog::warning("error in runFCS");
        else {
            ui->spinBoxMin->setValue(qMin(fd->tau.size()-1,ui->spinBoxMin->value()));
            ui->spinBoxMax->setMaximum(fd->tau.size());
            ui->spinBoxMax->setValue(fd->tau.size());
            plotFCS();
        }
    }
    ui->textBrowserInfoFCS->setPlainText(fd->resultFCS("FCSresults\n")+ad->FCSparams.toString("\n\n"));
    QApplication::restoreOverrideCursor();
}

void FCSwidget::loadAndRunFCS()
{
    if(ad->initFCS()) AlexEvalLog::warning("loadAndRunFCS: error reading ini file");
    fd->clearMarker();
    fd->clear();
    runFCS();
}

void FCSwidget::plotFCS()
{
    if(fd->isEmpty()) return;
    plotFCS1();
    plotFCS2();
}

void FCSwidget::plotFCS1()
{
    if(fd->isEmpty()) return;
    qDebug("plotFCS");
    plot=ui->widgetPlotFCS;
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    // empty datastore and grid
    ds->clear();
    plot->get_plotter()->clearGraphs(true);

    plot->setObjectName("FCS");
    plot->setGrid(false);
    plot->get_plotter()->set_plotLabel("species-selective FCS");
    plot->get_plotter()->getXAxis()->set_axisLabel("lag time $\\tau$ in s"); // $x$ axis with greek letters $\\sqrt{\\alpha\\cdot\\beta}$
    plot->get_plotter()->getYAxis()->set_axisLabel("g($\\tau$)");
    plot->get_plotter()->getXAxis()->set_logAxis(true);

    qDebug()<<"size of ACF_markerD = "<<fd->ACF_markerD.size();
    qDebug()<<"size of tau = "<<fd->tau.size();

    int start=ui->spinBoxMin->value()-1;
    int nPoints=qMin(ui->spinBoxMax->value()-start,fd->tau.size()-start);
    size_t xColumn=ds->addCopiedColumn(fd->tau.data()+start, nPoints, "tau");
    QVector<size_t> yColumns;
    yColumns.push_back(ds->addCopiedColumn(fd->ACF_Dem_Dex.data()+start, nPoints,  "ACF_Dem_Dex"));
    yColumns.push_back(ds->addCopiedColumn(fd->ACF_Aem_Dex.data()+start, nPoints,  "ACF_Aem_Dex"));
    yColumns.push_back(ds->addCopiedColumn(fd->CCF_Dex.data()+start, nPoints,  "CCF_Dex"));
    if(ad->method==AlexData::ALEX) yColumns.push_back(ds->addCopiedColumn(fd->ACF_markerD.data()+start, nPoints,  "g_MarkerD"));
//    QStringList titles;
//    titles<<"auto corr D"<<"auto corr A"<<"cross corr DA"<<"marker corr";
    QStringList titles=ds->getColumnNames().replaceInStrings("_"," "); titles.removeFirst(); // first contains the x column
    QList<QColor> colors;
    colors.append(QColor(COUNTRATE_COLOR_DONOR));
    colors.append(QColor(COUNTRATE_COLOR_ACCEPTOR));
    colors.append(QColor(FCS_COLOR_CROSS));
    colors.append(QColor(FCS_COLOR_MARKER));

    // line plots
    for (int i=0; i<yColumns.size(); i++) {
        JKQTPxyLineGraph*lg=new JKQTPxyLineGraph(plot->get_plotter());
        lg->set_drawLine(true);
        lg->set_xColumn(xColumn);
        lg->set_yColumn(yColumns.at(i));
        lg->set_title(titles.at(i));
        lg->set_color(colors.at(i));
        plot->addGraph(lg);
    }

    plot->zoomToFit();
}

void FCSwidget::plotFCS2()
{
    if(fd->isEmpty()) return;
    qDebug("plotFCS2");
    plot=ui->widgetPlotFCS2;
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    // empty datastore and grid
    ds->clear();
    plot->get_plotter()->clearGraphs(true);

    plot->setObjectName("FCS2");
    plot->setGrid(false);
    plot->get_plotter()->getXAxis()->set_axisLabel("lag time $\\tau$ in s");
    plot->get_plotter()->getYAxis()->set_axisLabel("g($\\tau$)");
    plot->get_plotter()->getXAxis()->set_logAxis(true);

//    unsigned long long nrows=fd->tau.size();
    int start=ui->spinBoxMin->value()-1;
    int nPoints=ui->spinBoxMax->value()-start;
    size_t xColumn=ds->addCopiedColumn(fd->tau.data()+start, nPoints,  "tau");
    QVector<size_t> yColumns;
    yColumns.push_back(ds->addCopiedColumn(fd->ACF_sumAD_Dex.data()+start, nPoints,  "ACF_sumAD_Dex"));
    yColumns.push_back(ds->addCopiedColumn(fd->ACF_Aem_Aex.data()+start, nPoints,  "ACF_Aem_Aex"));
    yColumns.push_back(ds->addCopiedColumn(fd->ACF_Aem.data()+start, nPoints,  "ACF_Aem"));
    if(ad->method==AlexData::ALEX) yColumns.push_back(ds->addCopiedColumn(fd->ACF_markerD.data()+start, nPoints, "ACF_markerD"));
    if(ad->method==AlexData::ALEX) yColumns.push_back(ds->addCopiedColumn(fd->ACF_markerA.data()+start, nPoints, "ACF_markerA"));
    yColumns.push_back(ds->addCopiedColumn(fd->ACF_Dem_Dex_later.data()+start, nPoints, "ACF_Dem_Dex_later"));
    QStringList titles=ds->getColumnNames().replaceInStrings("_"," "); titles.removeFirst(); // first contains the x column
    QList<QColor> colors;
    colors.append(QColor(COUNTRATE_COLOR_DONOR));
    colors.append(QColor(COUNTRATE_COLOR_ACCEPTOR));
    colors.append(QColor(FCS_COLOR_CROSS));
    if(ad->method==AlexData::ALEX) colors.append(QColor(FCS_COLOR_MARKER));
    if(ad->method==AlexData::ALEX) colors.append(QColor("yellow"));
    colors.append(QColor("skyblue"));

    // line plots
    for (int i=0; i<yColumns.size(); i++) {
        JKQTPxyLineGraph*lg=new JKQTPxyLineGraph(plot->get_plotter());
        lg->set_drawLine(true);
        lg->set_xColumn(xColumn);
        lg->set_yColumn(yColumns.at(i));
        lg->set_title(titles.at(i));
        lg->set_color(colors.at(i));
        plot->addGraph(lg);
    }

    plot->zoomToFit();
}


void FCSwidget::plotDensity()
{
    if(ad->isEmptyBurst()) return;
    plot=ui->widgetPlotAlex;
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    // empty datastore and grid
    plot->get_plotter()->clearGraphs(true);
    ds->clear();
    plot->setGrid(false);
//    plot->get_plotter()->set_plotLabel("ALEX density plot");
    plot->get_plotter()->getXAxis()->set_axisLabel("PR");
    plot->get_plotter()->getYAxis()->set_axisLabel("S");

    gsl_histogram2d * histAlex=gsl_histogram2d_calloc_uniform(nbins,nbins,ad->rangeALEX.minP,ad->rangeALEX.maxP,ad->rangeALEX.minS,ad->rangeALEX.maxS);
    for(int i=0; i<ad->burstVectorDual.size();++i)
        gsl_histogram2d_increment(histAlex, ad->burstVectorDual.at(i).proximityRatio, ad->burstVectorDual.at(i).stoichiometryRatio);

//    qDebug()<<histAlex->ny<<histAlex->nx;
    size_t colHist2D=ds->addCopiedImageAsColumnTranspose(histAlex->bin, histAlex->ny, histAlex->nx, tr("2Dhist"));
//    ds->getColumn(colHist2D).exchange(0, 1.0/0.0);



    JKQTPColumnMathImage* densityplot=new JKQTPColumnMathImage(plot->get_plotter()); // bin(i,j) = bin[i * ny + j].
    densityplot->set_x(ad->rangeALEX.minP);
    densityplot->set_y(ad->rangeALEX.minS);
    densityplot->set_width(ad->rangeALEX.width());
    densityplot->set_height(ad->rangeALEX.heigth());
    densityplot->set_Nx(histAlex->nx);
    densityplot->set_Ny(histAlex->ny);
    densityplot->set_palette(ui->comboBoxPalette->colorPalette());
    densityplot->set_imageColumn(colHist2D);
    densityplot->set_imageName(tr("#bursts"));
//    densityplot->set_infColor(Qt::transparent);
    densityplot->set_nanColor(Qt::transparent);

    plot->addGraph(densityplot);
    gsl_histogram2d_free(histAlex);

    // selection
    QColor fill=QColor("skyblue");
    fill.setAlphaF(0.2);
    pltSelectionRect=new JKQTPgeoRectangle(plot->get_plotter(), 0,0,0,0, QColor("darkblue"), 1, Qt::DotLine, fill);
    pltSelectionRect->set_bottomleftrectangle(ad->rangeALEX.minP,ad->rangeALEX.minS,ad->rangeALEX.width(),ad->rangeALEX.heigth());
    plot->addGraph(pltSelectionRect);
    setPlotRange(ad->rangeALEX,plot);
}

void FCSwidget::spinBoxMinSetMax(int max)
{
    ui->spinBoxMin->setMaximum(max);
}

void FCSwidget::plotAlex()
{
    if(ad->isEmptyFRET()) return;
    plotDensity();
}

void FCSwidget::on_chkSelect_toggled(bool checked)
{
    if (checked) {
        ui->widgetPlotAlex->set_mouseActionMode(JKQtPlotter::RectangleEvents);
    } else {
        ui->widgetPlotAlex->set_mouseActionMode(JKQtPlotter::ZoomRectangle);
    }
}

void FCSwidget::on_comboBoxPalette_currentIndexChanged()
{
    plotAlex();
}

void FCSwidget::on_pushButtonRefresh_clicked()
{
    plotAlex();
}


void FCSwidget::selectRangeRect(double x, double y, double width, double height)
{
//    qDebug()<<"selectRangeRect  x="<<x<<"  y="<<y<<"  width="<<width<<"  height="<<height;
    ad->FRETparams.MinS=qMin(y,y+height);
    ad->FRETparams.MaxS=qMax(y,y+height);
    ad->FRETparams.MinP=qMin(x,x+width);
    ad->FRETparams.MaxP=qMax(x,x+width);
    if (pltSelectionRect) {
        pltSelectionRect->set_bottomleftrectangle(x,y,width,height);
        ui->widgetPlotAlex->update_plot();
    }
}

void FCSwidget::setPlotRange(const Range_t &range, JKQtPlotter *plotter)
{
    plotter->zoom(range.minP,range.maxP,range.minS,range.maxS);
}

void FCSwidget::getParameters()
{
    if(ui->comboBoxFCSMethod->currentIndex()==0) ad->FCSparams.FCSMethod=FCSparams_t::FCS_TTTR;
    if(ui->comboBoxFCSMethod->currentIndex()==1) ad->FCSparams.FCSMethod=FCSparams_t::FCSstep;
    if(ui->comboBoxFCSMethod->currentIndex()>1) AlexEvalLog::warning("unknown FCS method");

    ad->FCSparams.CorrelationWindow=ui->doubleSpinBoxMaxLag->value()/1e3;

    ad->FCSparams.doBoxFiltering=ui->checkBoxBoxFilter->isChecked();
    ad->FCSparams.doDivideByMarkerCorr=ui->checkBoxOsc->isChecked();
    ad->FCSparams.doIgnoreZeros=ui->checkBoxIgnoreZeros->isChecked();
}
