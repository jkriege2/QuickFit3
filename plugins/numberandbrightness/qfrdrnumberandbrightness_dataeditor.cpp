#include "qfrdrnumberandbrightness_dataeditor.h"
#include "qfrdrnumberandbrightness_data.h"


QFRDRNumberAndBrightnessDataEditor::QFRDRNumberAndBrightnessDataEditor(QFPluginServices* services,  QFRawDataPropertyEditor*  propEditor, QWidget* parent):
    QFRawDataEditor(services, propEditor, parent)
{
    QColor ovlSelCol=QColor("red");
    ovlSelCol.setAlphaF(0.5);
    selectionColor=ovlSelCol;
    QColor ovlExCol=QColor("blue");
    ovlExCol.setAlphaF(0.5);
    excludedColor=ovlExCol;


    plteNumberData=NULL;
    plteBrightnessData=NULL;
    plteOverviewData=NULL;
    plteCorrelationData=NULL;

    plteOverviewSelectedData=NULL;
    plteOverviewExcludedData=NULL;
    plotsSize=0;

    createWidgets();
}

QFRDRNumberAndBrightnessDataEditor::~QFRDRNumberAndBrightnessDataEditor()
{
    reallocMem(0);
}


void QFRDRNumberAndBrightnessDataEditor::addPlotter(JKQtPlotter *&plotter, JKQTPMathImage *&plot, JKQTPOverlayImage *&plteSelected, JKQTPOverlayImage *&plteExcluded)
{
    plotter=new JKQtPlotter(this);
    plot=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0, JKQTPMathImage::MATLAB, plotter->get_plotter());
    plotter->addGraph(plot);
    plteExcluded=new JKQTPOverlayImage(0,0,1,1,NULL, 0, 0, selectionColor, plotter->get_plotter());
    plotter->addGraph(plteExcluded);
    plteSelected=new JKQTPOverlayImage(0,0,1,1,NULL, 0, 0, excludedColor, plotter->get_plotter());
    plotter->addGraph(plteSelected);

    plotter->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    plotter->get_plotter()->set_plotLabelFontSize(10);
    plotter->set_zoomByDoubleAndRightMouseClick(false);
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

void QFRDRNumberAndBrightnessDataEditor::reallocMem(int size) {
    if (size==0) {
        free(plteNumberData);
        free(plteBrightnessData);
        free(plteOverviewData);
        free(plteCorrelationData);
        free(plteOverviewSelectedData);
        free(plteOverviewExcludedData);
    } else if (size!=plotsSize) {
        plotsSize=size;
        plteNumberData=(double*)realloc(plteNumberData, plotsSize*sizeof(double));
        plteBrightnessData=(double*)realloc(plteBrightnessData, plotsSize*sizeof(double));
        plteOverviewData=(double*)realloc(plteOverviewData, plotsSize*sizeof(double));
        plteCorrelationData=(double*)realloc(plteCorrelationData, plotsSize*sizeof(double));

        plteOverviewSelectedData=(bool*)realloc(plteOverviewSelectedData, plotsSize*sizeof(bool));
        plteOverviewExcludedData=(bool*)realloc(plteOverviewExcludedData, plotsSize*sizeof(bool));
    }
}

void QFRDRNumberAndBrightnessDataEditor::createWidgets() {
    QGridLayout* layMain=new QGridLayout(this);
    setLayout(layMain);

    addPlotter(pltNumber, plteNumber, plteNumberSelected, plteNumberExcluded);
    pltNumber->get_plotter()->set_plotLabel(tr("particle number"));
    layMain->addWidget(pltNumber, 0,0);

    addPlotter(pltBrightness, plteBrightness, plteBrightnessSelected, plteBrightnessExcluded);
    pltBrightness->get_plotter()->set_plotLabel(tr("particle brightness"));
    layMain->addWidget(pltBrightness, 0,1);
    pltBrightness->get_plotter()->useExternalDatastore(pltNumber->getDatastore());

    addPlotter(pltOverview, plteOverview, plteOverviewSelected, plteOverviewExcluded);
    pltOverview->get_plotter()->set_plotLabel(tr("intensity"));
    layMain->addWidget(pltOverview, 1,0);
    pltOverview->get_plotter()->useExternalDatastore(pltNumber->getDatastore());

    pltCorrelation=new JKQtPlotter(this);
    pltCorrelation->get_plotter()->useExternalDatastore(pltNumber->getDatastore());
    pltCorrelation->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltCorrelation->get_plotter()->set_plotLabelFontSize(10);
    pltCorrelation->set_zoomByDoubleAndRightMouseClick(false);
    pltCorrelation->set_displayMousePosition(false);
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
    pltCorrelation->get_plotter()->set_plotLabel(tr("correlation"));

    plteCorrelation=new JKQTPxyLineGraph(pltCorrelation->get_plotter());
    pltCorrelation->addGraph(plteCorrelation);
    layMain->addWidget(pltCorrelation, 1,1);


}

void QFRDRNumberAndBrightnessDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
	// this functions connects to a new QFRawDataRecord which could also be NULL, so
	// you will have to connect and disconnect the signals! This is called whenever
	// a user navigates to the next record.
	// here is a basic skeleton:
	
    if (old) disconnect(old, 0, this, 0); // first disconnect from last record
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current); // check whether we have the right QFRawDataRecord class
    if (m) {
		// if so (and if current!=NULL anyways), connect to the new record and read some data
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
		
        replotData();
    } else {
    }

}

void QFRDRNumberAndBrightnessDataEditor::replotData()
{
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {
        qDebug()<<"replotData()  w="<<m->getWidth()<<"  h="<<m->getHeight();

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

        reallocMem( m->getWidth()*m->getHeight());
        memcpy(plteOverviewData, m->getImage(), m->getWidth()*m->getHeight()*sizeof(double));
        memcpy(plteNumberData, m->getNumberImage(), m->getWidth()*m->getHeight()*sizeof(double));
        memcpy(plteBrightnessData, m->getBrightnessImage(), m->getWidth()*m->getHeight()*sizeof(double));

        double w=m->getWidth();
        double h=m->getHeight();

        pltCorrelation->getDatastore()->clear();
        int cN=pltCorrelation->getDatastore()->addCopiedColumn(plteNumberData, m->getWidth()*m->getHeight(), tr("particle_number"));
        int cB=pltCorrelation->getDatastore()->addCopiedColumn(plteBrightnessData, m->getWidth()*m->getHeight(), tr("particle_brightness"));
        int cI=pltCorrelation->getDatastore()->addCopiedColumn(plteOverviewData, m->getWidth()*m->getHeight(), tr("image_data"));

        pltOverview->setAbsoluteXY(0, w, 0, h);
        pltOverview->get_plotter()->set_maintainAspectRatio(true);
        pltOverview->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltOverview->get_plotter()->set_maintainAxisAspectRatio(true);
        pltOverview->get_plotter()->set_axisAspectRatio(1.0*w/h);
        plteOverview->set_datatype(JKQTPMathImageBase::DoubleArray);
        plteOverview->set_data(plteOverviewData, m->getWidth(), m->getHeight());
        plteOverview->set_width(m->getWidth());
        plteOverview->set_height(m->getHeight());
        plteOverviewExcluded->set_data(plteOverviewExcludedData, m->getWidth(), m->getHeight());
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
        plteNumberExcluded->set_data(plteOverviewExcludedData, m->getWidth(), m->getHeight());
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
        plteBrightnessExcluded->set_data(plteOverviewExcludedData, m->getWidth(), m->getHeight());
        plteBrightnessExcluded->set_width(m->getWidth());
        plteBrightnessExcluded->set_height(m->getHeight());
        plteBrightnessSelected->set_data(plteOverviewSelectedData, m->getWidth(), m->getHeight());
        plteBrightnessSelected->set_width(m->getWidth());
        plteBrightnessSelected->set_height(m->getHeight());

        pltNumber->zoomToFit();
        pltBrightness->zoomToFit();
        pltOverview->zoomToFit();

        plteCorrelation->set_xColumn(cN);
        plteCorrelation->set_yColumn(cB);
        plteCorrelation->set_drawLine(false);
        plteCorrelation->set_symbol(JKQTPdot);
        plteCorrelation->set_color(QColor("red"));
        pltCorrelation->getXAxis()->set_axisLabel(tr("particle number $N$"));
        pltCorrelation->getYAxis()->set_axisLabel(tr("particle brightness $\\epsilon$"));
        pltCorrelation->get_plotter()->set_showKey(false);
        pltCorrelation->zoomToFit();

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

void QFRDRNumberAndBrightnessDataEditor::rawDataChanged() {

}

void QFRDRNumberAndBrightnessDataEditor::readSettings() {
	// read widget settings
    if (!settings) return;
}


void QFRDRNumberAndBrightnessDataEditor::writeSettings() {
	// rwrite widget settings
    if (!settings) return;
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
