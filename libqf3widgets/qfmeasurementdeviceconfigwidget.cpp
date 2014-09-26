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


#include "qfmeasurementdeviceconfigwidget.h"

QFMeasurementDeviceConfigWidget::QFMeasurementDeviceConfigWidget(QWidget *parent, bool doUseThread) :
    QFrame(parent)
{
    useThread=doUseThread;
    m_thread=new QFMeasurementDeviceConfigWidgetThread(this);
    connect(m_thread, SIGNAL(valuesChanged(QTime,QList<QVariant>,QStringList,QList<bool>)), this, SLOT(valuesChanged(QTime,QList<QVariant>,QStringList,QList<bool>)), Qt::QueuedConnection);
    setFrameStyle(QFrame::Panel|QFrame::Raised);
    setLineWidth(1);
    stateUpdateInterval=237;
    dontAccessWidgets=false;

    timUpdate=new QTimer(this);

    m_log=NULL;
    m_pluginServices=NULL;
    locked=false;


    createWidgets();
    createActions();
    updateStates();


    if (!useThread) {
        timUpdate->setSingleShot(true);
        timUpdate->setInterval(stateUpdateInterval);
        connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayStates()));
        timUpdate->start(stateUpdateInterval);
    }
}

QFMeasurementDeviceConfigWidget::~QFMeasurementDeviceConfigWidget()
{
    locked=true;
    disconnect(timUpdate, SIGNAL(timeout()), this, SLOT(displayStates()));
    timUpdate->stop();
    m_thread->stopThread();
}

void QFMeasurementDeviceConfigWidget::init(QFPluginLogService *log, QFPluginServices *pluginServices) {
    m_log=log;
    m_pluginServices=pluginServices;

    if (m_pluginServices) {
        cmbMeasurementDevice->init(m_pluginServices->getExtensionManager());
    } else {
        //stages.clear();
        cmbMeasurementDevice->clear();
    }
    updateStates();
}

void QFMeasurementDeviceConfigWidget::setLog(QFPluginLogService *log) {
    m_log=log;
}

void QFMeasurementDeviceConfigWidget::loadSettings(QSettings &settings, QString prefix) {
    cmbMeasurementDevice->loadSettings(settings, prefix+"MeasurementDevice/");

    stateUpdateInterval=settings.value(prefix+"update_interval", stateUpdateInterval).toDouble();

}

void QFMeasurementDeviceConfigWidget::loadSettings(QFManyFilesSettings &settings, QString prefix)
{
    cmbMeasurementDevice->loadSettings(settings, prefix+"MeasurementDevice/");

    stateUpdateInterval=settings.value(prefix+"update_interval", stateUpdateInterval).toDouble();
}

void QFMeasurementDeviceConfigWidget::saveSettings(QSettings &settings, QString prefix) {
    cmbMeasurementDevice->storeSettings(settings, prefix+"MeasurementDevice/");
    settings.setValue(prefix+"update_interval", stateUpdateInterval);
}

void QFMeasurementDeviceConfigWidget::saveSettings(QFManyFilesSettings &settings, QString prefix)
{
    cmbMeasurementDevice->storeSettings(settings, prefix+"MeasurementDevice/");
    settings.setValue(prefix+"update_interval", stateUpdateInterval);
}

void QFMeasurementDeviceConfigWidget::connectMeasurementDevice() {
    bool b=actConnect->signalsBlocked();
    actConnect->blockSignals(true);
    actConnect->setChecked(true);
    disConnect();
    actConnect->blockSignals(b);
}

void QFMeasurementDeviceConfigWidget::disconnectMeasurementDevice() {
    bool b=actConnect->signalsBlocked();
    actConnect->blockSignals(true);
    actConnect->setChecked(false);
    disConnect();
    actConnect->blockSignals(b);
}


void QFMeasurementDeviceConfigWidget::lockMeasurementDevice() {
    locked=true;
    if (useThread)  {
        m_thread->stopThread();
    } else {
        disconnect(timUpdate, SIGNAL(timeout()), this, SLOT(displayStates()));
        timUpdate->stop();
    }
}

