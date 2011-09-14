#include "qfrdrimagingfcs_dataeditor.h"
#include "qfrdrimagingfcs_data.h"
#include <QDebug>
#include <math.h>
#include "jkqttools.h"
#include "qffitfunction.h"
#include "qffitfunctionmanager.h"






QFRDRImagingFCSImageEditor::QFRDRImagingFCSImageEditor(QFPluginServices* services, QWidget* parent):
    QFRawDataEditor(services, parent)
{
    m_fitFunctions=services->getFitFunctionManager()->getModels("", this);
    plteOverviewSelectedData=NULL;
    plteOverviewExcludedData=NULL;
    plteOverviewSize=0;
    plteImageData=NULL;
    plteGofImageData=NULL;
    plteImageSize=0;
    createWidgets();
    //QTimer::singleShot(500, this, SLOT(debugInfo()));
}

QFRDRImagingFCSImageEditor::~QFRDRImagingFCSImageEditor()
{
    if (plteOverviewSelectedData) free(plteOverviewSelectedData);
    plteOverviewSelectedData=NULL;
    if (plteOverviewExcludedData) free(plteOverviewExcludedData);
    plteOverviewExcludedData=NULL;
    if (plteImageData) free(plteImageData);
    plteImageData=NULL;
    if (plteGofImageData) free(plteGofImageData);
    plteGofImageData=NULL;
}




