/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "dlgcalcdiffcoeff.h"
#include "ui_dlgcalcdiffcoeff.h"
#include "qfediffusioncoefficientcalculator.h"
#include "programoptions.h"
#include "dlgcomponentinfo.h"
#include "qfversion.h"

DlgCalcDiffCoeff::DlgCalcDiffCoeff(QFEDiffusionCoefficientCalculator *plg, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgCalcDiffCoeff)
{
    updating=false;
    userDDescriptor=tr(" (user)");
    dlgInfo=NULL;
    this->plugin=plg;
    tab=new QFTableModel(this);
    ui->setupUi(this);
    c_temp=-1;
    c_D=-1;
    c_visc=-1;
    c_density=-1;
    ui->tableView->setModel(tab);
    ui->plotter->get_plotter()->getXAxis()->set_axisLabel(tr("temperature [{\\degree}C]"));
    ui->edtGivenD->setValue(20);
    ui->edtGivenD->setRange(0,1e10);
    ui->spinGivenDInSolution->setValue(20);
    ui->spinGivenDInSolution->setRange(0,1e10);
    ui->edtGivenDVisc->setRange(0,1e10);
    ui->edtGivenDVisc->setValue(1);
    ui->spinGivenDT->setValue(20);
    ui->spinGivenDInSolutionT->setValue(20);
    ui->plotter->getYAxis()->set_labelDigits(4);
    ui->plotter->getYAxis()->set_minTicks(5);
    ui->plotter->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    ui->edtExperimentD->setRange(0,1e10);
    ui->edtExperimentRHyd->setRange(0,1e10);
    ui->edtExperimentVisc->setRange(0,1e10);
    QList<int> s;
    s<<ui->splitter->width()/2<<ui->splitter->width()/2;
    ui->splitter->setSizes(s);
    ui->plotter->get_plotter()->set_keyPosition(JKQTPkeyInsideTopLeft);
    readComponents();
    readSamples();
    readSettings();
    updateD();
    QTimer::singleShot(10000, this, SLOT(writeSettings()));
}

DlgCalcDiffCoeff::~DlgCalcDiffCoeff()
{
    if (dlgInfo) {
        dlgInfo->close();
        delete dlgInfo;
    }
    delete ui;
}