void QFMeasurementDeviceConfigWidget::unlockMeasurementDevice() {
    locked=false;
    if (useThread) {
        m_thread->start();
    } else {
        connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayStates()));
        timUpdate->setSingleShot(true);
        timUpdate->setInterval(stateUpdateInterval);
        timUpdate->start(stateUpdateInterval);
    }
}

void QFMeasurementDeviceConfigWidget::createWidgets() {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    widgetLayout=new QGridLayout();
    setLayout(widgetLayout);
    widgetLayout->setContentsMargins(4,4,4,4);
    widgetLayout->setHorizontalSpacing(2);
    widgetLayout->setVerticalSpacing(2);



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////

    cmbMeasurementDevice=new QFMeasurementDeviceComboBox(this);
    cmbMeasurementDevice->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    widgetLayout->addWidget(cmbMeasurementDevice,0,0);
    btnConnect=new QToolButton(this);
    widgetLayout->addWidget(btnConnect,0,1);
    btnConfigure=new QToolButton(this);
    widgetLayout->addWidget(btnConfigure,0,2);

    linesLayoutWidget=new QWidget(this);
    linesLayout=new QGridLayout();
    linesLayoutWidget->setLayout(linesLayout);
    linesLayout->setContentsMargins(0,0,0,0);
    linesLayout->setHorizontalSpacing(2);
    linesLayout->setVerticalSpacing(2);
    widgetLayout->addWidget(linesLayoutWidget, 1,0,1,3);
    linesLayout->addWidget(new QLabel(tr("<small><b>parameter</b></small>"), linesLayoutWidget), 0, 0);
    linesLayout->addWidget(new QLabel(tr("<small><b>value</b></small>"), linesLayoutWidget), 0, 1);

    stretch=new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Expanding);
    widgetLayout->addItem(stretch, widgetLayout->rowCount()-1,0);


    cmbMeasurementDevice->setEnabled(false);


}

void QFMeasurementDeviceConfigWidget::createActions() {
    actConnect=new QAction(QIcon(":/libqf3widgets/connect_measurementdevice.png"), tr("Connect to measurement device driver/hardware ..."), this);
    actConnect->setCheckable(true);
    connect(actConnect, SIGNAL(toggled(bool)), this, SLOT(disConnect()));
    btnConnect->setDefaultAction(actConnect);


    actConfigure=new QAction(QIcon(":/libqf3widgets/configure_measurementdevice.png"), tr("Configure measurement device ..."), this);
    connect(actConfigure, SIGNAL(triggered()), this, SLOT(configure()));
    btnConfigure->setDefaultAction(actConfigure);

}


QFExtensionMeasurementAndControlDevice *QFMeasurementDeviceConfigWidget::getMeasurementDevice() const {
    return cmbMeasurementDevice->currentExtensionDevice();
}

QFExtension *QFMeasurementDeviceConfigWidget::getMeasurementDeviceExtension() const {
    return cmbMeasurementDevice->currentExtension();
}

QObject *QFMeasurementDeviceConfigWidget::getMeasurementDeviceExtensionObject() const
{
    return cmbMeasurementDevice->currentExtensionObject();
}

QFMeasurementDeviceComboBox *QFMeasurementDeviceConfigWidget::getMeasurementDeviceComboBox() const
{
    return cmbMeasurementDevice;
}

void QFMeasurementDeviceConfigWidget::disableDeviceCombobox()
{
    cmbMeasurementDevice->setVisible(false);
    actConnect->setVisible(false);
    actConfigure->setVisible(false);
}

int QFMeasurementDeviceConfigWidget::getMeasurementDeviceID() const {
    return cmbMeasurementDevice->currentDeviceID();
}

bool QFMeasurementDeviceConfigWidget::isMeasurementDeviceConnected() const {
    return actConnect->isChecked();
}

bool QFMeasurementDeviceConfigWidget::isLastActionComplete() const {
    return true;
}

int QFMeasurementDeviceConfigWidget::getValueCount() const {
    QFExtensionMeasurementAndControlDevice* MeasurementDevice=getMeasurementDevice();
    int MeasurementDeviceID=getMeasurementDeviceID();
    if (MeasurementDevice && isMeasurementDeviceConnected()) {
        return MeasurementDevice->getMeasurementDeviceValueCount(MeasurementDeviceID);
    }
    return 0;
}

