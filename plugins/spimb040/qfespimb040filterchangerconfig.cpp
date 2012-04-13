#include "qfespimb040filterchangerconfig.h"
#include "qfespimb040mainwindow.h"
#include <QtGui>
#include <iostream>


QFESPIMB040FilterChangerConfig::QFESPIMB040FilterChangerConfig(QWidget* parent):
    QWidget(parent)
{

    FilterChangerStateUpdateInterval=251;

    timUpdate=new QTimer(this);
    timUpdate->setSingleShot(true);
    timUpdate->setInterval(FilterChangerStateUpdateInterval);

    m_log=NULL;
    m_pluginServices=NULL;
    locked=false;
    moving=false;


    createWidgets();
    createActions();
    updateStates();


    connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayFilterChangerStates()));
    timUpdate->start(FilterChangerStateUpdateInterval);
    //QTimer::singleShot(stageStateUpdateInterval, this, SLOT(displayAxisStates()));
}


QFESPIMB040FilterChangerConfig::~QFESPIMB040FilterChangerConfig()
{
    locked=true;
    disconnect(timUpdate, SIGNAL(timeout()), this, SLOT(displayFilterChangerStates()));
    timUpdate->stop();
}

void QFESPIMB040FilterChangerConfig::unlockFilterChangers() {
    locked=false;
    connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayFilterChangerStates()));
    timUpdate->setSingleShot(true);
    timUpdate->setInterval(FilterChangerStateUpdateInterval);
    timUpdate->start(FilterChangerStateUpdateInterval);
}


void QFESPIMB040FilterChangerConfig::lockFilterChangers() {
    locked=true;
    disconnect(timUpdate, SIGNAL(timeout()), this, SLOT(displayFilterChangerStates()));
    timUpdate->stop();
}

void QFESPIMB040FilterChangerConfig::setLog(QFPluginLogService* log) {
    m_log=log;
}

void QFESPIMB040FilterChangerConfig::init(QFPluginLogService* log, QFPluginServices* pluginServices) {
    m_log=log;
    m_pluginServices=pluginServices;

    if (m_pluginServices) {
        cmbFilterChanger->init(m_pluginServices->getExtensionManager());
    } else {
        //stages.clear();
        cmbFilterChanger->clear();
        cmbFilter->clear();
    }
    updateFilters();
    updateStates();
}

void QFESPIMB040FilterChangerConfig::loadSettings(QSettings& settings, QString prefix) {
    cmbFilterChanger->loadSettings(settings, prefix+"FilterChanger/");

    FilterChangerStateUpdateInterval=settings.value(prefix+"update_interval", FilterChangerStateUpdateInterval).toDouble();
}

void QFESPIMB040FilterChangerConfig::saveSettings(QSettings& settings, QString prefix) {
    cmbFilterChanger->storeSettings(settings, prefix+"FilterChanger/");
    settings.setValue(prefix+"update_interval", FilterChangerStateUpdateInterval);
}

void QFESPIMB040FilterChangerConfig::createWidgets() {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QGridLayout* widgetLayout=new QGridLayout(this);
    setLayout(widgetLayout);
    widgetLayout->setContentsMargins(0,0,0,0);
    widgetLayout->setHorizontalSpacing(2);



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    cmbFilter=new QComboBox(this);
    widgetLayout->addWidget(cmbFilter,0,0);
    connect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));

    cmbFilterChanger=new QFFilterChangerComboBox(this);
    widgetLayout->addWidget(cmbFilterChanger,0,1);
    btnConnect=new QToolButton(this);
    widgetLayout->addWidget(btnConnect,0,2);
    btnConfigure=new QToolButton(this);
    widgetLayout->addWidget(btnConfigure,0,3);
    QWidget* w=new QWidget(this);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    widgetLayout->addWidget(w,0,4);
    widgetLayout->setColumnStretch(4,1);
    cmbFilterChanger->setEnabled(false);


}

void QFESPIMB040FilterChangerConfig::createActions() {
    actConnect=new QAction(QIcon(":/spimb040/connect_filterchanger.png"), tr("Connect to filter changer driver/hardware ..."), this);
    actConnect->setCheckable(true);
    connect(actConnect, SIGNAL(toggled(bool)), this, SLOT(disConnect()));
    btnConnect->setDefaultAction(actConnect);


    actConfigure=new QAction(QIcon(":/spimb040/configure_filterchanger.png"), tr("Configure filter changer ..."), this);
    connect(actConfigure, SIGNAL(triggered()), this, SLOT(configure()));
    btnConfigure->setDefaultAction(actConfigure);



}

void QFESPIMB040FilterChangerConfig::updateStates() {
    QFExtensionFilterChanger* FilterChanger;
    int FilterChangerID;
    bool conn;


    FilterChanger=getFilterChanger();
    FilterChangerID=getFilterChangerID();
    conn=false;
    if (FilterChanger) {
        conn=FilterChanger->isFilterChangerConnected(FilterChangerID);
        if (conn) {
            actConnect->setChecked(true);
            actConnect->setIcon(QIcon(":/spimb040/disconnect_filterchanger.png"));
            actConnect->setText(tr("Disconnect from filter changer driver/hardware ..."));
        } else {
            actConnect->setChecked(false);
            actConnect->setIcon(QIcon(":/spimb040/connect_filterchanger.png"));
            actConnect->setText(tr("Connect from filter changer driver/hardware ..."));
        }
        disconnect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
        cmbFilter->setCurrentIndex(FilterChanger->getFilterChangerCurrentFilter(FilterChangerID));
        connect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
    }
    actConfigure->setEnabled(FilterChanger!=NULL && FilterChangerID>=0);
    actConnect->setEnabled(FilterChanger!=NULL && FilterChangerID>=0);
    cmbFilterChanger->setEnabled(!conn);
}