void QFRDRImagingFCSImageEditor::createWidgets() {
    QLabel* l;
    QGridLayout* lb=new QGridLayout(this);
    setLayout(lb);



    QGroupBox* grpTop=new QGroupBox(tr(" Parameter Image Plot Settings "), this);
    QGridLayout* topgrid=new QGridLayout(this);
    grpTop->setLayout(topgrid);

    int row=0;
    cmbResultGroup=new QComboBox(this);
    topgrid->addWidget((l=new QLabel(tr("&result set:"))), row, 0);
    l->setBuddy(cmbResultGroup);
    topgrid->addWidget(cmbResultGroup, row, 1);

    cmbParameter=new QComboBox(this);
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
    cmbGofParameter=new QComboBox(this);
    topgrid->addWidget((labGofParameter=new QLabel(tr("&goodness of fit:"))), row, 3);
    labGofParameter->setBuddy(cmbGofParameter);
    topgrid->addWidget(cmbGofParameter, row, 4);
    cmbGofParameterTransform=new QComboBox(this);
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/none.png"), tr("none"));
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/abs.png"), tr("abs"));
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/log.png"), tr("log"));
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/reci.png"), tr("reciprocal"));
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/sqrt.png"), tr("sqrt"));
    topgrid->addWidget((labGofParameterTransform=new QLabel(tr("    tr&ansform:"))), row, 5);
    labGofParameterTransform->setBuddy(cmbGofParameterTransform);
    topgrid->addWidget(cmbGofParameterTransform, row, 6);



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


    QWidget* w=new QWidget(this);
    QVBoxLayout* vbl=new QVBoxLayout();
    w->setLayout(vbl);

    QGroupBox* wimg=new QGroupBox(tr(" image plot styles "), this);
    vbl->addWidget(wimg);
    QFormLayout* gli=new QFormLayout(this);
    wimg->setLayout(gli);
    cmbColorbar=new QComboBox(wimg);
    QStringList sl=JKQTFPimagePlot_getPalettes();
    for (int i=0; i<sl.size(); i++) {
        cmbColorbar->addItem(JKQTFPimagePlot_getPaletteIcon(i), sl[i]);
    }
    gli->addRow(tr("color &palette:"), cmbColorbar);

    chkImageAutoScale=new QCheckBox("auto", wimg);
    gli->addRow(tr("color &range:"), chkImageAutoScale);

    edtColMin=new JKDoubleEdit(wimg);
    edtColMin->setCheckBounds(false, false);
    edtColMax=new JKDoubleEdit(wimg);
    edtColMax->setCheckBounds(false, false);
    QHBoxLayout* coll=new QHBoxLayout();
    coll->addWidget(edtColMin,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtColMax,1);
    coll->setContentsMargins(0,0,0,0);
    gli->addRow(QString(""), coll);

    chkDisplayImageOverlay=new QCheckBox(wimg);
    gli->addRow(tr("show &overlays:"), chkDisplayImageOverlay);





    connectParameterWidgets();

    QGroupBox* wcp=new QGroupBox(tr(" correlation plot styles "), this);
    vbl->addWidget(wcp);
    QFormLayout* gl=new QFormLayout(this);
    wcp->setLayout(gl);
    chkDisplayAverage=new QCheckBox(w);
    chkDisplayAverage->setChecked(true);
    gl->addRow(tr("display &average:"), chkDisplayAverage);

    cmbAverageStyle=new QComboBox(w);
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_lines.png"), tr("lines"));
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_points.png"), tr("points"));
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_pointslines.png"), tr("lines+points"));

    cmbAverageErrorStyle=new QComboBox(w);
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_enone.png"), tr("none"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_ebars.png"), tr("bars"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("lines"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elinesbars.png"), tr("lines+bars"));

    gl->addRow((l=new QLabel(tr("average &options:"))), cmbAverageStyle);
    gl->addRow("", cmbAverageErrorStyle);
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), cmbAverageStyle, SLOT(setEnabled(bool)));
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), cmbAverageErrorStyle, SLOT(setEnabled(bool)));
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), l, SLOT(setEnabled(bool)));

    cmbRunStyle=new QComboBox(w);
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_lines.png"), tr("lines"));
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_points.png"), tr("points"));
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_pointslines.png"), tr("lines+points"));

    cmbRunErrorStyle=new QComboBox(w);
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_enone.png"), tr("none"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_ebars.png"), tr("bars"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("lines"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elinesbars.png"), tr("lines+bars"));

    gl->addRow((labRunOptions=new QLabel(tr("run &options:"), w)), cmbRunStyle);
    labRunOptions->setBuddy(cmbRunStyle);
    gl->addRow("", cmbRunErrorStyle);

    chkLogTauAxis=new QCheckBox("", w);
    gl->addRow((l=new QLabel(tr("log &tau;-axis:"))), chkLogTauAxis);
    l->setTextFormat(Qt::RichText);

    chkDisplayResiduals=new QCheckBox(w);
    chkDisplayResiduals->setChecked(true);
    gl->addRow(tr("display &residuals:"), chkDisplayResiduals);

    chkKeys=new QCheckBox(w);
    chkKeys->setChecked(true);
    gl->addRow(tr("display &keys:"), chkKeys);

    connectImageWidgets(true);



    /*btnDontUse=new QPushButton(tr("exclude sel."), w);
    connect(btnDontUse, SIGNAL(clicked()), this, SLOT(excludeRuns()));
    gl->addWidget(btnDontUse, row, 1);
    btnUse=new QPushButton(tr("include sel."), w);
    connect(btnUse, SIGNAL(clicked()), this, SLOT(includeRuns()));
    gl->addWidget(btnUse, row, 2);
    row++;*/




    QWidget* wpltOverview=new QWidget(this);
    QVBoxLayout* lpltOverview=new QVBoxLayout();
    wpltOverview->setLayout(lpltOverview);
    pltOverview=new JKQTFastPlotter(wpltOverview);
    lpltOverview->addWidget(new QLabel(tr("Overview:")));
    lpltOverview->addWidget(pltOverview, 1);
    pltOverview->set_aspectRatio(1);
    pltOverview->set_maintainAspectRatio(true);
    pltOverview->set_xAxisLabelVisible(false);
    pltOverview->set_yAxisLabelVisible(false);
    pltOverview->set_tickFontSize(6);
    pltOverview->set_tickFontName(font().family());
    QFont fo=font();
    fo.setPointSizeF(pltOverview->get_tickFontSize());
    QFontMetricsF fm(fo);
    pltOverview->set_plotBorderBottom(2.0*fm.ascent());
    pltOverview->set_plotBorderLeft(fm.width("00000"));
    pltOverview->set_drawGrid(false);
    connect(pltOverview, SIGNAL(clicked(double,double,Qt::KeyboardModifiers)), this,SLOT(previewClicked(double,double,Qt::KeyboardModifiers)));

    plteOverview=new JKQTFPimagePlot(pltOverview, NULL, JKQTFP_uint16, 1, 1, 0, 1, 0, 1, JKQTFP_GRAY);
    pltOverview->addPlot(plteOverview);


    QColor ovlSelCol=QColor("green");
    ovlSelCol.setAlphaF(0.5);
    plteOverviewSelected=new JKQTFPimageOverlayPlot(pltOverview, NULL, 1, 1, 0, 1, 0, 1, ovlSelCol);
    pltOverview->addPlot(plteOverviewSelected);

    QColor ovlExCol=QColor("darkgrey");
    ovlExCol.setAlphaF(0.5);
    plteOverviewExcluded=new JKQTFPimageOverlayPlot(pltOverview, NULL, 1, 1, 0, 1, 0, 1, ovlExCol);
    pltOverview->addPlot(plteOverviewExcluded);














    QWidget* wpltImage=new QWidget(this);
    QVBoxLayout* lpltImage=new QVBoxLayout();
    wpltImage->setLayout(lpltImage);
    pltImage=new JKQtPlotter(wpltImage);
    lpltImage->addWidget((labParamImage=new QLabel(tr("Parameter Image:"))));
    lpltImage->addWidget(pltImage, 1);

    pltImage->set_zoomByDoubleAndRightMouseClick(false);
    pltImage->set_displayMousePosition(false);
    pltImage->set_displayToolbar(false);
    pltImage->get_plotter()->set_maintainAspectRatio(true);
    pltImage->get_plotter()->set_aspectRatio(1);
    pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
    pltImage->get_plotter()->set_axisAspectRatio(1);
    pltImage->setXY(0,0,1,1);
    pltImage->setAbsoluteXY(0,1,0,1);

    pltImage->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltImage->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltImage->get_plotter()->getXAxis()->set_axisLabel("");
    pltImage->get_plotter()->getYAxis()->set_axisLabel("");
    pltImage->get_plotter()->setGrid(false);
    pltImage->get_plotter()->set_useAntiAliasingForSystem(true);
    pltImage->get_plotter()->set_useAntiAliasingForGraphs(true);
    connect(pltImage, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));

    connect(pltImage, SIGNAL(plotMouseClicked(double,double,Qt::KeyboardModifiers,Qt::MouseButton)), this, SLOT(previewClicked(double,double,Qt::KeyboardModifiers)));

    plteImage=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0, JKQTPMathImage::GRAY, pltImage->get_plotter());
    pltImage->addGraph(plteImage);


    plteImageSelected=new JKQTPOverlayImage(0,0,1,1,NULL, 0, 0, ovlSelCol, pltImage->get_plotter());
    //pltImage->addGraph(plteImageSelected);


    plteImageExcluded=new JKQTPOverlayImage(0,0,1,1,NULL, 0, 0, ovlExCol, pltImage->get_plotter());
    //pltImage->addGraph(plteImageExcluded);

    plteImageData=NULL;














    QWidget* wpltGofImage=new QWidget(this);
    QVBoxLayout* lpltGofImage=new QVBoxLayout();
    wpltGofImage->setLayout(lpltGofImage);
    pltGofImage=new JKQtPlotter(wpltGofImage);
    lpltGofImage->addWidget((labParamImage=new QLabel(tr("\"Goodnes of Fit\" Image:"))));
    lpltGofImage->addWidget(pltGofImage, 1);

    pltGofImage->set_zoomByDoubleAndRightMouseClick(false);
    pltGofImage->set_displayMousePosition(false);
    pltGofImage->set_displayToolbar(false);
    pltGofImage->get_plotter()->set_maintainAspectRatio(true);
    pltGofImage->get_plotter()->set_aspectRatio(1);
    pltGofImage->get_plotter()->set_maintainAxisAspectRatio(true);
    pltGofImage->get_plotter()->set_axisAspectRatio(1);
    pltGofImage->setXY(0,0,1,1);
    pltGofImage->setAbsoluteXY(0,1,0,1);

    pltGofImage->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltGofImage->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltGofImage->get_plotter()->getXAxis()->set_axisLabel("");
    pltGofImage->get_plotter()->getYAxis()->set_axisLabel("");
    pltGofImage->get_plotter()->setGrid(false);
    pltGofImage->get_plotter()->set_useAntiAliasingForSystem(true);
    pltGofImage->get_plotter()->set_useAntiAliasingForGraphs(true);
    connect(pltGofImage, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));

    connect(pltGofImage, SIGNAL(plotMouseClicked(double,double,Qt::KeyboardModifiers,Qt::MouseButton)), this, SLOT(previewClicked(double,double,Qt::KeyboardModifiers)));

    plteGofImage=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0, JKQTPMathImage::GRAY, pltGofImage->get_plotter());
    pltGofImage->addGraph(plteGofImage);


    plteGofImageSelected=new JKQTPOverlayImage(0,0,1,1,NULL, 0, 0, ovlSelCol, pltGofImage->get_plotter());
    plteGofImageExcluded=new JKQTPOverlayImage(0,0,1,1,NULL, 0, 0, ovlExCol, pltGofImage->get_plotter());

    plteImageData=NULL;








    plotter = new JKQtPlotter(true, this);
    plotter->get_plotter()->set_showKey(true);
    plotterResid = new JKQtPlotter(true, this);
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
    plotter->getXAxis()->set_axisLabel("");
    plotter->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    plotter->getYAxis()->set_labelFontSize(11);
    plotter->getYAxis()->set_tickLabelFontSize(10);
    plotter->getXAxis()->set_drawMode1(JKQTPCADMticks);
    plotter->getXAxis()->set_tickLabelFontSize(10);
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
    plotter->get_plotter()->set_keyXMargin(2);
    plotter->get_plotter()->set_keyYMargin(2);
    plotterResid->get_plotter()->set_keyFontSize(9);
    plotterResid->get_plotter()->set_keyXMargin(2);
    plotterResid->get_plotter()->set_keyYMargin(2);
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
    tabFitvals->setReadonly(true);



    splitterTopBot=new QVisibleHandleSplitter(Qt::Vertical, this);
    splitterTop=new QVisibleHandleSplitter(Qt::Horizontal, this);
    splitterTop->addWidget(wpltImage);
    splitterTop->addWidget(wpltGofImage);
    splitterTop->addWidget(wpltOverview);
    splitterTopBot->addWidget(splitterTop);
    splitterBotPlots=new QVisibleHandleSplitter(Qt::Vertical, this);
    splitterBotPlots->addWidget(plotter);
    splitterBotPlots->addWidget(wp);
    splitterBot=new QVisibleHandleSplitter(Qt::Horizontal, this);
    splitterBot->addWidget(splitterBotPlots);
    splitterBot->addWidget(tvParams);
    splitterTopBot->addWidget(splitterBot);

    vbl->addWidget(new QWidget(this), 1);

    lb->addWidget(grpTop, 0, 0, 1, 2);
    lb->addWidget(splitterTopBot, 1, 0);
    lb->addWidget(w, 1, 1);
    lb->setColumnStretch(0,5);

};

