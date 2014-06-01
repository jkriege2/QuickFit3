#include "qffilterchangerconfigwidget.h"
#include <QtGui>
#include <iostream>


QFFilterChangerConfigWidget::QFFilterChangerConfigWidget(QWidget* parent):
    QFrame(parent)
{

    useThread=true;
    //setFrameStyle(QFrame::Panel|QFrame::Raised);
    //setLineWidth(1);
    setFrameStyle(QFrame::NoFrame);
    setLineWidth(0);
    FilterChangerStateUpdateInterval=251;

    m_thread=new QFFilterChangerConfigWidgetThread(this);
    connect(m_thread, SIGNAL(filterChanged(int)), this, SLOT(filterChanged(int)));


    timUpdate=new QTimer(this);

    m_log=NULL;
    m_pluginServices=NULL;
    locked=false;
    moving=false;


    createWidgets();
    createActions();
    updateStates();


    if (!useThread) {
        timUpdate->setSingleShot(true);
        timUpdate->setInterval(FilterChangerStateUpdateInterval);
        connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayFilterChangerStates()));
        timUpdate->start(FilterChangerStateUpdateInterval);
    }
    //QTimer::singleShot(stageStateUpdateInterval, this, SLOT(displayAxisStates()));
}


QFFilterChangerConfigWidget::~QFFilterChangerConfigWidget()
{
    locked=true;
    disconnect(timUpdate, SIGNAL(timeout()), this, SLOT(displayFilterChangerStates()));
    timUpdate->stop();
    m_thread->stopThread();
}

void QFFilterChangerConfigWidget::unlockFilterChangers() {
    if (useThread) {
        m_thread->start();
    } else {
        connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayFilterChangerStates()));
        timUpdate->setSingleShot(true);
        timUpdate->setInterval(FilterChangerStateUpdateInterval);
        timUpdate->start(FilterChangerStateUpdateInterval);
    }
    locked=false;
}


void QFFilterChangerConfigWidget::lockFilterChangers() {
    locked=true;
    if (useThread)  {
        m_thread->stopThread();
    } else {
        disconnect(timUpdate, SIGNAL(timeout()), this, SLOT(displayFilterChangerStates()));
        timUpdate->stop();
    }
}

void QFFilterChangerConfigWidget::setLog(QFPluginLogService* log) {
    m_log=log;
}

