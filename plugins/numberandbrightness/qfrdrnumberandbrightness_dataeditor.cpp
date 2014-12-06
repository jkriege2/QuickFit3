/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "qfrdrnumberandbrightness_dataeditor.h"
#include "qfrdrnumberandbrightness_data.h"
#include "qfrawdatapropertyeditor.h"


QFRDRNumberAndBrightnessDataEditor::QFRDRNumberAndBrightnessDataEditor(QFPluginServices* services,  QFRawDataPropertyEditor*  propEditor, QWidget* parent):
    QFRawDataEditor(services, propEditor, parent)
{
    maskTools=new QFRDRImageMaskEditTools(this, QString("QFRDRNumberAndBrightnessDataEditor/maskTools/"));

    QColor ovlSelCol=QColor("red");
    ovlSelCol.setAlphaF(0.8);
    selectionColor=ovlSelCol;
    QColor ovlExCol=QColor("black");
    ovlExCol.setAlphaF(0.8);
    excludedColor=ovlExCol;



    plteNumberData=NULL;
    plteBrightnessData=NULL;
    plteOverviewData=NULL;
    plteCorrelationData=NULL;

    plteOverviewSelectedData=NULL;
    //plteOverviewExcludedData=NULL;
    plotsSize=0;

    createWidgets();
}

QFRDRNumberAndBrightnessDataEditor::~QFRDRNumberAndBrightnessDataEditor()
{
    reallocMem(0,0);
}


void QFRDRNumberAndBrightnessDataEditor::addPlotter(QFPlotter *&plotter, JKQTPMathImage *&plot, JKQTPOverlayImage *&plteSelected, JKQTPOverlayImage *&plteExcluded)
{
    plotter=new QFPlotter(this);
    plot=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0, JKQTPMathImageMATLAB, plotter->get_plotter());
    plotter->addGraph(plot);
    plteExcluded=new JKQTPOverlayImage(0,0,1,1,NULL, 0, 0, excludedColor, plotter->get_plotter());
    plotter->addGraph(plteExcluded);
    plteSelected=new JKQTPOverlayImage(0,0,1,1,NULL, 0, 0, selectionColor, plotter->get_plotter());
    plotter->addGraph(plteSelected);

    plotter->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    plotter->get_plotter()->set_plotLabelFontSize(10);
    plotter->set_displayMousePosition(false);
    plotter->set_displayToolbar(true);
    plotter->get_plotter()->set_maintainAspectRatio(true);
    plotter->get_plotter()->set_aspectRatio(1);
    plotter->get_plotter()->set_maintainAxisAspectRatio(true);
    plotter->get_plotter()->set_axisAspectRatio(1);
    plotter->setXY(0,0,1,1);
    plotter->setAbsoluteXY(0,1,0,1);
    plotter->get_plotter()->getXAxis()->set_axisMinWidth(1);
    plotter->get_plotter()->getYAxis()->set_axisMinWidth(1);

    plotter->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    plotter->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    plotter->get_plotter()->getXAxis()->set_axisLabel("");
    plotter->get_plotter()->getYAxis()->set_axisLabel("");
    plotter->get_plotter()->setGrid(false);
    plotter->get_plotter()->set_useAntiAliasingForSystem(true);
    plotter->get_plotter()->set_useAntiAliasingForGraphs(true);
    plotter->set_userActionCompositionMode(QPainter::CompositionMode_Xor);

    connect(plotter, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));




}

void QFRDRNumberAndBrightnessDataEditor::reallocMem(int width, int height) {
    int size=width*height;
    if (size==0) {
        qfFree(plteNumberData);
        plteNumberData=NULL;
        qfFree(plteBrightnessData);
        plteBrightnessData=NULL;
        qfFree(plteOverviewData);
        plteOverviewData=NULL;
        qfFree(plteCorrelationData);
        plteCorrelationData=NULL;
        qfFree(plteOverviewSelectedData);
        plteOverviewSelectedData=NULL;
        /*qfFree(plteOverviewExcludedData);
        plteOverviewExcludedData=NULL;*/
        plotsSize=0;
    } else if (size!=plotsSize) {
        plotsSize=size;
        plteNumberData=(double*)qfRealloc(plteNumberData, plotsSize*sizeof(double));
        plteBrightnessData=(double*)qfRealloc(plteBrightnessData, plotsSize*sizeof(double));
        plteOverviewData=(double*)qfRealloc(plteOverviewData, plotsSize*sizeof(double));
        plteCorrelationData=(double*)qfRealloc(plteCorrelationData, plotsSize*sizeof(double));

        plteOverviewSelectedData=(bool*)qfRealloc(plteOverviewSelectedData, plotsSize*sizeof(bool));
        //plteOverviewExcludedData=(bool*)qfRealloc(plteOverviewExcludedData, plotsSize*sizeof(bool));
    }
    for (int i=0; i<plotsSize; i++) {
        plteOverviewSelectedData[i]=false;
    }

    if (maskTools) {
        maskTools->setAllowEditSelection(plteOverviewSelectedData&&(width>0)&&(height>0), plteOverviewSelectedData, width, height);
        if (size>0) {
            maskTools->setMaskEditing(true);
            maskTools->setEditingMode(0);
        }
    }

}

