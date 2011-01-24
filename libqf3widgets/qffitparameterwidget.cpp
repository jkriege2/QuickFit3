#include "qffitparameterwidget.h"
#include "tools.h"

#include <limits.h>

QFFitParameterWidget::QFFitParameterWidget(QFFitParameterBasicInterface* datastore, QString parameterID, WidgetType widget, bool editable, bool displayFix, bool displayError, bool editRangeAllowed, QWidget* parent):
    QWidget(parent)
{
    m_datastore=datastore;
    m_parameterID=parameterID;
    m_widgetType=widget;
    m_editable=editable;
    m_displayFix=displayFix;
    m_displayError=displayError;
    m_unit="";
    m_settingData=true;
    m_increment=1;
    m_editRange=editRangeAllowed;
    m_editRangeAllowed=editRangeAllowed;
    m_widgetWidth=75;
    m_checkWidth=24;

    neditValue=NULL;
    spinIntValue=NULL;
    spinIntMax=NULL;
    spinIntMin=NULL;
    neditMax=NULL;
    neditMin=NULL;
    chkFix=NULL;
    spCheck=NULL;
    hlabFix=NULL;
    hlabMax=NULL;
    hlabMin=NULL;
    hlabValue=NULL;
    labError=NULL;

    // create widgets:
    layMain=new QHBoxLayout(this);
    layMain->setContentsMargins(0,0,0,0);
    layMain->setMargin(1);
    setLayout(layMain);

    chkFix=new QCheckBox(this);
    chkFix->setMinimumWidth(m_checkWidth);
    chkFix->setMaximumWidth(m_checkWidth);
    if (displayFix) {
        if (widget==Header) {
            delete chkFix;
            chkFix=NULL;
            hlabFix=new QLabel(tr("<b>fix</b>"), this);
            layMain->addWidget(hlabFix);
        } else {
            layMain->addWidget(chkFix);
            connect(chkFix, SIGNAL(toggled(bool)), this, SLOT(sfixChanged(bool)));
        }
    } else {
        delete chkFix;
        chkFix=NULL;
        spCheck=new QWidget(this);
        spCheck->setMinimumWidth(m_checkWidth);
        spCheck->setMaximumWidth(m_checkWidth);
        layMain->addWidget(spCheck);
    }

    if (widget==FloatEdit) {
        neditValue=new NumberEdit(this);
        neditValue->setRange(datastore->getFitMin(parameterID), datastore->getFitMax(parameterID));
        layMain->addWidget(neditValue);
        neditValue->setSuffix(m_unit);
        neditValue->setSingleStep(m_increment);
        neditValue->setReadOnly(!editable);
        neditValue->setEnabled(editable);
        connect(neditValue, SIGNAL(valueChanged(double)), this, SLOT(doubleValueChanged(double)));
    } else if (widget==IntSpinBox) {
        spinIntValue=new QSpinBox(this);
        spinIntValue->setRange(datastore->getFitMin(parameterID), datastore->getFitMax(parameterID));
        spinIntValue->setSuffix(m_unit);
        spinIntValue->setSingleStep(m_increment);
        spinIntValue->setReadOnly(!editable);
        spinIntValue->setEnabled(editable);
        layMain->addWidget(spinIntValue);
        connect(spinIntValue, SIGNAL(valueChanged(int)), this, SLOT(intValueChanged(int)));
    } else if (widget==Header) {
        hlabValue=new QLabel(tr("<b>value</b>"), this);
        layMain->addWidget(hlabValue);
    }

    if (displayError) {
        labError=new QLabel(this);
        layMain->addWidget(labError);
        if (widget==Header) {
            labError->setText(tr("<b>&plusmn; error</b>"));
        }
        /*if (widget==Header) {
            hlabValue->setText(tr("<b>value &plusmn; error</b>"));
        } else {
            labError=new QLabel(this);
            layMain->addWidget(labError);
        }*/
    }

    layMain->addStretch();

    if (editable) {
        if (widget==FloatEdit) {
            neditMin=new NumberEdit(this);
            neditMin->setCheckBounds(false, false);
            neditMin->setSuffix(m_unit);
            neditMin->setSingleStep(m_increment);
            connect(neditMin, SIGNAL(valueChanged(double)), this, SLOT(doubleMinChanged(double)));

            neditMax=new NumberEdit(this);
            neditMax->setCheckBounds(false, false);
            neditMax->setSuffix(m_unit);
            neditMax->setSingleStep(m_increment);
            connect(neditMax, SIGNAL(valueChanged(double)), this, SLOT(doubleMaxChanged(double)));

            layMain->addWidget(neditMin);
            layMain->addWidget(neditMax);
        } else if (widget==IntSpinBox) {
            spinIntMin=new QSpinBox(this);
            spinIntMin->setSuffix(m_unit);
            spinIntMin->setSingleStep(m_increment);
            spinIntMin->setRange(INT_MIN, INT_MAX);
            connect(spinIntMin, SIGNAL(valueChanged(int)), this, SLOT(intMinChanged(int)));

            spinIntMax=new QSpinBox(this);
            spinIntMax->setSuffix(m_unit);
            spinIntMax->setSingleStep(m_increment);
            spinIntMax->setRange(INT_MIN, INT_MAX);
            connect(spinIntMax, SIGNAL(valueChanged(int)), this, SLOT(intMaxChanged(int)));

            layMain->addWidget(spinIntMin);
            layMain->addWidget(spinIntMax);
        } else if (widget==Header) {
            hlabMin=new QLabel(tr("<b>minimum</b>"), this);
            layMain->addWidget(hlabMin);
            hlabMax=new QLabel(tr("<b>maximum</b>"), this);
            layMain->addWidget(hlabMax);
        }
    }


    setEditRange(editRangeAllowed);
    setWidgetWidth(m_widgetWidth, m_checkWidth);
    setUnit(m_unit);
    setIncrement(m_increment);
    m_settingData=false;
    reloadValues();
}