void QFRDRImagingFCSImageEditor::saveImageSettings() {
    if (current){
        QString egroup=filenameize(currentEvalGroup());
        QString param=filenameize(currentFitParameter());
        if (egroup.isEmpty() || param.isEmpty()) {
            writeSettings();
        } else {
            current->setQFProperty(QString("imfcs_imed_colorbar_%1_%2").arg(egroup).arg(param), cmbColorbar->currentIndex(), false, false);
            current->setQFProperty(QString("imfcs_imed_overlay_%1_%2").arg(egroup).arg(param), chkDisplayImageOverlay->isChecked(), false, false);
            current->setQFProperty(QString("imfcs_imed_autorange_%1_%2").arg(egroup).arg(param), chkImageAutoScale->isChecked(), false, false);
            current->setQFProperty(QString("imfcs_imed_colmin_%1_%2").arg(egroup).arg(param), edtColMin->value(), false, false);
            current->setQFProperty(QString("imfcs_imed_colmax_%1_%2").arg(egroup).arg(param), edtColMax->value(), false, false);
        }

    }
}

void QFRDRImagingFCSImageEditor::loadImageSettings() {
    if (current){
        QString egroup=filenameize(currentEvalGroup());
        QString param=filenameize(currentFitParameter());
        if (egroup.isEmpty() || param.isEmpty()) {
            writeSettings();
        } else if (settings) {
            double mi=0, ma=1;
            connectParameterWidgets(false);
            plteImage->getDataMinMax(mi, ma);
            int d=current->getProperty(QString("imfcs_imed_colorbar_%1_%2").arg(egroup).arg(param),
                                       settings->getQSettings()->value(QString("imfcsimageeditor/colorbar"), cmbColorbar->currentIndex())).toInt();
            if (d>=0) cmbColorbar->setCurrentIndex(d);
            else if (cmbColorbar->count()>0) cmbColorbar->setCurrentIndex(0);
            chkDisplayImageOverlay->setChecked(current->getProperty(QString("imfcs_imed_overlay_%1_%2").arg(egroup).arg(param), false).toBool());
            chkImageAutoScale->setChecked(current->getProperty(QString("imfcs_imed_autorange_%1_%2").arg(egroup).arg(param), true).toBool());
            edtColMin->setValue(current->getProperty(QString("imfcs_imed_colmin_%1_%2").arg(egroup).arg(param), mi).toDouble());
            edtColMax->setValue(current->getProperty(QString("imfcs_imed_colmax_%1_%2").arg(egroup).arg(param), ma).toDouble());
            connectParameterWidgets(true);
            pltImage->update_plot();
        }

    }
}

void QFRDRImagingFCSImageEditor::displayOverlayChanged() {
    if (!chkDisplayImageOverlay->isChecked()) {
        pltImage->deleteGraph(plteImageSelected, false);
        pltImage->deleteGraph(plteImageExcluded, false);
        pltGofImage->deleteGraph(plteImageSelected, false);
        pltGofImage->deleteGraph(plteImageExcluded, false);
    } else {
        pltImage->addGraph(plteImageSelected);
        pltImage->addGraph(plteImageExcluded);
        pltGofImage->addGraph(plteImageSelected);
        pltGofImage->addGraph(plteImageExcluded);
    }
    saveImageSettings();
    //pltImage->update_data();
}

void QFRDRImagingFCSImageEditor::paletteChanged() {
    plteImage->set_palette(cmbColorbar->currentIndex());
    plteImage->set_autoImageRange(chkImageAutoScale->isChecked());
    edtColMin->setEnabled(!chkImageAutoScale->isChecked());
    edtColMax->setEnabled(!chkImageAutoScale->isChecked());
    if (!chkImageAutoScale->isChecked()) {
        plteImage->set_imageMin(edtColMin->value());
        plteImage->set_imageMax(edtColMax->value());
    } else {
        double mi=0, ma=0;
        plteImage->getDataMinMax(mi, ma);
        edtColMin->setValue(mi);
        edtColMax->setValue(ma);
    }
    saveImageSettings();
    pltImage->update_plot();
}

void QFRDRImagingFCSImageEditor::excludeRuns() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        QSetIterator<int32_t> i(selected);
        while (i.hasNext()) {
             int32_t run=i.next();
             m->leaveoutAddRun(run);
        }
        m->recalcCorrelations();
    }
    replotData();
}

void QFRDRImagingFCSImageEditor::includeRuns() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        QSetIterator<int32_t> i(selected);
        while (i.hasNext()) {
             int32_t run=i.next();
             m->leaveoutRemoveRun(run);
        }
        m->recalcCorrelations();
    }
    replotData();
}

void QFRDRImagingFCSImageEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) disconnect(old, 0, this, 0);
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        connect(current, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        sliders->disableSliderSignals();
        sliders->set_min(0);
        sliders->set_max(m->getCorrelationN());
        sliders->set_userMin(current->getProperty("imfcs_imed_datacut_min", 0).toInt());
        sliders->set_userMax(current->getProperty("imfcs_imed_datacut_max", m->getCorrelationN()).toInt());
        sliders->enableSliderSignals();
        selected.clear();
        selected.insert(0);
    } else {
//        runs.setCurrent(current);
        selected.clear();
    }

    fillParameterSet();

    //readSettings();
    //qDebug()<<"connectWidgets ...  done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
};


void QFRDRImagingFCSImageEditor::previewClicked(double x, double y, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;

    int xx=(int)floor(x);
    int yy=(int)floor(y);

    int idx=m->xyToRun(xx, yy);

    if (modifiers==Qt::ControlModifier) {
        if (selected.contains(idx)) selected.remove(idx);
        else selected.insert(idx);
    } else {
        selected.clear();
        selected.insert(idx);
    }
    replotData();
}

void QFRDRImagingFCSImageEditor::rawDataChanged() {
    replotImage();
    replotData();
};

void QFRDRImagingFCSImageEditor::resultsChanged() {
    QString egroup=currentEvalGroup();
    QString fp=currentFitParameter();
    QString gfp=currentGofParameter();
    fillParameterSet();
    int grp=cmbResultGroup->findData(egroup);
    if (grp>=0) {
        cmbResultGroup->setCurrentIndex(grp);
        int p=cmbParameter->findData(fp);
        if (p>=0) {
            cmbParameter->setCurrentIndex(p);
        } else if (cmbParameter->count()>0) {
            cmbParameter->setCurrentIndex(0);
        }

        p=cmbGofParameter->findData(gfp);
        if (p>=0) {
            cmbGofParameter->setCurrentIndex(p);
        } else if (cmbGofParameter->count()>0) {
            cmbGofParameter->setCurrentIndex(0);
        }
    } else if (cmbResultGroup->count()>0) {
        cmbResultGroup->setCurrentIndex(0);
    }
};

void QFRDRImagingFCSImageEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!current) return;
    current->setQFProperty("imfcs_imed_datacut_min", userMin, false, false);
    current->setQFProperty("imfcs_imed_datacut_max", userMax, false, false);
    //replotData();

    JKQTPdatastore* ds=plotter->getDatastore();
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) {
        plotter->clearGraphs();
        plotterResid->clearGraphs();
        ds->clear();
        return;
    }

    plotter->set_doDrawing(false);
    plotter->set_emitSignals(false);
    plotterResid->set_doDrawing(false);
    plotterResid->set_emitSignals(false);
    sliders->set_min(0);
    sliders->set_max(m->getCorrelationN());
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    plotter->get_plotter()->setGraphsDataRange(sliders->get_userMin(), sliders->get_userMax());
    plotterResid->get_plotter()->setGraphsDataRange(sliders->get_userMin(), sliders->get_userMax());
    plotter->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
    plotter->zoomToFit(true, true, !chkLogTauAxis->isChecked(),false);
    plotterResid->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
    plotterResid->setX(plotter->getXMin(), plotter->getXMax());
    plotterResid->zoomToFit(false, true, !chkLogTauAxis->isChecked(),false);

    QApplication::restoreOverrideCursor();
    plotter->set_doDrawing(true);
    plotter->set_emitSignals(true);
    plotter->update_plot();
    plotterResid->set_doDrawing(true);
    plotterResid->set_emitSignals(true);
    plotterResid->update_plot();

}

void QFRDRImagingFCSImageEditor::replotImage() {
    //qDebug()<<"replotImage";
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    pltImage->set_doDrawing(false);
    pltGofImage->set_doDrawing(false);

    if (!chkDisplayImageOverlay->isChecked()) {
        pltImage->deleteGraph(plteImageSelected, false);
        pltImage->deleteGraph(plteImageExcluded, false);
        pltGofImage->deleteGraph(plteImageSelected, false);
        pltGofImage->deleteGraph(plteImageExcluded, false);
    } else {
        pltImage->addGraph(plteImageSelected);
        pltImage->addGraph(plteImageExcluded);
        pltGofImage->addGraph(plteImageSelected);
        pltGofImage->addGraph(plteImageExcluded);
    }

    if (!m) {
        plteImage->set_data(NULL, 0, 0, JKQTPMathImageBase::DoubleArray);
        plteImageSelected->set_data(NULL, 0, 0);
        plteImageExcluded->set_data(NULL, 0, 0);
        plteGofImage->set_data(NULL, 0, 0, JKQTPMathImageBase::DoubleArray);
        plteGofImageSelected->set_data(NULL, 0, 0);
        plteGofImageExcluded->set_data(NULL, 0, 0);
        //qDebug()<<"replotImage !m";
    } else {
        double w=m->getDataImageWidth();
        double h=m->getDataImageHeight();
        if ((w==0) || (h==0)) {
            w=h=1;
        }
        //qDebug()<<w<<h;
        double dx=1;
        if (w>1) dx=pow(10.0,floor(log(w)/log(10.0)));
        double dy=1;
        if (h>1) dy=pow(10.0,floor(log(h)/log(10.0)));

        pltImage->setAbsoluteXY(0, w, 0, h);
        pltImage->get_plotter()->set_maintainAspectRatio(true);
        pltImage->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
        pltImage->get_plotter()->set_axisAspectRatio(1*w/h);
        pltGofImage->setAbsoluteXY(0, w, 0, h);
        pltGofImage->get_plotter()->set_maintainAspectRatio(true);
        pltGofImage->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltGofImage->get_plotter()->set_maintainAxisAspectRatio(true);
        pltGofImage->get_plotter()->set_axisAspectRatio(1*w/h);

        if (w>3*h) {
            pltImage->get_plotter()->getXAxis()->set_minTicks(3);
            plteImage->get_colorBarAxis()->set_minTicks(3);
            pltGofImage->get_plotter()->getXAxis()->set_minTicks(3);
            plteGofImage->get_colorBarAxis()->set_minTicks(3);
        } else {
            pltImage->get_plotter()->getXAxis()->set_minTicks(7);
            plteImage->get_colorBarAxis()->set_minTicks(5);
            pltGofImage->get_plotter()->getXAxis()->set_minTicks(7);
            plteGofImage->get_colorBarAxis()->set_minTicks(5);
        }
        pltImage->setXY(0, w, 0, h);
        pltGofImage->setXY(0, w, 0, h);

        if (plteImageSize<m->getDataImageWidth()*m->getDataImageHeight()) {
            plteImageSize=m->getDataImageWidth()*m->getDataImageHeight();
            plteImageData=(double*)realloc(plteImageData, plteImageSize*sizeof(double));
            plteGofImageData=(double*)realloc(plteGofImageData, plteImageSize*sizeof(double));
        }

        readParameterImage(plteImageData, plteGofImageData, m->getDataImageWidth(), m->getDataImageHeight(), currentEvalGroup(), currentFitParameter(), currentFitParameterTransfrom(), currentGofParameter(), currentGofParameterTransfrom());

        plteImage->set_data(plteImageData, m->getDataImageWidth(), m->getDataImageHeight(), JKQTPMathImageBase::DoubleArray);
        plteImage->set_width(w);
        plteImage->set_height(h);
        plteImage->set_palette(cmbColorbar->currentIndex());

        plteGofImage->set_data(plteGofImageData, m->getDataImageWidth(), m->getDataImageHeight(), JKQTPMathImageBase::DoubleArray);
        plteGofImage->set_width(w);
        plteGofImage->set_height(h);

    }

    pltImage->set_doDrawing(true);
    pltImage->update_plot();
    pltGofImage->set_doDrawing(true);
    pltGofImage->update_plot();
    QApplication::restoreOverrideCursor();
    //qDebug()<<"replotImage ...  done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
}

