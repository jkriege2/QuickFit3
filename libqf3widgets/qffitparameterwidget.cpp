#include "qffitparameterwidget.h"

#include <limits.h>

QFFitParameterWidget::QFFitParameterWidget(QFFitParameterBasicInterface* datastore, QString parameterID, WidgetType widget, bool editable, bool displayFix, bool displayError, bool editRange, QWidget* parent):
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
    m_editRange=editRange;
    m_widgetWidth=75;

    neditValue=NULL;
    spinIntValue=NULL;
    spinIntMax=NULL;
    spinIntMin=NULL;
    neditMax=NULL;
    neditMin=NULL;
    labRangeMax=NULL;
    labRangeMin=NULL;
    chkFix=NULL;

    // create widgets:
    layMain=new QHBoxLayout(this);
    layMain->setContentsMargins(0,0,0,0);
    setLayout(layMain);
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
        connect(spinIntValue, SIGNAL(valueChanged(int)), this, SLOT(doubleValueChanged(double)));
    }

    if (displayError) {
        labError=new QLabel(this);
        layMain->addWidget(labError);
    }

    layMain->addStretch();

    if (editable) {
        labRangeMin=new QLabel(tr("min: "), this);
        labRangeMax=new QLabel(tr("max: "), this);
        layMain->addWidget(labRangeMin);
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
            layMain->addWidget(labRangeMax);
            layMain->addWidget(neditMax);
        } else if (widget==IntSpinBox) {
            spinIntMin=new QSpinBox(this);
            spinIntMin->setSuffix(m_unit);
            spinIntMin->setSingleStep(m_increment);
            spinIntMin->setRange(INT_MIN, INT_MAX);
            connect(spinIntMin, SIGNAL(valueChanged(int)), this, SLOT(doubleMinChanged(double)));

            spinIntMax=new QSpinBox(this);
            spinIntMax->setSuffix(m_unit);
            spinIntMax->setSingleStep(m_increment);
            spinIntMax->setRange(INT_MIN, INT_MAX);
            connect(spinIntMax, SIGNAL(valueChanged(int)), this, SLOT(doubleMaxChanged(double)));

            layMain->addWidget(spinIntMin);
            layMain->addWidget(labRangeMax);
            layMain->addWidget(spinIntMax);
        }
    }


    chkFix=new QCheckBox(tr("fix"), this);
    int fixwidth=chkFix->width();
    chkFix->setMinimumWidth(fixwidth);
    chkFix->setMaximumWidth(fixwidth);
    if (displayFix) {
        layMain->addWidget(chkFix);
        connect(chkFix, SIGNAL(toggled(bool)), this, SLOT(sfixChanged(bool)));
    } else {
        delete chkFix;
        chkFix=NULL;
        QWidget* sp=new QWidget(this);
        sp->setMinimumWidth(fixwidth);
        sp->setMaximumWidth(fixwidth);
        layMain->addWidget(sp);
    }
    setEditRange(editRange);
    setWidgetWidth(m_widgetWidth);
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

    if (m_widgetType==FloatEdit) {
        neditValue->setValue(m_datastore->getFitValue(m_parameterID));
        neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
    } else if (m_widgetType==IntSpinBox) {
        spinIntValue->setValue(m_datastore->getFitValue(m_parameterID));
        spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
    }
    if (m_displayError) {
        double error=m_datastore->getFitError(m_parameterID);
        labError->setTextFormat(Qt::RichText);
        if (error != 0) labError->setText(tr("&plusmn; %1 %2").arg(error).arg(m_unit));
        else labError->setText("");
    }
    if (m_displayFix && m_editable) {
        if (chkFix) chkFix->setChecked(m_datastore->getFitFix(m_parameterID));
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
        emit rangeChanged(m_parameterID, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
    }
}

void QFFitParameterWidget::doubleMaxChanged(double value) {
    if ((!m_settingData) && m_editable && m_editRange) {
        m_datastore->setFitMax(m_parameterID, value);
        emit rangeChanged(m_parameterID, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
    }
}

void QFFitParameterWidget::setWidgetWidth(int width) {
    m_widgetWidth=width;
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
}

void QFFitParameterWidget::setUnit(QString unit) {
    m_unit=unit;
    if (neditValue) {
        neditValue->setSuffix(m_unit);
    }
    if (neditMin) {
        neditMin->setSuffix(m_unit);
    }
    if (neditMax) {
        neditMax->setSuffix(m_unit);
    }
    if (spinIntValue) {
        spinIntValue->setSuffix(m_unit);
    }
    if (spinIntMin) {
        spinIntMin->setSuffix(m_unit);
    }
    if (spinIntMax) {
        spinIntMax->setSuffix(m_unit);
    }
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
    m_editRange=editRange;
    if (labRangeMax) labRangeMax->setVisible(editRange);
    if (labRangeMin) labRangeMin->setVisible(editRange);
    if (spinIntMax) spinIntMax->setVisible(editRange);
    if (spinIntMin) spinIntMin->setVisible(editRange);
    if (neditMax) neditMax->setVisible(editRange);
    if (neditMin) neditMin->setVisible(editRange);
}
