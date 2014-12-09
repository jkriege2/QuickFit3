/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2014-09-02 17:40:55 +0200 (Di, 02 Sep 2014) $  (revision $Rev: 3430 $)

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
#include "qfrdrnumberandbrightness_settingseditor.h"
#include "ui_qfrdrnumberandbrightness_settingseditor.h"
#include "qfrdrnumberandbrightness_data.h"
#include "qfselectrdrdialog.h"

QFRDRNumberAndBrightnessSettingsEditor::QFRDRNumberAndBrightnessSettingsEditor(QFPluginServices *services, QFRawDataPropertyEditor *propEditor, QWidget *parent) :
    QFRawDataEditor(services, propEditor, parent),
    ui(new Ui::QFRDRNumberAndBrightnessSettingsEditor)
{
    ui->setupUi(this);
    ui->edtBackground->setCheckBounds(false, false);
    ui->edtBackground->setValue(0);
    ui->edtBackgroundSD->setCheckBounds(false, false);
    ui->edtBackgroundSD->setValue(0);

    ui->cmbImagePortion->setItemData(0, "all");
    ui->cmbImagePortion->setItemData(1, "left");
    ui->cmbImagePortion->setItemData(2, "right");
    ui->cmbImagePortion->setItemData(3, "top");
    ui->cmbImagePortion->setItemData(4, "bottom");

    ui->cmbDetector->setItemData(0, "photon_counting");
    ui->cmbDetector->setItemData(1, "analog");
    ui->cmbDetector->setItemData(2, "emccd");

    on_cmbDetector_currentIndexChanged(ui->cmbDetector->currentIndex());
}

QFRDRNumberAndBrightnessSettingsEditor::~QFRDRNumberAndBrightnessSettingsEditor()
{
    delete ui;
}


void QFRDRNumberAndBrightnessSettingsEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    // this functions connects to a new QFRawDataRecord which could also be NULL, so
    // you will have to connect and disconnect the signals! This is called whenever
    // a user navigates to the next record.
    // here is a basic skeleton:


    if (old) {
        disconnect(old, 0, this, 0); // first disconnect from last record
        connectAllWidgets(false);
    }

    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current); // check whether we have the right QFRawDataRecord class
    if (m) {
        rawDataChanged();
        connectAllWidgets(true);
    } else {
    }

}

void QFRDRNumberAndBrightnessSettingsEditor::connectAllWidgets(bool en)
{
    if (en) {
        connect(ui->edtBackground, SIGNAL(valueChanged(double)), this, SLOT(writeData()));
        connect(ui->edtBackgroundSD, SIGNAL(valueChanged(double)), this, SLOT(writeData()));
        connect(ui->chkIIsBackCorrected, SIGNAL(toggled(bool)), this, SLOT(writeData()));
        connect(ui->cmbImagePortion, SIGNAL(currentIndexChanged(int)), this, SLOT(writeData()));
        connect(ui->cmbDetector, SIGNAL(currentIndexChanged(int)), this, SLOT(writeData()));
        connect(ui->spinGain, SIGNAL(valueChanged(double)), this, SLOT(writeData()));
        connect(ui->edtExcessNoise, SIGNAL(valueChanged(double)), this, SLOT(writeData()));
        connect(current, SIGNAL(propertiesChanged(QString,bool)), this, SLOT(rawDataChanged()));
    } else {
        disconnect(ui->edtBackground, SIGNAL(valueChanged(double)), this, SLOT(writeData()));
        disconnect(ui->edtBackgroundSD, SIGNAL(valueChanged(double)), this, SLOT(writeData()));
        disconnect(ui->chkIIsBackCorrected, SIGNAL(toggled(bool)), this, SLOT(writeData()));
        disconnect(ui->cmbImagePortion, SIGNAL(currentIndexChanged(int)), this, SLOT(writeData()));
        disconnect(ui->cmbDetector, SIGNAL(currentIndexChanged(int)), this, SLOT(writeData()));
        disconnect(ui->spinGain, SIGNAL(valueChanged(double)), this, SLOT(writeData()));
        disconnect(ui->edtExcessNoise, SIGNAL(valueChanged(double)), this, SLOT(writeData()));
        disconnect(current, SIGNAL(propertiesChanged(QString,bool)), this, SLOT(rawDataChanged()));
    }
}

void QFRDRNumberAndBrightnessSettingsEditor::recalcNB()
{
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {
        m->recalcNumberAndBrightness();
    }
}

void QFRDRNumberAndBrightnessSettingsEditor::writeData()
{
    connectAllWidgets(false);
    writeData(current);
    connectAllWidgets(true);
}

