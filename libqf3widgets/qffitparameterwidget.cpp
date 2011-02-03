#include "qffitparameterwidget.h"
#include "tools.h"

#include <limits.h>
#include <cfloat>

#define COL_LABEL 0
#define STRETCH_LABEL 0
#define COL_FIX 1
#define STRETCH_FIX 0
#define COL_VALUE 2
#define STRETCH_VALUE 2
#define COL_ERROR 3
#define STRETCH_ERROR 1
#define COL_MIN 4
#define STRETCH_MIN 1
#define COL_MAX 5
#define STRETCH_MAX 1


double roundError(double error, int addSignifcant) {
    if (fabs(error)<DBL_MIN*10.0) return error;
    int sbits_error=ceil(log(fabs(error))/log(10.0));
    double f=pow(10.0, sbits_error-1-addSignifcant);
    return round(error/f)*f;
}

#define QFFitParameterWidget_setMaxHeight(widget, height) { if (widget) { widget->setMaximumHeight(height); } }

QFFitParameterWidget::QFFitParameterWidget(QFFitParameterBasicInterface* datastore,  QGridLayout* layout, int row, QString parameterID, WidgetType widget, bool editable, bool displayFix, bool displayError, bool editRangeAllowed, QWidget* parent, QString label):
    QObject(parent)
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
    m_layout=layout;
    m_label=label;
    m_paramDescription="";
    m_parent=parent;
    m_visible=true;

    neditValue=NULL;
    spinIntValue=NULL;
    spinIntMax=NULL;
    spinIntMin=NULL;
    neditMax=NULL;
    neditMin=NULL;
    chkFix=NULL;
    labLabel=NULL;
    hlabFix=NULL;
    hlabMax=NULL;
    hlabMin=NULL;
    hlabValue=NULL;
    labError=NULL;
    cmbIntValue=NULL;
    scSpace=NULL;
    scReturn=NULL;
    scEnter=NULL;

    actCopyValue = new QAction(tr("copy &value to all files"), this);
    connect(actCopyValue, SIGNAL(triggered()), this, SLOT(s_actCopyValue()));
    actCopyFix = new QAction(tr("copy &fix to all files"), this);
    connect(actCopyFix, SIGNAL(triggered()), this, SLOT(s_actCopyFix()));
    actCopyValueFix = new QAction(tr("copy value && fix to all files"), this);
    connect(actCopyValueFix, SIGNAL(triggered()), this, SLOT(s_actCopyValueFix()));
    actCopyValueInit = new QAction(tr("copy &value to initial"), this);
    connect(actCopyValueInit, SIGNAL(triggered()), this, SLOT(s_actCopyValueInit()));
    actCopyFixInit = new QAction(tr("copy &fix to initial"), this);
    connect(actCopyFixInit, SIGNAL(triggered()), this, SLOT(s_actCopyFixInit()));
    actCopyValueFixInit = new QAction(tr("copy value && fix to initial"), this);
    connect(actCopyValueFixInit, SIGNAL(triggered()), this, SLOT(s_actCopyValueFixInit()));


    actResetValue = new QAction(tr("reset this value"), this);
    connect(actResetValue, SIGNAL(triggered()), this, SLOT(s_actResetValue()));
    actResetFix = new QAction(tr("reset this fix"), this);
    connect(actResetFix, SIGNAL(triggered()), this, SLOT(s_actResetFix()));
    actResetValueFix = new QAction(tr("reset this value && fix"), this);
    connect(actResetValueFix, SIGNAL(triggered()), this, SLOT(s_actResetValueFix()));


    int height=5;

    //int row=row;

    if (!label.isEmpty()) {
        labLabel=new QLabel(label, parent);
        layout->addWidget(labLabel, row, COL_LABEL);
        if (widget==Header) labLabel->setText("<b>"+label+"</b>");
        labLabel->setAlignment(Qt::AlignLeft);
        height=qMax(height, labLabel->minimumSizeHint().height());
    }

    if (displayFix) {
        if (widget==Header) {
            hlabFix=new QLabel(tr("<b>fix</b>"), parent);
            hlabFix->setAlignment(Qt::AlignHCenter);
            layout->addWidget(hlabFix, row, COL_FIX);
            height=qMax(height, hlabFix->minimumSizeHint().height());
        } else {
            chkFix=new QCheckBox(parent);
            chkFix->setFocusPolicy(Qt::NoFocus);
            layout->addWidget(chkFix, row, COL_FIX);
            connect(chkFix, SIGNAL(toggled(bool)), this, SLOT(sfixChanged(bool)));
            height=qMax(height, chkFix->minimumSizeHint().height());
        }
    }

    if (widget==FloatEdit) {
        neditValue=new JKDoubleEdit(parent);
        neditValue->setRange(datastore->getFitMin(parameterID), datastore->getFitMax(parameterID));
        layout->addWidget(neditValue, row, COL_VALUE);
        neditValue->setSingleStep(m_increment);
        neditValue->setReadOnly(!editable);
        if (editable) {
            neditValue->setBackgroundColor(m_parent->palette().color(QPalette::Base));
        } else {
            neditValue->setBackgroundColor(m_parent->palette().color(QPalette::Window));
        }
        neditValue->setShowUpDown(false);
        connect(neditValue, SIGNAL(valueChanged(double)), this, SLOT(doubleValueChanged(double)));
        height=qMax(height, neditValue->minimumSizeHint().height());
        neditValue->addContextmenuAction(actCopyValue);
        neditValue->addContextmenuAction(actCopyFix);
        neditValue->addContextmenuAction(actCopyValueFix);
        neditValue->addContextmenuAction(actCopyValueInit);
        neditValue->addContextmenuAction(actCopyFixInit);
        neditValue->addContextmenuAction(actCopyValueFixInit);
        neditValue->addContextmenuAction(actResetValue);
        neditValue->addContextmenuAction(actResetFix);
        neditValue->addContextmenuAction(actResetValueFix);

        if (chkFix) {
            scSpace=new QShortcut(QKeySequence(Qt::Key_Space), neditValue);
            connect(scSpace, SIGNAL(activated()), chkFix, SLOT(toggle()));
        }
        scEnter=new QShortcut(QKeySequence(Qt::Key_Enter), neditValue);
        connect(scEnter, SIGNAL(activated()), this, SLOT(pEnterPressed()));
        scReturn=new QShortcut(QKeySequence(Qt::Key_Return), neditValue);
        connect(scReturn, SIGNAL(activated()), this, SLOT(pEnterPressed()));
        // TODO: Enter/Space do not work ... use activatedAmbiguously()?

    } else if (widget==IntSpinBox) {
        spinIntValue=new QSpinBox(parent);
        spinIntValue->setRange(datastore->getFitMin(parameterID), datastore->getFitMax(parameterID));
        spinIntValue->setSingleStep(m_increment);
        spinIntValue->setReadOnly(!editable);
        spinIntValue->setEnabled(editable);
        layout->addWidget(spinIntValue, row, COL_VALUE);
        connect(spinIntValue, SIGNAL(valueChanged(int)), this, SLOT(intValueChanged(int)));
        height=qMax(height, spinIntValue->minimumSizeHint().height());

        if (chkFix) {
            scSpace=new QShortcut(QKeySequence(Qt::Key_Space), spinIntValue);
            connect(scSpace, SIGNAL(activated()), chkFix, SLOT(toggle()));
        }
        scEnter=new QShortcut(QKeySequence(Qt::Key_Enter), spinIntValue);
        connect(scEnter, SIGNAL(activated()), this, SLOT(pEnterPressed()));
        scReturn=new QShortcut(QKeySequence(Qt::Key_Return), spinIntValue);
        connect(scReturn, SIGNAL(activated()), this, SLOT(pEnterPressed()));
    } else if (widget==IntDropDown) {
        cmbIntValue=new QComboBox(parent);
        fillCombo(cmbIntValue, datastore->getFitMin(parameterID), datastore->getFitMax(parameterID));
        cmbIntValue->setEditable(false);
        cmbIntValue->setEnabled(editable);
        layout->addWidget(cmbIntValue, row, COL_VALUE);
        connect(cmbIntValue, SIGNAL(currentIndexChanged(int)), this, SLOT(intValueChangedFromCombo(int)));
        height=qMax(height, cmbIntValue->minimumSizeHint().height());

        if (chkFix) {
            scSpace=new QShortcut(QKeySequence(Qt::Key_Space), cmbIntValue);
            connect(scSpace, SIGNAL(activated()), chkFix, SLOT(toggle()));
        }
        scEnter=new QShortcut(QKeySequence(Qt::Key_Enter), cmbIntValue);
        connect(scEnter, SIGNAL(activated()), this, SLOT(pEnterPressed()));
        scReturn=new QShortcut(QKeySequence(Qt::Key_Return), cmbIntValue);
        connect(scReturn, SIGNAL(activated()), this, SLOT(pEnterPressed()));
    } else if (widget==Header) {
        hlabValue=new QLabel(tr("<b>value</b>"), parent);
        hlabValue->setAlignment(Qt::AlignHCenter);
        layout->addWidget(hlabValue, row, COL_VALUE);
        height=qMax(height, hlabValue->minimumSizeHint().height());
    }

    if (displayError) {
        labError=new QLabel(parent);
        layout->addWidget(labError, row, COL_ERROR);
        if (widget==Header) {
            labError->setText(tr("<b>&plusmn; error</b>"));
            labError->setAlignment(Qt::AlignHCenter);
        }
        height=qMax(height, labError->minimumSizeHint().height());
    }

    if (editable) {
        if (widget==FloatEdit) {
            neditMin=new JKDoubleEdit(parent);
            neditMin->setCheckBounds(false, false);
            neditMin->setSingleStep(m_increment);
            neditMin->setShowUpDown(false);
            connect(neditMin, SIGNAL(valueChanged(double)), this, SLOT(doubleMinChanged(double)));

            neditMax=new JKDoubleEdit(parent);
            neditMax->setCheckBounds(false, false);
            neditMax->setSingleStep(m_increment);
            neditMax->setShowUpDown(false);
            connect(neditMax, SIGNAL(valueChanged(double)), this, SLOT(doubleMaxChanged(double)));

            layout->addWidget(neditMin, row, COL_MIN);
            layout->addWidget(neditMax, row, COL_MAX);
            height=qMax(height, neditMin->minimumSizeHint().height());
            height=qMax(height, neditMax->minimumSizeHint().height());
        } else if ((widget==IntSpinBox)||(widget==IntDropDown)) {
            spinIntMin=new QSpinBox(parent);
            spinIntMin->setSingleStep(m_increment);
            spinIntMin->setRange(INT_MIN, INT_MAX);
            connect(spinIntMin, SIGNAL(valueChanged(int)), this, SLOT(intMinChanged(int)));

            spinIntMax=new QSpinBox(parent);
            spinIntMax->setSingleStep(m_increment);
            spinIntMax->setRange(INT_MIN, INT_MAX);
            connect(spinIntMax, SIGNAL(valueChanged(int)), this, SLOT(intMaxChanged(int)));

            layout->addWidget(spinIntMin, row, COL_MIN);
            layout->addWidget(spinIntMax, row, COL_MAX);
            height=qMax(height, spinIntMin->minimumSizeHint().height());
            height=qMax(height, spinIntMax->minimumSizeHint().height());
        } else if (widget==Header) {
            hlabMin=new QLabel(tr("<b>minimum</b>"), parent);
            hlabMin->setAlignment(Qt::AlignHCenter);
            hlabMax=new QLabel(tr("<b>maximum</b>"), parent);
            hlabMax->setAlignment(Qt::AlignHCenter);
            layout->addWidget(hlabMin, row, COL_MIN);
            layout->addWidget(hlabMax, row, COL_MAX);
            height=qMax(height, hlabMin->minimumSizeHint().height());
            height=qMax(height, hlabMax->minimumSizeHint().height());
        }
    }

    layout->setRowStretch(row, 0);

    layout->setColumnStretch(COL_LABEL, STRETCH_LABEL);
    layout->setColumnStretch(COL_FIX, STRETCH_FIX);
    layout->setColumnStretch(COL_VALUE, STRETCH_VALUE);
    layout->setColumnStretch(COL_ERROR, STRETCH_ERROR);
    layout->setColumnStretch(COL_MIN, STRETCH_MIN);
    layout->setColumnStretch(COL_MAX, STRETCH_MAX);

    QFFitParameterWidget_setMaxHeight(neditValue, height);
    QFFitParameterWidget_setMaxHeight(spinIntValue, height);
    QFFitParameterWidget_setMaxHeight(cmbIntValue, height);
    QFFitParameterWidget_setMaxHeight(neditMin, height);
    QFFitParameterWidget_setMaxHeight(spinIntMin, height);
    QFFitParameterWidget_setMaxHeight(neditMax, height);
    QFFitParameterWidget_setMaxHeight(spinIntMax, height);
    QFFitParameterWidget_setMaxHeight(chkFix, height);
    QFFitParameterWidget_setMaxHeight(labError, height);
    QFFitParameterWidget_setMaxHeight(hlabValue, height);
    QFFitParameterWidget_setMaxHeight(hlabMin, height);
    QFFitParameterWidget_setMaxHeight(hlabMax, height);
    QFFitParameterWidget_setMaxHeight(hlabFix, height);
    QFFitParameterWidget_setMaxHeight(labLabel, height);

    setToolTip("");
    setEditRange(editRangeAllowed);
    setRangeEnabled(true);
    setEditValues(true);
    setEditRange(true);
    setVisible(true);
    setWidgetWidth(m_widgetWidth);
    setUnit(m_unit);
    setIncrement(m_increment);
    m_settingData=false;
    reloadValues();
}