QString QFMeasurementDeviceConfigWidget::getValueName(int line) const {
    QFExtensionMeasurementAndControlDevice* MeasurementDevice=getMeasurementDevice();
    int MeasurementDeviceID=getMeasurementDeviceID();
    if (MeasurementDevice && isMeasurementDeviceConnected()) {
        return MeasurementDevice->getMeasurementDeviceValueName(MeasurementDeviceID, line);
    }
    return QString("");
}


QVariant QFMeasurementDeviceConfigWidget::getValue(int line) const {
    QFExtensionMeasurementAndControlDevice* MeasurementDevice=getMeasurementDevice();
    int MeasurementDeviceID=getMeasurementDeviceID();
    if (MeasurementDevice && isMeasurementDeviceConnected()) {
        return MeasurementDevice->getMeasurementDeviceValue(MeasurementDeviceID, line);
    }
    return 0;
}



void QFMeasurementDeviceConfigWidget::updateStates() {
    QFExtensionMeasurementAndControlDevice* MeasurementDevice;
    int MeasurementDeviceID;
    bool conn;


    MeasurementDevice=getMeasurementDevice();
    MeasurementDeviceID=getMeasurementDeviceID();
    //qDebug()<<"updateStates(    MeasurementDevice="<<MeasurementDevice<<", MeasurementDeviceID="<<MeasurementDeviceID<<")";
    conn=false;
    if (MeasurementDevice) {
        conn=MeasurementDevice->isMeasurementDeviceConnected(MeasurementDeviceID);
        if (conn) {
            if (!actConnect->isChecked()) actConnect->setChecked(true);
            QString txt=tr("Disconnect from measurement device driver/hardware ...");
            if (actConnect->text()!=txt) {
                actConnect->setText(txt);
                actConnect->setIcon(QIcon(":/libqf3widgets/disconnect_measurementdevice.png"));
            }
        } else {
            if (actConnect->isChecked()) actConnect->setChecked(false);

            QString txt=tr("Connect from measurement device driver/hardware ...");
            if (actConnect->text()!=txt) {
                actConnect->setText(txt);
                actConnect->setIcon(QIcon(":/libqf3widgets/connect_measurementdevice.png"));
            }
        }
    }
    bool en=(MeasurementDevice!=NULL) && (MeasurementDeviceID>=0);
    if (actConfigure->isEnabled()!=en) actConfigure->setEnabled(en);
    if (!actConnect->isEnabled()) actConnect->setEnabled(true);//(MeasurementDevice!=NULL) && (MeasurementDeviceID>=0));
    if (cmbMeasurementDevice->isEnabled()!=!conn) cmbMeasurementDevice->setEnabled(!conn);
}

void QFMeasurementDeviceConfigWidget::setValueVChanged(const QVariant &value) {
    QFExtensionMeasurementAndControlDevice* MeasurementDevice;
    int MeasurementDeviceID;
    MeasurementDevice=getMeasurementDevice();
    MeasurementDeviceID=getMeasurementDeviceID();
    if (MeasurementDevice) {
        setValue(getMDByWidget(sender()), value);
    }

}

void QFMeasurementDeviceConfigWidget::setValueChanged(double value)
{
    setValueVChanged(value);
}

void QFMeasurementDeviceConfigWidget::setValueChanged(int value)
{
    setValueVChanged(value);
}

void QFMeasurementDeviceConfigWidget::setValueChanged(bool value)
{
    setValueVChanged(value);
}

void QFMeasurementDeviceConfigWidget::setValueChanged(const QString &value)
{
    setValueVChanged(value);
}