void QFRDRImagingFCSImageEditor::replotOverview() {
    //qDebug()<<"replotOverview";
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    pltOverview->set_doDrawing(false);

    if (!m) {
        plteOverview->set_image(NULL, JKQTFP_uint16, 1, 1);
        plteOverviewSelected->set_data(NULL, 1, 1);
        plteOverviewExcluded->set_data(NULL, 1, 1);
    } else {
        uint16_t* ov=m->getDataImagePreview();
        double w=m->getDataImageWidth();
        double h=m->getDataImageHeight();
        double dx=1;
        if (w>1) dx=pow(10.0,floor(log(w)/log(10.0)));
        double dy=1;
        if (h>1) dy=pow(10.0,floor(log(h)/log(10.0)));

        pltOverview->setXRange(0,w);
        pltOverview->setYRange(0,h);
        pltOverview->set_xTickDistance(dx);
        pltOverview->set_yTickDistance(dy);
        pltOverview->set_aspectRatio(qMax(0.01, qMin(100.0, w/h)));

        if (plteOverviewSize<m->getDataImageWidth()*m->getDataImageHeight()) {
            plteOverviewSize=m->getDataImageWidth()*m->getDataImageHeight();
            plteOverviewSelectedData=(bool*)realloc(plteOverviewSelectedData, plteOverviewSize*sizeof(bool));
            plteOverviewExcludedData=(bool*)realloc(plteOverviewExcludedData, plteOverviewSize*sizeof(bool));
        }

        plteOverviewSelected->set_data(plteOverviewSelectedData, m->getDataImageWidth(), m->getDataImageHeight());
        plteOverviewSelected->set_xmax(w);
        plteOverviewSelected->set_ymax(h);

        plteImageSelected->set_data(plteOverviewSelectedData, m->getDataImageWidth(), m->getDataImageHeight());
        plteImageSelected->set_width(w);
        plteImageSelected->set_height(h);

        plteOverviewExcluded->set_data(plteOverviewExcludedData, m->getDataImageWidth(), m->getDataImageHeight());
        plteOverviewExcluded->set_xmax(w);
        plteOverviewExcluded->set_ymax(h);

        plteImageExcluded->set_data(plteOverviewExcludedData, m->getDataImageWidth(), m->getDataImageHeight());
        plteImageExcluded->set_width(w);
        plteImageExcluded->set_height(h);

        plteOverview->set_image(ov, JKQTFP_uint16, m->getDataImageWidth(), m->getDataImageHeight());
        plteOverview->set_xmax(w);
        plteOverview->set_ymax(h);
        if (plteOverviewSelectedData) {
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                int x=m->runToX(i);
                int y=m->runToY(i);
                int idx=y*m->getDataImageWidth()+x;
                plteOverviewSelectedData[idx]=selected.contains(i);
                plteOverviewExcludedData[idx]=m->leaveoutRun(i);
            }
        }
    }

    pltOverview->set_doDrawing(true);
    pltOverview->update_data();
    QApplication::restoreOverrideCursor();
    //qDebug()<<"replotOverview ... done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
}