void QFRDRNumberAndBrightnessDataEditor::createWidgets() {
    tabMain=new QTabWidget(this);
    QHBoxLayout* layMain=new QHBoxLayout();
    layMain->setContentsMargins(3,3,3,3);
    setLayout(layMain);
    layMain->addWidget(tabMain);

    QGridLayout* layPlots=new QGridLayout();
    int row=0;
    QWidget* widPlots=new QWidget(this);
    widPlots->setLayout(layPlots);
    tabMain->addTab(widPlots, tr("image plots"));

    tbDisplay=new QToolBar(tr("toolbar_display"), this);
    layPlots->addWidget(tbDisplay, row, 0, 1, 2);
    row++;


    cmbColorbarN=new JKQTPMathImageColorPaletteComboBox(this);
    tbDisplay->addWidget(new QLabel(tr("particle number: "), this));
    tbDisplay->addWidget(cmbColorbarN);
    cmbColorbarB=new JKQTPMathImageColorPaletteComboBox(this);
    tbDisplay->addWidget(new QLabel(tr("brightness: "), this));
    tbDisplay->addWidget(cmbColorbarB);
    cmbColorbarOvr=new JKQTPMathImageColorPaletteComboBox(this);
    tbDisplay->addWidget(new QLabel(tr("image: "), this));
    tbDisplay->addWidget(cmbColorbarOvr);
    tbRanges=new QToolBar(tr("toolbar_ranges"), this);
    layPlots->addWidget(tbRanges, row, 0, 1, 2);
    row++;


    tbRanges->addWidget(new QLabel(tr("<b>ranges:</b>&nbsp;&nbsp;&nbsp;particle number: "), this));
    chkAutoN=new QCheckBox(tr("auto"), tbRanges);
    chkAutoN->setChecked(true);
    edtNMin=new QFDoubleEdit(tbRanges);
    edtNMin->setEnabled(false);
    edtNMin->setCheckBounds(false, false);
    connect(chkAutoN, SIGNAL(toggled(bool)), edtNMin, SLOT(setDisabled(bool)));
    edtNMax=new QFDoubleEdit(tbRanges);
    edtNMax->setEnabled(false);
    edtNMax->setCheckBounds(false, false);
    connect(chkAutoN, SIGNAL(toggled(bool)), edtNMax, SLOT(setDisabled(bool)));
    tbRanges->addWidget(edtNMin);
    tbRanges->addWidget(new QLabel(tr(" ... ")));
    tbRanges->addWidget(edtNMax);
    tbRanges->addWidget(chkAutoN);



    tbRanges->addWidget(new QLabel(tr("   particle brightness: "), this));
    chkAutoB=new QCheckBox(tr("auto"), tbRanges);
    chkAutoB->setChecked(true);
    edtBMin=new QFDoubleEdit(tbRanges);
    edtBMin->setEnabled(false);
    edtBMin->setCheckBounds(false, false);
    connect(chkAutoB, SIGNAL(toggled(bool)), edtBMin, SLOT(setDisabled(bool)));
    edtBMax=new QFDoubleEdit(tbRanges);
    edtBMax->setEnabled(false);
    edtBMax->setCheckBounds(false, false);
    connect(chkAutoB, SIGNAL(toggled(bool)), edtBMax, SLOT(setDisabled(bool)));
    tbRanges->addWidget(edtBMin);
    tbRanges->addWidget(new QLabel(tr(" ... ")));
    tbRanges->addWidget(edtBMax);
    tbRanges->addWidget(chkAutoB);



    tbRanges->addWidget(new QLabel(tr("   intensity: "), this));
    chkAutoOvr=new QCheckBox(tr("auto"), tbRanges);
    chkAutoOvr->setChecked(true);
    edtOvrMin=new QFDoubleEdit(tbRanges);
    edtOvrMin->setEnabled(false);
    edtOvrMin->setCheckBounds(false, false);
    connect(chkAutoOvr, SIGNAL(toggled(bool)), edtOvrMin, SLOT(setDisabled(bool)));
    edtOvrMax=new QFDoubleEdit(tbRanges);
    edtOvrMax->setEnabled(false);
    edtOvrMax->setCheckBounds(false, false);
    connect(chkAutoOvr, SIGNAL(toggled(bool)), edtOvrMax, SLOT(setDisabled(bool)));
    tbRanges->addWidget(edtOvrMin);
    tbRanges->addWidget(new QLabel(tr(" ... ")));
    tbRanges->addWidget(edtOvrMax);
    tbRanges->addWidget(chkAutoOvr);

    toolbar=new QToolBar(tr("toolbar_mask"), this);
    layPlots->addWidget(toolbar, row, 0, 1, 2);
    row++;


    addPlotter(pltNumber, plteNumber, plteNumberSelected, plteNumberExcluded);
    cmbColorbarN->setCurrentColorPalette(plteNumber->get_palette());
    pltNumber->get_plotter()->set_plotLabel(tr("particle number"));
    layPlots->addWidget(pltNumber, row,0);
    maskTools->registerPlotter(pltNumber);

    addPlotter(pltBrightness, plteBrightness, plteBrightnessSelected, plteBrightnessExcluded);
    pltBrightness->get_plotter()->set_plotLabel(tr("particle brightness"));
    cmbColorbarB->setCurrentColorPalette(plteBrightness->get_palette());
    layPlots->addWidget(pltBrightness, row,1);
    pltBrightness->get_plotter()->useExternalDatastore(pltNumber->getDatastore());
    maskTools->registerPlotter(pltBrightness);
    row++;

    addPlotter(pltOverview, plteOverview, plteOverviewSelected, plteOverviewExcluded);
    pltOverview->get_plotter()->set_plotLabel(tr("intensity"));
    cmbColorbarOvr->setCurrentColorPalette(plteOverview->get_palette());
    layPlots->addWidget(pltOverview, row,0);
    pltOverview->get_plotter()->useExternalDatastore(pltNumber->getDatastore());
    maskTools->registerPlotter(pltOverview);

    pltCorrelation=new QFPlotter(this);
    pltCorrelation->get_plotter()->useExternalDatastore(pltNumber->getDatastore());
    pltCorrelation->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltCorrelation->get_plotter()->set_plotLabelFontSize(10);
    pltCorrelation->set_displayMousePosition(true);
    pltCorrelation->set_displayToolbar(true);
    pltCorrelation->setXY(0,0,1,1);
    pltCorrelation->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltCorrelation->get_plotter()->getYAxis()->set_axisMinWidth(1);

    pltCorrelation->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltCorrelation->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltCorrelation->get_plotter()->getXAxis()->set_axisLabel("");
    pltCorrelation->get_plotter()->getYAxis()->set_axisLabel("");
    pltCorrelation->get_plotter()->set_useAntiAliasingForSystem(true);
    pltCorrelation->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltCorrelation->set_userActionCompositionMode(QPainter::CompositionMode_Xor);
    pltCorrelation->get_plotter()->set_plotLabel(tr("$N$-$\\epsilon$ scatter plot"));
    pltCorrelation->set_mouseActionMode(JKQtPlotter::RectangleEvents);
    connect(pltCorrelation, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(correlationRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));

    plteCorrelation=new JKQTPxyLineGraph(pltCorrelation->get_plotter());
    pltCorrelation->addGraph(plteCorrelation);

    plteRangeB=new JKQTPhorizontalRange(pltCorrelation->get_plotter());
    plteRangeB->set_visible(false);
    plteRangeB->set_unlimitedSizeMin(true);
    plteRangeB->set_unlimitedSizeMax(true);
    plteRangeB->set_invertedRange(true);
    plteRangeB->set_fillRange(true);
    plteRangeB->set_plotCenterLine(false);
    pltCorrelation->addGraph(plteRangeB);
    plteRangeN=new JKQTPverticalRange(pltCorrelation->get_plotter());
    plteRangeN->set_visible(false);
    plteRangeN->set_unlimitedSizeMin(true);
    plteRangeN->set_unlimitedSizeMax(true);
    plteRangeN->set_invertedRange(true);
    plteRangeN->set_fillRange(true);
    plteRangeN->set_plotCenterLine(false);
    pltCorrelation->addGraph(plteRangeN);


    layPlots->addWidget(pltCorrelation, row,1);
    row++;

    QGroupBox* grp=new QGroupBox(tr(" selection in correlation plot "), this);
    layPlots->addWidget(grp, row, 0, 1, 2);
    QHBoxLayout* laySelCor=new QHBoxLayout();
    grp->setLayout(laySelCor);

    chkRangeN=new QCheckBox(tr("range on N:  "), grp);
    chkRangeN->setChecked(false);
    laySelCor->addWidget(chkRangeN);
    edtCorrNMin=new QFDoubleEdit(grp);
    laySelCor->addWidget(edtCorrNMin);
    edtCorrNMin->setEnabled(false);
    edtCorrNMin->setCheckBounds(true, false);
    edtCorrNMin->setMinimum(0);
    connect(chkRangeN, SIGNAL(toggled(bool)), edtCorrNMin, SLOT(setEnabled(bool)));

    laySelCor->addWidget(new QLabel(" ... "));

    edtCorrNMax=new QFDoubleEdit(grp);
    laySelCor->addWidget(edtCorrNMax);
    edtCorrNMax->setEnabled(false);
    edtCorrNMax->setCheckBounds(true, false);
    edtCorrNMax->setMinimum(0);
    connect(chkRangeN, SIGNAL(toggled(bool)), edtCorrNMax, SLOT(setEnabled(bool)));

    laySelCor->addStretch();

    chkRangeB=new QCheckBox(tr("range on B:  "), grp);
    chkRangeB->setChecked(false);
    laySelCor->addWidget(chkRangeB);
    edtCorrBMin=new QFDoubleEdit(grp);
    laySelCor->addWidget(edtCorrBMin);
    edtCorrBMin->setEnabled(false);
    edtCorrBMin->setCheckBounds(true, false);
    edtCorrBMin->setMinimum(0);
    connect(chkRangeB, SIGNAL(toggled(bool)), edtCorrBMin, SLOT(setEnabled(bool)));


    laySelCor->addWidget(new QLabel(" ... "));

    edtCorrBMax=new QFDoubleEdit(grp);
    laySelCor->addWidget(edtCorrBMax);
    edtCorrBMax->setEnabled(false);
    edtCorrBMax->setCheckBounds(true, false);
    edtCorrBMax->setMinimum(0);
    connect(chkRangeB, SIGNAL(toggled(bool)), edtCorrBMax, SLOT(setEnabled(bool)));


    laySelCor->addStretch();

    QPushButton* btnUpdate;
    laySelCor->addWidget(btnUpdate=new QPushButton(tr("update selection"), grp));
    connect(btnUpdate, SIGNAL(clicked()), this, SLOT(updateCorrSelection()));



    toolbar->addAction(pltOverview->get_plotter()->get_actZoomAll());
    toolbar->addAction(pltOverview->get_plotter()->get_actZoomIn());
    toolbar->addAction(pltOverview->get_plotter()->get_actZoomOut());
    toolbar->addSeparator();
    maskTools->registerMaskToolsToToolbar(toolbar);
    toolbar->addSeparator();
    maskTools->registerPlotterMaskToolsToToolbar(toolbar);




















    QVBoxLayout* layHistogram=new QVBoxLayout();
    QWidget* widHistograms=new QWidget(this);
    widHistograms->setLayout(layHistogram);
    tabMain->addTab(widHistograms, tr("histograms"));

    histNumber=new QFHistogramView(this);
    histBrightness=new QFHistogramView(this);
    histIntensity=new QFHistogramView(this);
    layHistogram->addWidget(histIntensity);
    layHistogram->addWidget(histNumber);
    layHistogram->addWidget(histBrightness);















    QGridLayout* layCorrelation=new QGridLayout();
    QWidget* wwidCorrelation=new QWidget(this);
    wwidCorrelation->setLayout(layCorrelation);
    tabMain->addTab(wwidCorrelation, tr("correlations"));

    widCorrelation=new QFParameterCorrelationView(this);
    cmbCorrelationP1=new QFEnhancedComboBox(this);
    cmbCorrelationP1->addItem(tr("particle number"), "N");
    cmbCorrelationP1->addItem(tr("brightness"), "EPSILON");
    cmbCorrelationP1->addItem(tr("intensity"), "INTENSITY");
    cmbCorrelationP2=new QFEnhancedComboBox(this);
    cmbCorrelationP2->addItem(tr("brightness"), "EPSILON");
    cmbCorrelationP2->addItem(tr("particle number"), "N");
    cmbCorrelationP2->addItem(tr("intensity"), "INTENSITY");
    cmbCorrelationPCol=new QFEnhancedComboBox(this);
    cmbCorrelationPCol->addItem(tr("---"), "NONE");
    cmbCorrelationPCol->addItem(tr("intensity"), "INTENSITY");
    cmbCorrelationPCol->addItem(tr("particle number"), "N");
    cmbCorrelationPCol->addItem(tr("brightness"), "EPSILON");
    layCorrelation->addWidget(new QLabel("x-parameter: "), 0, 0);
    layCorrelation->addWidget(cmbCorrelationP1, 0, 1);
    layCorrelation->addWidget(new QLabel("y-parameter: "), 0, 2);
    layCorrelation->addWidget(cmbCorrelationP2, 0, 3);
    layCorrelation->addWidget(new QLabel("color-parameter: "), 0, 4);
    layCorrelation->addWidget(cmbCorrelationPCol, 0, 5);
    layCorrelation->addWidget(widCorrelation,1,0,1,7);
    layCorrelation->setRowStretch(1,1);
    layCorrelation->setColumnStretch(6,1);









    menuMask=propertyEditor->addMenu("&Mask", 0);
    maskTools->registerMaskToolsToMenu(menuMask);
}