void QFMeasurementDeviceConfigWidget::disConnect() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool conn=actConnect->isChecked();
    QFExtensionMeasurementAndControlDevice* MeasurementDevice=getMeasurementDevice();
    int MeasurementDeviceID=getMeasurementDeviceID();
    bool gotConnection=false;

    if (MeasurementDevice) {
        //qDebug()<<"connecting "<<conn;
        if (conn) {
            MeasurementDevice->setMeasurementDeviceLogging(m_log);
            MeasurementDevice->connectMeasurementDevice(MeasurementDeviceID);
            if (MeasurementDevice->isMeasurementDeviceConnected(MeasurementDeviceID)) {
                m_log->log_text("connected to measurement device driver ...\n");
                gotConnection=true;
            } else {
                actConnect->setChecked(false);
                MeasurementDevice->disconnectMeasurementDevice(MeasurementDeviceID);
                m_log->log_error("error connecting to measurement device driver ...\n");
            }
        } else {
            MeasurementDevice->disconnectMeasurementDevice(MeasurementDeviceID);
            m_log->log_text("disconnected from measurement device driver ...\n");
        }
    } else {
        actConnect->setChecked(false);
    }
    updateStates();
    updateMDWidgets();
    if (useThread) {
        if (gotConnection) m_thread->start();
        else m_thread->stopThread();
    }
    QApplication::restoreOverrideCursor();
}

void QFMeasurementDeviceConfigWidget::configure() {
    QFExtensionMeasurementAndControlDevice* MeasurementDevice=getMeasurementDevice();
    int MeasurementDeviceID=getMeasurementDeviceID();
    if (MeasurementDevice) MeasurementDevice->showMeasurementDeviceSettingsDialog(MeasurementDeviceID, this);
    updateMDWidgets();
    updateStates();
}