QFFitParameterWidget::~QFFitParameterWidget()
{
    //dtor
}


void QFFitParameterWidget::reloadValues() {
    bool old_m_settingData=m_settingData;
    m_settingData=true;

    if (m_editRange) {
        if (neditMin) neditMin->setValue(m_datastore->getFitMin(m_parameterID));
        if (neditMax) neditMax->setValue(m_datastore->getFitMax(m_parameterID));
        if (spinIntMin) spinIntMin->setValue(m_datastore->getFitMin(m_parameterID));
        if (spinIntMax) spinIntMax->setValue(m_datastore->getFitMax(m_parameterID));
    }

    double value=m_datastore->getFitValue(m_parameterID);

    if (m_widgetType==FloatEdit) {
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (neditValue && (neditValue->value()!=value)) neditValue->setValue(value);
    } else if (m_widgetType==IntSpinBox) {
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue && (spinIntValue->value()!=value)) spinIntValue->setValue(value);
    }
    if (m_displayError && labError && (m_widgetType!=Header)) {
        double error=m_datastore->getFitError(m_parameterID);
        labError->setTextFormat(Qt::RichText);
        labError->setText(tr("&plusmn; %1").arg(floattohtmlstr(error, 2, true).c_str()));
    }
    if (m_displayFix && m_editable && chkFix) {
        chkFix->setChecked(m_datastore->getFitFix(m_parameterID));
    }
    m_settingData=old_m_settingData;
}

void QFFitParameterWidget::setValue(double value, double error, bool writeback) {
    bool old_m_settingData=m_settingData;
    m_settingData=true;

    reloadValues();

    if (writeback) {
        m_datastore->setFitValue(m_parameterID, value);
    }

    if (m_widgetType==FloatEdit) {
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (neditValue && (neditValue->value()!=value)) neditValue->setValue(value);
    } else if (m_widgetType==IntSpinBox) {
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue && (spinIntValue->value()!=value)) spinIntValue->setValue(value);
    }
    if (m_displayError && labError && (m_widgetType!=Header)) {
        labError->setTextFormat(Qt::RichText);
        labError->setText(tr("&plusmn; %1").arg(floattohtmlstr(error, 2, true).c_str()));
    }

    m_settingData=old_m_settingData;
}

void QFFitParameterWidget::doubleValueChanged(double value) {
    if ((!m_settingData) && m_editable) {
        m_datastore->setFitValue(m_parameterID, value);
        emit valueChanged(m_parameterID, value);
    }
}

void QFFitParameterWidget::intValueChanged(int value) {
    if ((!m_settingData) && m_editable) {
        m_datastore->setFitValue(m_parameterID, value);
        emit valueChanged(m_parameterID, value);
    }
}

void QFFitParameterWidget::sfixChanged(bool fix) {
    if ((!m_settingData) && m_editable) {
        m_datastore->setFitFix(m_parameterID, fix);
        emit fixChanged(m_parameterID, fix);
    }
}