void QFESPIMB040FilterChangerConfig::updateFilters() {
    QString idx=cmbFilter->currentText();
    disconnect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
    cmbFilter->clear();
    QFExtensionFilterChanger* FilterChanger=getFilterChanger();
    int FilterChangerID=getFilterChangerID();

    if (FilterChanger) {
        if (FilterChanger->isFilterChangerConnected(FilterChangerID)) {
            for (unsigned int i=0; i<FilterChanger->getFilterChangerFilterCount(FilterChangerID); i++) {
                cmbFilter->addItem(tr("filter #%1").arg(i+1));
            }
        }
    }


    cmbFilter->setCurrentIndex(cmbFilter->findText(idx));
    connect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
    updateStates();
}

void QFESPIMB040FilterChangerConfig::disConnect() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool conn=actConnect->isChecked();
    QFExtensionFilterChanger* FilterChanger=getFilterChanger();
    int FilterChangerID=getFilterChangerID();

    if (FilterChanger) {
        //qDebug()<<"connecting "<<conn;
        if (conn) {
            FilterChanger->setFilterChangerLogging(m_log);
            FilterChanger->filterChangerConnect(FilterChangerID);
            if (FilterChanger->isFilterChangerConnected(FilterChangerID)) {
                m_log->log_text("connected to FilterChanger driver ...\n");
            } else {
                actConnect->setChecked(false);
                FilterChanger->filterChangerDisonnect(FilterChangerID);
                m_log->log_error("error connecting to FilterChanger driver ...\n");
            }
        } else {
            FilterChanger->filterChangerDisonnect(FilterChangerID);
            m_log->log_text("disconnected from FilterChanger driver ...\n");
        }
    } else {
        actConnect->setChecked(false);
    }
    updateFilters();
    updateStates();
    QApplication::restoreOverrideCursor();
}





void QFESPIMB040FilterChangerConfig::configure() {
    QFExtensionFilterChanger* FilterChanger=getFilterChanger();
    int FilterChangerID=getFilterChangerID();
    if (FilterChanger) FilterChanger->showFilterChangerSettingsDialog(FilterChangerID, this);
    updateStates();
}




QFExtensionFilterChanger* QFESPIMB040FilterChangerConfig::getFilterChanger() {
    return cmbFilterChanger->currentExtensionFilterChanger();
}

QFExtension* QFESPIMB040FilterChangerConfig::getFilterChangerExtension() {
    return cmbFilterChanger->currentExtension();
}

int QFESPIMB040FilterChangerConfig::getFilterChangerID() {

    return cmbFilterChanger->currentFilterChangerID();
}

int QFESPIMB040FilterChangerConfig::getFilterChangerState() {
    QFExtensionFilterChanger* FilterChanger;
    int FilterChangerID;
    FilterChanger=getFilterChanger();
    FilterChangerID=getFilterChangerID();
    if (FilterChanger) {
        return FilterChanger->getFilterChangerCurrentFilter(FilterChangerID);
    }
    return -1;
}


void QFESPIMB040FilterChangerConfig::displayFilterChangerStates(/*bool automatic*/) {
    if (locked) return;
    QFExtensionFilterChanger* FilterChanger;
    int FilterChangerID;
    FilterChanger=getFilterChanger();
    FilterChangerID=getFilterChangerID();
    if (FilterChanger) {
        if (!moving) {
            disconnect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
            cmbFilter->setCurrentIndex(FilterChanger->getFilterChangerCurrentFilter(FilterChangerID));
            connect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
            cmbFilter->setEnabled(true);
        } else {
            cmbFilter->setEnabled(false);
        }

    }

    updateStates();

    //QTimer::singleShot(FilterChangerStateUpdateInterval, this, SLOT(displayFilterChangerStates()));
    if (!locked) {
        timUpdate->setSingleShot(true);
        timUpdate->setInterval(FilterChangerStateUpdateInterval);
        timUpdate->start(FilterChangerStateUpdateInterval);

    }
}

void QFESPIMB040FilterChangerConfig::setFilterChanger(int filter) {
    cmbFilter->setCurrentIndex(filter);
}

void QFESPIMB040FilterChangerConfig::FilterChangerNewFilterSelected(int index) {
    QFExtensionFilterChanger* FilterChanger;
    int FilterChangerID;
    FilterChanger=getFilterChanger();
    FilterChangerID=getFilterChangerID();
    if (FilterChanger) {
        //qDebug()<<"set FilterChanger state opened="<<opened;
        FilterChanger->setFilterChangerFilter(FilterChangerID, index);
        moving=true;
        QTime started=QTime::currentTime();
        while (!FilterChanger->isLastFilterChangerActionFinished(FilterChangerID) && (started.elapsed()<5000)) {
            //qDebug()<<started.elapsed();
            QApplication::processEvents();
        }
        moving=false;
    }
}

void QFESPIMB040FilterChangerConfig::connectFilterChanger() {
    actConnect->setChecked(true);
}

void QFESPIMB040FilterChangerConfig::disconnectFilterChanger() {
    actConnect->setChecked(false);
}

void QFESPIMB040FilterChangerConfig::setReadOnly(bool readonly) {
    cmbFilterChanger->setReadOnly(readonly);
}

void QFESPIMB040FilterChangerConfig::setFilter(int filter) {
    if (isFilterChangerConnected()) {
        if (filter>=0 && filter<cmbFilter->count())
            cmbFilter->setCurrentIndex(filter);
    }
}



bool QFESPIMB040FilterChangerConfig::isFilterChangerConnected() const {
    return actConnect->isChecked();
}