void QFMeasurementDeviceConfigWidget::updateMDWidgets() {
    dontAccessWidgets=true;
    bool updt=updatesEnabled(); setUpdatesEnabled(false);
    int oldLines=mdWidgets.size();
    int newLines=0;
    QFExtensionMeasurementAndControlDevice* device=getMeasurementDevice();
    int id=getMeasurementDeviceID();
    if (device && isMeasurementDeviceConnected()) {
        newLines=device->getMeasurementDeviceValueCount(id);
    }

    if (newLines<oldLines) {
        for (int i=oldLines-1; i>=newLines; i--) {
            mdWidgets[i].removeFromLayout(linesLayout);
            mdWidgets[i].deleteLater();
            mdWidgets.removeAt(i);
        }
    } else if (newLines>oldLines) {
        for (int i=oldLines; i<newLines; i++) {
            MDWidgets w;
            w.labelName=new QLabel(this);
            int line=widgetLayout->rowCount();
            linesLayout->addWidget(w.labelName, i+1, 0);
            mdWidgets.append(w);
        }
    }

    for (int i=0; i<mdWidgets.size(); i++) {

        MDWidgets& w=mdWidgets[i];
        w.setTitle(device->getMeasurementDeviceValueName(id, i));

        QStringList sl;
        QWidget* wadd=NULL;
        if (device->isMeasurementDeviceValueEditable(id, i)) {
            switch (device->getMeasurementDeviceValueWidget(id, i, &sl)) {
                case QFExtensionMeasurementAndControlDevice::mdDoubleSlider:
                case QFExtensionMeasurementAndControlDevice::mdDoubleEdit:
                    if (!w.spinDouble){
                        w.removeInputFromLayout(linesLayout);
                        w.deleteInputLater();
                        wadd=w.spinDouble=new QDoubleSpinBox(this);

                        connect(w.spinDouble, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
                    }
                    break;
                case QFExtensionMeasurementAndControlDevice::mdIntSlider:
                case QFExtensionMeasurementAndControlDevice::mdIntEdit:
                    if (!w.spinInt){
                        w.removeInputFromLayout(linesLayout);
                        w.deleteInputLater();
                        wadd=w.spinInt=new QSpinBox(this);

                        connect(w.spinInt, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
                    }
                    break;
                case QFExtensionMeasurementAndControlDevice::mdLineEdit:
                    if (!w.lineedit){
                        w.removeInputFromLayout(linesLayout);
                        w.deleteInputLater();

                        wadd=w.lineedit=new QLineEdit(this);

                        connect(w.lineedit, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
                    }
                    break;
            case QFExtensionMeasurementAndControlDevice::mdCheckBox:
                if (!w.checkbox){
                    w.removeInputFromLayout(linesLayout);
                    w.deleteInputLater();

                    wadd=w.checkbox=new QCheckBox(this);
                    w.checkbox->setText("");

                    connect(w.checkbox, SIGNAL(toggled(bool)), this, SLOT(setPowerEditingFinished()));
                }
                break;
                case QFExtensionMeasurementAndControlDevice::mdIntCombobBox:
                    if (!w.cmbInt){
                        w.removeInputFromLayout(linesLayout);
                        w.deleteInputLater();
                        wadd=w.cmbInt=new QComboBox(this);

                        connect(w.cmbInt, SIGNAL(currentIndexChanged(int)), this, SLOT(setPowerEditingFinished()));
                    }
                    break;
                case QFExtensionMeasurementAndControlDevice::mdLabel:
                    if (!w.label){
                        w.removeInputFromLayout(linesLayout);
                        w.deleteInputLater();
                        wadd=w.label=new QLabel(this);
                    }
                    break;
                default: break;
            }

        } else {
            if (!w.label){
                w.removeInputFromLayout(linesLayout);
                w.deleteInputLater();
                wadd=w.label=new QLabel(this);
            }
        }
        if (wadd) linesLayout->addWidget(wadd, i+1, 1);

        double mi=0, ma=0;
        device->getMeasurementDeviceEditableValueRange(id, i, mi, ma);
        w.setRange(mi, ma, sl);
        w.setValue(device->getMeasurementDeviceValue(id, i));

    }

    linesLayoutWidget->setVisible(newLines>0);
    dontAccessWidgets=false;
    setUpdatesEnabled(updt);
}

int QFMeasurementDeviceConfigWidget::getMDByWidget(QObject *widget) {
    int result=-1;
    for (int i=0; i<mdWidgets.size(); i++){
        if (mdWidgets[i].checkbox==widget) return i;
        if (mdWidgets[i].lineedit==widget) return i;
        if (mdWidgets[i].cmbInt==widget) return i;
        if (mdWidgets[i].spinInt==widget) return i;
        if (mdWidgets[i].label==widget) return i;
        if (mdWidgets[i].spinDouble==widget) return i;
    }
    return result;
}

void QFMeasurementDeviceConfigWidget::displayStates() {
    if (useThread) return;
    if (locked) return;

    bool updt=updatesEnabled(); setUpdatesEnabled(false);

    QFExtensionMeasurementAndControlDevice* MeasurementDevice;
    int MeasurementDeviceID;
    MeasurementDevice=getMeasurementDevice();
    MeasurementDeviceID=getMeasurementDeviceID();
    if (MeasurementDevice) {
        for (int i=0; i<mdWidgets.size(); i++) {
            if (mdWidgets[i].checkbox) disconnect(mdWidgets[i].checkbox, SIGNAL(toggled(bool)), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].spinDouble) disconnect(mdWidgets[i].spinDouble, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].spinInt) disconnect(mdWidgets[i].spinInt, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].lineedit) disconnect(mdWidgets[i].lineedit, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].cmbInt) disconnect(mdWidgets[i].cmbInt, SIGNAL(currentIndexChanged(int)), this, SLOT(setPowerEditingFinished()));

            mdWidgets[i].setTitle(MeasurementDevice->getMeasurementDeviceValueName(MeasurementDeviceID, i));
            mdWidgets[i].setValueUpdate(MeasurementDevice->getMeasurementDeviceValue(MeasurementDeviceID, i));

            if (mdWidgets[i].checkbox) connect(mdWidgets[i].checkbox, SIGNAL(toggled(bool)), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].spinDouble) connect(mdWidgets[i].spinDouble, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].spinInt) connect(mdWidgets[i].spinInt, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].lineedit) connect(mdWidgets[i].lineedit, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].cmbInt) connect(mdWidgets[i].cmbInt, SIGNAL(currentIndexChanged(int)), this, SLOT(setPowerEditingFinished()));
        }
    }

    updateStates();
    setUpdatesEnabled(updt);

    //QTimer::singleShot(stateUpdateInterval, this, SLOT(displayMeasurementDeviceStates()));
    if (!locked) {
        if (!useThread)  {
            timUpdate->setSingleShot(true);
            timUpdate->setInterval(stateUpdateInterval);
            timUpdate->start(stateUpdateInterval);
        }

    }
}

void QFMeasurementDeviceConfigWidget::setReadOnly(bool readonly) {
    cmbMeasurementDevice->setReadOnly(readonly);
}


