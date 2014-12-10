/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#include "qfrdrimagingfcsmaskbuilder.h"
#include "ui_qfrdrimagingfcsmaskbuilder.h"
#include "programoptions.h"

QFRDRImagingFCSMaskBuilder::QFRDRImagingFCSMaskBuilder(const QStringList& parameters, const QStringList& parameterNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSMaskBuilder)
{
    ui->setupUi(this);
    ui->spinT11->setCheckBounds(false, false);
    ui->spinT12->setCheckBounds(false, false);
    ui->spinT21->setCheckBounds(false, false);
    ui->spinT22->setCheckBounds(false, false);
    ui->spinT31->setCheckBounds(false, false);
    ui->spinT32->setCheckBounds(false, false);
    ui->spinT41->setCheckBounds(false, false);
    ui->spinT42->setCheckBounds(false, false);
    ui->spinT51->setCheckBounds(false, false);
    ui->spinT52->setCheckBounds(false, false);
    for (int i=0; i<parameters.size(); i++) {
        ui->cmbP1->addItem(parameterNames.value(i, parameters.value(i, tr("parameter %1").arg(i))), parameters[i]);
        ui->cmbP2->addItem(parameterNames.value(i, parameters.value(i, tr("parameter %1").arg(i))), parameters[i]);
        ui->cmbP3->addItem(parameterNames.value(i, parameters.value(i, tr("parameter %1").arg(i))), parameters[i]);
        ui->cmbP4->addItem(parameterNames.value(i, parameters.value(i, tr("parameter %1").arg(i))), parameters[i]);
        ui->cmbP5->addItem(parameterNames.value(i, parameters.value(i, tr("parameter %1").arg(i))), parameters[i]);
    }
    ui->cmbMode1->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/Mode1", 0).toInt());
    ui->cmbMode2->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/Mode2", 0).toInt());
    ui->cmbMode3->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/Mode3", 0).toInt());
    ui->cmbMode4->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/Mode4", 0).toInt());
    ui->cmbMode5->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/Mode5", 0).toInt());
    ui->cmbP1->setCurrentIndex(ui->cmbP1->findData(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/P1", parameters.value(0, "")).toString()));
    ui->cmbP2->setCurrentIndex(ui->cmbP2->findData(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/P2", parameters.value(1, "")).toString()));
    ui->cmbP3->setCurrentIndex(ui->cmbP3->findData(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/P3", parameters.value(2, "")).toString()));
    ui->cmbP4->setCurrentIndex(ui->cmbP4->findData(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/P4", parameters.value(3, "")).toString()));
    ui->cmbP5->setCurrentIndex(ui->cmbP5->findData(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/P5", parameters.value(4, "")).toString()));
    ui->spinT11->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/T11", 0).toDouble());
    ui->spinT12->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/T12", 100).toDouble());
    ui->spinT21->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/T21", 0).toDouble());
    ui->spinT22->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/T22", 100).toDouble());
    ui->spinT31->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/T31", 0).toDouble());
    ui->spinT32->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/T32", 100).toDouble());
    ui->spinT41->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/T41", 0).toDouble());
    ui->spinT42->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/T42", 100).toDouble());
    ui->spinT51->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/T51", 0).toDouble());
    ui->spinT52->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/T52", 100).toDouble());
    ui->chkMask->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/masked", true).toBool());
    ui->chkUse2->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/use2", false).toBool());
    ui->chkUse3->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/use3", false).toBool());
    ui->chkUse4->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/use4", false).toBool());
    ui->chkUse5->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/use5", false).toBool());

    ui->chkSaveDel->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/chkSaveDel", false).toBool());
    ui->radCurrent->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/radCurrent", true).toBool());
    ui->edtSelName->setText(ProgramOptions::getConfigValue("QFRDRImagingFCSMaskBuilder/edtSelName", "").toString());
}

QFRDRImagingFCSMaskBuilder::~QFRDRImagingFCSMaskBuilder()
{

    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/Mode1", ui->cmbMode1->currentIndex());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/Mode2", ui->cmbMode2->currentIndex());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/Mode3", ui->cmbMode3->currentIndex());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/Mode4", ui->cmbMode4->currentIndex());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/Mode5", ui->cmbMode5->currentIndex());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/P1", ui->cmbP1->itemData(ui->cmbP1->currentIndex()).toString());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/P2", ui->cmbP2->itemData(ui->cmbP2->currentIndex()).toString());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/P3", ui->cmbP3->itemData(ui->cmbP3->currentIndex()).toString());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/P4", ui->cmbP3->itemData(ui->cmbP4->currentIndex()).toString());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/P5", ui->cmbP3->itemData(ui->cmbP5->currentIndex()).toString());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/T11", ui->spinT11->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/T12", ui->spinT12->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/T21", ui->spinT21->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/T22", ui->spinT22->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/T31", ui->spinT31->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/T32", ui->spinT32->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/T41", ui->spinT41->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/T42", ui->spinT42->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/T51", ui->spinT51->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/T52", ui->spinT52->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/masked", ui->chkMask->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/use2", ui->chkUse2->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/use3", ui->chkUse3->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/use4", ui->chkUse4->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/use5", ui->chkUse5->isChecked());

    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/chkSaveDel", ui->chkSaveDel->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/radCurrent", ui->radCurrent->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSMaskBuilder/edtSelName", ui->edtSelName->text());

    delete ui;
}

bool QFRDRImagingFCSMaskBuilder::use1(QString &parameter, int &mode, double &thresh1, double &thresh2)
{
    parameter=ui->cmbP1->itemData(ui->cmbP1->currentIndex()).toString();
    mode=ui->cmbMode1->currentIndex();
    thresh1=ui->spinT11->value();
    thresh2=ui->spinT12->value();
    return true;
}

bool QFRDRImagingFCSMaskBuilder::use2(QString &parameter, int &mode, double &thresh1, double &thresh2)
{
    parameter=ui->cmbP2->itemData(ui->cmbP2->currentIndex()).toString();
    mode=ui->cmbMode2->currentIndex();
    thresh1=ui->spinT21->value();
    thresh2=ui->spinT22->value();
    return ui->chkUse2->isChecked();
}

bool QFRDRImagingFCSMaskBuilder::use3(QString &parameter, int &mode, double &thresh1, double &thresh2)
{
    parameter=ui->cmbP3->itemData(ui->cmbP3->currentIndex()).toString();
    mode=ui->cmbMode3->currentIndex();
    thresh1=ui->spinT31->value();
    thresh2=ui->spinT32->value();
    return ui->chkUse3->isChecked();
}

bool QFRDRImagingFCSMaskBuilder::use4(QString &parameter, int &mode, double &thresh1, double &thresh2)
{
    parameter=ui->cmbP4->itemData(ui->cmbP4->currentIndex()).toString();
    mode=ui->cmbMode4->currentIndex();
    thresh1=ui->spinT41->value();
    thresh2=ui->spinT42->value();
    return ui->chkUse4->isChecked();
}

bool QFRDRImagingFCSMaskBuilder::use5(QString &parameter, int &mode, double &thresh1, double &thresh2)
{
    parameter=ui->cmbP5->itemData(ui->cmbP5->currentIndex()).toString();
    mode=ui->cmbMode5->currentIndex();
    thresh1=ui->spinT51->value();
    thresh2=ui->spinT52->value();
    return ui->chkUse5->isChecked();
}

bool QFRDRImagingFCSMaskBuilder::onlyMasked() const
{
    return ui->chkMask->isChecked();
}

bool QFRDRImagingFCSMaskBuilder::saveSelection() const
{
    return ui->chkSaveDel->isChecked();
}

QString QFRDRImagingFCSMaskBuilder::selName() const
{
    return ui->edtSelName->text();
}

bool QFRDRImagingFCSMaskBuilder::applyToOtherRDRs() const
{
    return ui->radSeveral->isChecked();
}

void QFRDRImagingFCSMaskBuilder::enableSelectionName(bool en)
{
    ui->chkSaveDel->setEnabled(en);
    ui->edtSelName->setEnabled(en);
}

