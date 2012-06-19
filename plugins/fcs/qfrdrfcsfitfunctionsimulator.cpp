#include "qfrdrfcsfitfunctionsimulator.h"
#include "ui_qfrdrfcsfitfunctionsimulator.h"
#include "qfrdrfcsdata.h"

QFRDRFCSFitFunctionSimulator::QFRDRFCSFitFunctionSimulator(QFPluginServices* services, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRFCSFitFunctionSimulator)
{
    tauN=0;
    tau=NULL;
    corr=NULL;
    this->services=services;
    ui->setupUi(this);
    ui->edtMinTau->setRange(1e-10, 1e10);
    ui->edtMinTau->setValue(1e-7);
    ui->edtMaxTau->setRange(1e-10, 1e10);
    ui->edtMaxTau->setValue(1e1);

    ui->pltFunction->getXAxis()->set_axisLabel("lag time $\\tau$ [seconds]");
    ui->pltFunction->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    ui->pltFunction->getYAxis()->set_labelFontSize(11);
    ui->pltFunction->getYAxis()->set_tickLabelFontSize(10);
    ui->pltFunction->getXAxis()->set_tickLabelFontSize(10);
    ui->pltFunction->getXAxis()->set_logAxis(true);
    ui->pltFunction->get_plotter()->setBorder(1,1,1,1);
    ui->pltFunction->get_plotter()->set_useAntiAliasingForSystem(true);
    ui->pltFunction->get_plotter()->set_useAntiAliasingForGraphs(true);
    ui->pltFunction->get_plotter()->set_keyFontSize(9);
    ui->pltFunction->get_plotter()->set_keyXMargin(2);
    ui->pltFunction->get_plotter()->set_keyYMargin(2);

    tbEditRanges=new QTabBar(this);
    tbEditRanges->addTab(tr("Parameter Values"));
    tbEditRanges->addTab(tr("Parameter Ranges"));
    tbEditRanges->setShape(QTabBar::TriangularNorth);
    tbEditRanges->setDrawBase(false);
    tbEditRanges->setCurrentIndex(0);
    ui->layParams->addWidget(tbEditRanges);


    scrollParameters=new JKVerticalScrollArea(this);
    ui->layParams->addWidget(scrollParameters);
    QWidget* widParameters=new QWidget(this);
    scrollParameters->setWidget(widParameters);
    scrollParameters->setWidgetResizable(true);
    layParameters=new QGridLayout(this);
    layParameters->setSpacing(2);
    layParameters->setVerticalSpacing(1);
    layParameters->setHorizontalSpacing(0);
    layParameters->setMargin(4);
    widParameters->setLayout(layParameters);


    m_fitFunctions=services->getFitFunctionManager()->getModels("", this);
    QStringList ff=m_fitFunctions.keys();


    ui->cmbFunction->clear();
    for (int i=0; i<ff.size(); i++) {
        QString id=ff[i];
        ui->cmbFunction->addItem(m_fitFunctions[id]->name(), id);
    }
    int idx=ff.indexOf("fcs_diff");
    if (idx>=0) {
        ui->cmbFunction->setCurrentIndex(idx);
    } else {
        ui->cmbFunction->setCurrentIndex(0);
    }

    modelChanged(ui->cmbFunction->currentIndex());
    updateFitFunction();

    connect(ui->cmbFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
    connect(ui->edtMinTau, SIGNAL(valueChanged(double)), this, SLOT(updateFitFunction()));
    connect(ui->edtMaxTau, SIGNAL(valueChanged(double)), this, SLOT(updateFitFunction()));

}

QFRDRFCSFitFunctionSimulator::~QFRDRFCSFitFunctionSimulator()
{
    delete ui;
    QMapIterator<QString, QFFitFunction*> i(m_fitFunctions);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
    m_fitFunctions.clear();
    if (tau) free(tau);
    if (corr) free(corr);
}

void QFRDRFCSFitFunctionSimulator::setFitValue(const QString &id, double value)
{
    params[id].value=value;
    params[id].valueset=true;
}

double QFRDRFCSFitFunctionSimulator::getFitValue(const QString &id)
{
    QFFitFunction* ffunc=getFitFunction();

    if (ffunc) {
        for (int p=0; p<ffunc->paramCount(); p++) {
            QFFitFunction::ParameterDescription d=ffunc->getDescription(p);
            if (d.id==id) {
                if (params.contains(id)) {
                    if (params[id].valueset) return params[id].value;
                }
                /*double value=0;
                if (overrideFitFunctionPreset(id, value)) return value;*/

                return d.initialValue;
            }
        }
    }
    return 0;
}

double QFRDRFCSFitFunctionSimulator::getFitError(const QString &id)
{
    QFFitFunction* ffunc=getFitFunction();

    if (ffunc) {
        for (int p=0; p<ffunc->paramCount(); p++) {
            QFFitFunction::ParameterDescription d=ffunc->getDescription(p);
            if (d.id==id) {
                if (params.contains(id)) {
                    return params[id].error;
                }
            }
        }
    }
    return 0;
}

void QFRDRFCSFitFunctionSimulator::setFitError(const QString &id, double error)
{
    params[id].error=error;
}

bool QFRDRFCSFitFunctionSimulator::getFitFix(const QString &id)
{
    return false;
}

void QFRDRFCSFitFunctionSimulator::setFitRange(const QString &id, double min, double max)
{
    params[id].min=min;
    params[id].max=max;
    params[id].minset=true;
    params[id].maxset=true;
}

void QFRDRFCSFitFunctionSimulator::setInitFitValue(const QString &id, double value, double error)
{
}

void QFRDRFCSFitFunctionSimulator::resetDefaultFitFix(const QString &id)
{
}

double QFRDRFCSFitFunctionSimulator::getDefaultFitValue(const QString &id) {
    QFFitFunction* ffunc=getFitFunction();

    if (ffunc) {
        for (int p=0; p<ffunc->paramCount(); p++) {
            QFFitFunction::ParameterDescription d=ffunc->getDescription(p);
            if (d.id==id) {
                /*double value=0;
                if (overrideFitFunctionPreset(id, value)) return value;*/

                return d.initialValue;

            }
        }
    }
    return 0;
}

bool QFRDRFCSFitFunctionSimulator::getDefaultFitFix(const QString &id) {
    return false;
}

QFFitFunction *QFRDRFCSFitFunctionSimulator::getFitFunction() const {
    return m_fitFunctions.value(ui->cmbFunction->itemData(ui->cmbFunction->currentIndex()).toString(), NULL);
}

void QFRDRFCSFitFunctionSimulator::updateTau() {
    if (tau) free(tau);
    if (corr) free(corr);
    tau=NULL;
    corr=NULL;
    tauN=0;
    double dt=ui->edtMinTau->value();
    double t=dt;
    while (t<=ui->edtMaxTau->value()) {
        tauN++;
        t=t+dt;
        if (tauN%8==0)dt=dt*2;
    }
    if (tauN>0) {
        tau=(double*)calloc(tauN, sizeof(double));
        corr=(double*)calloc(tauN, sizeof(double));
        dt=ui->edtMinTau->value();
        t=dt;
        int i=0;
        while (t<=ui->edtMaxTau->value()) {
            if (i<tauN) tau[i]=t;
            i++;
            t=t+dt;
            if (i%8==0)dt=dt*2;
        }
    }
}


void QFRDRFCSFitFunctionSimulator::displayModel(bool newWidget){
    QFFitFunction* ffunc=getFitFunction();


    if (!ffunc) {
        /////////////////////////////////////////////////////////////////////////////////////////////
        // delete all fit parameter widgets
        /////////////////////////////////////////////////////////////////////////////////////////////
        for (int i=0; i<m_fitParameters.size(); i++) {
            if (m_fitParameters[i]) {
                m_fitParameters[i]->disableDatastore();
                disconnect(tbEditRanges, SIGNAL(currentChanged(int)), m_fitParameters[i], SLOT(setEditRange(int)));
                disconnect(m_fitParameters[i], SIGNAL(valueChanged(QString, double)), this, SLOT(updateFitFunction()));
                disconnect(m_fitParameters[i], SIGNAL(errorChanged(QString, double)), this, SLOT(updateFitFunction()));
                disconnect(m_fitParameters[i], SIGNAL(rangeChanged(QString, double, double)), this, SLOT(updateFitFunction()));
                delete m_fitParameters[i];
            }
        }
        m_fitParameters.clear();
        return;
    }

    if (newWidget) {
        /////////////////////////////////////////////////////////////////////////////////////////////
        // first delete all fit parameter widgets
        /////////////////////////////////////////////////////////////////////////////////////////////
        for (int i=0; i<m_fitParameters.size(); i++) {
            if (m_fitParameters[i]) {
                m_fitParameters[i]->disableDatastore();
                //disconnect(btnEditRanges, SIGNAL(toggled(bool)), m_fitParameters[i], SLOT(setEditRange(bool)));
                //disconnect(btnEditRanges, SIGNAL(toggled(bool)), m_fitParameters[i], SLOT(unsetEditValues(bool)));
                disconnect(tbEditRanges, SIGNAL(currentChanged(int)), m_fitParameters[i], SLOT(setEditRange(int)));
                disconnect(tbEditRanges, SIGNAL(currentChanged(int)), m_fitParameters[i], SLOT(unsetEditValues(int)));
                disconnect(m_fitParameters[i], SIGNAL(valueChanged(QString, double)), this, SLOT(updateFitFunction()));
                disconnect(m_fitParameters[i], SIGNAL(errorChanged(QString, double)), this, SLOT(updateFitFunction()));
                disconnect(m_fitParameters[i], SIGNAL(rangeChanged(QString, double, double)), this, SLOT(updateFitFunction()));
                delete m_fitParameters[i];
            }
        }
        // remove all widgets from layout
        QLayoutItem *child;
        while ((child = layParameters->takeAt(0)) != 0) {
            delete child;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////
        // create header widget
        /////////////////////////////////////////////////////////////////////////////////////////////
        QFFitParameterWidgetWrapper* header=new QFFitParameterWidgetWrapper(this, layParameters, 0, "", QFFitParameterWidgetWrapper::Header, true, true, QFFitFunction::DisplayError, true, this, tr("parameter"));
        m_fitParameters.append(header);
        //connect(btnEditRanges, SIGNAL(toggled(bool)), header, SLOT(setEditRange(bool)));
        //connect(btnEditRanges, SIGNAL(toggled(bool)), header, SLOT(unsetEditValues(bool)));
        connect(tbEditRanges, SIGNAL(currentChanged(int)), header, SLOT(setEditRange(int)));
        connect(tbEditRanges, SIGNAL(currentChanged(int)), header, SLOT(unsetEditValues(int)));
        //header->setEditRange(btnEditRanges->isChecked());
        //header->unsetEditValues(!btnEditRanges->isChecked());
        header->setEditRange(tbEditRanges->currentIndex());
        header->unsetEditValues(tbEditRanges->currentIndex());

        /////////////////////////////////////////////////////////////////////////////////////////////
        // create new parameter widgets
        /////////////////////////////////////////////////////////////////////////////////////////////

        for (int i=0; i<ffunc->paramCount(); i++) {
            QString id=ffunc->getParameterID(i);

            QFFitFunction::ParameterDescription d=ffunc->getDescription(i);
            QFFitParameterWidgetWrapper::WidgetType wtype=QFFitParameterWidgetWrapper::FloatEdit;
            if (d.type==QFFitFunction::IntNumber) wtype=QFFitParameterWidgetWrapper::IntSpinBox;
            if (d.type==QFFitFunction::IntCombo) wtype=QFFitParameterWidgetWrapper::IntDropDown;
            bool editable=d.userEditable;
            bool displayFix=d.userEditable;
            QFFitFunction::ErrorDisplayMode displayError=d.displayError;
            bool editRange=d.userEditable && d.userRangeEditable;
            if (!d.fit) {
                displayFix=false;
            }
            QString l=QString("<font size=\"+1\">%1</font>:").arg(d.label);
            if (!d.unit.isEmpty()) l=QString("<font size=\"+1\">%1 [%2]</font>:").arg(d.label).arg(d.unitLabel);
            l.replace("<sub>", "<sub><font size=\"+2\">", Qt::CaseInsensitive);
            l.replace("<sup>", "<sup><font size=\"+2\">", Qt::CaseInsensitive);
            l.replace("</sup>", "</font></sup>", Qt::CaseInsensitive);
            l.replace("</sub>", "</font></sub>", Qt::CaseInsensitive);
            QFFitParameterWidgetWrapper* fpw=new QFFitParameterWidgetWrapper(this, layParameters, i+1, id, wtype, editable, displayFix, displayError, editRange, this, l, d.comboItems);
            fpw->setUnit(d.unit);
            fpw->setIncrement(d.inc);
            fpw->setWidgetWidth(75);
            fpw->setRangeEnabled(true);
            fpw->setValueAbsoluteRange(d.absMinValue, d.absMaxValue);
            fpw->setRangeEnabled(true);
            fpw->setToolTip(d.name);
            m_fitParameters.append(fpw);
            connect(tbEditRanges, SIGNAL(currentChanged(int)), fpw, SLOT(unsetEditValues(int)));
            connect(tbEditRanges, SIGNAL(currentChanged(int)), fpw, SLOT(setEditRange(int)));
            connect(fpw, SIGNAL(valueChanged(QString, double)), this, SLOT(updateFitFunction()));
            connect(fpw, SIGNAL(errorChanged(QString, double)), this, SLOT(updateFitFunction()));
            connect(fpw, SIGNAL(rangeChanged(QString, double, double)), this, SLOT(updateFitFunction()));
            fpw->setEditRange(tbEditRanges->currentIndex());
            fpw->unsetEditValues(tbEditRanges->currentIndex());
        }
        // add stretcher item in bottom row
        layParameters->addItem(new QSpacerItem(5,5, QSizePolicy::Minimum, QSizePolicy::Expanding), layParameters->rowCount(), 0);
    }

    updateParameterValues();
}



void QFRDRFCSFitFunctionSimulator::updateFitFunction() {
    updateParameterValues();
    replotFitFunction();

}

void QFRDRFCSFitFunctionSimulator::updateParameterValues() {
    QFFitFunction* ffunc=getFitFunction();

    if (!ffunc) return;

    double* fullParams=(double*)calloc(ffunc->paramCount(), sizeof(double));
    double* errors=(double*)calloc(ffunc->paramCount(), sizeof(double));
    for (int p=0; p<ffunc->paramCount(); p++) {
        QFFitFunction::ParameterDescription d=ffunc->getDescription(p);
        QString id=d.id;
        if (params.contains(id)) {
            fullParams[p]=params[id].value;
            errors[p]=params[id].error;
        } else {
            fullParams[p]=d.initialValue;
            /*double value=0;
            if (overrideFitFunctionPreset(d.id, value)) fullParams[p]=value;*/

            errors[p]=0;
        }
    }
    ffunc->calcParameter(fullParams, errors);


    for (int i=0; i<m_fitParameters.size(); i++) {
        if (m_fitParameters[i]) {
            //m_fitParameters[i]->reloadValues();
            QString id=m_fitParameters[i]->parameterID();
            int num=ffunc->getParameterNum(id);
            if (!id.isEmpty()) {
                if (num>=0) m_fitParameters[i]->setValue(fullParams[num], errors[num], false);
                bool visible=ffunc->isParameterVisible(ffunc->getParameterNum(id), fullParams);
                m_fitParameters[i]->setVisible(visible);
                //layParameters->labelForField(m_fitParameters[i])->setVisible(visible);
            }
        }
    }

    free(fullParams);
    free(errors);

}

void QFRDRFCSFitFunctionSimulator::modelChanged(int index) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    displayModel(true);
    replotFitFunction();
    QApplication::restoreOverrideCursor();
}

double QFRDRFCSFitFunctionSimulator::getFitMax(const QString &id)
{
    QFFitFunction* ffunc=getFitFunction();

    if (ffunc) {
        for (int p=0; p<ffunc->paramCount(); p++) {
            QFFitFunction::ParameterDescription d=ffunc->getDescription(p);
            if (d.id==id) {
                if (params.contains(id)) {
                    if (params[id].maxset) return params[id].max;
                }
                return d.maxValue;
            }
        }
    }
    return 0;
}

double QFRDRFCSFitFunctionSimulator::getFitMin(const QString &id)
{
    QFFitFunction* ffunc=getFitFunction();

    if (ffunc) {
        for (int p=0; p<ffunc->paramCount(); p++) {
            QFFitFunction::ParameterDescription d=ffunc->getDescription(p);
            if (d.id==id) {
                if (params.contains(id)) {
                    if (params[id].minset) return params[id].min;
                }
                return d.minValue;
            }
        }
    }
    return 0;
}

void QFRDRFCSFitFunctionSimulator::setFitMax(const QString &id, double max)
{
    params[id].max=max;
    params[id].maxset=true;
}

void QFRDRFCSFitFunctionSimulator::setFitMin(const QString &id, double min)
{
    params[id].min=min;
    params[id].minset=true;
}


void QFRDRFCSFitFunctionSimulator::replotFitFunction() {
    JKQTPdatastore* ds=ui->pltFunction->getDatastore();
    QFFitFunction* ffunc=getFitFunction();

    if (!ffunc) return;

    try {
        ui->pltFunction->set_doDrawing(false);
        ui->pltFunction->clearGraphs();
        ds->clear();
        updateTau();
        if (tauN) {


            /////////////////////////////////////////////////////////////////////////////////
            // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
            // we are working with a complete set of parameters
            /////////////////////////////////////////////////////////////////////////////////

            double* fullParams=(double*)calloc(ffunc->paramCount(), sizeof(double));
            double* errors=(double*)calloc(ffunc->paramCount(), sizeof(double));
            for (int p=0; p<ffunc->paramCount(); p++) {
                QFFitFunction::ParameterDescription d=ffunc->getDescription(p);
                QString id=d.id;
                if (params.contains(id)) {
                    fullParams[p]=params[id].value;
                    errors[p]=params[id].error;
                } else {
                    fullParams[p]=d.initialValue;
                    /*double value=0;
                    if (overrideFitFunctionPreset(id, value)) d.initialValue=value;*/

                    errors[p]=0;
                }
            }
            ffunc->calcParameter(fullParams, errors);
            csv="";

            for (int i=0; i<tauN; i++) {
                corr[i]=ffunc->evaluate(tau[i], fullParams);
                csv=csv+CDoubleToQString(tau[i])+", "+CDoubleToQString(corr[i])+"\n";
            }


            size_t c_tau = ds->addCopiedColumn(tau, tauN, "tau");
            size_t c_fit = ds->addCopiedColumn(corr, tauN, "function");

            /////////////////////////////////////////////////////////////////////////////////
            // plot fit model and additional function graphs
            /////////////////////////////////////////////////////////////////////////////////
            JKQTPxyLineGraph* g_fit=new JKQTPxyLineGraph(ui->pltFunction->get_plotter());
            g_fit->set_drawLine(true);
            g_fit->set_title("function");
            g_fit->set_xColumn(c_tau);
            g_fit->set_yColumn(c_fit);
            ui->pltFunction->addGraph(g_fit);
        }

        ui->pltFunction->set_doDrawing(true);
        ui->pltFunction->zoomToFit();
    } catch(std::exception& E) {
        services->log_error(tr("error during plotting, error message: %1\n").arg(E.what()));
    }

}