void QFMeasurementDeviceConfigWidget::setValue(int line, const QVariant &power) {
    QFExtensionMeasurementAndControlDevice* MeasurementDevice=getMeasurementDevice();
    int MeasurementDeviceID=getMeasurementDeviceID();
    if (MeasurementDevice) {
        lockMeasurementDevice();
        //qDebug()<<"set param no. "<<line<<" to "<<power;
        MeasurementDevice->setMeasurementDeviceValue(MeasurementDeviceID, line, power);
        //qDebug()<<"set param no. "<<line<<" to "<<power<<"   DONE";
        unlockMeasurementDevice();

    }
}

void QFMeasurementDeviceConfigWidget::valuesChanged(QTime time, QList<QVariant> values, QStringList lineNames, QList<bool> widgetsEnabled) {
    if (dontAccessWidgets) return;
    bool updt=updatesEnabled(); setUpdatesEnabled(false);
    //qDebug()<<"QFMeasurementDeviceConfigWidget::valuesChanged("<< time<< values <<lineNames<<")";
    for (int i=0; i<mdWidgets.size(); i++) {
        if (i<values.size()) {

            if (mdWidgets[i].checkbox) disconnect(mdWidgets[i].checkbox, SIGNAL(toggled(bool)), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].spinDouble) disconnect(mdWidgets[i].spinDouble, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].spinInt) disconnect(mdWidgets[i].spinInt, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].lineedit) disconnect(mdWidgets[i].lineedit, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].cmbInt) disconnect(mdWidgets[i].cmbInt, SIGNAL(currentIndexChanged(int)), this, SLOT(setPowerEditingFinished()));

            mdWidgets[i].setTitle(lineNames.value(i, QString()));
            mdWidgets[i].setValueUpdate(values.value(i, QVariant()));

            if (mdWidgets[i].checkbox) connect(mdWidgets[i].checkbox, SIGNAL(toggled(bool)), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].spinDouble) connect(mdWidgets[i].spinDouble, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].spinInt) connect(mdWidgets[i].spinInt, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].lineedit) connect(mdWidgets[i].lineedit, SIGNAL(editingFinished()), this, SLOT(setPowerEditingFinished()));
            if (mdWidgets[i].cmbInt) connect(mdWidgets[i].cmbInt, SIGNAL(currentIndexChanged(int)), this, SLOT(setPowerEditingFinished()));
        }
    }
    updateStates();
    setUpdatesEnabled(updt);
}

void QFMeasurementDeviceConfigWidget::setPowerEditingFinished() {
    {
        QDoubleSpinBox* spin=qobject_cast<QDoubleSpinBox*>(sender());
        if (spin) {
            setValueChanged(spin->value());
        }
    }
    {
        QSpinBox* spin=qobject_cast<QSpinBox*>(sender());
        if (spin) {
            setValueChanged(spin->value());
        }
    }
    {
        QCheckBox* spin=qobject_cast<QCheckBox*>(sender());
        if (spin) {
            setValueChanged(spin->isChecked());
        }
    }
    {
        QLineEdit* spin=qobject_cast<QLineEdit*>(sender());
        if (spin) {
            setValueChanged(spin->text());
        }
    }
    {
        QComboBox* spin=qobject_cast<QComboBox*>(sender());
        if (spin) {
            setValueChanged(spin->itemData(spin->currentIndex()).toInt());
        }
    }
    {
        QLabel* spin=qobject_cast<QLabel*>(sender());
        if (spin) {
            //setValueChanged(spin->itemData(spin->currentIndex()).toInt());
        }
    }

}





void QFMeasurementDeviceConfigWidget::MDWidgets::deleteLater()
{
    deleteInputLater();
    if (labelName) {
        labelName->hide();
        labelName->deleteLater();
        labelName=NULL;
    }
}

void QFMeasurementDeviceConfigWidget::MDWidgets::deleteInputLater()
{
    if (checkbox) {
        checkbox->hide();
        checkbox->deleteLater();
        checkbox=NULL;
    }
    if (lineedit) {
        lineedit->hide();
        lineedit->deleteLater();
        lineedit=NULL;
    }
    if (spinDouble) {
        spinDouble->hide();
        spinDouble->deleteLater();
        spinDouble=NULL;
    }
    if (spinInt) {
        spinInt->hide();
        spinInt->deleteLater();
        spinInt=NULL;
    }
    if (cmbInt) {
        cmbInt->hide();
        cmbInt->deleteLater();
        cmbInt=NULL;
    }
    if (label) {
        label->hide();
        label->deleteLater();
        label=NULL;
    }
}