void QFFilterChangerConfigWidget::init(const QString &globalfilterconfig, const QString &filterconfig, QFPluginLogService *log, QFPluginServices *pluginServices) {
    m_filterconfig=filterconfig;
    m_globalfilterconfig=globalfilterconfig;
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

void QFFilterChangerConfigWidget::loadSettings(QSettings& settings, QString prefix) {
    cmbFilterChanger->loadSettings(settings, prefix+"FilterChanger/");

    FilterChangerStateUpdateInterval=settings.value(prefix+"update_interval", FilterChangerStateUpdateInterval).toDouble();

    int fcnt=settings.value(prefix+"filter_count", 0).toInt();
    for (int i=0; i<fcnt; i++) {
        if (i<filters.size()) {
            filters[i]=settings.value(prefix+"filter"+QString::number(i), "").toString();
        } else {
            filters.append(settings.value(prefix+"filter"+QString::number(i), "").toString());
        }
    }
    updateFilters();
}

void QFFilterChangerConfigWidget::loadSettings(QFManyFilesSettings &settings, QString prefix)
{
    cmbFilterChanger->loadSettings(settings, prefix+"FilterChanger/");

    FilterChangerStateUpdateInterval=settings.value(prefix+"update_interval", FilterChangerStateUpdateInterval).toDouble();

    int fcnt=settings.value(prefix+"filter_count", 0).toInt();
    for (int i=0; i<fcnt; i++) {
        if (i<filters.size()) {
            filters[i]=settings.value(prefix+"filter"+QString::number(i), "").toString();
        } else {
            filters.append(settings.value(prefix+"filter"+QString::number(i), "").toString());
        }
    }
    updateFilters();
}

void QFFilterChangerConfigWidget::saveSettings(QSettings& settings, QString prefix) {
    cmbFilterChanger->storeSettings(settings, prefix+"FilterChanger/");
    settings.setValue(prefix+"update_interval", FilterChangerStateUpdateInterval);
    settings.setValue(prefix+"filter_count", filters.size());
    for (int i=0; i<filters.size(); i++) {
        settings.setValue(prefix+"filter"+QString::number(i), filters[i]);
    }
}

void QFFilterChangerConfigWidget::saveSettings(QFManyFilesSettings &settings, QString prefix)
{
    cmbFilterChanger->storeSettings(settings, prefix+"FilterChanger/");
    settings.setValue(prefix+"update_interval", FilterChangerStateUpdateInterval);
    settings.setValue(prefix+"filter_count", filters.size());
    for (int i=0; i<filters.size(); i++) {
        settings.setValue(prefix+"filter"+QString::number(i), filters[i]);
    }
}

void QFFilterChangerConfigWidget::createWidgets() {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QGridLayout* widgetLayout=new QGridLayout();
    setLayout(widgetLayout);
    widgetLayout->setContentsMargins(0,0,0,0);
    widgetLayout->setHorizontalSpacing(2);
    widgetLayout->setVerticalSpacing(2);



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    cmbFilter=new QComboBox(this);
    widgetLayout->addWidget(cmbFilter,0,0,1,2);
    cmbFilter->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
    btnSelectFilters=new QToolButton(this);
    widgetLayout->addWidget(btnSelectFilters,0,2);
    cmbFilterChanger=new QFFilterChangerComboBox(this);
    cmbFilterChanger->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    widgetLayout->addWidget(cmbFilterChanger,0,3);//1,0);
    btnConnect=new QToolButton(this);
    widgetLayout->addWidget(btnConnect,0,4);//1,1);
    btnConfigure=new QToolButton(this);
    widgetLayout->addWidget(btnConfigure,0,5);//1,2);

    QWidget* w=new QWidget(this);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    widgetLayout->addWidget(w,widgetLayout->rowCount()-1,widgetLayout->columnCount());
    widgetLayout->setColumnStretch(widgetLayout->columnCount()-1,1);
    cmbFilterChanger->setEnabled(false);


}

void QFFilterChangerConfigWidget::createActions() {
    actConnect=new QAction(QIcon(":/libqf3widgets/connect_filterchanger.png"), tr("Connect to filter changer driver/hardware ..."), this);
    actConnect->setCheckable(true);
    connect(actConnect, SIGNAL(toggled(bool)), this, SLOT(disConnect()));
    btnConnect->setDefaultAction(actConnect);


    actConfigure=new QAction(QIcon(":/libqf3widgets/configure_filterchanger.png"), tr("Configure filter changer ..."), this);
    connect(actConfigure, SIGNAL(triggered()), this, SLOT(configure()));
    btnConfigure->setDefaultAction(actConfigure);


    actSetFilters=new QAction(QIcon(":/libqf3widgets/filterchanger_selectfilters.png"), tr("select the filters in the filter changer ..."), this);
    connect(actSetFilters, SIGNAL(triggered()), this, SLOT(selectFilters()));
    btnSelectFilters->setDefaultAction(actSetFilters);


}

void QFFilterChangerConfigWidget::updateStates() {
    bool updt=updatesEnabled();
    setUpdatesEnabled(false);
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
            actConnect->setIcon(QIcon(":/libqf3widgets/disconnect_filterchanger.png"));
            actConnect->setText(tr("Disconnect from filter changer driver/hardware ..."));
        } else {
            actConnect->setChecked(false);
            actConnect->setIcon(QIcon(":/libqf3widgets/connect_filterchanger.png"));
            actConnect->setText(tr("Connect from filter changer driver/hardware ..."));
        }
        disconnect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
        cmbFilter->setCurrentIndex(FilterChanger->getFilterChangerCurrentFilter(FilterChangerID));
        connect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
    }
    actConfigure->setEnabled(FilterChanger!=NULL && FilterChangerID>=0);
    actConnect->setEnabled(FilterChanger!=NULL && FilterChangerID>=0);
    cmbFilterChanger->setEnabled(!conn);
    actSetFilters->setEnabled(conn);
    cmbFilter->setEnabled(conn);
    setUpdatesEnabled(updt);
}