void DlgCalcDiffCoeff::updateD() {
    if (updating) return;






    // calculate solution properties
    tab->setReadonly(false);
    tab->clear();
    tab->appendColumn();
    tab->appendColumn();
    tab->appendColumn();
    tab->appendColumn();
    tab->setColumnTitle(0, tr("temperature [%1C]").arg(QLatin1String("\xB0")));
    tab->setColumnTitle(1, QLatin1String("D [\xB5m^2/s]"));
    tab->setColumnTitle(2, tr("viscosity [mPa s]"));
    tab->setColumnTitle(3, tr("density [kg/l]"));

    QList<QFEDiffusionCoefficientCalculator::Component> comps;
    QComboBox* cmb;
    cmb=ui->cmbCType1;
    if (cmb->currentIndex()>0) {
        QDoubleSpinBox* spin=ui->spinCConcentration1;
        QString unit=plugin->getComponentUnit(cmb->currentIndex()-1);
        double cmax=plugin->getComponentCMax(cmb->currentIndex()-1);
        if (unit=="M") {
            spin->setRange(0, cmax*1000.0);
            spin->setSuffix(QString(" mM"));
        } else {
            spin->setSuffix(QString(" %1").arg(unit));
            spin->setRange(0, cmax);
        }
        QFEDiffusionCoefficientCalculator::Component c;
        c.id=cmb->currentIndex()-1;
        if (unit=="M") c.concentration_molar=spin->value()/1000.0;
        else c.concentration_molar=spin->value();
        comps.append(c);
    }
    cmb=ui->cmbCType2;
    if (cmb->currentIndex()>0) {
        QDoubleSpinBox* spin=ui->spinCConcentration2;
        QString unit=plugin->getComponentUnit(cmb->currentIndex()-1);
        double cmax=plugin->getComponentCMax(cmb->currentIndex()-1);
        if (unit=="M") {
            spin->setRange(0, cmax*1000.0);
            spin->setSuffix(QString(" mM"));
        } else {
            spin->setSuffix(QString(" %1").arg(unit));
            spin->setRange(0, cmax);
        }
        QFEDiffusionCoefficientCalculator::Component c;
        c.id=cmb->currentIndex()-1;
        if (unit=="M") c.concentration_molar=spin->value()/1000.0;
        else c.concentration_molar=spin->value();
        comps.append(c);
    }
    cmb=ui->cmbCType3;
    if (cmb->currentIndex()>0) {
        QDoubleSpinBox* spin=ui->spinCConcentration3;
        QString unit=plugin->getComponentUnit(cmb->currentIndex()-1);
        double cmax=plugin->getComponentCMax(cmb->currentIndex()-1);
        if (unit=="M") {
            spin->setRange(0, cmax*1000.0);
            spin->setSuffix(QString(" mM"));
        } else {
            spin->setSuffix(QString(" %1").arg(unit));
            spin->setRange(0, cmax);
        }
        QFEDiffusionCoefficientCalculator::Component c;
        c.id=cmb->currentIndex()-1;
        if (unit=="M") c.concentration_molar=spin->value()/1000.0;
        else c.concentration_molar=spin->value();
        comps.append(c);
    }
    cmb=ui->cmbCType4;
    if (cmb->currentIndex()>0) {
        QDoubleSpinBox* spin=ui->spinCConcentration4;
        QString unit=plugin->getComponentUnit(cmb->currentIndex()-1);
        double cmax=plugin->getComponentCMax(cmb->currentIndex()-1);
        if (unit=="M") {
            spin->setRange(0, cmax*1000.0);
            spin->setSuffix(QString(" mM"));
        } else {
            spin->setSuffix(QString(" %1").arg(unit));
            spin->setRange(0, cmax);
        }
        QFEDiffusionCoefficientCalculator::Component c;
        c.id=cmb->currentIndex()-1;
        if (unit=="M") c.concentration_molar=spin->value()/1000.0;
        else c.concentration_molar=spin->value();
        comps.append(c);
    }
    cmb=ui->cmbCType5;
    if (cmb->currentIndex()>0) {
        QDoubleSpinBox* spin=ui->spinCConcentration5;
        QString unit=plugin->getComponentUnit(cmb->currentIndex()-1);
        double cmax=plugin->getComponentCMax(cmb->currentIndex()-1);
        if (unit=="M") {
            spin->setRange(0, cmax*1000.0);
            spin->setSuffix(QString(" mM"));
        } else {
            spin->setSuffix(QString(" %1").arg(unit));
            spin->setRange(0, cmax);
        }
        QFEDiffusionCoefficientCalculator::Component c;
        c.id=cmb->currentIndex()-1;
        if (unit=="M") c.concentration_molar=spin->value()/1000.0;
        else c.concentration_molar=spin->value();
        comps.append(c);
    }
    temp.clear();
    D.clear();
    Dsphere.clear();
    Dwater.clear();
    Dsolution.clear();
    visc.clear();
    density.clear();
    double DD=0;
    for (double T=ui->spinSolutionTemperature->value(); T<=ui->spinSolutionTemperatureEnd->value(); T=T+ui->spinSolutionTemperatureDelta->value()) {
        temp.append(T);
        visc.append(plugin->getSolutionViscosity(ui->cmbSolutionName->currentIndex(), 273.15+T, comps)*1000.0*ui->spinViscosityFactor->value());
        density.append(plugin->getSolutionDensity(ui->cmbSolutionName->currentIndex(), 273.15+T, comps));
        if (ui->tabWidget->currentWidget()==ui->tabStdSample) {
                    D.append(plugin->getDCoeff_from_D(ui->cmbSolutionName->currentIndex(), ui->edtGivenD->value()/1e12, ui->edtGivenDVisc->value()/1000.0, ui->spinGivenDT->value()+273.15, 273.15+T, comps, ui->spinViscosityFactor->value())*1e12);
            Dsolution.append(plugin->getDCoeff_from_D(ui->cmbSolutionName->currentIndex(), ui->edtGivenD->value()/1e12, ui->edtGivenDVisc->value()/1000.0, ui->spinGivenDT->value()+273.15, 273.15+T, QList<QFEDiffusionCoefficientCalculator::Component>(), ui->spinViscosityFactor->value())*1e12);
               Dwater.append(plugin->getDCoeff_from_D(0, ui->edtGivenD->value()/1e12, ui->edtGivenDVisc->value()/1000.0, ui->spinGivenDT->value()+273.15, 273.15+T)*1e12);
        } else if (ui->tabWidget->currentWidget()==ui->givenD) {
                    D.append(plugin->getDCoeff_from_D(ui->cmbSolutionName->currentIndex(), ui->spinGivenDInSolution->value()/1e12, plugin->getSolutionViscosity(ui->cmbSolutionName->currentIndex(), 273.15+ui->spinGivenDInSolutionT->value(), comps)*ui->spinViscosityFactor->value(), ui->spinGivenDInSolutionT->value()+273.15, 273.15+T, comps, ui->spinViscosityFactor->value())*1e12);
            Dsolution.append(plugin->getDCoeff_from_D(ui->cmbSolutionName->currentIndex(), ui->spinGivenDInSolution->value()/1e12, plugin->getSolutionViscosity(ui->cmbSolutionName->currentIndex(), 273.15+ui->spinGivenDInSolutionT->value(), comps)*ui->spinViscosityFactor->value(), ui->spinGivenDInSolutionT->value()+273.15, 273.15+T, QList<QFEDiffusionCoefficientCalculator::Component>(), ui->spinViscosityFactor->value())*1e12);
               Dwater.append(plugin->getDCoeff_from_D(0, ui->spinGivenDInSolution->value()/1e12, plugin->getSolutionViscosity(ui->cmbSolutionName->currentIndex(), 273.15+ui->spinGivenDInSolutionT->value(), comps)*ui->spinViscosityFactor->value(), ui->spinGivenDInSolutionT->value()+273.15, 273.15+T)*1e12);
        } else if (ui->tabWidget->currentWidget()==ui->givenD20W) {
                    D.append(plugin->getDCoeff_from_D20W(ui->cmbSolutionName->currentIndex(), ui->spinGivenD20W->value()/1e12, 273.15+T, comps, ui->spinViscosityFactor->value())*1e12);
            Dsolution.append(plugin->getDCoeff_from_D20W(ui->cmbSolutionName->currentIndex(), ui->spinGivenD20W->value()/1e12, 273.15+T, QList<QFEDiffusionCoefficientCalculator::Component>(), ui->spinViscosityFactor->value())*1e12);
               Dwater.append(plugin->getDCoeff_from_D20W(0, ui->spinGivenD20W->value()/1e12, 273.15+T)*1e12);
        } else if (ui->tabWidget->currentWidget()==ui->tabGeometry) {
            double DS=0;
                    D.append(plugin->getShapeDCoeff(ui->cmbSolutionName->currentIndex(), ui->spinRotationAxis->value()*1e-9, ui->spinOtherAxis->value()*1e-9, QFEDiffusionCoefficientCalculator::SpheroidType(ui->cmbShapeType->currentIndex()), 273.15+T, comps, ui->spinViscosityFactor->value(), &DS)*1e12);
            Dsolution.append(plugin->getShapeDCoeff(ui->cmbSolutionName->currentIndex(), ui->spinRotationAxis->value()*1e-9, ui->spinOtherAxis->value()*1e-9, QFEDiffusionCoefficientCalculator::SpheroidType(ui->cmbShapeType->currentIndex()), 273.15+T, QList<QFEDiffusionCoefficientCalculator::Component>(), ui->spinViscosityFactor->value())*1e12);
               Dwater.append(plugin->getShapeDCoeff(0, ui->spinRotationAxis->value()*1e-9, ui->spinOtherAxis->value()*1e-9, QFEDiffusionCoefficientCalculator::SpheroidType(ui->cmbShapeType->currentIndex()), 273.15+T)*1e12);
              Dsphere.append(DS*1e12);
        } else  {
            D.append(0);
            Dsolution.append(0);
            Dwater.append(0);
        }
        tab->appendRow();
        tab->setCell(tab->rowCount()-1, 0, temp.last());
        tab->setCell(tab->rowCount()-1, 1, D.last());
        tab->setCell(tab->rowCount()-1, 2, visc.last());
        tab->setCell(tab->rowCount()-1, 3, density.last());
    }
    double T=ui->spinExperimentTemperature->value();
    if (ui->tabWidget->currentWidget()==ui->tabStdSample) {
        ui->labGivenDD20W->setText(tr("D<sub>20,W</sub> = %1 &mu;m<sup>2</sup>/s").arg(plugin->getDCoeff_from_D(0, ui->edtGivenD->value()/1e12, ui->edtGivenDVisc->value()/1000.0, ui->spinGivenDT->value()+273.15, 293.15)*1e12));
        ui->labGivenDRH->setText(tr("R<sub>H</sub> = %1 nm").arg(plugin->getHydrodynRadius_from_DEtaT(ui->edtGivenD->value()/1e12, ui->edtGivenDVisc->value()/1000.0, ui->spinGivenDT->value()+273.15)*1e9));
        DD=plugin->getDCoeff_from_D(ui->cmbSolutionName->currentIndex(), ui->edtGivenD->value()/1e12, ui->edtGivenDVisc->value()/1000.0, ui->spinGivenDT->value()+273.15, 273.15+T, comps, ui->spinViscosityFactor->value())*1e12;
    } else if (ui->tabWidget->currentWidget()==ui->givenD) {
        double lvisc=plugin->getSolutionViscosity(ui->cmbSolutionName->currentIndex(), 273.15+ui->spinGivenDInSolutionT->value(), comps)*ui->spinViscosityFactor->value();
        ui->labGivenDInSolutionVisc->setText(QString::number(lvisc*1000.0));
        DD=plugin->getDCoeff_from_D(ui->cmbSolutionName->currentIndex(), ui->spinGivenDInSolution->value()/1e12,
                                          lvisc, ui->spinGivenDInSolutionT->value()+273.15, 273.15+T, comps, ui->spinViscosityFactor->value())*1e12;
        ui->labGivenDInSolutionD20W->setText(QString::number(plugin->getDCoeff_from_D(ui->cmbSolutionName->currentIndex(), ui->spinGivenDInSolution->value()/1e12,
                                                                                      lvisc, ui->spinGivenDInSolutionT->value()+273.15, 293.15, comps, ui->spinViscosityFactor->value())*1e12));
        ui->labGivenDInSolutionRH->setText(QString::number(plugin->getHydrodynRadius_from_DEtaT(ui->spinGivenDInSolution->value()/1e12, lvisc, ui->spinGivenDInSolutionT->value()+273.15)*1e9));
    } else if (ui->tabWidget->currentWidget()==ui->givenD20W) {
        DD=plugin->getDCoeff_from_D20W(ui->cmbSolutionName->currentIndex(), ui->spinGivenD20W->value()/1e12, 273.15+T, comps, ui->spinViscosityFactor->value())*1e12;
        ui->labGivenD20WRH->setText(QString::number(plugin->getHydrodynRadius_from_DEtaT(ui->spinGivenD20W->value()/1e12, plugin->getSolutionViscosity(0, 293.15), 293.15)*1e9));
    } else if (ui->tabWidget->currentWidget()==ui->tabGeometry) {
        DD=plugin->getShapeDCoeff(ui->cmbSolutionName->currentIndex(), ui->spinRotationAxis->value()*1e-9, ui->spinOtherAxis->value()*1e-9, QFEDiffusionCoefficientCalculator::SpheroidType(ui->cmbShapeType->currentIndex()), 273.15+T, comps, ui->spinViscosityFactor->value())*1e12;
        double D20Wsphere=0;
        double volume=0;
        double D20W=plugin->getShapeDCoeff(0, ui->spinRotationAxis->value()*1e-9, ui->spinOtherAxis->value()*1e-9, QFEDiffusionCoefficientCalculator::SpheroidType(ui->cmbShapeType->currentIndex()), 273.15+20.0, QList<QFEDiffusionCoefficientCalculator::Component>(), 1, &D20Wsphere, &volume)*1e12;
        double etaW20=plugin->getSolutionViscosity(0, 293.15);
        D20Wsphere*=1e12;
        volume=volume*1e27;
        double dsphere=2.0*pow(volume/4.0*3.0/M_PI, 1.0/3.0);
        double RH=plugin->getHydrodynRadius_from_DEtaT(D20W/1e12, etaW20, 293.15)*1e9;
        ui->labShapeD->setText(tr("D<sub>20,W</sub> = %1 &mu;m<sup>2</sup>/s&nbsp;&nbsp;&nbsp;&nbsp;R<sub>H</sub> = %5 nm&nbsp;&nbsp;&nbsp;&nbsp;D<sub>20,W</sub>(sphere) = %2 &mu;m<sup>2</sup>/s&nbsp;&nbsp;&nbsp;&nbsp;V = %3 nm<sup>3</sup>&nbsp;&nbsp;&nbsp;&nbsp;d<sub>sphere</sub> = %4 nm").arg(D20W).arg(D20Wsphere).arg(volume).arg(dsphere).arg(RH));
    } else  {
        DD=0;
    }
    ui->edtExperimentD->setValue(DD);
    ui->edtExperimentRHyd->setValue(plugin->getHydrodynRadius_from_D(DD/1e12,ui->cmbSolutionName->currentIndex(), 273.15+ui->spinExperimentTemperature->value(), comps, ui->spinViscosityFactor->value())*1e9);
    ui->edtExperimentVisc->setValue(plugin->getSolutionViscosity(ui->cmbSolutionName->currentIndex(), 273.15+ui->spinExperimentTemperature->value(), comps)*ui->spinViscosityFactor->value()*1000.0);
    tab->setReadonly(true);
    redoPlot();
    on_cmbShapeType_currentIndexChanged(ui->cmbShapeType->currentIndex());
}