void QFRDRImagingFCSImageEditor::replotData() {
    //qDebug()<<"replotData";
    JKQTPdatastore* ds=plotter->getDatastore();
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);

    plotterResid->setVisible(chkDisplayResiduals->isChecked());

    if (!m) {
        plotter->clearGraphs();
        plotterResid->clearGraphs();
        ds->clear();
        //qDebug()<<"replotData ... done (!m) ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();;
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    plotter->set_doDrawing(false);
    plotter->set_emitSignals(false);
    sliders->set_min(0);
    sliders->set_max(m->getCorrelationN());
    plotter->clearGraphs();
    plotterResid->clearGraphs();
    plotter->get_plotter()->set_showKey(chkKeys->isChecked());
    plotterResid->get_plotter()->set_showKey(chkKeys->isChecked());
    ds->clear();


    if (m->getCorrelationN()>0) {


        JKQTPerrorPlotstyle runerrorstyle=JKQTPnoError;
        switch (cmbRunErrorStyle->currentIndex()) {
            case 1: runerrorstyle=JKQTPerrorBars; break;
            case 2: runerrorstyle=JKQTPerrorLines; break;
            case 3: runerrorstyle=JKQTPerrorBarsLines; break;
        }
        bool runLine=(cmbRunStyle->currentIndex()!=1);
        JKQTPgraphSymbols runSymbol=JKQTPcross;
        if (cmbRunStyle->currentIndex()==0) runSymbol=JKQTPnoSymbol;

        JKQTPerrorPlotstyle avgerrorstyle=JKQTPnoError;
        switch (cmbAverageErrorStyle->currentIndex()) {
            case 1: avgerrorstyle=JKQTPerrorBars; break;
            case 2: avgerrorstyle=JKQTPerrorLines; break;
            case 3: avgerrorstyle=JKQTPerrorBarsLines; break;
        }
        bool avgLine=(cmbAverageStyle->currentIndex()!=1);
        JKQTPgraphSymbols avgSymbol=JKQTPcross;
        if (cmbAverageStyle->currentIndex()==0) avgSymbol=JKQTPnoSymbol;



        size_t c_tau=ds->addColumn(m->getCorrelationT(), m->getCorrelationN(), "tau");


        //QTime t;
        //t.start();
        //////////////////////////////////////////////////////////////////////////////////
        // Plot average + error markers
        //////////////////////////////////////////////////////////////////////////////////
        if (chkDisplayAverage->isChecked()) {
            size_t c_mean=ds->addColumn(m->getCorrelationMean(), m->getCorrelationN(), "cmean");
            size_t c_std=ds->addColumn(m->getCorrelationStdDev(), m->getCorrelationN(), "cstddev");

            JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plotter->get_plotter());
            //g->set_color(QColor("blue"));
            g->set_errorColor(g->get_color().lighter());
            g->set_lineWidth(2);
            g->set_xColumn(c_tau);
            g->set_yColumn(c_mean);
            g->set_yErrorColumn(c_std);
            g->set_title(tr("average"));
            g->set_yErrorStyle(avgerrorstyle);
            g->set_xErrorStyle(JKQTPnoError);
            g->set_datarange_start(sliders->get_userMin());
            g->set_datarange_end(sliders->get_userMax());
            g->set_drawLine(avgLine);
            g->set_symbolSize(5);
            g->set_symbol(avgSymbol);
            plotter->addGraph(g);
        }

       //////////////////////////////////////////////////////////////////////////////////
       // Plot ALL RUNS (left out runs in gray)
       // also evaluates and plots the fit function (if possible) + the residuals
       //////////////////////////////////////////////////////////////////////////////////
        tvParams->setModel(NULL);
        tabFitvals->setReadonly(false);
        tabFitvals->clear();
        tabFitvals->appendColumn();
        tabFitvals->setColumnTitle(0, tr("parameter"));
        for (int i=0; i<m->getCorrelationRuns(); i++) {
            if (selected.contains(i)) {
                size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("run %1 %2").arg(i).arg(m->getCorrelationRunName(i)).toStdString());
                size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("run error %1 %2").arg(i).arg(m->getCorrelationRunName(i)).toStdString());
                JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plotter->get_plotter());
                g->set_lineWidth(1);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_run);
                g->set_drawLine(runLine);
                g->set_symbol(runSymbol);
                g->set_title(tr("run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));
                g->set_datarange_start(sliders->get_userMin());
                g->set_datarange_end(sliders->get_userMax());
                g->set_yErrorColumn(c_rune);
                g->set_yErrorStyle(runerrorstyle);
                g->set_xErrorStyle(JKQTPnoError);
                g->set_symbolSize(5);
                g->set_errorWidth(1);
                if (!m->leaveoutRun(i)) {
                    //g->set_color(QColor("darkgreen"));
                } else {
                    g->set_color(QColor("grey"));
                }
                g->set_errorColor(g->get_color().lighter());
                plotter->addGraph(g);


                double* corr=(double*)calloc(m->getCorrelationN(), sizeof(double));
                double* resid=(double*)calloc(m->getCorrelationN(), sizeof(double));
                QStringList names, units, unitlabels, namelabels;
                QList<double> values, errors;
                QList<bool> fix;

                // search for the evaluationID that matches the current group(has to be in evals)  and run (i)
                QStringList evals=current->resultsCalcEvaluationsInGroup(currentEvalGroup());
                QString en="";
                for (register int ev=0; ev<evals.size(); ev++) {
                    //en=evals[i];
                    if (current->resultsGetEvaluationGroupIndex(evals[ev])==i) {
                        en=evals[ev];
                        break;
                    }
                }

                // try to evaluate the fit function. If it succeeds, add plots and store the parameters & description to the display model!
                if (evaluateFitFunction(m->getCorrelationT(), corr, m->getCorrelationN(), names, namelabels, values, errors, fix, units, unitlabels, en)) {
                    double* acf=m->getCorrelationRun(i);
                    for (int nn=0; nn< m->getCorrelationN(); nn++) {
                        resid[nn]=corr[nn]-acf[nn];
                    }
                    size_t c_fit=ds->addCopiedColumn(corr, m->getCorrelationN(), QString("fit to run %1 %2").arg(i).arg(m->getCorrelationRunName(i)).toStdString());
                    size_t c_resid=ds->addCopiedColumn(resid, m->getCorrelationN(), QString("residuals for run %1 %2").arg(i).arg(m->getCorrelationRunName(i)).toStdString());
                    JKQTPxyLineGraph* gfit=new JKQTPxyLineGraph();
                    gfit->set_lineWidth(1.5);
                    gfit->set_xColumn(c_tau);
                    gfit->set_yColumn(c_fit);
                    gfit->set_drawLine(true);
                    gfit->set_style(Qt::DotLine);
                    gfit->set_symbol(JKQTPnoSymbol);
                    gfit->set_title(tr("fit to run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));
                    gfit->set_datarange_start(sliders->get_userMin());
                    gfit->set_datarange_end(sliders->get_userMax());
                    gfit->set_symbolSize(5);
                    gfit->set_color(g->get_color());
                    plotter->addGraph(gfit);
                    JKQTPxyLineGraph* gr=new JKQTPxyLineGraph();
                    gr->set_lineWidth(1.5);
                    gr->set_xColumn(c_tau);
                    gr->set_yColumn(c_resid);
                    gr->set_drawLine(true);
                    gr->set_style(Qt::DotLine);
                    gr->set_symbol(JKQTPnoSymbol);
                    gr->set_title(tr("residuals for run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));
                    gr->set_datarange_start(sliders->get_userMin());
                    gr->set_datarange_end(sliders->get_userMax());
                    gr->set_symbolSize(5);
                    gr->set_color(g->get_color());
                    plotterResid->addGraph(gr);

                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, tr("run %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                    int col=tabFitvals->columnCount()-1;
                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, tr("error"));
                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, QString(""));
                    for (int ii=0; ii<names.size(); ii++) {
                        int row=tabFitvals->getAddRow(0, namelabels[ii]);
                        tabFitvals->setCellEditRole(row, 0, names[ii]);
                        tabFitvals->setCell(row, col, values[ii]);
                        tabFitvals->setCell(row, col+1, errors[ii]);
                        tabFitvals->setCell(row, col+2, unitlabels[ii]);
                        tabFitvals->setCellEditRole(row, col+2, units[ii]);
                        QBrush c=QBrush(g->get_color().lighter(170));
                        tabFitvals->setCellBackgroundRole(row, col, c);
                        tabFitvals->setCellBackgroundRole(row, col+1, c);
                        tabFitvals->setCellBackgroundRole(row, col+2, c);
                        if (fix[ii]) tabFitvals->setCellCheckedRole(row, col, Qt::Checked);
                        else tabFitvals->setCellCheckedRole(row, col, Qt::Unchecked);
                    }

                }

                free(corr);
                free(resid);
            }
        }
        tabFitvals->setReadonly(true);
        tvParams->setModel(tabFitvals);
        tvParams->resizeColumnsToContents();





        plotter->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
        plotter->zoomToFit(true, true, !chkLogTauAxis->isChecked(),false);
        plotterResid->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
        plotterResid->setX(plotter->getXMin(), plotter->getXMax());
        plotterResid->zoomToFit(false, true, !chkLogTauAxis->isChecked(),false);

    }
    plotter->set_doDrawing(true);
    plotter->set_emitSignals(true);
    //QTime t;
    //t.start();
    plotter->update_plot();
    //qDebug()<<"plotting in "<<t.elapsed()<<" ms";
    QApplication::restoreOverrideCursor();
    //qDebug()<<"replotData ... done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
};


void QFRDRImagingFCSImageEditor::readSettings() {
    if (!settings) return;
    plotter->loadSettings(*(settings->getQSettings()), QString("imfcsimageeditor/corrplot"));
    chkLogTauAxis->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/log_tau_axis"), true).toBool());
    chkKeys->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_keys"), false).toBool());
    chkDisplayResiduals->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_resid"), true).toBool());
    chkDisplayAverage->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_avg"), true).toBool());
    cmbAverageStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/avg_style"), 0).toInt());
    cmbAverageErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/avg_error_style"), 2).toInt());
    cmbColorbar->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/colorbar"), 0).toInt());
    chkDisplayImageOverlay->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/image_overlays"), false).toBool());
    cmbRunStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/run_style"), 1).toInt());
    cmbRunErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/run_error_style"), 1).toInt());
    loadSplitter(*(settings->getQSettings()), splitterTop, "imfcsimageeditor/splittertopSizes");
    loadSplitter(*(settings->getQSettings()), splitterTopBot, "imfcsimageeditor/splittertopbotSizes");
    loadSplitter(*(settings->getQSettings()), splitterBot, "imfcsimageeditor/splitterbotSizes");
    loadSplitter(*(settings->getQSettings()), splitterBotPlots, "imfcsimageeditor/splitterbotplotsSizes");
};


void QFRDRImagingFCSImageEditor::writeSettings() {
    if (!settings) return;
    settings->getQSettings()->setValue(QString("imfcsimageeditor/autoscale"), chkImageAutoScale->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/colmin"), edtColMin->value());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/colmax"), edtColMax->value());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/log_tau_axis"), chkLogTauAxis->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_keys"), chkKeys->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/image_overlays"), chkDisplayImageOverlay->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_resid"), chkDisplayResiduals->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_avg"), chkDisplayAverage->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/colorbar"), cmbColorbar->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/avg_style"), cmbAverageStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/avg_error_style"), cmbAverageErrorStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/run_style"), cmbRunStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/run_error_style"), cmbRunErrorStyle->currentIndex());
    saveSplitter(*(settings->getQSettings()), splitterTop, "imfcsimageeditor/splittertopSizes");
    saveSplitter(*(settings->getQSettings()), splitterBot, "imfcsimageeditor/splitterbotSizes");
    saveSplitter(*(settings->getQSettings()), splitterBotPlots, "imfcsimageeditor/splitterbotplotsSizes");
    saveSplitter(*(settings->getQSettings()), splitterTopBot, "imfcsimageeditor/splittertopbotSizes");
};