void QFRDRNumberAndBrightnessDataEditor::connectAllWidgets(bool enabled) {
    if (enabled) {

        connect(edtNMin, SIGNAL(valueChanged(double)), this, SLOT(replotData()));
        connect(edtNMax, SIGNAL(valueChanged(double)), this, SLOT(replotData()));
        connect(edtBMin, SIGNAL(valueChanged(double)), this, SLOT(replotData()));
        connect(edtBMax, SIGNAL(valueChanged(double)), this, SLOT(replotData()));
        connect(edtOvrMin, SIGNAL(valueChanged(double)), this, SLOT(replotData()));
        connect(edtOvrMax, SIGNAL(valueChanged(double)), this, SLOT(replotData()));
        connect(chkAutoN, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        connect(chkAutoB, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        connect(chkAutoOvr, SIGNAL(toggled(bool)), this, SLOT(replotData()));


        connect(maskTools, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        connect(chkRangeN, SIGNAL(toggled(bool)), this, SLOT(updateCorrSelection()));
        connect(edtCorrNMin, SIGNAL(valueChanged(double)), this, SLOT(updateCorrSelection()));
        connect(edtCorrNMax, SIGNAL(valueChanged(double)), this, SLOT(updateCorrSelection()));
        connect(chkRangeB, SIGNAL(toggled(bool)), this, SLOT(updateCorrSelection()));
        connect(edtCorrBMin, SIGNAL(valueChanged(double)), this, SLOT(updateCorrSelection()));
        connect(edtCorrBMax, SIGNAL(valueChanged(double)), this, SLOT(updateCorrSelection()));
        connect(cmbCorrelationP1, SIGNAL(currentIndexChanged(int)), this, SLOT(updateHistograms()));
        connect(cmbCorrelationP2, SIGNAL(currentIndexChanged(int)), this, SLOT(updateHistograms()));
        connect(cmbCorrelationPCol, SIGNAL(currentIndexChanged(int)), this, SLOT(updateHistograms()));
        connect(cmbColorbarN, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), this, SLOT(setColorbarN(JKQTPMathImageColorPalette)));
        connect(cmbColorbarB, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), this, SLOT(setColorbarB(JKQTPMathImageColorPalette)));
        connect(cmbColorbarOvr, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), this, SLOT(setColorbarImg(JKQTPMathImageColorPalette)));
    } else {
        //
        disconnect(edtNMin, SIGNAL(valueChanged(double)), this, SLOT(replotData()));
        disconnect(edtNMax, SIGNAL(valueChanged(double)), this, SLOT(replotData()));
        disconnect(edtBMin, SIGNAL(valueChanged(double)), this, SLOT(replotData()));
        disconnect(edtBMax, SIGNAL(valueChanged(double)), this, SLOT(replotData()));
        disconnect(edtOvrMin, SIGNAL(valueChanged(double)), this, SLOT(replotData()));
        disconnect(edtOvrMax, SIGNAL(valueChanged(double)), this, SLOT(replotData()));
        disconnect(chkAutoN, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        disconnect(chkAutoB, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        disconnect(chkAutoOvr, SIGNAL(toggled(bool)), this, SLOT(replotData()));

        disconnect(maskTools, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        disconnect(chkRangeN, SIGNAL(toggled(bool)), this, SLOT(updateCorrSelection()));
        disconnect(edtCorrNMin, SIGNAL(valueChanged(double)), this, SLOT(updateCorrSelection()));
        disconnect(edtCorrNMax, SIGNAL(valueChanged(double)), this, SLOT(updateCorrSelection()));
        disconnect(chkRangeB, SIGNAL(toggled(bool)), this, SLOT(updateCorrSelection()));
        disconnect(edtCorrBMin, SIGNAL(valueChanged(double)), this, SLOT(updateCorrSelection()));
        disconnect(edtCorrBMax, SIGNAL(valueChanged(double)), this, SLOT(updateCorrSelection()));
        disconnect(cmbCorrelationP1, SIGNAL(currentIndexChanged(int)), this, SLOT(updateHistograms()));
        disconnect(cmbCorrelationP2, SIGNAL(currentIndexChanged(int)), this, SLOT(updateHistograms()));
        disconnect(cmbCorrelationPCol, SIGNAL(currentIndexChanged(int)), this, SLOT(updateHistograms()));
        disconnect(cmbColorbarN, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), this, SLOT(setColorbarN(JKQTPMathImageColorPalette)));
        disconnect(cmbColorbarB, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), this, SLOT(setColorbarB(JKQTPMathImageColorPalette)));
        disconnect(cmbColorbarOvr, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), this, SLOT(setColorbarImg(JKQTPMathImageColorPalette)));
    }
}