void QFMeasurementDeviceConfigWidget::MDWidgets::removeFromLayout(QGridLayout *layout)
{
    removeInputFromLayout(layout);
    if (labelName) layout->removeWidget(labelName);
}

void QFMeasurementDeviceConfigWidget::MDWidgets::removeInputFromLayout(QGridLayout *layout)
{
    if (checkbox) layout->removeWidget(checkbox);
    if (lineedit) layout->removeWidget(lineedit);
    if (spinDouble) layout->removeWidget(spinDouble);
    if (spinInt) layout->removeWidget(spinInt);
    if (cmbInt) layout->removeWidget(cmbInt);
    if (label) layout->removeWidget(label);

}

void QFMeasurementDeviceConfigWidget::MDWidgets::setValue(const QVariant &value)
{
    if (checkbox) {
        checkbox->setChecked(value.toBool());
    }
    if (lineedit) {
        lineedit->setText(value.toString());
    }
    if (spinDouble) {
        spinDouble->setValue(value.toDouble());
    }
    if (spinInt) {
        spinInt->setValue(value.toInt());
    }
    if (cmbInt) {
        int idx=cmbInt->findData(value.toInt());
        cmbInt->setCurrentIndex(idx);
    }
    if (label) {
        label->setText(value.toString());
    }

}

void QFMeasurementDeviceConfigWidget::MDWidgets::setValueUpdate(const QVariant &value)
{
    if (checkbox) {
        if (value.toBool()!=checkbox->isChecked() && !checkbox->hasFocus()) checkbox->setChecked(value.toBool());
    }
    if (lineedit) {
        if (value.toString()!=lineedit->text() && !lineedit->hasFocus()) lineedit->setText(value.toString());
    }
    if (spinDouble) {
        if (value.toDouble()!=spinDouble->value() && !spinDouble->hasFocus()) spinDouble->setValue(value.toDouble());
    }
    if (spinInt) {
        if (value.toInt()!=spinInt->value() && !spinInt->hasFocus()) spinInt->setValue(value.toInt());
    }
    if (cmbInt) {
        int idx=cmbInt->findData(value.toInt());
        if (idx!=cmbInt->currentIndex() && !cmbInt->hasFocus()) cmbInt->setCurrentIndex(idx);
    }
    if (label) {
        label->setText(value.toString());
    }
}

void QFMeasurementDeviceConfigWidget::MDWidgets::setTitle(const QString &title)
{
    if (labelName) {
        labelName->setText(title);
    }
}

void QFMeasurementDeviceConfigWidget::MDWidgets::setRange(double mi, double ma, const QStringList &sl)
{
    if (spinDouble) {
        if (spinDouble->minimum()!=mi) spinDouble->setMinimum(mi);
        if (spinDouble->maximum()!=ma) spinDouble->setMaximum(ma);
    }
    if (spinInt) {
        if (spinInt->minimum()!=mi) spinInt->setMinimum(mi);
        if (spinInt->maximum()!=ma) spinInt->setMaximum(ma);
    }
    if (cmbInt) {
        cmbInt->setUpdatesEnabled(false);
        cmbInt->clear();
        int cnt=0;
        for (int cbi=mi; cbi<=ma; cbi++) {
            cmbInt->addItem(sl.value(cnt, QString::number(cbi)));
            cnt++;
        }
        cmbInt->setUpdatesEnabled(true);
    }
}

QVariant QFMeasurementDeviceConfigWidget::MDWidgets::getValue()
{
    if (checkbox) {
        return checkbox->isChecked();
    }
    if (lineedit) {
        return lineedit->text();
    }
    if (spinDouble) {
        return spinDouble->value();
    }
    if (spinInt) {
        return spinInt->value();
    }
    if (cmbInt) {
        return cmbInt->itemData(cmbInt->currentIndex());
    }
    if (label) {
        return label->text();
    }

    return QVariant();
}