void QFRDRImagingFCSImageEditor::parameterSetChanged() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    int grp=cmbResultGroup->currentIndex();
    if ((!m)||(grp<0)) {
        labParameter->setEnabled(false);
        cmbParameter->setEnabled(false);
        labParameterTransform->setEnabled(false);
        cmbParameterTransform->setEnabled(false);
        labGofParameter->setEnabled(false);
        cmbGofParameter->setEnabled(false);
        labGofParameterTransform->setEnabled(false);
        cmbGofParameterTransform->setEnabled(false);
        clearImage();
        cmbParameter->clear();
        cmbGofParameter->clear();
        //cmbParameter->setCurrentIndex(-1);
    } else {
        connectParameterWidgets(false);
        m->setQFProperty("imfcs_imed_evalgroup", grp, false, false);
        labParameter->setEnabled(true);
        cmbParameter->setEnabled(true);
        labParameterTransform->setEnabled(true);
        cmbParameterTransform->setEnabled(true);
        labGofParameter->setEnabled(true);
        cmbGofParameter->setEnabled(true);
        labGofParameterTransform->setEnabled(true);
        cmbGofParameterTransform->setEnabled(true);
        //QStringList egroup=m->resultsCalcEvalGroups();
        QString egroup=currentEvalGroup();
        QList<QPair<QString, QString> > params=m->resultsCalcNamesAndLabels("", "fit results", egroup);
        cmbParameter->clear();
        cmbGofParameter->clear();
        for (int i=0; i<params.size(); i++) {
            cmbParameter->addItem(tr("fit result: %1").arg(params[i].first), params[i].second);
            cmbGofParameter->addItem(tr("fit result: %1").arg(params[i].first), params[i].second);
        }
        params=m->resultsCalcNamesAndLabels("", "fit properties", egroup);
        for (int i=0; i<params.size(); i++) {
            cmbParameter->addItem(tr("fit prop.: %1").arg(params[i].first), params[i].second);
            cmbGofParameter->insertItem(0, tr("fit prop.: %1").arg(params[i].first), params[i].second);
        }
        int d=cmbParameter->findData(current->getProperty(QString("imfcs_imed_param_%1").arg(filenameize(egroup)), ""));
        if (d>=0) cmbParameter->setCurrentIndex(d);
        d=cmbGofParameter->findData(current->getProperty(QString("imfcs_imed_gofparam_%1").arg(filenameize(egroup)), "fitalg_error_sum"));
        if (d>=0) cmbGofParameter->setCurrentIndex(d);
        cmbGofParameterTransform->setCurrentIndex(current->getProperty(QString("imfcs_imed_gofparamtrans_%1_%2").arg(filenameize(egroup)).arg(cmbGofParameter->currentIndex()), 0).toInt());
        cmbParameterTransform->setCurrentIndex(current->getProperty(QString("imfcs_imed_paramtrans_%1_%2").arg(filenameize(egroup)).arg(cmbParameter->currentIndex()), 0).toInt());
        connectParameterWidgets(true);

    }
    //qDebug()<<"parameterSetChanged ... done   cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
    parameterChanged();
    replotData();
    //qDebug()<<"parameterSetChanged ... done   cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
}

void QFRDRImagingFCSImageEditor::parameterChanged() {
    if (!current) return;
    QString egroup=currentEvalGroup();

    connectParameterWidgets(false);
    cmbGofParameterTransform->setCurrentIndex(current->getProperty(QString("imfcs_imed_gofparamtrans_%1_%2").arg(filenameize(egroup)).arg(cmbGofParameter->currentIndex()), 0).toInt());
    cmbParameterTransform->setCurrentIndex(current->getProperty(QString("imfcs_imed_paramtrans_%1_%2").arg(filenameize(egroup)).arg(cmbParameter->currentIndex()), 0).toInt());

    current->setQFProperty(QString("imfcs_imed_param_%1").arg(filenameize(egroup)), cmbParameter->itemData(cmbParameter->currentIndex()).toString(), false, false);
    current->setQFProperty(QString("imfcs_imed_gofparam_%1").arg(filenameize(egroup)), cmbGofParameter->itemData(cmbGofParameter->currentIndex()).toString(), false, false);
    connectParameterWidgets(true);

    transformChanged();
}

void QFRDRImagingFCSImageEditor::transformChanged() {
    QString egroup=currentEvalGroup();
    current->setQFProperty(QString("imfcs_imed_gofparamtrans_%1_%2").arg(filenameize(egroup)).arg(cmbGofParameter->currentIndex()), cmbGofParameterTransform->currentIndex(), false, false);
    current->setQFProperty(QString("imfcs_imed_paramtrans_%1_%2").arg(filenameize(egroup)).arg(cmbParameter->currentIndex()), cmbParameterTransform->currentIndex(), false, false);
    connectParameterWidgets(false);
    replotImage();
    loadImageSettings();
    paletteChanged();
    displayOverlayChanged();
    connectParameterWidgets(true);
}


void QFRDRImagingFCSImageEditor::clearImage() {
    if (plteImageData) free(plteImageData);
    plteImageData=NULL;
    plteImageSize=0;
    if (plteGofImageData) free(plteGofImageData);
    plteGofImageData=NULL;
}

void QFRDRImagingFCSImageEditor::fillParameterSet() {
    //qDebug()<<"fillParameterSet";

    if (!current) {
        //cmbResultGroup->setEnabled(false);
        cmbResultGroup->clear();
        //cmbResultGroup->setCurrentIndex(-1);
    } else {
        connectParameterWidgets(false);
        //cmbResultGroup->setEnabled(true);
        cmbResultGroup->clear();

        // fill the list of available fit result groups
        QStringList egroups=current->resultsCalcEvalGroups();
        //qDebug()<<"egroups="<<egroups;
        for (int i=0; i<egroups.size(); i++) {
            cmbResultGroup->addItem(QString("%1").arg(current->resultsGetLabelForEvaluationGroup(egroups[i])), egroups[i]);
            //qDebug()<<"egroup["<<i<<"/"<<cmbResultGroup->count()<<"]: "<<egroups[i];
        }
        int d=cmbResultGroup->findData(current->getProperty("imfcs_imed_evalgroup", "").toString());
        if (d>=0) cmbResultGroup->setCurrentIndex(d);
        else if (cmbResultGroup->count()>0) cmbResultGroup->setCurrentIndex(0);
        connectParameterWidgets(true);

    }
    // update the list of available fit parameters
    parameterSetChanged(); // this also replots !!!
    //qDebug()<<"fillParameterSet ... done   cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();;

}

void QFRDRImagingFCSImageEditor::debugInfo() {
    //qDebug()<<"fillParameterSet ... done   cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();;
    QTimer::singleShot(500, this, SLOT(debugInfo()));
}

