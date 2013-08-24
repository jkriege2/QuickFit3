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


void QFRDRNumberAndBrightnessDataEditor::addPlotter(QFPlotter *&plotter, JKQTPMathImage *&plot, JKQTPOverlayImage *&plteSelected, JKQTPOverlayImage *&plteExcluded)
{
    plotter=new QFPlotter(this);
    plot=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0, JKQTPMathImage::MATLAB, plotter->get_plotter());
    plotter->addGraph(plot);
    plteExcluded=new JKQTPOverlayImage(0,0,1,1,NULL, 0, 0, selectionColor, plotter->get_plotter());
    plotter->addGraph(plteExcluded);
    plteSelected=new JKQTPOverlayImage(0,0,1,1,NULL, 0, 0, excludedColor, plotter->get_plotter());
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
        for (int i=0; i<plotsSize; i++) {
            plteOverviewSelectedData[i]=false;
            plteOverviewExcludedData[i]=false;
        }
    }
}

void QFRDRNumberAndBrightnessDataEditor::createWidgets() {
    tabMain=new QTabWidget(this);
    QHBoxLayout* layMain=new QHBoxLayout(this);
    layMain->setContentsMargins(3,3,3,3);
    setLayout(layMain);
    layMain->addWidget(tabMain);

    QGridLayout* layPlots=new QGridLayout(this);
    QWidget* widPlots=new QWidget(this);
    widPlots->setLayout(layPlots);
    tabMain->addTab(widPlots, tr("image plots"));

    addPlotter(pltNumber, plteNumber, plteNumberSelected, plteNumberExcluded);
    pltNumber->get_plotter()->set_plotLabel(tr("particle number"));
    layPlots->addWidget(pltNumber, 0,0);

    addPlotter(pltBrightness, plteBrightness, plteBrightnessSelected, plteBrightnessExcluded);
    pltBrightness->get_plotter()->set_plotLabel(tr("particle brightness"));
    layPlots->addWidget(pltBrightness, 0,1);
    pltBrightness->get_plotter()->useExternalDatastore(pltNumber->getDatastore());

    addPlotter(pltOverview, plteOverview, plteOverviewSelected, plteOverviewExcluded);
    pltOverview->get_plotter()->set_plotLabel(tr("intensity"));
    layPlots->addWidget(pltOverview, 1,0);
    pltOverview->get_plotter()->useExternalDatastore(pltNumber->getDatastore());

    pltCorrelation=new QFPlotter(this);
    pltCorrelation->get_plotter()->useExternalDatastore(pltNumber->getDatastore());
    pltCorrelation->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltCorrelation->get_plotter()->set_plotLabelFontSize(10);
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
    pltCorrelation->get_plotter()->set_plotLabel(tr("scatter plot"));

    plteCorrelation=new JKQTPxyLineGraph(pltCorrelation->get_plotter());
    pltCorrelation->addGraph(plteCorrelation);
    layPlots->addWidget(pltCorrelation, 1,1);





    QVBoxLayout* layHistogram=new QVBoxLayout(this);
    QWidget* widHistograms=new QWidget(this);
    widHistograms->setLayout(layHistogram);
    tabMain->addTab(widHistograms, tr("histograms"));

    histNumber=new QFHistogramView(this);
    histBrightness=new QFHistogramView(this);
    layHistogram->addWidget(histNumber);
    layHistogram->addWidget(histBrightness);

}

void QFRDRNumberAndBrightnessDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
	// this functions connects to a new QFRawDataRecord which could also be NULL, so
	// you will have to connect and disconnect the signals! This is called whenever
	// a user navigates to the next record.
	// here is a basic skeleton:
	
    if (old) {
        savePlotSettings();
        disconnect(old, 0, this, 0); // first disconnect from last record
    }
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current); // check whether we have the right QFRawDataRecord class
    if (m) {
        m->recalcNumberAndBrightness();
		// if so (and if current!=NULL anyways), connect to the new record and read some data
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        loadPlotSettings();
        replotData();
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

        reallocMem( m->getWidth()*m->getHeight());
        for (int i=0; i<m->getWidth()*m->getHeight(); i++) {
            plteOverviewData[i]=m->getImage()[i];
            plteNumberData[i]=m->getNumberImage()[i];
            plteBrightnessData[i]=m->getBrightnessImage()[i];
        }
        /*memcpy(plteOverviewData, m->getImage(), m->getWidth()*m->getHeight()*sizeof(double));
        memcpy(plteNumberData, m->getNumberImage(), m->getWidth()*m->getHeight()*sizeof(double));
        memcpy(plteBrightnessData, m->getBrightnessImage(), m->getWidth()*m->getHeight()*sizeof(double));*/
        updateSelectionArrays();

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

        updateHistograms();

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


void QFRDRNumberAndBrightnessDataEditor::updateSelectionArrays() {
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    //qDebug()<<"updateSelectionArrays";
    //pltOverview->set_doDrawing(false);


    if (m) {
        int siz=m->getWidth()*m->getHeight();
        for (register int i=0; i<siz; i++) {
            int x=m->indexToX(i);
            int y=m->indexToY(i);
            int idx=y*m->getWidth()+x;
            plteOverviewSelectedData[idx]=selected.contains(i);
            plteOverviewExcludedData[idx]=m->leaveoutRun(i);
        }
    } else {
        for (register int i=0; i<plotsSize; i++) {
            plteOverviewSelectedData[i]=false;
            plteOverviewExcludedData[i]=false;
        }
    }
    //qDebug()<<"updateSelectionArrays ... end";
}

void QFRDRNumberAndBrightnessDataEditor::updateHistograms()
{
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {

        savePlotSettings();
        updateSelectionArrays();


        int dataSize=m->getWidth()*m->getHeight();
        int dataSelSize=m->getWidth()*m->getHeight();

        double* data=(double*)calloc(dataSize, sizeof(double));
        double* dataSel=(double*)calloc(dataSelSize, sizeof(double));
        dataSize=0;
        dataSelSize=0;
        for (int i=0; i<m->getWidth()*m->getHeight(); i++) {
            if (!plteOverviewExcludedData[i]) {
                data[dataSize]=plteNumberData[i];
                dataSize++;
            }
            if (!plteOverviewExcludedData[i] && plteOverviewSelectedData[i]) {
                dataSel[dataSelSize]=plteNumberData[i];
                dataSelSize++;
            }
        }
        histNumber->clear();
        histNumber->setHistogramXLabel(tr("particle number $N$"));
        histNumber->addHistogram("complete", data, dataSize, false);
        free(data);
        if (dataSelSize>1) histNumber->addHistogram("selection", dataSel, dataSelSize, false);
        free(dataSel);


        dataSize=m->getWidth()*m->getHeight();
        dataSelSize=m->getWidth()*m->getHeight();

        data=(double*)calloc(dataSize, sizeof(double));
        dataSel=(double*)calloc(dataSelSize, sizeof(double));
        dataSize=0;
        dataSelSize=0;
        for (int i=0; i<m->getWidth()*m->getHeight(); i++) {
            if (!plteOverviewExcludedData[i]) {
                data[dataSize]=plteBrightnessData[i];
                dataSize++;
            }
            if (!plteOverviewExcludedData[i] && plteOverviewSelectedData[i]) {
                dataSel[dataSelSize]=plteBrightnessData[i];
                dataSelSize++;
            }
        }
        histBrightness->clear();
        histBrightness->setHistogramXLabel(tr("particle brightness $\\epsilon$"));
        histBrightness->addHistogram("complete", data, dataSize, false);
        free(data);
        if (dataSelSize>1) histBrightness->addHistogram("selection", dataSel, dataSelSize, false);
        free(dataSel);

        histNumber->updateHistogram(true);
        histBrightness->updateHistogram(true);
    }
}

void QFRDRNumberAndBrightnessDataEditor::rawDataChanged() {

}

void QFRDRNumberAndBrightnessDataEditor::readSettings() {
	// read widget settings
    if (!settings) return;
    histNumber->readSettings(*(settings->getQSettings()), "QFRDRNumberAndBrightnessDataEditor/histNumber/");
    histBrightness->readSettings(*(settings->getQSettings()), "QFRDRNumberAndBrightnessDataEditor/histBrightness/");
}


void QFRDRNumberAndBrightnessDataEditor::writeSettings() {
	// rwrite widget settings
    if (!settings) return;
    histNumber->writeSettings(*(settings->getQSettings()), "QFRDRNumberAndBrightnessDataEditor/histNumber/");
    histBrightness->writeSettings(*(settings->getQSettings()), "QFRDRNumberAndBrightnessDataEditor/histBrightness/");
}

void QFRDRNumberAndBrightnessDataEditor::loadPlotSettings() {
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {
        histNumber->setAutorange(m->getProperty(QString("NANDB%1_HISTNUMBER_AUTORANGE").arg(peID), true).toBool());
        histNumber->setMin(m->getProperty(QString("NANDB%1_HISTNUMBER_MIN").arg(peID), 0).toDouble());
        histNumber->setMax(m->getProperty(QString("NANDB%1_HISTNUMBER_MAX").arg(peID), 10).toDouble());
        histNumber->setNormalized(m->getProperty(QString("NANDB%1_HISTNUMBER_NORMALIZED").arg(peID), true).toBool());
        histNumber->setLog(m->getProperty(QString("NANDB%1_HISTNUMBER_LOG").arg(peID), false).toBool());
        histNumber->setBins(m->getProperty(QString("NANDB%1_HISTNUMBER_BINS").arg(peID), 100).toInt());

        histBrightness->setAutorange(m->getProperty(QString("NANDB%1_HISTNUMBER_AUTORANGE").arg(peID), true).toBool());
        histBrightness->setMin(m->getProperty(QString("NANDB%1_HISTBRIGHTNESS_MIN").arg(peID), 0).toDouble());
        histBrightness->setMax(m->getProperty(QString("NANDB%1_HISTBRIGHTNESS_MAX").arg(peID), 10).toDouble());
        histBrightness->setNormalized(m->getProperty(QString("NANDB%1_HISTBRIGHTNESS_NORMALIZED").arg(peID), true).toBool());
        histBrightness->setLog(m->getProperty(QString("NANDB%1_HISTBRIGHTNESS_LOG").arg(peID), false).toBool());
        histBrightness->setBins(m->getProperty(QString("NANDB%1_HISTBRIGHTNESS_BINS").arg(peID), 100).toInt());
    }
}

void QFRDRNumberAndBrightnessDataEditor::savePlotSettings() {
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {
        m->setQFProperty(QString("NANDB%1_HISTNUMBER_AUTORANGE").arg(peID), histNumber->getAutorange(), false, false);
        m->setQFProperty(QString("NANDB%1_HISTNUMBER_MIN").arg(peID), histNumber->getMin(), false, false);
        m->setQFProperty(QString("NANDB%1_HISTNUMBER_MAX").arg(peID), histNumber->getMax(), false, false);
        m->setQFProperty(QString("NANDB%1_HISTNUMBER_NORMALIZED").arg(peID), histNumber->getNormalized(), false, false);
        m->setQFProperty(QString("NANDB%1_HISTNUMBER_LOG").arg(peID), histNumber->getLog(), false, false);
        m->setQFProperty(QString("NANDB%1_HISTNUMBER_BINS").arg(peID), histNumber->getBins(), false, false);

        m->setQFProperty(QString("NANDB%1_HISTNUMBER_AUTORANGE").arg(peID), histBrightness->getAutorange(), false, false);
        m->setQFProperty(QString("NANDB%1_HISTBRIGHTNESS_MIN").arg(peID),histBrightness->getMin()  , false, false);
        m->setQFProperty(QString("NANDB%1_HISTBRIGHTNESS_MAX").arg(peID), histBrightness->getMax() , false, false);
        m->setQFProperty(QString("NANDB%1_HISTBRIGHTNESS_NORMALIZED").arg(peID), histBrightness->getNormalized() , false, false);
        m->setQFProperty(QString("NANDB%1_HISTBRIGHTNESS_LOG").arg(peID),histBrightness->getLog()  , false, false);
        m->setQFProperty(QString("NANDB%1_HISTBRIGHTNESS_BINS").arg(peID),histBrightness->getBins() , false, false);
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