void QFRDRNumberAndBrightnessDataEditor::setColorbarN(JKQTPMathImageColorPalette pal)
{
    writeSettings();
    savePlotSettings();
    plteNumber->set_palette(pal);
    pltNumber->update_plot();
}

void QFRDRNumberAndBrightnessDataEditor::setColorbarB(JKQTPMathImageColorPalette pal)
{
    writeSettings();
    savePlotSettings();
    plteBrightness->set_palette(pal);
    pltBrightness->update_plot();
}

void QFRDRNumberAndBrightnessDataEditor::setColorbarImg(JKQTPMathImageColorPalette pal)
{
    writeSettings();
    savePlotSettings();
    plteOverview->set_palette(pal);
    pltOverview->update_plot();
}


void QFRDRNumberAndBrightnessDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
	// this functions connects to a new QFRawDataRecord which could also be NULL, so
	// you will have to connect and disconnect the signals! This is called whenever
	// a user navigates to the next record.
	// here is a basic skeleton:
	
    if (old) {
        savePlotSettings();
        disconnect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        connectAllWidgets(false);
        disconnect(old, 0, this, 0); // first disconnect from last record


    }
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current); // check whether we have the right QFRawDataRecord class
    if (m) {
        reallocMem(m->getWidth(), m->getHeight());
        maskTools->setRDR(current);
        m->recalcNumberAndBrightness();
        chkRangeB->setChecked(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/chkRangeB", false).toBool());
        chkRangeN->setChecked(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/chkRangeN", false).toBool());
        edtCorrNMin->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtCorrNMin", statisticsMin(m->getNumberImage(), m->getWidth()*m->getHeight())).toDouble());
        edtCorrNMax->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtCorrNMax", statisticsMax(m->getNumberImage(), m->getWidth()*m->getHeight())).toDouble());
        edtCorrBMin->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtCorrBMin", statisticsMin(m->getBrightnessImage(), m->getWidth()*m->getHeight())).toDouble());
        edtCorrBMax->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtCorrBMax", statisticsMax(m->getBrightnessImage(), m->getWidth()*m->getHeight())).toDouble());
        cmbCorrelationP1->setCurrentIndex(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationP1", 0).toInt());
        cmbCorrelationP2->setCurrentIndex(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationP2", 0).toInt());
        cmbCorrelationPCol->setCurrentIndex(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationPCol", 0).toInt());
        cmbColorbarN->setCurrentIndex(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbColorbarN", JKQTPMathImageMATLAB).toInt());
        cmbColorbarB->setCurrentIndex(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbColorbarB", JKQTPMathImageMATLAB).toInt());
        cmbColorbarOvr->setCurrentIndex(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbColorbarOvr", JKQTPMathImageGRAY).toInt());

        chkAutoN->setChecked(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/chkAutoN", true).toBool());
        chkAutoB->setChecked(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/chkAutoB", true).toBool());
        chkAutoOvr->setChecked(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/chkAutoOvr", true).toBool());
        if (!chkAutoN->isChecked()) {
            edtNMin->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtNMin", statisticsMin(m->getNumberImage(), m->getWidth()*m->getHeight())).toDouble());
            edtNMax->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtNMax", statisticsMax(m->getNumberImage(), m->getWidth()*m->getHeight())).toDouble());
        }
        if (!chkAutoB->isChecked()) {
            edtBMin->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtBMin", statisticsMin(m->getBrightnessImage(), m->getWidth()*m->getHeight())).toDouble());
            edtBMax->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtBMax", statisticsMax(m->getBrightnessImage(), m->getWidth()*m->getHeight())).toDouble());
        }
        if (!chkAutoOvr->isChecked()) {
            edtOvrMin->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtOvrMin", statisticsMin(m->getImage(), m->getWidth()*m->getHeight())).toDouble());
            edtOvrMax->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtOvrMax", statisticsMax(m->getImage(), m->getWidth()*m->getHeight())).toDouble());
        }
        // if so (and if current!=NULL anyways), connect to the new record and read some data
        connectAllWidgets(true);

        loadPlotSettings();
        replotData();
        maskTools->clearSelection();
    } else {
    }

}

void QFRDRNumberAndBrightnessDataEditor::replotData()
{
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {
        //qDebug()<<"replotData()  w="<<m->getWidth()<<"  h="<<m->getHeight();

        savePlotSettings();

        bool reOvr=pltOverview->get_doDrawing();
        bool emOvr=pltOverview->get_emitSignals();
        pltOverview->set_doDrawing(false);
        pltOverview->set_emitSignals(false);
        bool reNum=pltNumber->get_doDrawing();
        bool emNum=pltNumber->get_emitSignals();
        pltNumber->set_doDrawing(false);
        pltNumber->set_emitSignals(false);
        bool reBr=pltBrightness->get_doDrawing();
        bool emBr=pltBrightness->get_emitSignals();
        pltBrightness->set_doDrawing(false);
        pltBrightness->set_emitSignals(false);
        bool reCor=pltCorrelation->get_doDrawing();
        pltCorrelation->set_doDrawing(false);

        //reallocMem( m->getWidth(),m->getHeight());
        for (int i=0; i<m->getWidth()*m->getHeight(); i++) {
            plteOverviewData[i]=m->getImage()[i];
            plteNumberData[i]=m->getNumberImage()[i];
            plteBrightnessData[i]=m->getBrightnessImage()[i];
        }
        /*memcpy(plteOverviewData, m->getImage(), m->getWidth()*m->getHeight()*sizeof(double));
        memcpy(plteNumberData, m->getNumberImage(), m->getWidth()*m->getHeight()*sizeof(double));
        memcpy(plteBrightnessData, m->getBrightnessImage(), m->getWidth()*m->getHeight()*sizeof(double));*/


        connectAllWidgets(false);



        double w=m->getWidth();
        double h=m->getHeight();

        pltCorrelation->getDatastore()->clear();
        int cN=pltCorrelation->getDatastore()->addCopiedColumn(plteNumberData, m->getWidth()*m->getHeight(), tr("particle_number"));
        int cB=pltCorrelation->getDatastore()->addCopiedColumn(plteBrightnessData, m->getWidth()*m->getHeight(), tr("particle_brightness"));
        int cI=pltCorrelation->getDatastore()->addCopiedColumn(plteOverviewData, m->getWidth()*m->getHeight(), tr("image_data"));
        int cMN=pltCorrelation->getDatastore()->addCopiedColumnMasked(plteNumberData, m->maskGet(), m->getWidth()*m->getHeight(), tr("particle_number_masked"));
        int cMB=pltCorrelation->getDatastore()->addCopiedColumnMasked(plteBrightnessData, m->maskGet(), m->getWidth()*m->getHeight(), tr("particle_brightness_masked"));
        int cMI=pltCorrelation->getDatastore()->addCopiedColumnMasked(plteOverviewData, m->maskGet(), m->getWidth()*m->getHeight(), tr("image_data_masked"));

        pltOverview->setAbsoluteXY(0, w, 0, h);
        pltOverview->get_plotter()->set_maintainAspectRatio(true);
        pltOverview->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltOverview->get_plotter()->set_maintainAxisAspectRatio(true);
        pltOverview->get_plotter()->set_axisAspectRatio(1.0*w/h);
        plteOverview->set_datatype(JKQTPMathImageBase::DoubleArray);
        plteOverview->set_data(plteOverviewData, m->getWidth(), m->getHeight());
        plteOverview->set_width(m->getWidth());
        plteOverview->set_height(m->getHeight());
        plteOverview->set_autoImageRange(false);

        double mi,ma;
        statisticsMaskedMinMax(plteOverviewData,m->maskGet(), m->getWidth()*m->getHeight(), mi, ma);
        if (!chkAutoOvr->isChecked()) {
            /*edtOvrMin->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtOvrMin", mi).toDouble());
            edtOvrMax->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtOvrMax", ma).toDouble());*/
        } else {
            edtOvrMin->setValue(mi);
            edtOvrMax->setValue(ma);
        }
        plteOverview->set_imageMin(edtOvrMin->value());
        plteOverview->set_imageMax(edtOvrMax->value());
        plteOverview->set_palette(cmbColorbarOvr->currentColorPalette());

        plteOverviewExcluded->set_data(m->maskGet(), m->maskGetWidth(), m->maskGetHeight());
        plteOverviewExcluded->set_width(m->getWidth());
        plteOverviewExcluded->set_height(m->getHeight());
        plteOverviewSelected->set_data(plteOverviewSelectedData, m->getWidth(), m->getHeight());
        plteOverviewSelected->set_width(m->getWidth());
        plteOverviewSelected->set_height(m->getHeight());


        pltNumber->setAbsoluteXY(0, w, 0, h);
        pltNumber->get_plotter()->set_maintainAspectRatio(true);
        pltNumber->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltNumber->get_plotter()->set_maintainAxisAspectRatio(true);
        pltNumber->get_plotter()->set_axisAspectRatio(1.0*w/h);
        plteNumber->set_datatype(JKQTPMathImageBase::DoubleArray);
        plteNumber->set_data(plteNumberData, m->getWidth(), m->getHeight());
        plteNumber->set_width(m->getWidth());
        plteNumber->set_height(m->getHeight());
        plteNumber->set_autoImageRange(false);
        statisticsMaskedMinMax(plteNumberData, m->maskGet(), m->getWidth()*m->getHeight(), mi, ma);

        if (!chkAutoN->isChecked()) {
            /*edtNMin->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtNMin", mi).toDouble());
            edtNMax->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtNMax", ma).toDouble());*/
        } else {
            edtNMin->setValue(mi);
            edtNMax->setValue(ma);
        }

        plteNumber->set_imageMin(edtNMin->value());
        plteNumber->set_imageMax(edtNMax->value());
        plteNumber->set_palette(cmbColorbarN->currentColorPalette());

        plteNumberExcluded->set_data(m->maskGet(), m->maskGetWidth(), m->maskGetHeight());
        plteNumberExcluded->set_width(m->getWidth());
        plteNumberExcluded->set_height(m->getHeight());
        plteNumberSelected->set_data(plteOverviewSelectedData, m->getWidth(), m->getHeight());
        plteNumberSelected->set_width(m->getWidth());
        plteNumberSelected->set_height(m->getHeight());


        pltBrightness->setAbsoluteXY(0, w, 0, h);
        pltBrightness->get_plotter()->set_maintainAspectRatio(true);
        pltBrightness->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltBrightness->get_plotter()->set_maintainAxisAspectRatio(true);
        pltBrightness->get_plotter()->set_axisAspectRatio(1.0*w/h);
        plteBrightness->set_datatype(JKQTPMathImageBase::DoubleArray);
        plteBrightness->set_data(plteBrightnessData, m->getWidth(), m->getHeight());
        plteBrightness->set_width(m->getWidth());
        plteBrightness->set_height(m->getHeight());
        plteBrightness->set_autoImageRange(false);
        statisticsMaskedMinMax(plteBrightnessData,m->maskGet(), m->getWidth()*m->getHeight(), mi, ma);

        if (!chkAutoB->isChecked()) {
            /*edtBMin->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtBMin", mi).toDouble());
            edtBMax->setValue(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/edtBMax", ma).toDouble());*/
        } else {
            edtBMin->setValue(mi);
            edtBMax->setValue(ma);
        }
        plteBrightness->set_imageMin(edtBMin->value());
        plteBrightness->set_imageMax(edtBMax->value());
        plteBrightness->set_palette(cmbColorbarB->currentColorPalette());

        plteBrightnessExcluded->set_data(m->maskGet(), m->maskGetWidth(), m->maskGetHeight());
        plteBrightnessExcluded->set_width(m->getWidth());
        plteBrightnessExcluded->set_height(m->getHeight());
        plteBrightnessSelected->set_data(plteOverviewSelectedData, m->getWidth(), m->getHeight());
        plteBrightnessSelected->set_width(m->getWidth());
        plteBrightnessSelected->set_height(m->getHeight());

        pltNumber->zoomToFit();
        pltBrightness->zoomToFit();
        pltOverview->zoomToFit();

        plteCorrelation->set_xColumn(cMN);
        plteCorrelation->set_yColumn(cMB);
        plteCorrelation->set_drawLine(false);
        plteCorrelation->set_symbol(JKQTPdot);
        plteCorrelation->set_color(QColor("red"));
        pltCorrelation->getXAxis()->set_axisLabel(tr("particle number $N$"));
        pltCorrelation->getYAxis()->set_axisLabel(tr("particle brightness $\\epsilon$"));
        pltCorrelation->get_plotter()->set_showKey(false);
        pltCorrelation->zoomToFit();

        updateHistograms();

        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/chkAutoN", chkAutoN->isChecked(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/chkAutoB", chkAutoB->isChecked(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/chkAutoOvr", chkAutoOvr->isChecked(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/edtNMin", edtNMin->value(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/edtNMax", edtNMax->value(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/edtBMin", edtBMin->value(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/edtBMax", edtBMax->value(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/edtOvrMin", edtOvrMin->value(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/edtOvrMax", edtOvrMax->value(), false, false);

        connectAllWidgets(true);

        if (reOvr) {
            pltOverview->set_doDrawing(true);
            pltOverview->update_plot();
        }
        pltOverview->set_emitSignals(emOvr);
        if (reNum) {
            pltNumber->set_doDrawing(true);
            pltNumber->update_plot();
        }
        pltNumber->set_emitSignals(emNum);
        if (reBr) {
            pltBrightness->set_doDrawing(true);
            pltBrightness->update_plot();
        }
        pltBrightness->set_emitSignals(emBr);
        if (reCor) {
            pltCorrelation->set_doDrawing(true);
            pltCorrelation->update_plot();
        }

    }

}



void QFRDRNumberAndBrightnessDataEditor::updateHistograms()
{
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {

        savePlotSettings();


        int dataSize=m->getWidth()*m->getHeight();
        int dataSelSize=m->getWidth()*m->getHeight();

        double* dataEps=(double*)qfCalloc(dataSize, sizeof(double));
        double* dataEpsSel=(double*)qfCalloc(dataSelSize, sizeof(double));
        double* dataN=(double*)qfCalloc(dataSize, sizeof(double));
        double* dataNSel=(double*)qfCalloc(dataSelSize, sizeof(double));

        double* dataOvr=(double*)qfCalloc(dataSize, sizeof(double));
        double* dataOvrSel=(double*)qfCalloc(dataSelSize, sizeof(double));
        dataSize=0;
        dataSelSize=0;
        for (int i=0; i<m->getWidth()*m->getHeight(); i++) {
            if (!m->maskGetIdx(i)) {
                dataN[dataSize]=plteNumberData[i];
                dataEps[dataSize]=plteBrightnessData[i];
                dataOvr[dataSize]=plteOverviewData[i];
                dataSize++;
            }
            if (!m->maskGetIdx(i) && plteOverviewSelectedData[i]) {
                dataNSel[dataSelSize]=plteNumberData[i];
                dataEpsSel[dataSelSize]=plteBrightnessData[i];
                dataOvrSel[dataSelSize]=plteOverviewData[i];
                dataSelSize++;
            }
        }

        histNumber->clear();
        histNumber->setHistogramXLabel(tr("particle number $N$"));
        histNumber->addHistogram("complete", dataN, dataSize, false);

        histBrightness->clear();
        histBrightness->setHistogramXLabel(tr("particle brightness $\\epsilon$"));
        histBrightness->addHistogram("complete", dataEps, dataSize, false);

        histIntensity->clear();
        histIntensity->setHistogramXLabel(tr("image intensity $I$"));
        histIntensity->addHistogram("complete", dataOvr, dataSize, false);

        if (dataSelSize>1) histNumber->addHistogram("selection", dataNSel, dataSelSize, false);
        if (dataSelSize>1) histBrightness->addHistogram("selection", dataEpsSel, dataSelSize, false);
        if (dataSelSize>1) histIntensity->addHistogram("selection", dataOvrSel, dataSelSize, false);
        double* c1=NULL;
        double* c2=NULL;
        double* cC=NULL;

        widCorrelation->clear();

        if (cmbCorrelationP1->currentIndex()==0) {
            c1=dataN;
            widCorrelation->setCorrelation1Label(tr("particle number $N$"));
        } else if (cmbCorrelationP1->currentIndex()==1) {
            c1=dataEps;
            widCorrelation->setCorrelation1Label(tr("brightness $\\epsilon$"));
        } else if (cmbCorrelationP1->currentIndex()==2) {
            widCorrelation->setCorrelation1Label(tr("intensity $I$"));
            c1=dataOvr;
        }
        if (cmbCorrelationP2->currentIndex()==0) {
            c2=dataEps;
            widCorrelation->setCorrelation2Label(tr("brightness $\\epsilon$"));
        } else if (cmbCorrelationP2->currentIndex()==1) {
            c2=dataN;
            widCorrelation->setCorrelation2Label(tr("particle number $N$"));
        } else if (cmbCorrelationP2->currentIndex()==2) {
            widCorrelation->setCorrelation2Label(tr("intensity $I$"));
            c2=dataOvr;
        }

        if (cmbCorrelationPCol->currentIndex()==1) {
            widCorrelation->setCorrelationColorLabel(tr("intensity $I$"));
            cC=dataOvr;
        } else if (cmbCorrelationPCol->currentIndex()==2) {
            cC=dataN;
            widCorrelation->setCorrelationColorLabel(tr("particle number $N$"));
        } else if (cmbCorrelationPCol->currentIndex()==3) {
            cC=dataEps;
            widCorrelation->setCorrelationColorLabel(tr("brightness $\\epsilon$"));
        }
        widCorrelation->addCorrelation(tr("complete"), c1, c2, cC, dataSize, false);

        qfFree(dataN);
        qfFree(dataNSel);
        qfFree(dataEps);
        qfFree(dataEpsSel);
        qfFree(dataOvr);
        qfFree(dataOvrSel);



        histNumber->updateHistogram(true);
        histBrightness->updateHistogram(true);
        histIntensity->updateHistogram(true);
        widCorrelation->updateCorrelation(true);
    }
}

void QFRDRNumberAndBrightnessDataEditor::rawDataChanged() {
    replotData();
}

void QFRDRNumberAndBrightnessDataEditor::readSettings() {
	// read widget settings
    if (!settings) return;
    /*histNumber->readSettings(*(settings->getQSettings()), "QFRDRNumberAndBrightnessDataEditor/histNumber/");
    histBrightness->readSettings(*(settings->getQSettings()), "QFRDRNumberAndBrightnessDataEditor/histBrightness/");
    histIntensity->readSettings(*(settings->getQSettings()), "QFRDRNumberAndBrightnessDataEditor/histIntensity/");
    widCorrelation->readSettings(*(settings->getQSettings()), "QFRDRNumberAndBrightnessDataEditor/widCorrelation/");

    cmbCorrelationP1->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationP1", 0).toInt());
    cmbCorrelationP2->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationP2", 0).toInt());
    cmbCorrelationPCol->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationPCol", 0).toInt());
    cmbColorbarN->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRNumberAndBrightnessDataEditor/cmbColorbarN", JKQTPMathImageMATLAB).toInt());
    cmbColorbarB->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRNumberAndBrightnessDataEditor/cmbColorbarB", JKQTPMathImageMATLAB).toInt());
    cmbColorbarOvr->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRNumberAndBrightnessDataEditor/cmbColorbarOvr", JKQTPMathImageGRAY).toInt());*/

}


void QFRDRNumberAndBrightnessDataEditor::writeSettings() {
	// rwrite widget settings
    if (!settings) return;
    histNumber->writeSettings(*(settings->getQSettings()), "QFRDRNumberAndBrightnessDataEditor/histNumber/");
    histBrightness->writeSettings(*(settings->getQSettings()), "QFRDRNumberAndBrightnessDataEditor/histBrightness/");
    histIntensity->writeSettings(*(settings->getQSettings()), "QFRDRNumberAndBrightnessDataEditor/histIntensity/");
    widCorrelation->writeSettings(*(settings->getQSettings()), "QFRDRNumberAndBrightnessDataEditor/widCorrelation/");

    ProgramOptions::setConfigValue("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationP1", cmbCorrelationP1->currentIndex());
    ProgramOptions::setConfigValue("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationP2", cmbCorrelationP2->currentIndex());
    ProgramOptions::setConfigValue("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationPCol", cmbCorrelationPCol->currentIndex());
    ProgramOptions::setConfigValue("QFRDRNumberAndBrightnessDataEditor/cmbColorbarN", cmbColorbarN->currentIndex());
    ProgramOptions::setConfigValue("QFRDRNumberAndBrightnessDataEditor/cmbColorbarB", cmbColorbarB->currentIndex());
    ProgramOptions::setConfigValue("QFRDRNumberAndBrightnessDataEditor/cmbColorbarOvr", cmbColorbarOvr->currentIndex());
}

void QFRDRNumberAndBrightnessDataEditor::loadPlotSettings() {
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {
        histNumber->readQFProperties(m, QString("NANDB%1_HISTNUMBER_").arg(peID), "", "");
        histBrightness->readQFProperties(m, QString("NANDB%1_HISTBRIGHTNESS_").arg(peID), "", "");
        histIntensity->readQFProperties(m, QString("NANDB%1_HISTINTENSITY_").arg(peID), "", "");
        widCorrelation->readQFProperties(m, QString("NANDB%1_CORRPLOT_").arg(peID), "", cmbCorrelationP1->currentData().toString()+"_"+cmbCorrelationP2->currentData().toString()+"_"+cmbCorrelationPCol->currentData().toString());

        cmbCorrelationP1->setCurrentIndex(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationP1", 0).toInt());
        cmbCorrelationP2->setCurrentIndex(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationP2", 0).toInt());
        cmbCorrelationPCol->setCurrentIndex(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationPCol", 0).toInt());
        cmbColorbarN->setCurrentIndex(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbColorbarN", JKQTPMathImageMATLAB).toInt());
        cmbColorbarB->setCurrentIndex(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbColorbarB", JKQTPMathImageMATLAB).toInt());
        cmbColorbarOvr->setCurrentIndex(m->getQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbColorbarOvr", JKQTPMathImageGRAY).toInt());
    }
}

void QFRDRNumberAndBrightnessDataEditor::savePlotSettings() {
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {
        histNumber->writeQFProperties(m, QString("NANDB%1_HISTNUMBER_").arg(peID), "", "");
        histBrightness->writeQFProperties(m, QString("NANDB%1_HISTBRIGHTNESS_").arg(peID), "", "");
        histIntensity->writeQFProperties(m, QString("NANDB%1_HISTINTENSITY_").arg(peID), "", "");
        widCorrelation->writeQFProperties(m, QString("NANDB%1_CORRPLOT_").arg(peID), "", cmbCorrelationP1->currentData().toString()+"_"+cmbCorrelationP2->currentData().toString()+"_"+cmbCorrelationPCol->currentData().toString());


        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationP1", cmbCorrelationP1->currentIndex(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationP2", cmbCorrelationP2->currentIndex(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbCorrelationPCol", cmbCorrelationPCol->currentIndex(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbColorbarN", cmbColorbarN->currentIndex(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbColorbarB", cmbColorbarB->currentIndex(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/cmbColorbarOvr", cmbColorbarOvr->currentIndex(), false, false);
    }
}




void QFRDRNumberAndBrightnessDataEditor::imageZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender) {
    if (sender==pltOverview) {
        pltNumber->setXY(newxmin, newxmax, newymin, newymax);
        pltBrightness->setXY(newxmin, newxmax, newymin, newymax);
    } else if (sender==pltNumber) {
        pltOverview->setXY(newxmin, newxmax, newymin, newymax);
        pltBrightness->setXY(newxmin, newxmax, newymin, newymax);
    } else if (sender==pltBrightness) {
        pltOverview->setXY(newxmin, newxmax, newymin, newymax);
        pltNumber->setXY(newxmin, newxmax, newymin, newymax);
    }
}

void QFRDRNumberAndBrightnessDataEditor::updateCorrSelection()
{
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {
        bool reOvr=pltOverview->get_doDrawing();
        bool emOvr=pltOverview->get_emitSignals();
        pltOverview->set_doDrawing(false);
        pltOverview->set_emitSignals(false);
        bool reNum=pltNumber->get_doDrawing();
        bool emNum=pltNumber->get_emitSignals();
        pltNumber->set_doDrawing(false);
        pltNumber->set_emitSignals(false);
        bool reBr=pltBrightness->get_doDrawing();
        bool emBr=pltBrightness->get_emitSignals();
        pltBrightness->set_doDrawing(false);
        pltBrightness->set_emitSignals(false);
        bool reCor=pltCorrelation->get_doDrawing();
        bool emCor=pltBrightness->get_emitSignals();
        pltCorrelation->set_doDrawing(false);
        pltCorrelation->set_emitSignals(false);

        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/chkRangeB", chkRangeB->isChecked(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/chkRangeN", chkRangeN->isChecked(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/edtCorrNMin", edtCorrNMin->value(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/edtCorrNMax", edtCorrNMax->value(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/edtCorrBMin", edtCorrBMin->value(), false, false);
        m->setQFProperty("QFRDRNumberAndBrightnessDataEditor/edtCorrBMax", edtCorrBMax->value(), false, false);




        plteRangeB->set_visible(chkRangeB->isChecked());
        plteRangeB->set_rangeMin(edtCorrBMin->value());
        plteRangeB->set_rangeMax(edtCorrBMax->value());
        plteRangeN->set_visible(chkRangeN->isChecked());
        plteRangeN->set_rangeMin(edtCorrNMin->value());
        plteRangeN->set_rangeMax(edtCorrNMax->value());

        //if (chkRangeB->isChecked() || chkRangeN->isChecked()) {
            double* B=m->getBrightnessImage();
            double* N=m->getNumberImage();
            for (int i=0; i<m->getWidth()*m->getHeight(); i++) {
                plteOverviewSelectedData[i]=false;
                if (chkRangeB->isChecked() && !chkRangeN->isChecked()) {
                    if (B[i]>=edtCorrBMin->value() && B[i]<=edtCorrBMax->value()) {
                        plteOverviewSelectedData[i]=true;
                    }
                } else if (chkRangeN->isChecked() && !chkRangeB->isChecked()) {
                    if (N[i]>=edtCorrNMin->value() && N[i]<=edtCorrNMax->value()) {
                        plteOverviewSelectedData[i]=true;
                    }
                } else if (chkRangeN->isChecked() && chkRangeB->isChecked()) {
                    if (N[i]>=edtCorrNMin->value() && N[i]<=edtCorrNMax->value() && B[i]>=edtCorrBMin->value() && B[i]<=edtCorrBMax->value()) {
                        plteOverviewSelectedData[i]=true;
                    }
                }
            }
        //}

        if (reOvr) {
            pltOverview->set_doDrawing(true);
            pltOverview->update_plot();
        }
        pltOverview->set_emitSignals(emOvr);
        if (reNum) {
            pltNumber->set_doDrawing(true);
            pltNumber->update_plot();
        }
        pltNumber->set_emitSignals(emNum);
        if (reBr) {
            pltBrightness->set_doDrawing(true);
            pltBrightness->update_plot();
        }
        pltBrightness->set_emitSignals(emBr);
        if (reCor) {
            pltCorrelation->set_doDrawing(true);
            pltCorrelation->update_plot();
        }
        pltBrightness->set_emitSignals(emCor);
    }
}

void QFRDRNumberAndBrightnessDataEditor::correlationRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers)
{
    connectAllWidgets(false);
    edtCorrNMin->setValue(qMin(x, x+width));
    edtCorrNMax->setValue(qMax(x, x+width));
    edtCorrBMin->setValue(qMin(y, y+height));
    edtCorrBMax->setValue(qMax(y, y+height));
    if (!chkRangeB->isChecked() && !chkRangeN->isChecked()) {
        if (QMessageBox::question(this, tr("Number & Brightness"), tr("You selected a rectangle in the correlation plot, but the selection tool is not activated.\nIf thsi feature is activated, thepixels within the selected range will be selected in the N- and epsilon-images.\n\nActivate now?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
            chkRangeB->setChecked(true);
            chkRangeN->setChecked(true);
        }
    }
    connectAllWidgets(true);
    updateCorrSelection();
    updateHistograms();
}