void QFRDRNumberAndBrightnessSettingsEditor::writeDataToAllRDR()
{
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {
        QFSelectRDRDialog* dlg=new QFSelectRDRDialog(new QFMatchRDRFunctorSelectType(m->getType()), this);
        dlg->setAllowCreateNew(false);
        dlg->setAllowMultiSelect(true);
        dlg->setProject(m->getProject());
        dlg->setDescription(tr("Select all N and B RDRs, to which the current evaluation settings should be copied!"));
        if (dlg->exec()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            QList<QPointer<QFRawDataRecord> > rs=dlg->getSelectedRDRs();
            QProgressDialog progress(tr("Setting Properties ..."), tr("&Cancel"), 0, rs.size(), this);
            progress.setWindowModality(Qt::WindowModal);
            for (int i=0; i<rs.size(); i++) {
                QFRDRNumberAndBrightnessData* mr=qobject_cast<QFRDRNumberAndBrightnessData*>(rs[i]);
                if (mr) {
                    writeData(mr, true);
                }
                progress.setValue(i);
                QApplication::processEvents();
                if (progress.wasCanceled()) break;
            }
            progress.setValue(rs.size());
            QApplication::restoreOverrideCursor();
        }
         delete dlg;
    }
    writeData();
}

void QFRDRNumberAndBrightnessSettingsEditor::writeData(QFRawDataRecord *current, bool notImageProp)
{
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {
        bool en=m->isEmitPropertiesChangedEnabled();
        m->disableEmitPropertiesChanged();
        m->setQFProperty("BACKGROUND", ui->edtBackground->value(), false, true);
        m->setQFProperty("BACKGROUND_STD", ui->edtBackgroundSD->value(), false, true);
        m->setQFProperty("BACKGROUND_CORRECTED", ui->chkIIsBackCorrected->isChecked(), false, true);
        if (!notImageProp) m->setQFProperty("SELECT_IMAGE_HALF", ui->cmbImagePortion->currentData().toString(), false, true);
        m->setQFProperty("N_AND_B_DETECTOR_TYPE", ui->cmbDetector->currentData().toString(), false, true);
        m->setQFProperty("N_AND_B_DETECTOR_GAIN", ui->spinGain->value(), false, true);
        m->setQFProperty("N_AND_B_DETECTOR_EXCESSNOISE", ui->edtExcessNoise->value(), false, true);
        if (en) m->enableEmitPropertiesChanged();
    }

}


void QFRDRNumberAndBrightnessSettingsEditor::rawDataChanged() {
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
    if (m) {
        connectAllWidgets(false);
        ui->edtBackground->setValue(m->getQFProperty("BACKGROUND", 0.0).toDouble());
        ui->edtBackgroundSD->setValue(m->getQFProperty("BACKGROUND_STD", 0.0).toDouble());
        ui->chkIIsBackCorrected->setChecked(m->getQFProperty("BACKGROUND_CORRECTED", false).toBool());
        ui->cmbImagePortion->setCurrentData(m->getQFProperty("SELECT_IMAGE_HALF", "all").toString());
        ui->cmbDetector->setCurrentData(m->getQFProperty("N_AND_B_DETECTOR_TYPE", "photon_counting").toString());
        ui->spinGain->setValue(m->getQFProperty("N_AND_B_DETECTOR_GAIN", 1.0).toDouble());
        ui->edtExcessNoise->setValue(m->getQFProperty("N_AND_B_DETECTOR_EXCESSNOISE", 2.0).toDouble());
        connectAllWidgets(true);
    }
}

void QFRDRNumberAndBrightnessSettingsEditor::on_cmbDetector_currentIndexChanged(int index)
{
    if (index==0) {
        ui->labExcessNoise->setVisible(false);
        ui->edtExcessNoise->setVisible(false);
        ui->labGain->setVisible(false);
        ui->spinGain->setVisible(false);
        ui->labBackgroundSD->setVisible(false);
        ui->edtBackgroundSD->setVisible(false);
    } else if (index==1) {
        ui->labExcessNoise->setVisible(false);
        ui->edtExcessNoise->setVisible(false);
        ui->labGain->setVisible(true);
        ui->spinGain->setVisible(true);
        ui->labBackgroundSD->setVisible(true);
        ui->edtBackgroundSD->setVisible(true);
    } else  {
        ui->labExcessNoise->setVisible(true);
        ui->edtExcessNoise->setVisible(true);
        ui->labGain->setVisible(true);
        ui->spinGain->setVisible(true);
        ui->labBackgroundSD->setVisible(true);
        ui->edtBackgroundSD->setVisible(true);
    }
}

void QFRDRNumberAndBrightnessSettingsEditor::readSettings() {
    // read widget settings
    if (!settings) return;
}


void QFRDRNumberAndBrightnessSettingsEditor::writeSettings() {
    // rwrite widget settings
    if (!settings) return;
}