void DlgCalcDiffCoeff::updateGivenD() {
    QString filename=QFPluginServices::getInstance()->getAssetsDirectory()+"/plugins/"+plugin->getID()+"/samples.ini";
    QSettings set(filename, QSettings::IniFormat);
    QString filenameC=ProgramOptions::getConfigValue("calc_diffcoeff/user_database", QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/calc_diffcoeff/mysamples.ini").toString();
    /*QFileInfo fi(filenameC);
    QDir d;
    d.mkpath(fi.absolutePath());*/
    QSettings setC(filenameC, QSettings::IniFormat);

    updating=true;

    QString name=ui->cmbGivenDName->currentText();
    if (name.endsWith(userDDescriptor)) name=name.left(name.size()-userDDescriptor.size());
    //ui->cmbGivenDName->clear();
    // search for the name
    QStringList groups=set.childGroups();
    for (int i=0; i<groups.size(); i++) {
        QString n=set.value(groups[i]+"/name", "").toString();
        if (n==name) {
            ui->spinGivenDT->setValue(set.value(groups[i]+"/temperature", 20).toDouble());
            ui->edtGivenD->setValue(set.value(groups[i]+"/D", 10).toDouble());
            ui->edtGivenDVisc->setValue(set.value(groups[i]+"/viscosity", 1).toDouble());
            //ui->cmbGivenDName->setToolTip(set.value(groups[i]+"/reference", "").toString());
            QVariant ref=set.value(groups[i]+"/reference", "");
            if (ref.type()==QVariant::StringList) ref=ref.toStringList().join(", ");
            ui->edtReference->setText(ref.toString());
            ui->edtGivenDError->setValue(set.value(groups[i]+"/D_error", 0).toDouble());
            ui->edtBuffer->setText(set.value(groups[i]+"/solvent", set.value(groups[i]+"/buffer", "").toString()).toString());
            ui->edtMethod->setText(set.value(groups[i]+"/method", "").toString());
            break;
        }
    }
    groups=setC.childGroups();
    for (int i=0; i<groups.size(); i++) {
        QString n=setC.value(groups[i]+"/name", "").toString();
        if (n==name) {
            ui->spinGivenDT->setValue(setC.value(groups[i]+"/temperature", 20).toDouble());
            ui->edtGivenD->setValue(setC.value(groups[i]+"/D", 10).toDouble());
            ui->edtGivenDVisc->setValue(setC.value(groups[i]+"/viscosity", 1).toDouble());
            //ui->cmbGivenDName->setToolTip(setC.value(groups[i]+"/reference", "").toString());
            QVariant ref=setC.value(groups[i]+"/reference", "");
            if (ref.type()==QVariant::StringList) ref=ref.toStringList().join(", ");
            ui->edtReference->setText(ref.toString());
            ui->edtGivenDError->setValue(set.value(groups[i]+"/D_error", 0).toDouble());
            ui->edtBuffer->setText(set.value(groups[i]+"/solvent", set.value(groups[i]+"/buffer", "").toString()).toString());
            ui->edtMethod->setText(set.value(groups[i]+"/method", "").toString());
            break;
        }
    }

    updating=false;
    updateD();
}

void DlgCalcDiffCoeff::updatePlot()
{
    updateD();
}

void DlgCalcDiffCoeff::on_btnDeleteGivenD_clicked() {
    //QString filenameC=QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/"+plugin->getID()+"/mysamples.ini";
    QString filenameC=ProgramOptions::getConfigValue("calc_diffcoeff/user_database", QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/calc_diffcoeff/mysamples.ini").toString();
    QDir().mkpath(QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/calc_diffcoeff/");

    QSettings set(filenameC, QSettings::IniFormat);

    //if (set.isWritable()) {

        QString name=ui->cmbGivenDName->currentText();
        if (name.endsWith(userDDescriptor)) name=name.left(name.size()-userDDescriptor.size());

        // search for the name
        QStringList groups=set.childGroups();
        int idx_max=0;
        int idx_name=-1;
        for (int i=0; i<groups.size(); i++) {
            QRegExp rx("sample(\\d+)", Qt::CaseInsensitive);
            if (rx.indexIn(groups[i])>-1) {
                idx_max=rx.cap(1).toInt();
                if (set.value(QString("%1/name").arg(groups[i]), "").toString()==name) idx_name=rx.cap(1).toInt();
            }
        }

        if (idx_name>=0) {
            if (QMessageBox::question(this, tr("Delete Sample Data?"), tr("Delete the sample with the name '%1'?").arg(name), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
                set.remove(QString("sample%1").arg(idx_max));
                readSamples();
            }
        }


}

void DlgCalcDiffCoeff::on_btnSaveGivenD_clicked() {
    //QString filenameC=QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/"+plugin->getID()+"/mysamples.ini";
    QString filenameC=ProgramOptions::getConfigValue("calc_diffcoeff/user_database", QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/calc_diffcoeff/mysamples.ini").toString();
    QDir().mkpath(QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/calc_diffcoeff/");

    QSettings set(filenameC, QSettings::IniFormat);

    //if (set.isWritable()) {

        QString name=ui->cmbGivenDName->currentText();
        if (name.endsWith(userDDescriptor)) name=name.left(name.size()-userDDescriptor.size());

        // search for the name
        QStringList groups=set.childGroups();
        int idx_max=0;
        int idx_name=-1;
        for (int i=0; i<groups.size(); i++) {
            QRegExp rx("sample(\\d+)", Qt::CaseInsensitive);
            if (rx.indexIn(groups[i])>-1) {
                idx_max=rx.cap(1).toInt();
                if (set.value(QString("%1/name").arg(groups[i]), "").toString()==name) idx_name=rx.cap(1).toInt();
            }
        }

        idx_max++;
        bool write=true;
        if (idx_name>=0) {
            if (QMessageBox::question(this, tr("Overwrite Sample Data?"), tr("A sample with the name '%1' already exists. Overwrite?").arg(name), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::No) {
                write=false;
            } else {
                idx_max=idx_name;
                write=true;
            }
        }
        if (write) {
            set.setValue(QString("sample%1/name").arg(idx_max), name);
            set.setValue(QString("sample%1/D").arg(idx_max), ui->edtGivenD->value());
            set.setValue(QString("sample%1/viscosity").arg(idx_max), ui->edtGivenDVisc->value());
            set.setValue(QString("sample%1/temperature").arg(idx_max), ui->spinGivenDT->value());
            set.setValue(QString("sample%1/reference").arg(idx_max), ui->edtReference->text());
            set.setValue(QString("sample%1/D_error").arg(idx_max), ui->edtGivenDError->value());
            set.setValue(QString("sample%1/solvent").arg(idx_max), ui->edtBuffer->text());
            set.setValue(QString("sample%1/method").arg(idx_max), ui->edtMethod->text());
            readSamples();
        }
    //} else {

    //}
}

void DlgCalcDiffCoeff::redoPlot()
{
    if (updating) return;
    ui->plotter->set_doDrawing(false);
    ui->plotter->clearGraphs();
    ui->plotter->getDatastore()->clear();
    c_temp=ui->plotter->getDatastore()->addColumn(temp.data(), temp.size(), QLatin1String("temperature [\xB0 C]"));
    c_D=ui->plotter->getDatastore()->addColumn(D.data(), D.size(), QLatin1String("D [\xB5m^2/s]"));
    c_visc=ui->plotter->getDatastore()->addColumn(visc.data(), visc.size(), "viscosity [mPa s]");
    c_density=ui->plotter->getDatastore()->addColumn(density.data(), density.size(), "density [kg/l]");
    c_Dwater=ui->plotter->getDatastore()->addColumn(Dwater.data(), Dwater.size(), QLatin1String("D_water [\xB5m^2/s]"));
    c_Dsolution=ui->plotter->getDatastore()->addColumn(Dsolution.data(), Dsolution.size(), QLatin1String("D_{solution} [\xB5m^2/s]"));
    c_Dsphere=ui->plotter->getDatastore()->addColumn(Dsphere.data(), Dsphere.size(), QLatin1String("D of sphere [\xB5m^2/s]"));
    ui->plotter->get_plotter()->getYAxis()->set_axisLabel(tr(""));
    ui->plotter->get_plotter()->set_showKey(true);

    if (ui->cmbPlot->currentIndex()==0) {
        JKQTPxyLineGraph* g;


        if (ui->tabWidget->currentWidget()==ui->tabGeometry) {
            g=new JKQTPxyLineGraph(ui->plotter->get_plotter());
            g->set_drawLine(true);
            g->set_xColumn(c_temp);
            g->set_yColumn(c_Dsphere);
            g->set_title(tr("D of a sphere in solution"));
            ui->plotter->addGraph(g);
        }

        if (ui->cmbSolutionName->currentIndex()>0) {
            g=new JKQTPxyLineGraph(ui->plotter->get_plotter());
            g->set_drawLine(true);
            g->set_xColumn(c_temp);
            g->set_yColumn(c_Dwater);
            g->set_title(tr("D in water"));
            g->set_style(Qt::DotLine);
            ui->plotter->addGraph(g);
        }
        g=new JKQTPxyLineGraph(ui->plotter->get_plotter());
        g->set_drawLine(true);
        g->set_xColumn(c_temp);
        g->set_yColumn(c_Dsolution);
        g->set_title(tr("D in solution w/o components"));
        g->set_style(Qt::DashLine);
        ui->plotter->addGraph(g);

        g=new JKQTPxyLineGraph(ui->plotter->get_plotter());
        g->set_drawLine(true);
        g->set_xColumn(c_temp);
        g->set_yColumn(c_D);
        g->set_title(tr("D in solution"));
        ui->plotter->addGraph(g);

        ui->plotter->get_plotter()->getYAxis()->set_axisLabel(tr("diffusion coefficient [{\\mu}m^2/s]"));
    } else if (ui->cmbPlot->currentIndex()==1) {
        JKQTPxyLineGraph* g=new JKQTPxyLineGraph(ui->plotter->get_plotter());
        g->set_drawLine(true);
        g->set_xColumn(c_temp);
        g->set_yColumn(c_density);
        ui->plotter->addGraph(g);
        ui->plotter->get_plotter()->getYAxis()->set_axisLabel(tr("solution density [kg/l]"));
    } else if (ui->cmbPlot->currentIndex()==2) {
        JKQTPxyLineGraph* g=new JKQTPxyLineGraph(ui->plotter->get_plotter());
        g->set_drawLine(true);
        g->set_xColumn(c_temp);
        g->set_yColumn(c_visc);
        ui->plotter->addGraph(g);
        ui->plotter->get_plotter()->getYAxis()->set_axisLabel(tr("solution viscosity [mPa{\\cdot}s]"));
    }

    ui->plotter->zoomToFit();
    ui->plotter->set_doDrawing(true);
    ui->plotter->update_plot();
}

void DlgCalcDiffCoeff::readSettings() {
    ProgramOptions* opt=ProgramOptions::getInstance();
    if (opt) {
        QSettings* set=opt->getQSettings();
        if (set) {
            loadWidgetGeometry(*set, this, plugin->getID()+"/widget/");
            loadSplitter(*set, ui->splitter, plugin->getID()+"/splitter/");
            ui->cmbPlot->setCurrentIndex(set->value(plugin->getID()+"/plot", 0).toInt());
            ui->tabWidget->setCurrentIndex(set->value(plugin->getID()+"/sample", 0).toInt());
            ui->cmbPlot->setCurrentIndex(set->value(plugin->getID()+"/plot", 0).toInt());
            ui->spinExperimentTemperature->setValue(set->value(plugin->getID()+"/etemperature", 21).toDouble());
            ui->spinSolutionTemperature->setValue(set->value(plugin->getID()+"/temperature", 10).toDouble());
            ui->spinSolutionTemperatureEnd->setValue(set->value(plugin->getID()+"/temperature_end", 40).toDouble());
            ui->spinSolutionTemperatureDelta->setValue(set->value(plugin->getID()+"/temperature_delta", 0.5).toDouble());
            //ui->edtGivenD->setValue(set->value(plugin->getID()+"/givenD/D", 100).toDouble());
            //ui->edtGivenDVisc->setValue(set->value(plugin->getID()+"/givenD/visc", 1).toDouble());
            //ui->spinGivenDT->setValue(set->value(plugin->getID()+"/givenD/Temp", 20).toDouble());
            ui->spinGivenD20W->setValue(set->value(plugin->getID()+"/givenD20W", 100).toDouble());

            ui->spinGivenDInSolution->setValue(set->value(plugin->getID()+"/givenDInSolution/D", 100).toDouble());
            ui->spinGivenDInSolutionT->setValue(set->value(plugin->getID()+"/givenDInSolution/Temp", 20).toDouble());

            ui->cmbSolutionName->setCurrentIndex(set->value(plugin->getID()+"/solution", 0).toInt());
            ui->cmbGivenDName->setCurrentIndex(set->value(plugin->getID()+"/givenD/name", 0).toInt());
            ui->cmbCType1->setCurrentIndex(set->value(plugin->getID()+"/solution/component1", 0).toInt());
            ui->spinCConcentration1->setValue(set->value(plugin->getID()+"/solution/cncentration1", 0).toDouble());
            ui->cmbCType2->setCurrentIndex(set->value(plugin->getID()+"/solution/component2", 0).toInt());
            ui->spinCConcentration2->setValue(set->value(plugin->getID()+"/solution/cncentration2", 0).toDouble());
            ui->cmbCType3->setCurrentIndex(set->value(plugin->getID()+"/solution/component3", 0).toInt());
            ui->spinCConcentration3->setValue(set->value(plugin->getID()+"/solution/cncentration3", 0).toDouble());
            ui->cmbCType4->setCurrentIndex(set->value(plugin->getID()+"/solution/component4", 0).toInt());
            ui->spinCConcentration4->setValue(set->value(plugin->getID()+"/solution/cncentration4", 0).toDouble());
            ui->cmbCType5->setCurrentIndex(set->value(plugin->getID()+"/solution/component5", 0).toInt());
            ui->spinCConcentration5->setValue(set->value(plugin->getID()+"/solution/cncentration5", 0).toDouble());

            ui->spinViscosityFactor->setValue(set->value(plugin->getID()+"/solution/viscosityfactor", 1).toDouble());

            ui->spinRotationAxis->setValue(set->value(plugin->getID()+"/shape/rot", 4).toDouble());
            ui->spinOtherAxis->setValue(set->value(plugin->getID()+"/shape/other", 3).toDouble());
            ui->cmbShapeType->setCurrentIndex(set->value(plugin->getID()+"/shape/type", 0).toDouble());
            on_cmbShapeType_currentIndexChanged(ui->cmbShapeType->currentIndex());

            updateD();
            updateGivenD();
        }
    }
}

void DlgCalcDiffCoeff::writeSettings() {
    ProgramOptions* opt=ProgramOptions::getInstance();
    if (opt) {
        QSettings* set=opt->getQSettings();
        if (set) {
            saveWidgetGeometry(*set, this, plugin->getID()+"/widget/");
            saveSplitter(*set, ui->splitter, plugin->getID()+"/splitter/");
            set->setValue(plugin->getID()+"/plot", ui->cmbPlot->currentIndex());
            set->setValue(plugin->getID()+"/solution", ui->cmbSolutionName->currentIndex());
            set->setValue(plugin->getID()+"/sample", ui->tabWidget->currentIndex());
            set->setValue(plugin->getID()+"/plot", ui->cmbPlot->currentIndex());
            set->setValue(plugin->getID()+"/temperature", ui->spinSolutionTemperature->value());
            set->setValue(plugin->getID()+"/etemperature", ui->spinExperimentTemperature->value());
            set->setValue(plugin->getID()+"/temperature_end", ui->spinSolutionTemperatureEnd->value());
            set->setValue(plugin->getID()+"/temperature_delta", ui->spinSolutionTemperatureDelta->value());
            set->setValue(plugin->getID()+"/givenDInSolution/D", ui->spinGivenDInSolution->value());
            set->setValue(plugin->getID()+"/givenDInSolution/Temp", ui->spinGivenDInSolutionT->value());
            set->setValue(plugin->getID()+"/givenD/D", ui->edtGivenD->value());
            set->setValue(plugin->getID()+"/givenD/D_error", ui->edtGivenDError->value());
            set->setValue(plugin->getID()+"/givenD/visc", ui->edtGivenDVisc->value());
            set->setValue(plugin->getID()+"/givenD/Temp", ui->spinGivenDT->value());
            set->setValue(plugin->getID()+"/givenD/name", ui->cmbGivenDName->currentIndex());
            set->setValue(plugin->getID()+"/givenD/buffer", ui->edtBuffer->text());
            set->setValue(plugin->getID()+"/givenD/method", ui->edtMethod->text());
            set->setValue(plugin->getID()+"/shape/rot", ui->spinRotationAxis->value());
            set->setValue(plugin->getID()+"/shape/other", ui->spinOtherAxis->value());
            set->setValue(plugin->getID()+"/shape/type", ui->cmbShapeType->currentIndex());

            set->setValue(plugin->getID()+"/givenD20W", ui->spinGivenD20W->value());

            set->setValue(plugin->getID()+"/solution/component1", ui->cmbCType1->currentIndex());
            set->setValue(plugin->getID()+"/solution/cncentration1", ui->spinCConcentration1->value());
            set->setValue(plugin->getID()+"/solution/component2", ui->cmbCType2->currentIndex());
            set->setValue(plugin->getID()+"/solution/cncentration2", ui->spinCConcentration2->value());
            set->setValue(plugin->getID()+"/solution/component3", ui->cmbCType3->currentIndex());
            set->setValue(plugin->getID()+"/solution/cncentration3", ui->spinCConcentration3->value());
            set->setValue(plugin->getID()+"/solution/component4", ui->cmbCType4->currentIndex());
            set->setValue(plugin->getID()+"/solution/cncentration4", ui->spinCConcentration4->value());
            set->setValue(plugin->getID()+"/solution/component5", ui->cmbCType5->currentIndex());
            set->setValue(plugin->getID()+"/solution/cncentration5", ui->spinCConcentration5->value());


            set->setValue(plugin->getID()+"/solution/viscosityfactor", ui->spinViscosityFactor->value());
        }
    }
}

void DlgCalcDiffCoeff::readSamples() {
    QString filename=QFPluginServices::getInstance()->getAssetsDirectory()+"/plugins/"+plugin->getID()+"/samples.ini";
    //QString filenameC=QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/"+plugin->getID()+"/mysamples.ini";
    QString filenameC=ProgramOptions::getConfigValue("calc_diffcoeff/user_database", QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/calc_diffcoeff/mysamples.ini").toString();
    //qDebug()<<"reading samples.ini: "<<filename;
    QSettings set(filename, QSettings::IniFormat);
    QSettings setC(filenameC, QSettings::IniFormat);

    updating=true;

    QString ct=ui->cmbGivenDName->currentText();
    QStringList items;

    // search for the name
    QStringList groups=set.childGroups();
    for (int i=0; i<groups.size(); i++) {
        set.beginGroup(groups[i]);
        QString n=set.value("name", "").toString();
        //ui->cmbGivenDName->addItem(n);
        items<<n;
        set.endGroup();
    }

    groups=setC.childGroups();
    for (int i=0; i<groups.size(); i++) {
        setC.beginGroup(groups[i]);
        QString n=setC.value("name", "").toString();
        //ui->cmbGivenDName->addItem(n);
        items<<n+userDDescriptor;

        setC.endGroup();
    }
    ui->cmbGivenDName->clear();
    ui->cmbGivenDName->addItems(items);

    updating=false;
    ui->cmbGivenDName->setCurrentIndex(ui->cmbGivenDName->findText(ct));
    updateD();
}

void DlgCalcDiffCoeff::readComponents()
{
    QStringList comps;
    comps<<tr("--- none ---");

    for (int c=0; c<plugin->getComponentCount(); c++) {
        comps.append(plugin->getComponentName(c));
    }

    ui->cmbCType1->clear();
    ui->cmbCType2->clear();
    ui->cmbCType3->clear();
    ui->cmbCType4->clear();
    ui->cmbCType5->clear();
    ui->cmbCType1->addItems(comps);
    ui->cmbCType2->addItems(comps);
    ui->cmbCType3->addItems(comps);
    ui->cmbCType4->addItems(comps);
    ui->cmbCType5->addItems(comps);
}

void DlgCalcDiffCoeff::updateReport(const QModelIndex &index) {
    QVariant d=ui->tableView->model()->data(index);
    switch (index.column()) {
        case 0:
            plugin->setReportVal("temperature", d.toDouble());
            break;
        case 1:
            plugin->setReportVal("diff_coeff", d.toDouble());
            break;
        case 2:
            plugin->setReportVal("viscosity", d.toDouble());
            break;
        case 3:
            plugin->setReportVal("density", d.toDouble());
            break;
    }

}

void DlgCalcDiffCoeff::on_btnUseVisc_clicked() {
    on_btnUseD_clicked();
}

void DlgCalcDiffCoeff::on_btnUseD_clicked() {
    plugin->setReportVal("temperature", ui->spinExperimentTemperature->value());
    plugin->setReportVal("diff_coeff", ui->edtExperimentD->value());
    plugin->setReportVal("viscosity", ui->edtExperimentVisc->value());
    close();
}

void DlgCalcDiffCoeff::on_btnCHelp1_clicked()
{
    if (ui->cmbCType1->currentIndex()>0) {
        if (!dlgInfo) dlgInfo=new DlgComponentInfo(plugin, ui->cmbCType1->currentIndex()-1, NULL);
        dlgInfo->setComponent(ui->cmbCType1->currentIndex()-1);
        dlgInfo->show();
        dlgInfo->raise();
    }
}

void DlgCalcDiffCoeff::on_btnCHelp2_clicked()
{
    if (ui->cmbCType2->currentIndex()>0) {
        dlgInfo=new DlgComponentInfo(plugin, ui->cmbCType2->currentIndex()-1, NULL);
        dlgInfo->setComponent(ui->cmbCType2->currentIndex()-1);
        dlgInfo->show();
        dlgInfo->raise();
    }
}

void DlgCalcDiffCoeff::on_btnCHelp3_clicked()
{
    if (ui->cmbCType3->currentIndex()>0) {
        dlgInfo=new DlgComponentInfo(plugin, ui->cmbCType3->currentIndex()-1, NULL);
        dlgInfo->setComponent(ui->cmbCType3->currentIndex()-1);
        dlgInfo->show();
        dlgInfo->raise();
    }
}

void DlgCalcDiffCoeff::on_btnCHelp4_clicked()
{
    if (ui->cmbCType4->currentIndex()>0) {
        dlgInfo=new DlgComponentInfo(plugin, ui->cmbCType4->currentIndex()-1, NULL);
        dlgInfo->setComponent(ui->cmbCType4->currentIndex()-1);
        dlgInfo->show();
        dlgInfo->raise();
    }
}

void DlgCalcDiffCoeff::on_btnCHelp5_clicked()
{
    if (ui->cmbCType5->currentIndex()>0) {
        dlgInfo=new DlgComponentInfo(plugin, ui->cmbCType5->currentIndex()-1, NULL);
        dlgInfo->setComponent(ui->cmbCType5->currentIndex()-1);
        dlgInfo->show();
        dlgInfo->raise();
    }
}

void DlgCalcDiffCoeff::on_btnEditGivenD_toggled(bool checked)
{
    ui->cmbGivenDName->setEditable(checked);

}

void DlgCalcDiffCoeff::on_cmbShapeType_currentIndexChanged(int index) {
    if (index==0) {
        if (ui->spinRotationAxis->value()/ui->spinOtherAxis->value()<=1.0) ui->labShapePic->setPixmap(QPixmap(":/calc_diffcoeff/oblate.png"));
        else  ui->labShapePic->setPixmap(QPixmap(":/calc_diffcoeff/prolate.png"));
        ui->labRotAxis->setText(tr("rotation axis diameter a:"));
        ui->labOtherAxis->setText(tr("other axes diameter b:"));
        ui->labOtherAxis->setVisible(true);
        ui->spinOtherAxis->setVisible(true);
    } else if (index==1) {
        ui->labShapePic->setPixmap(QPixmap(":/calc_diffcoeff/cylinder.png"));
        ui->labRotAxis->setText(tr("cylinder length L:"));
        ui->labOtherAxis->setText(tr("diameter d:"));
        ui->labOtherAxis->setVisible(true);
        ui->spinOtherAxis->setVisible(true);
    } else {
        ui->labShapePic->setPixmap(QPixmap(":/calc_diffcoeff/sphere.png"));
        ui->labRotAxis->setText(tr("diameter d:"));
        ui->labOtherAxis->setText(tr("---"));
        ui->labOtherAxis->setVisible(false);
        ui->spinOtherAxis->setVisible(false);
    }
}

void DlgCalcDiffCoeff::showHelp()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelp(plugin->getID()));
}

void DlgCalcDiffCoeff::on_btnSendEmailGivenD_clicked() {
    //QString filenameC=QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/"+plugin->getID()+"/mysamples.ini";
    QString filenameC=ProgramOptions::getConfigValue("calc_diffcoeff/user_database", QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/calc_diffcoeff/mysamples.ini").toString();
    QFile f(filenameC);
    QString filecontents;
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        filecontents=f.readAll();
        f.close();
    }

    QUrl url=QUrl(QByteArray("mailto:")+qfInfoEmail().toLocal8Bit()+"?subject="+QUrl::toPercentEncoding("QuickFit3/calc_diffcoeff: new given D data")+
                  "&body="+QUrl::toPercentEncoding(QString("Dear authors,\nfind attatched my given D data for inclusion in the next QuickFit release,\n\nBest,\n\n\nDATA:\n---------------------------------------------------------------------\n%1\n---------------------------------------------------------------------\n").arg(filecontents)));
    QDesktopServices::openUrl(url);
}