void QFFitParameterWidget::doubleMinChanged(double value) {
    if ((!m_settingData) && m_editable && m_editRange) {
        m_datastore->setFitMin(m_parameterID, value);
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        emit rangeChanged(m_parameterID, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
    }
}

void QFFitParameterWidget::doubleMaxChanged(double value) {
    if ((!m_settingData) && m_editable && m_editRange) {
        m_datastore->setFitMax(m_parameterID, value);
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        emit rangeChanged(m_parameterID, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
    }
}

void QFFitParameterWidget::intMinChanged(int value) {
    if ((!m_settingData) && m_editable && m_editRange) {
        m_datastore->setFitMin(m_parameterID, value);
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        emit rangeChanged(m_parameterID, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
    }
}

void QFFitParameterWidget::intMaxChanged(int value) {
    if ((!m_settingData) && m_editable && m_editRange) {
        m_datastore->setFitMax(m_parameterID, value);
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        emit rangeChanged(m_parameterID, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
    }
}

void QFFitParameterWidget::setWidgetWidth(int width, int fixWidth) {
    m_widgetWidth=width;
    m_checkWidth=fixWidth;
    if (neditValue) {
        neditValue->setMinimumWidth(m_widgetWidth);
        neditValue->setMaximumWidth(m_widgetWidth);
    }
    if (neditMin) {
        neditMin->setMinimumWidth(m_widgetWidth);
        neditMin->setMaximumWidth(m_widgetWidth);
    }
    if (neditMax) {
        neditMax->setMinimumWidth(m_widgetWidth);
        neditMax->setMaximumWidth(m_widgetWidth);
    }
    if (spinIntValue) {
        spinIntValue->setMinimumWidth(m_widgetWidth);
        spinIntValue->setMaximumWidth(m_widgetWidth);
    }
    if (spinIntMin) {
        spinIntMin->setMinimumWidth(m_widgetWidth);
        spinIntMin->setMaximumWidth(m_widgetWidth);
    }
    if (spinIntMax) {
        spinIntMax->setMinimumWidth(m_widgetWidth);
        spinIntMax->setMaximumWidth(m_widgetWidth);
    }
    if (labError) {
        labError->setMinimumWidth(m_widgetWidth);
        labError->setMaximumWidth(m_widgetWidth);
    }
    if (chkFix) {
        chkFix->setMinimumWidth(m_checkWidth);
        chkFix->setMaximumWidth(m_checkWidth);
    }
    if (spCheck) {
        spCheck->setMinimumWidth(m_checkWidth);
        spCheck->setMaximumWidth(m_checkWidth);
    }
    if (hlabFix) {
        hlabFix->setMinimumWidth(m_checkWidth);
        hlabFix->setMaximumWidth(m_checkWidth);
    }
    if (hlabMax) {
        hlabMax->setMinimumWidth(m_widgetWidth);
        hlabMax->setMaximumWidth(m_widgetWidth);
    }
    if (hlabMin) {
        hlabMin->setMinimumWidth(m_widgetWidth);
        hlabMin->setMaximumWidth(m_widgetWidth);
    }
    if (hlabValue) {
        hlabValue->setMinimumWidth(m_widgetWidth);
        hlabValue->setMaximumWidth(m_widgetWidth);
        /*if (m_displayError) {
            hlabValue->setMinimumWidth(2*m_widgetWidth+layMain->margin());
            hlabValue->setMaximumWidth(2*m_widgetWidth+layMain->margin());
        } else {
            hlabValue->setMinimumWidth(m_widgetWidth);
            hlabValue->setMaximumWidth(m_widgetWidth);
        }*/
    }
}

void QFFitParameterWidget::setUnit(QString unit) {
    m_unit=unit;
}

void QFFitParameterWidget::setIncrement(double increment) {
    m_increment=increment;
    if (neditValue) {
        neditValue->setSingleStep(m_increment);
    }
    if (neditMin) {
        neditMin->setSingleStep(m_increment);
    }
    if (neditMax) {
        neditMax->setSingleStep(m_increment);
    }
    if (spinIntValue) {
        spinIntValue->setSingleStep(m_increment);
    }
    if (spinIntMin) {
        spinIntMin->setSingleStep(m_increment);
    }
    if (spinIntMax) {
        spinIntMax->setSingleStep(m_increment);
    }
}

void QFFitParameterWidget::setEditRange(bool editRange) {
    if (!m_editRangeAllowed) {
        m_editRange=false;
        if (spinIntMax) spinIntMax->setVisible(false);
        if (spinIntMin) spinIntMin->setVisible(false);
        if (neditMax) neditMax->setVisible(false);
        if (neditMin) neditMin->setVisible(false);
        if (hlabMin) hlabMin->setVisible(false);
        if (hlabMax) hlabMax->setVisible(false);
    } else {
        m_editRange=editRange;
        if (spinIntMax) spinIntMax->setVisible(editRange);
        if (spinIntMin) spinIntMin->setVisible(editRange);
        if (neditMax) neditMax->setVisible(editRange);
        if (neditMin) neditMin->setVisible(editRange);
        if (hlabMin) hlabMin->setVisible(editRange);
        if (hlabMax) hlabMax->setVisible(editRange);
    }
}