void QFRDRImagingFCSImageEditor::connectImageWidgets(bool connectTo) {
    if (connectTo) {
        connect(chkDisplayAverage, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        connect(cmbAverageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        connect(cmbAverageErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        connect(cmbRunStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        connect(cmbRunErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        connect(chkLogTauAxis, SIGNAL(clicked()), this, SLOT(replotData()));
        connect(chkDisplayResiduals, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        connect(chkKeys, SIGNAL(toggled(bool)), this, SLOT(replotData()));
    } else {
        disconnect(chkDisplayAverage, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        disconnect(cmbAverageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        disconnect(cmbAverageErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        disconnect(cmbRunStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        disconnect(cmbRunErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        disconnect(chkLogTauAxis, SIGNAL(clicked()), this, SLOT(replotData()));
        disconnect(chkDisplayResiduals, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        disconnect(chkKeys, SIGNAL(toggled(bool)), this, SLOT(replotData()));
    }
}

void QFRDRImagingFCSImageEditor::connectParameterWidgets(bool connectTo) {
    if (connectTo) {
        connect(cmbResultGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterSetChanged()));
        connect(cmbParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
        connect(cmbGofParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
        connect(cmbParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
        connect(cmbGofParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
        connect(cmbColorbar, SIGNAL(currentIndexChanged(int)), this, SLOT(paletteChanged()));
        connect(chkImageAutoScale, SIGNAL(toggled(bool)), this, SLOT(paletteChanged()));
        connect(edtColMin, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
        connect(edtColMax, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
        connect(chkDisplayImageOverlay, SIGNAL(toggled(bool)), this, SLOT(displayOverlayChanged()));
    } else {
        disconnect(cmbResultGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterSetChanged()));
        disconnect(cmbParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
        disconnect(cmbGofParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
        disconnect(cmbParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
        disconnect(cmbGofParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
        disconnect(cmbColorbar, SIGNAL(currentIndexChanged(int)), this, SLOT(paletteChanged()));
        disconnect(chkImageAutoScale, SIGNAL(toggled(bool)), this, SLOT(paletteChanged()));
        disconnect(edtColMin, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
        disconnect(edtColMax, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
        disconnect(chkDisplayImageOverlay, SIGNAL(toggled(bool)), this, SLOT(displayOverlayChanged()));
    }

    //qDebug()<<"connectParameterWidgets ...  done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();

}

void QFRDRImagingFCSImageEditor::imageZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender) {
    if (sender==pltImage) {
        pltOverview->setXYRange(newxmin, newxmax, newymin, newymax);
        pltGofImage->setXY(newxmin, newxmax, newymin, newymax);
    } else if (sender==pltGofImage) {
            pltOverview->setXYRange(newxmin, newxmax, newymin, newymax);
            pltImage->setXY(newxmin, newxmax, newymin, newymax);
    }
}
void QFRDRImagingFCSImageEditor::acfZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender) {
    if (sender==plotter) {
        plotterResid->setX(newxmin, newxmax);
        //qDebug()<<newxmin << newxmax;
    }
}




QString QFRDRImagingFCSImageEditor::currentEvalGroup() const {
    int grp=cmbResultGroup->currentIndex();
    if (grp<0) return "";
    return cmbResultGroup->itemData(grp).toString();

}

QString QFRDRImagingFCSImageEditor::currentFitParameter() const {
    if (cmbParameter->currentIndex()<0) return "";
    return cmbParameter->itemData(cmbParameter->currentIndex()).toString();
}

QString QFRDRImagingFCSImageEditor::currentGofParameter() const {
    if (cmbGofParameter->currentIndex()<0) return "";
    return cmbGofParameter->itemData(cmbGofParameter->currentIndex()).toString();
}

QFRDRImagingFCSImageEditor::ImageTransforms QFRDRImagingFCSImageEditor::currentFitParameterTransfrom() const {
    //return QFRDRImagingFCSImageEditor::itNone;
    if (cmbParameterTransform->currentIndex()<0) return QFRDRImagingFCSImageEditor::itNone;
    return (QFRDRImagingFCSImageEditor::ImageTransforms)cmbParameterTransform->currentIndex();

}

QFRDRImagingFCSImageEditor::ImageTransforms QFRDRImagingFCSImageEditor::currentGofParameterTransfrom() const {
    //return QFRDRImagingFCSImageEditor::itSqrt;
    if (cmbGofParameterTransform->currentIndex()<0) return QFRDRImagingFCSImageEditor::itNone;
    return (QFRDRImagingFCSImageEditor::ImageTransforms)cmbGofParameterTransform->currentIndex();
}






void QFRDRImagingFCSImageEditor::readParameterImage(double* image, double* gof_image, uint16_t width, uint16_t height, QString evalGroup, QString fitParam, QFRDRImagingFCSImageEditor::ImageTransforms tranFitParam, QString gofParam, QFRDRImagingFCSImageEditor::ImageTransforms tranGofParam) {
    uint16_t arraysize=width*height;
    for (register uint16_t i=0; i<arraysize; i++) {
        image[i]=NAN;
        gof_image[i]=0;
    }
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if ( (!m) || evalGroup.isEmpty() || fitParam.isEmpty() ) return;

    QStringList evals=current->resultsCalcEvaluationsInGroup(evalGroup);
    for (register int i=0; i<evals.size(); i++) {
        const QString& en=evals[i];
        int grpIdx=current->resultsGetEvaluationGroupIndex(en);
        int x=m->runToX(grpIdx);
        int y=m->runToY(grpIdx);
        if (x>=0 && x<width && y>=0 && y<height) {
            if (current->resultsExists(en, fitParam)) {
                image[y*width+x]=current->resultsGetAsDouble(en, fitParam);
                switch(tranFitParam) {
                    case QFRDRImagingFCSImageEditor::itAbs: {
                            image[y*width+x]=fabs(image[y*width+x]);
                        } break;
                    case QFRDRImagingFCSImageEditor::itLog: {
                            if (image[y*width+x]>0) image[y*width+x]=log(image[y*width+x])/log(10.0);
                            else  image[y*width+x]=NAN;

                        } break;
                    case QFRDRImagingFCSImageEditor::itReciprocal: {
                            if (image[y*width+x]!=0) image[y*width+x]=1.0/image[y*width+x];
                            else  image[y*width+x]=NAN;
                        } break;
                    case QFRDRImagingFCSImageEditor::itSqrt: {
                            if (image[y*width+x]>=0) image[y*width+x]=sqrt(image[y*width+x]);
                            else  image[y*width+x]=NAN;
                        } break;
                    default: break;
                }
            }
            if (current->resultsExists(en, gofParam)) {
                gof_image[y*width+x]=current->resultsGetAsDouble(en, gofParam);
                switch(tranGofParam) {
                    case QFRDRImagingFCSImageEditor::itAbs: {
                            gof_image[y*width+x]=fabs(gof_image[y*width+x]);
                        } break;
                    case QFRDRImagingFCSImageEditor::itLog: {
                            if (gof_image[y*width+x]>0) gof_image[y*width+x]=log(gof_image[y*width+x])/log(10.0);
                            else  gof_image[y*width+x]=NAN;

                        } break;
                    case QFRDRImagingFCSImageEditor::itReciprocal: {
                            if (gof_image[y*width+x]!=0) gof_image[y*width+x]=1.0/gof_image[y*width+x];
                            else  gof_image[y*width+x]=NAN;
                        } break;
                    case QFRDRImagingFCSImageEditor::itSqrt: {
                            if (gof_image[y*width+x]>=0) gof_image[y*width+x]=sqrt(gof_image[y*width+x]);
                            else  gof_image[y*width+x]=NAN;
                        } break;
                    default: break;
                }
            }
        }
    }

}

bool QFRDRImagingFCSImageEditor::evaluateFitFunction(const double* tau, double* fit, uint32_t N, QStringList& names, QStringList& namelabels, QList<double>& values, QList<double>& errors, QList<bool>& fix, QStringList& units, QStringList& unitlabels, QString evaluation) {
    QString fitfunc=current->resultsGetAsString(evaluation, "fit_model_name");
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
    for (int i=0; i<ff->paramCount(); i++) {
        QString id=ff->getParameterID(i);
        pids.append(id);
        errs[i]=0;
        fixs[i]=false;
        params[i]=ff->getDescription(id).initialValue;
        if (current->resultsExists(evaluation, "fitparam_"+id)) {
            params[i]=current->resultsGetAsDouble(evaluation, "fitparam_"+id);
            errs[i]=current->resultsGetErrorAsDouble(evaluation, "fitparam_"+id);
        }
        if (current->resultsExists(evaluation, "fitparam_"+id+"_fix")) {
            fixs[i]=current->resultsGetAsBoolean(evaluation, "fitparam_"+id+"_fix");
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