#define QFFitParameterWidget_delete(widget) { if (m_layout && widget) m_layout->removeWidget(widget); if (widget) { delete widget; widget=0; } }

QFFitParameterWidget::~QFFitParameterWidget() {
    QFFitParameterWidget_delete(neditValue);
    QFFitParameterWidget_delete(spinIntValue);
    QFFitParameterWidget_delete(cmbIntValue);
    QFFitParameterWidget_delete(neditMin);
    QFFitParameterWidget_delete(spinIntMin);
    QFFitParameterWidget_delete(neditMax);
    QFFitParameterWidget_delete(spinIntMax);
    QFFitParameterWidget_delete(chkFix);
    QFFitParameterWidget_delete(labError);
    QFFitParameterWidget_delete(hlabValue);
    QFFitParameterWidget_delete(hlabMin);
    QFFitParameterWidget_delete(hlabMax);
    QFFitParameterWidget_delete(hlabFix);
    QFFitParameterWidget_delete(labLabel);
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
    } else if (m_widgetType==IntDropDown) {
        if (cmbIntValue) {
            fillCombo(cmbIntValue, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
            cmbIntValue->setCurrentIndex(cmbIntValue->findData((int)value));
        }
    }
    if (m_displayError && labError && (m_widgetType!=Header)) {
        double error=m_datastore->getFitError(m_parameterID);
        labError->setTextFormat(Qt::RichText);
        labError->setText(tr("&plusmn; %1").arg(floattohtmlstr(roundError(error,2), 5, true).c_str()));
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
    } else if (m_widgetType==IntDropDown) {
        fillCombo(cmbIntValue, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        cmbIntValue->setCurrentIndex(cmbIntValue->findData((int)value));    }
    if (m_displayError && labError && (m_widgetType!=Header)) {
        labError->setTextFormat(Qt::RichText);
        labError->setText(tr("&plusmn; %1").arg(floattohtmlstr(roundError(error,2), 5, true).c_str()));
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

void QFFitParameterWidget::intValueChangedFromCombo(int valueIn) {
    int value=valueIn;
    if (cmbIntValue) value=cmbIntValue->itemData(valueIn).toInt();
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

void QFFitParameterWidget::setWidgetWidth(int width) {
    m_widgetWidth=width;
    /*if (neditValue) {
        neditValue->setMinimumWidth(m_widgetWidth);
    }
    if (neditMin) {
        neditMin->setMinimumWidth(m_widgetWidth);
    }
    if (neditMax) {
        neditMax->setMinimumWidth(m_widgetWidth);
    }
    if (spinIntValue) {
        spinIntValue->setMinimumWidth(m_widgetWidth);
    }
    if (cmbIntValue) {
        cmbIntValue->setMinimumWidth(m_widgetWidth);
    }
    if (spinIntMin) {
        spinIntMin->setMinimumWidth(m_widgetWidth);
    }
    if (spinIntMax) {
        spinIntMax->setMinimumWidth(m_widgetWidth);
    }
    if (labError) {
        labError->setMinimumWidth(m_widgetWidth);
    }
    if (hlabMax) {
        hlabMax->setMinimumWidth(m_widgetWidth);
    }
    if (hlabMin) {
        hlabMin->setMinimumWidth(m_widgetWidth);
    }
    if (hlabValue) {
        hlabValue->setMinimumWidth(m_widgetWidth);
    }*/
    //m_layout->setColumnMinimumWidth(2, m_widgetWidth);
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
        if (spinIntMax) spinIntMax->setVisible(m_visible && m_editRange);
        if (spinIntMin) spinIntMin->setVisible(m_visible && m_editRange);
        if (neditMax) neditMax->setVisible(m_visible && m_editRange);
        if (neditMin) neditMin->setVisible(m_visible && m_editRange);
        if (hlabMin) hlabMin->setVisible(m_visible && m_editRange);
        if (hlabMax) hlabMax->setVisible(m_visible && m_editRange);
    }
    if (m_visible && m_editRange && m_editRangeAllowed) {
        m_layout->setColumnStretch(COL_MIN, STRETCH_MIN);
        m_layout->setColumnStretch(COL_MAX, STRETCH_MAX);
    } else {
        m_layout->setColumnStretch(COL_MIN, 0);
        m_layout->setColumnStretch(COL_MAX, 0);
    }
    if ((!m_editValues) && (!m_editRange)) {
        labLabel->setVisible(false);
    } else {
        labLabel->setVisible(m_visible);
    }
}

void QFFitParameterWidget::setEditValues(bool editValues) {
    m_editValues=editValues;
    if (spinIntValue) spinIntValue->setVisible(m_visible && editValues);
    if (neditValue) neditValue->setVisible(m_visible && editValues);
    if (cmbIntValue) cmbIntValue->setVisible(m_visible && editValues);
    if (chkFix) chkFix->setVisible(m_visible && editValues);
    if (labError) labError->setVisible(m_visible && editValues);
    if (hlabFix) hlabFix->setVisible(m_visible && editValues);
    if (hlabValue) hlabValue->setVisible(m_visible && editValues);
    if (m_visible && m_editValues) {
        m_layout->setColumnStretch(COL_FIX, STRETCH_FIX);
        m_layout->setColumnStretch(COL_VALUE, STRETCH_VALUE);
        m_layout->setColumnStretch(COL_ERROR, STRETCH_ERROR);
    } else {
        m_layout->setColumnStretch(COL_FIX, 0);
        m_layout->setColumnStretch(COL_VALUE, 0);
        m_layout->setColumnStretch(COL_ERROR, 0);
    }
    if ((!m_editValues) && (!m_editRange)) {
        labLabel->setVisible(false);
    } else {
        labLabel->setVisible(m_visible);
    }
}

void QFFitParameterWidget::setRangeEnabled(bool enabled) {
    if (spinIntMax) spinIntMax->setEnabled(enabled);
    if (spinIntMin) spinIntMin->setEnabled(enabled);
    if (neditMax) neditMax->setEnabled(enabled);
    if (neditMin) neditMin->setEnabled(enabled);
    if (hlabMin) hlabMin->setEnabled(enabled);
    if (hlabMax) hlabMax->setEnabled(enabled);
}

void QFFitParameterWidget::fillCombo(QComboBox* cmb, int min, int max) {
    cmb->clear();
    for(int i=min; i<=max; i++) {
        cmb->addItem(QString::number(i), i);
    }
}

void QFFitParameterWidget::setValueAbsoluteRange(double min, double max) {
    if ( min != -DBL_MAX ) {
        if (neditMin) neditMin->setMinimum(min);
        if (spinIntMin) {
            double rm=round(min);
            if (rm<(double)INT_MIN) spinIntMin->setMinimum(INT_MIN);
            else if (rm>(double)INT_MAX) spinIntMin->setMinimum(INT_MAX);
            else spinIntMin->setMinimum(rm);
        }
    } else {
        if (spinIntMin) spinIntMin->setMinimum(INT_MIN);
        if (neditMin) neditMin->setCheckMinimum(false);
    }

    if ( max != DBL_MAX ) {
        if (neditMax) neditMin->setMaximum(max);
        if (spinIntMax) {
            double rm=round(max);
            if (rm<(double)INT_MIN) spinIntMax->setMaximum(INT_MIN);
            else if (rm>(double)INT_MAX) spinIntMax->setMaximum(INT_MAX);
            else spinIntMax->setMaximum(rm);
        }
    } else {
        if (spinIntMax) spinIntMax->setMaximum(INT_MIN);
        if (neditMax) neditMax->setCheckMaximum(false);
    }
}

void QFFitParameterWidget::setToolTip(QString paramDescription) {
    m_paramDescription=paramDescription;
    QString ttLabel=m_paramDescription;
    QString ttValue=m_paramDescription;
    QString ttFix=tr("click to fix/unfix this parameter\n (%1)").arg(m_paramDescription);
    QString ttError=tr("estimated uncertainty of this parameter\n (%1)").arg(m_paramDescription);
    QString ttMin=tr("choose the lower bound for this parameter\n (%1)").arg(m_paramDescription);
    QString ttMax=tr("choose the upper bound for this parameter\n (%1)").arg(m_paramDescription);
    if (neditValue) neditValue->setToolTip(ttValue);
    if (spinIntValue) spinIntValue->setToolTip(ttLabel);
    if (cmbIntValue) cmbIntValue->setToolTip(ttLabel);
    if (neditMin) neditMin->setToolTip(ttLabel);
    if (spinIntMin) spinIntMin->setToolTip(ttMin);
    if (neditMax) neditMax->setToolTip(ttMin);
    if (spinIntMax) spinIntMax->setToolTip(ttMax);
    if (chkFix) chkFix->setToolTip(ttMax);
    if (labError) labError->setToolTip(ttError);
    if (labLabel) labLabel->setToolTip(ttLabel);
}

#define QFFitParameterWidget_setVisible(widget) if (widget) widget->setVisible(visible);

void QFFitParameterWidget::setVisible(bool visible) {
    m_visible=visible;
    if (!visible) {
        QFFitParameterWidget_setVisible(neditValue);
        QFFitParameterWidget_setVisible(spinIntValue);
        QFFitParameterWidget_setVisible(cmbIntValue);
        QFFitParameterWidget_setVisible(neditMin);
        QFFitParameterWidget_setVisible(spinIntMin);
        QFFitParameterWidget_setVisible(neditMax);
        QFFitParameterWidget_setVisible(spinIntMax);
        QFFitParameterWidget_setVisible(chkFix);
        QFFitParameterWidget_setVisible(labError);
        QFFitParameterWidget_setVisible(hlabValue);
        QFFitParameterWidget_setVisible(hlabMin);
        QFFitParameterWidget_setVisible(hlabMax);
        QFFitParameterWidget_setVisible(hlabFix);
        QFFitParameterWidget_setVisible(labLabel);
    } else {
        setEditRange(m_editRange);
        setEditValues(m_editValues);
    }
}


void QFFitParameterWidget::pEnterPressed() {
    emit enterPressed(m_parameterID);
}

void QFFitParameterWidget::s_actCopyValue() {
    m_datastore->setAllFitValues(m_parameterID, m_datastore->getFitValue(m_parameterID), m_datastore->getFitError(m_parameterID));
    m_datastore->setInitFitValue(m_parameterID, m_datastore->getFitValue(m_parameterID));
}

void QFFitParameterWidget::s_actCopyFix() {
    m_datastore->setAllFitFixes(m_parameterID, m_datastore->getFitFix(m_parameterID));
    m_datastore->setInitFitFix(m_parameterID, m_datastore->getFitFix(m_parameterID));
}

void QFFitParameterWidget::s_actCopyValueFix() {
    s_actCopyValue();
    s_actCopyFix();
}

void QFFitParameterWidget::s_actCopyValueInit() {
    m_datastore->setInitFitValue(m_parameterID, m_datastore->getFitValue(m_parameterID));
}

void QFFitParameterWidget::s_actCopyFixInit() {
    m_datastore->setInitFitFix(m_parameterID, m_datastore->getFitFix(m_parameterID));
}

void QFFitParameterWidget::s_actCopyValueFixInit() {
    s_actCopyValueInit();
    s_actCopyFixInit();
}

void QFFitParameterWidget::s_actResetValue() {
    m_datastore->resetDefaultFitValue(m_parameterID);
    reloadValues();
}

void QFFitParameterWidget::s_actResetFix() {
    m_datastore->resetDefaultFitFix(m_parameterID);
    reloadValues();
}

void QFFitParameterWidget::s_actResetValueFix() {
    s_actResetValue();
    s_actResetFix();
}
