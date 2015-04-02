/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qffitparameterwidget.h"
#include <QApplication>
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


double QFFitParameterWidgetWrapper_roundError(double error, int addSignifcant) {
    if (fabs(error)<DBL_MIN*10.0) return error;
    int sbits_error=ceil(log(fabs(error))/log(10.0));
    double f=pow(10.0, sbits_error-1-addSignifcant);
    return round(error/f)*f;
}

#define QFFitParameterWidgetWrapper_setMaxHeight(widget, height) { if (widget) { widget->setMaximumHeight(height); } }

QFFitParameterWidgetWrapper::QFFitParameterWidgetWrapper(QFFitParameterBasicInterface* datastore,  QGridLayout* layout, int row, QString parameterID, WidgetType widget, bool editable, bool displayFix, QFFitFunction::ErrorDisplayMode displayError, bool editRangeAllowed, QWidget* parent, QString label, QStringList comboValues):
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
    m_comboValues=comboValues;

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
    neditError=NULL;

    actCopyValue = new QAction(tr("copy &value to all files"), this);
    connect(actCopyValue, SIGNAL(triggered()), this, SLOT(s_actCopyValue()));
    actCopyFix = new QAction(tr("copy &fix to all files"), this);
    connect(actCopyFix, SIGNAL(triggered()), this, SLOT(s_actCopyFix()));
    actCopyValueFix = new QAction(tr("copy value && fix to all files"), this);
    connect(actCopyValueFix, SIGNAL(triggered()), this, SLOT(s_actCopyValueFix()));


    actCopyValueRuns = new QAction(tr("copy &value to all runs"), this);
    connect(actCopyValueRuns, SIGNAL(triggered()), this, SLOT(s_actCopyValueRuns()));
    actCopyFixRuns = new QAction(tr("copy &fix to all runs"), this);
    connect(actCopyFixRuns, SIGNAL(triggered()), this, SLOT(s_actCopyFixRuns()));
    actCopyValueFixRuns = new QAction(tr("copy value && fix to all runs"), this);
    connect(actCopyValueFixRuns, SIGNAL(triggered()), this, SLOT(s_actCopyValueFixRuns()));


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


    sep1=new QAction(this);
    sep1->setSeparator(true);
    sep2=new QAction(this);
    sep2->setSeparator(true);
    sep3=new QAction(this);
    sep3->setSeparator(true);
    sep4=new QAction(this);
    sep4->setSeparator(true);


    int height=5;

    //int row=row;

    if (!label.isEmpty()) {
        labLabel=new QLabel(label, parent);
        layout->addWidget(labLabel, row, COL_LABEL);
        QString labelHeader="<b>"+label+"</b>";
        QString labelFitParam="<font color=\"darkred\">"+label+"</font>";
        QString labelCalcParam="<font color=\"#0F0F0F\"><i>"+label+"</i></font>";
        QString labelEditParam="<font color=\"black\">"+label+"</font>";
        if (widget==Header) labLabel->setText(labelHeader);
        else {
            if (displayFix && editable) labLabel->setText(labelFitParam);
            else if (!displayFix && editable) labLabel->setText(labelEditParam);
            else labLabel->setText(labelCalcParam);
        }
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

    if (widget==FloatEdit || widget==LogFloatEdit) {
        neditValue=new QFDoubleEdit(parent);
        neditValue->setRange(datastore->getFitMin(parameterID), datastore->getFitMax(parameterID));
        layout->addWidget(neditValue, row, COL_VALUE);
        neditValue->setSingleStep(m_increment);
        neditValue->setLogScale(widget==LogFloatEdit, qMin(10.0,m_increment));
        neditValue->setReadOnly(!editable);
        if (editable) {
            neditValue->setBackgroundColor(m_parent->palette().color(QPalette::Base));
        } else {
            neditValue->setBackgroundColor(m_parent->palette().color(QPalette::Window));
        }
        neditValue->setShowUpDown(false);
        connect(neditValue, SIGNAL(valueChanged(double)), this, SLOT(doubleValueChanged(double)));
        height=qMax(height, neditValue->minimumSizeHint().height());
        neditValue->addContextmenuAction(sep1);
        neditValue->addContextmenuAction(actCopyValue);
        neditValue->addContextmenuAction(actCopyFix);
        neditValue->addContextmenuAction(actCopyValueFix);
        neditValue->addContextmenuAction(sep2);
        neditValue->addContextmenuAction(actCopyValueRuns);
        neditValue->addContextmenuAction(actCopyFixRuns);
        neditValue->addContextmenuAction(actCopyValueFixRuns);
        neditValue->addContextmenuAction(sep3);
        neditValue->addContextmenuAction(actResetValue);
        neditValue->addContextmenuAction(actResetFix);
        neditValue->addContextmenuAction(actResetValueFix);
        neditValue->addContextmenuAction(sep4);
        neditValue->addContextmenuAction(actCopyValueInit);
        neditValue->addContextmenuAction(actCopyFixInit);
        neditValue->addContextmenuAction(actCopyValueFixInit);

        connect(neditValue, SIGNAL(keyEventMatches(int, Qt::KeyboardModifiers)), this, SLOT(keyEventMatches(int, Qt::KeyboardModifiers)));
        neditValue->addKeyEvent(Qt::Key_Space, Qt::NoModifier);
        neditValue->addKeyEvent(Qt::Key_Enter, Qt::NoModifier);
        neditValue->addKeyEvent(Qt::Key_Return, Qt::NoModifier);

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

    if (widget==Header) {
        if ((displayError==QFFitFunction::DisplayError)||(displayError==QFFitFunction::EditError)) {
            labError=new QLabel(parent);
            layout->addWidget(labError, row, COL_ERROR);
            labError->setText(tr("<b>&nbsp;&plusmn; error</b>"));
            labError->setAlignment(Qt::AlignHCenter);
            height=qMax(height, labError->minimumSizeHint().height());
        }
    } else {
        if (displayError==QFFitFunction::DisplayError) {
            labError=new QLabel(parent);
            layout->addWidget(labError, row, COL_ERROR);
            height=qMax(height, labError->minimumSizeHint().height());
        } else if (displayError==QFFitFunction::EditError) {
            neditError=new QFDoubleEdit(parent);
            neditError->setCheckBounds(false, false);
            neditError->setShowUpDown(false);
            layout->addWidget(neditError, row, COL_ERROR);
            height=qMax(height, neditError->minimumSizeHint().height());
            connect(neditError, SIGNAL(valueChanged(double)), this, SLOT(doubleErrorChanged(double)));
            connect(neditError, SIGNAL(keyEventMatches(int, Qt::KeyboardModifiers)), this, SLOT(keyEventMatches(int, Qt::KeyboardModifiers)));
            neditError->addKeyEvent(Qt::Key_Space, Qt::NoModifier);
            neditError->addKeyEvent(Qt::Key_Enter, Qt::NoModifier);
            neditError->addKeyEvent(Qt::Key_Return, Qt::NoModifier);
        }
    }

    if (editable) {
        if (widget==FloatEdit || widget==LogFloatEdit) {
            neditMin=new QFDoubleEdit(parent);
            neditMin->setCheckBounds(false, false);
            neditMin->setSingleStep(m_increment);            
            neditMin->setShowUpDown(false);
            connect(neditMin, SIGNAL(valueChanged(double)), this, SLOT(doubleMinChanged(double)));

            neditMax=new QFDoubleEdit(parent);
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

    QFFitParameterWidgetWrapper_setMaxHeight(neditValue, height);
    QFFitParameterWidgetWrapper_setMaxHeight(spinIntValue, height);
    QFFitParameterWidgetWrapper_setMaxHeight(cmbIntValue, height);
    QFFitParameterWidgetWrapper_setMaxHeight(neditMin, height);
    QFFitParameterWidgetWrapper_setMaxHeight(spinIntMin, height);
    QFFitParameterWidgetWrapper_setMaxHeight(neditMax, height);
    QFFitParameterWidgetWrapper_setMaxHeight(spinIntMax, height);
    QFFitParameterWidgetWrapper_setMaxHeight(chkFix, height);
    QFFitParameterWidgetWrapper_setMaxHeight(labError, height);
    QFFitParameterWidgetWrapper_setMaxHeight(neditError, height);
    QFFitParameterWidgetWrapper_setMaxHeight(hlabValue, height);
    QFFitParameterWidgetWrapper_setMaxHeight(hlabMin, height);
    QFFitParameterWidgetWrapper_setMaxHeight(hlabMax, height);
    QFFitParameterWidgetWrapper_setMaxHeight(hlabFix, height);
    QFFitParameterWidgetWrapper_setMaxHeight(labLabel, height);

    setToolTip("");
    setEditRange(editRangeAllowed);
    setRangeEnabled(true);
    setEditRange(false);
    setEditValues(true);
    setVisible(true);
    setWidgetWidth(m_widgetWidth);
    setUnit(m_unit);
    setIncrement(m_increment);
    m_settingData=false;
    reloadValues();
}

#define QFFitParameterWidgetWrapper_delete(widget) { if (m_layout && widget) m_layout->removeWidget(widget); if (widget) { delete widget; widget=0; } }

QFFitParameterWidgetWrapper::~QFFitParameterWidgetWrapper() {
    QFFitParameterWidgetWrapper_delete(neditValue);
    QFFitParameterWidgetWrapper_delete(spinIntValue);
    QFFitParameterWidgetWrapper_delete(cmbIntValue);
    QFFitParameterWidgetWrapper_delete(neditMin);
    QFFitParameterWidgetWrapper_delete(spinIntMin);
    QFFitParameterWidgetWrapper_delete(neditMax);
    QFFitParameterWidgetWrapper_delete(spinIntMax);
    QFFitParameterWidgetWrapper_delete(chkFix);
    QFFitParameterWidgetWrapper_delete(labError);
    QFFitParameterWidgetWrapper_delete(neditError);
    QFFitParameterWidgetWrapper_delete(hlabValue);
    QFFitParameterWidgetWrapper_delete(hlabMin);
    QFFitParameterWidgetWrapper_delete(hlabMax);
    QFFitParameterWidgetWrapper_delete(hlabFix);
    QFFitParameterWidgetWrapper_delete(labLabel);
}


void QFFitParameterWidgetWrapper::reloadValues() {
    bool old_m_settingData=m_settingData;
    m_settingData=true;

    //if (m_editRange) {
        if (neditMin) neditMin->setValue(m_datastore->getFitMin(m_parameterID));
        if (neditMax) neditMax->setValue(m_datastore->getFitMax(m_parameterID));
        if (spinIntMin) spinIntMin->setValue(m_datastore->getFitMin(m_parameterID));
        if (spinIntMax) spinIntMax->setValue(m_datastore->getFitMax(m_parameterID));
    //}

    double value=m_datastore->getFitValue(m_parameterID);

    if (m_widgetType==FloatEdit || m_widgetType==LogFloatEdit) {
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
    double error=m_datastore->getFitError(m_parameterID);
    if (m_widgetType!=Header) {
        if (labError) {
            labError->setTextFormat(Qt::RichText);
            labError->setText(tr("&plusmn; %1").arg(floattohtmlstr(QFFitParameterWidgetWrapper_roundError(error,2), 5, true).c_str()));
        } else if (neditError) {
            neditError->setValue(error);
        }
    }
    if (chkFix) {
        chkFix->setChecked(m_datastore->getFitFix(m_parameterID));
    }
    m_settingData=old_m_settingData;
}

void QFFitParameterWidgetWrapper::setValue(double value, double error, bool writeback) {
    bool old_m_settingData=m_settingData;
    m_settingData=true;

    reloadValues();

    if (writeback) {
        m_datastore->setFitValue(m_parameterID, value);
    }

    if (m_widgetType==FloatEdit || m_widgetType==LogFloatEdit) {
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (neditValue && (neditValue->value()!=value)) neditValue->setValue(value);
    } else if (m_widgetType==IntSpinBox) {
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue && (spinIntValue->value()!=value)) spinIntValue->setValue(value);
    } else if (m_widgetType==IntDropDown) {
        fillCombo(cmbIntValue, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        cmbIntValue->setCurrentIndex(cmbIntValue->findData((int)value));
    }
    if (m_displayError  && (m_widgetType!=Header)) {
        if (labError) {
            labError->setTextFormat(Qt::RichText);
            labError->setText(tr("&plusmn; %1").arg(floattohtmlstr(QFFitParameterWidgetWrapper_roundError(error,2), 5, true).c_str()));
        } else if (neditError) {
            neditError->setValue(error);
        }
    }

    m_settingData=old_m_settingData;
}

void QFFitParameterWidgetWrapper::doubleValueChanged(double value) {
    if ((!m_settingData) && m_editable) {
        m_datastore->setFitValue(m_parameterID, value);
        emit valueChanged(m_parameterID, value);
    }
}

void QFFitParameterWidgetWrapper::doubleErrorChanged(double error) {
    if ((!m_settingData) && m_editable) {
        m_datastore->setFitError(m_parameterID, error);
        emit errorChanged(m_parameterID, error);
    }
}

void QFFitParameterWidgetWrapper::intValueChanged(int value) {
    if ((!m_settingData) && m_editable) {
        m_datastore->setFitValue(m_parameterID, value);
        emit valueChanged(m_parameterID, value);
    }
}

void QFFitParameterWidgetWrapper::intValueChangedFromCombo(int valueIn) {
    int value=valueIn;
    if (cmbIntValue) value=cmbIntValue->itemData(valueIn).toInt();
    if ((!m_settingData) && m_editable) {
        m_datastore->setFitValue(m_parameterID, value);
        emit valueChanged(m_parameterID, value);
    }
}
void QFFitParameterWidgetWrapper::sfixChanged(bool fix) {
    if ((!m_settingData) && m_editable) {
        m_datastore->setFitFix(m_parameterID, fix);
        emit fixChanged(m_parameterID, fix);
    }
}


void QFFitParameterWidgetWrapper::doubleMinChanged(double value) {
    if ((!m_settingData) && m_editable && m_editRange) {
        m_datastore->setFitMin(m_parameterID, value);
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        emit rangeChanged(m_parameterID, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
    }
}

void QFFitParameterWidgetWrapper::doubleMaxChanged(double value) {
    if ((!m_settingData) && m_editable && m_editRange) {
        m_datastore->setFitMax(m_parameterID, value);
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        emit rangeChanged(m_parameterID, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
    }
}

void QFFitParameterWidgetWrapper::intMinChanged(int value) {
    if ((!m_settingData) && m_editable && m_editRange) {
        m_datastore->setFitMin(m_parameterID, value);
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        emit rangeChanged(m_parameterID, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
    }
}

void QFFitParameterWidgetWrapper::intMaxChanged(int value) {
    if ((!m_settingData) && m_editable && m_editRange) {
        m_datastore->setFitMax(m_parameterID, value);
        if (neditValue) neditValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        if (spinIntValue) spinIntValue->setRange(m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
        emit rangeChanged(m_parameterID, m_datastore->getFitMin(m_parameterID), m_datastore->getFitMax(m_parameterID));
    }
}

void QFFitParameterWidgetWrapper::setWidgetWidth(int width) {
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

void QFFitParameterWidgetWrapper::setUnit(QString unit) {
    m_unit=unit;
}

void QFFitParameterWidgetWrapper::setIncrement(double increment) {
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

void QFFitParameterWidgetWrapper::setEditRange(int range) {
    setEditRange(range!=0);
}

void QFFitParameterWidgetWrapper::unsetEditValues(int range) {
    unsetEditValues(range!=0);
}

void QFFitParameterWidgetWrapper::setEditRange(bool editRange) {
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

void QFFitParameterWidgetWrapper::setEditValues(bool editValues) {
    bool updEn=m_parent->updatesEnabled();
    bool widVisible=m_parent->isVisible(); if (widVisible) m_parent->setUpdatesEnabled(false);
    m_editValues=editValues;
    if (spinIntValue) spinIntValue->setVisible(m_visible && editValues);
    if (neditValue) neditValue->setVisible(m_visible && editValues);
    if (cmbIntValue) cmbIntValue->setVisible(m_visible && editValues);
    if (chkFix) chkFix->setVisible(m_visible && editValues);
    if (labError) labError->setVisible(m_visible && editValues);
    if (neditError) neditError->setVisible(m_visible && editValues);
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
    if (widVisible)  m_parent->setUpdatesEnabled(updEn);
}

void QFFitParameterWidgetWrapper::setRangeEnabled(bool enabled) {
    bool updEn=m_parent->updatesEnabled();
    bool widVisible=m_parent->isVisible(); if (widVisible) m_parent->setUpdatesEnabled(false);
    if (spinIntMax) spinIntMax->setEnabled(enabled);
    if (spinIntMin) spinIntMin->setEnabled(enabled);
    if (neditMax) neditMax->setEnabled(enabled);
    if (neditMin) neditMin->setEnabled(enabled);
    if (hlabMin) hlabMin->setEnabled(enabled);
    if (hlabMax) hlabMax->setEnabled(enabled);
    if (widVisible) m_parent->setUpdatesEnabled(updEn);
}

void QFFitParameterWidgetWrapper::fillCombo(QComboBox* cmb, int min, int max) {
    cmb->clear();
    int j=0;
    bool widVisible=cmb->isVisible(); if (widVisible) cmb->setUpdatesEnabled(false);
    for(int i=min; i<=max; i++) {
        if (j<m_comboValues.size()) cmb->addItem(m_comboValues[j], i);
        else cmb->addItem(QString::number(i), i);
        j++;
    }
    if (widVisible) cmb->setUpdatesEnabled(true);
}

void QFFitParameterWidgetWrapper::setValueAbsoluteRange(double min, double max) {
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

void QFFitParameterWidgetWrapper::setToolTip(QString paramDescription) {
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
    if (chkFix) chkFix->setToolTip(ttFix);
    if (labError) labError->setToolTip(ttError);
    if (neditError) neditError->setToolTip(ttError);
    if (labLabel) labLabel->setToolTip(ttLabel);
}

#define QFFitParameterWidgetWrapper_setVisible(widget) if (widget) widget->setVisible(visible);

void QFFitParameterWidgetWrapper::setVisible(bool visible) {
    bool updEn=m_parent->updatesEnabled();
    bool widVisible=m_parent->isVisible(); if (widVisible) m_parent->setUpdatesEnabled(false);
    m_visible=visible;
    if (!visible) {
        QFFitParameterWidgetWrapper_setVisible(neditValue);
        QFFitParameterWidgetWrapper_setVisible(spinIntValue);
        QFFitParameterWidgetWrapper_setVisible(cmbIntValue);
        QFFitParameterWidgetWrapper_setVisible(neditMin);
        QFFitParameterWidgetWrapper_setVisible(spinIntMin);
        QFFitParameterWidgetWrapper_setVisible(neditMax);
        QFFitParameterWidgetWrapper_setVisible(spinIntMax);
        QFFitParameterWidgetWrapper_setVisible(chkFix);
        QFFitParameterWidgetWrapper_setVisible(labError);
        QFFitParameterWidgetWrapper_setVisible(neditError);
        QFFitParameterWidgetWrapper_setVisible(hlabValue);
        QFFitParameterWidgetWrapper_setVisible(hlabMin);
        QFFitParameterWidgetWrapper_setVisible(hlabMax);
        QFFitParameterWidgetWrapper_setVisible(hlabFix);
        QFFitParameterWidgetWrapper_setVisible(labLabel);
    } else {
        setEditRange(m_editRange);
        setEditValues(m_editValues);
    }
    if (widVisible) m_parent->setUpdatesEnabled(updEn);
}


void QFFitParameterWidgetWrapper::pEnterPressed() {
    QWidget* w=QApplication::focusWidget();
    w->clearFocus();
    w->setFocus(Qt::TabFocusReason);
    emit enterPressed(m_parameterID);
}

void QFFitParameterWidgetWrapper::s_actCopyValue() {
    m_datastore->setAllFitValues(m_parameterID, m_datastore->getFitValue(m_parameterID), m_datastore->getFitError(m_parameterID));

    if (QMessageBox::question(NULL, tr("Copy values to all files"), tr("Do you want to also copy the current value to the initial settings?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        m_datastore->setInitFitValue(m_parameterID, m_datastore->getFitValue(m_parameterID));
    }
}

void QFFitParameterWidgetWrapper::s_actCopyFix() {
    m_datastore->setAllFitFixes(m_parameterID, m_datastore->getFitFix(m_parameterID));
    if (QMessageBox::question(NULL, tr("Copy fix to all files"), tr("Do you want to also copy the current fix to the initial settings?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        m_datastore->setInitFitFix(m_parameterID, m_datastore->getFitFix(m_parameterID));
    }
}

void QFFitParameterWidgetWrapper::s_actCopyValueFix() {
    m_datastore->setAllFitValues(m_parameterID, m_datastore->getFitValue(m_parameterID), m_datastore->getFitError(m_parameterID));
    m_datastore->setAllFitFixes(m_parameterID, m_datastore->getFitFix(m_parameterID));

    if (QMessageBox::question(NULL, tr("Copy values & fix to all files"), tr("Do you want to also copy the current value and fix to the initial settings?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        m_datastore->setInitFitValue(m_parameterID, m_datastore->getFitValue(m_parameterID));
        m_datastore->setInitFitFix(m_parameterID, m_datastore->getFitFix(m_parameterID));
    }
}

void QFFitParameterWidgetWrapper::s_actCopyValueRuns() {
    m_datastore->setAllFitValues(m_parameterID, m_datastore->getFitValue(m_parameterID), m_datastore->getFitError(m_parameterID), true);

    if (QMessageBox::question(NULL, tr("Copy values to all runs"), tr("Do you want to also copy the current value to the initial settings?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        m_datastore->setInitFitValue(m_parameterID, m_datastore->getFitValue(m_parameterID));
    }
}

void QFFitParameterWidgetWrapper::s_actCopyFixRuns() {
    m_datastore->setAllFitFixes(m_parameterID, m_datastore->getFitFix(m_parameterID), true);
    if (QMessageBox::question(NULL, tr("Copy fix to all runs"), tr("Do you want to also copy the current fix to the initial settings?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        m_datastore->setInitFitFix(m_parameterID, m_datastore->getFitFix(m_parameterID));
    }
}

void QFFitParameterWidgetWrapper::s_actCopyValueFixRuns() {
    m_datastore->setAllFitValues(m_parameterID, m_datastore->getFitValue(m_parameterID), m_datastore->getFitError(m_parameterID), true);
    m_datastore->setAllFitFixes(m_parameterID, m_datastore->getFitFix(m_parameterID), true);

    if (QMessageBox::question(NULL, tr("Copy values & fix to all runs"), tr("Do you want to also copy the current value and fix to the initial settings?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        m_datastore->setInitFitValue(m_parameterID, m_datastore->getFitValue(m_parameterID));
        m_datastore->setInitFitFix(m_parameterID, m_datastore->getFitFix(m_parameterID));
    }
}

void QFFitParameterWidgetWrapper::s_actCopyValueInit() {
    m_datastore->setInitFitValue(m_parameterID, m_datastore->getFitValue(m_parameterID), m_datastore->getFitError(m_parameterID));
}

void QFFitParameterWidgetWrapper::s_actCopyFixInit() {
    m_datastore->setInitFitFix(m_parameterID, m_datastore->getFitFix(m_parameterID));
}

void QFFitParameterWidgetWrapper::s_actCopyValueFixInit() {
    m_datastore->setInitFitValue(m_parameterID, m_datastore->getFitValue(m_parameterID), m_datastore->getFitError(m_parameterID));
    m_datastore->setInitFitFix(m_parameterID, m_datastore->getFitFix(m_parameterID));
}

void QFFitParameterWidgetWrapper::s_actResetValue() {
    m_datastore->resetDefaultFitValue(m_parameterID);
    reloadValues();
}

void QFFitParameterWidgetWrapper::s_actResetFix() {
    m_datastore->resetDefaultFitFix(m_parameterID);
    reloadValues();
}

void QFFitParameterWidgetWrapper::s_actResetValueFix() {
    m_datastore->resetDefaultFitValue(m_parameterID);
    m_datastore->resetDefaultFitFix(m_parameterID);
    reloadValues();
}

void QFFitParameterWidgetWrapper::keyEventMatches(int key, Qt::KeyboardModifiers modifiers) {
    if ((modifiers==Qt::NoModifier)&&(key==Qt::Key_Space)) {
        if ((!m_settingData) && m_editable && neditValue) {
            m_datastore->setFitValue(m_parameterID, neditValue->value());
            emit valueChanged(m_parameterID, neditValue->value());
        }
        if (chkFix) chkFix->toggle();
    }
    if ((modifiers==Qt::NoModifier)&&((key==Qt::Key_Return)||(key==Qt::Key_Enter))) {
        emit enterPressed(m_parameterID);
    }
}