void QFFilterChangerConfigWidget::updateFilters() {
    bool updt=updatesEnabled();
    setUpdatesEnabled(false);
    QString idx=cmbFilter->currentText();
    disconnect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
    cmbFilter->clear();
    QFExtensionFilterChanger* FilterChanger=getFilterChanger();
    int FilterChangerID=getFilterChangerID();

    if (FilterChanger) {
        if (FilterChanger->isFilterChangerConnected(FilterChangerID)) {
            for (unsigned int i=0; i<FilterChanger->getFilterChangerFilterCount(FilterChangerID); i++) {
                QString f="---";
                if (i<uint64_t(filters.size())) {
                    if (!filters[i].isEmpty()) {
                        f=filters[i];
                    }
                }
                cmbFilter->addItem(QF3FilterCombobox::getFilterIcon(f, m_globalfilterconfig, m_filterconfig),
                                   tr("#%1: %2").arg(i+1).arg(f), f);
            }
        }
    }


    cmbFilter->setCurrentIndex(cmbFilter->findText(idx));
    connect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
    updateStates();
    setUpdatesEnabled(true);
}

void QFFilterChangerConfigWidget::selectFilters() {
    QFExtensionFilterChanger* FilterChanger=getFilterChanger();
    int FilterChangerID=getFilterChangerID();

    if (!FilterChanger) return;

    QDialog* dlg=new QDialog(this);

    QVBoxLayout* lay=new QVBoxLayout();
    dlg->setLayout(lay);

    QFormLayout* formlayout=new QFormLayout();

    QList<QF3FilterCombobox*> combos;
    for (unsigned int i=0; i<FilterChanger->getFilterChangerFilterCount(FilterChangerID); i++) {
        QF3FilterCombobox* cmb=new QF3FilterCombobox(this);
        combos.append(cmb);
        cmb->setFilterINI(m_globalfilterconfig, m_filterconfig);
        formlayout->addRow(tr("filter #%1:").arg(i+1), cmb);
        if (int64_t(i)<filters.size()) cmb->setCurrentFilter(filters[i]);
    }

    lay->addLayout(formlayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
    lay->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    if ( dlg->exec()==QDialog::Accepted ) {
         //  read back values entered into the widgets and store in settings
        for (int i=0; i<combos.size(); i++) {
            QString fn=combos[i]->filter().name;
            if (!combos[i]->filter().isValid) fn=tr(" --- none ---");
            if (i<filters.size()) {
                filters[i]=fn;
            } else {
                filters.append(fn);
            }
        }
    }
    delete dlg;
    updateFilters();
}

void QFFilterChangerConfigWidget::disConnect() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool conn=actConnect->isChecked();
    QFExtensionFilterChanger* FilterChanger=getFilterChanger();
    int FilterChangerID=getFilterChangerID();

    bool gotConnection=false;

    if (FilterChanger) {
        //qDebug()<<"connecting "<<conn;
        if (conn) {
            FilterChanger->setFilterChangerLogging(m_log);
            FilterChanger->filterChangerConnect(FilterChangerID);
            if (FilterChanger->isFilterChangerConnected(FilterChangerID)) {
                m_log->log_text("connected to FilterChanger driver ...\n");
                gotConnection=true;
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
    if (useThread) {
        if (gotConnection) m_thread->start();
        else m_thread->stopThread();
    }
    QApplication::restoreOverrideCursor();
}





void QFFilterChangerConfigWidget::configure() {
    QFExtensionFilterChanger* FilterChanger=getFilterChanger();
    int FilterChangerID=getFilterChangerID();
    if (FilterChanger) FilterChanger->showFilterChangerSettingsDialog(FilterChangerID, this);
    updateStates();
}




QFExtensionFilterChanger* QFFilterChangerConfigWidget::getFilterChanger() const {
    return cmbFilterChanger->currentExtensionFilterChanger();
}

QFExtension* QFFilterChangerConfigWidget::getFilterChangerExtension() const {
    return cmbFilterChanger->currentExtension();
}

QObject *QFFilterChangerConfigWidget::getFilterChangerExtensionObject() const
{
    return cmbFilterChanger->currentExtensionObject();
}

int QFFilterChangerConfigWidget::getFilterChangerID() const {

    return cmbFilterChanger->currentFilterChangerID();
}

int QFFilterChangerConfigWidget::getFilterChangerState() {
    QFExtensionFilterChanger* FilterChanger;
    int FilterChangerID;
    FilterChanger=getFilterChanger();
    FilterChangerID=getFilterChangerID();
    if (FilterChanger) {
        return FilterChanger->getFilterChangerCurrentFilter(FilterChangerID);
    }
    return -1;
}

QString QFFilterChangerConfigWidget::getCurrentFilter() const {
    if (cmbFilter->currentIndex()<0) return QString("---");
    return cmbFilter->itemData(cmbFilter->currentIndex()).toString();
}

FilterDescription QFFilterChangerConfigWidget::getCurrentFilterDescription() const {
    QString f="---";
    if (cmbFilter->currentIndex()>=0) f=cmbFilter->itemData(cmbFilter->currentIndex()).toString();
    return QF3FilterCombobox::getFilter(f, m_globalfilterconfig, m_filterconfig);
}


void QFFilterChangerConfigWidget::displayFilterChangerStates(/*bool automatic*/) {
    if (locked) return;
    if (useThread) return;
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
    if (!locked && !useThread) {
        timUpdate->setSingleShot(true);
        timUpdate->setInterval(FilterChangerStateUpdateInterval);
        timUpdate->start(FilterChangerStateUpdateInterval);

    }
}

void QFFilterChangerConfigWidget::setFilterChanger(int filter) {
    cmbFilter->setCurrentIndex(filter);
}

void QFFilterChangerConfigWidget::FilterChangerNewFilterSelected(int index) {
    QFExtensionFilterChanger* FilterChanger;
    int FilterChangerID;
    FilterChanger=getFilterChanger();
    FilterChangerID=getFilterChangerID();
    if (FilterChanger) {
        //qDebug()<<"set FilterChanger state opened="<<opened;
        FilterChanger->setFilterChangerFilter(FilterChangerID, index);
        if (!locked) {
            moving=true;
            QTime started=QTime::currentTime();
            cmbFilter->setEnabled(false);
            while (!FilterChanger->isLastFilterChangerActionFinished(FilterChangerID) && (started.elapsed()<20000)) {
                //qDebug()<<started.elapsed();
                QApplication::processEvents();
            }
            moving=false;
        }
    }
}

void QFFilterChangerConfigWidget::connectFilterChanger() {
    bool b=actConnect->signalsBlocked();
    actConnect->blockSignals(true);
    actConnect->setChecked(true);
    disConnect();
    actConnect->blockSignals(b);
}

void QFFilterChangerConfigWidget::disconnectFilterChanger() {
    actConnect->setChecked(false);
}

void QFFilterChangerConfigWidget::setReadOnly(bool readonly) {
    cmbFilterChanger->setReadOnly(readonly);
}

void QFFilterChangerConfigWidget::setFilter(int filter) {
    if (isFilterChangerConnected() && isLastActionComplete() && !moving) {
        if (filter>=0 && filter<cmbFilter->count())
            cmbFilter->setCurrentIndex(filter);
    }
}

void QFFilterChangerConfigWidget::setFilter0() {
    setFilter(0);
}

void QFFilterChangerConfigWidget::setFilter1() {
    setFilter(1);
}

void QFFilterChangerConfigWidget::setFilter2() {
    setFilter(2);
}

void QFFilterChangerConfigWidget::setFilter3() {
    setFilter(3);
}

void QFFilterChangerConfigWidget::setFilter4() {
    setFilter(4);
}

void QFFilterChangerConfigWidget::setFilter5() {
    setFilter(5);
}

void QFFilterChangerConfigWidget::setFilter6() {
    setFilter(6);
}

void QFFilterChangerConfigWidget::setFilter7() {
    setFilter(7);
}

void QFFilterChangerConfigWidget::setFilter8() {
    setFilter(8);
}

void QFFilterChangerConfigWidget::setFilter9() {
    setFilter(9);
}



bool QFFilterChangerConfigWidget::isFilterChangerConnected() const {
    return actConnect->isChecked();
}

bool QFFilterChangerConfigWidget::isLastActionComplete() const {
    QFExtensionFilterChanger* FilterChanger;
    int FilterChangerID;
    FilterChanger=getFilterChanger();
    FilterChangerID=getFilterChangerID();
    if (FilterChanger) {
        //qDebug()<<"set FilterChanger state opened="<<opened;
        return FilterChanger->isLastFilterChangerActionFinished(FilterChangerID);
    }
    return true;
}

void QFFilterChangerConfigWidget::filterChanged(int filter)
{
    if (filter!=cmbFilter->currentIndex()) {
        disconnect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
        cmbFilter->setCurrentIndex(filter);
        connect(cmbFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(FilterChangerNewFilterSelected(int)));
    }
    if (!cmbFilter->isEnabled()) cmbFilter->setEnabled(true);
}


