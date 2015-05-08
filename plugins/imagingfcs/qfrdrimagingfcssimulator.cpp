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



#include "qfrdrimagingfcssimulator.h"
#include "ui_qfrdrimagingfcssimulator.h"
#include "qfpluginservices.h"
#include "programoptions.h"
#include "qfstyledbutton.h"


QFRDRImagingFCSSimulator::QFRDRImagingFCSSimulator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSSimulator)
{
    dstate=dsParameterInput;
    lastSimFile="";
    ui->setupUi(this);
    readSettings();
    ui->groupBox->setVisible(false);
    sim=new QFRDRImagingFCSSimulationThread(this);
    connect(sim, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(sim, SIGNAL(statusMessage(QString)), ui->labProgress, SLOT(setText(QString)));
    connect(sim, SIGNAL(finished()), this, SLOT(threadFinished()));
    ui->edtFilename->addInsertContextMenuEntry(tr("insert %counter%"), QString("%counter%"));
    ui->edtFilename->addButton(new QFStyledButton(QFStyledButton::SelectNewFile, ui->edtFilename, ui->edtFilename));
}

QFRDRImagingFCSSimulator::~QFRDRImagingFCSSimulator()
{
    sim->cancel();
    sim->waitForFinish();
    //writeSettings();
    delete ui;
}

QString QFRDRImagingFCSSimulator::getSimulationFilename() const
{
    return lastSimFile;
}

void QFRDRImagingFCSSimulator::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("imaging_fcs")+"/simulation.html");
}

void QFRDRImagingFCSSimulator::writeSettings() const {
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/DG", ui->spinDG->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/DR", ui->spinDR->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/DRG", ui->spinDRG->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/walkersr", ui->spinWalkersR->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/walkersg", ui->spinWalkersG->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/walkersrg", ui->spinWalkersRG->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/brightnessG", ui->spinBrightnessG->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/brightnessR", ui->spinBrigthnessR->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DG", ui->chkBottomG->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DR", ui->chkBottomR->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DRG", ui->chkBottomRG->isChecked());

    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/DG_2", ui->spinDG_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/DR_2", ui->spinDR_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/DRG_2", ui->spinDRG_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/walkersr_2", ui->spinWalkersR_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/walkersg_2", ui->spinWalkersG_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/walkersrg_2", ui->spinWalkersRG_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/brightnessG_2", ui->spinBrightnessG_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/brightnessR_2", ui->spinBrigthnessR_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DG_2", ui->chkBottomG_2->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DR_2", ui->chkBottomR_2->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DRG_2", ui->chkBottomRG_2->isChecked());

    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/FlowOnlyHalf", ui->chkFlowOnlyHalf->isCheckable());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/height", ui->spinHeight->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/width", ui->spinWidth->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/pixel_size", ui->spinPixelSize->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/frames", ui->spinFrames->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/frametime", ui->spinFrametime->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/warmup", ui->spinWarmup->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/crosstalk", ui->spinCrosstalk->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/psfSizeGreen", ui->spinPSFSizeGreen->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/psfSizeRed", ui->spinPSFSizeRed->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/VX", ui->spinVX->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/VY", ui->spinVY->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/DualView", ui->chkDualView->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/filename", ui->edtFilename->text());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/background", ui->spinBackground->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/backgroundNoise", ui->spinBackgroundNoise->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/deltax", ui->spinDeltaX->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/deltay", ui->spinDeltaY->value());
}

void QFRDRImagingFCSSimulator::readSettings()
{
    ui->edtFilename->setText(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/filename", "").toString());
    ui->spinDG->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DG", 10).toDouble());
    ui->spinDR->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DR", 10).toDouble());
    ui->spinDRG->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DRG", 10).toDouble());
    ui->spinHeight->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/height", 10).toInt());
    ui->spinWidth->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/width", 10).toInt());
    ui->spinPixelSize->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/pixel_size", 0.4).toDouble());
    ui->spinWalkersR->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/walkersR", 20).toInt());
    ui->spinWalkersG->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/walkersG", 20).toInt());
    ui->spinWalkersRG->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/walkersRG", 20).toInt());
    ui->spinFrames->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/frames", 20000).toInt());
    ui->spinFrametime->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/frametime", 20).toDouble());
    ui->spinWarmup->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/warmup", 10000).toDouble());
    ui->spinPSFSizeGreen->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/psfSizeGreen", 0.6).toDouble());
    ui->spinPSFSizeRed->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/psfSizeRed", 0.7).toDouble());
    ui->spinCrosstalk->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/crosstalk", 5).toDouble());
    ui->spinVX->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/VX", 0).toDouble());
    ui->spinVY->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/VY", 0).toDouble());
    ui->chkDualView->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DualView", false).toBool());
    ui->spinBrightnessG->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/brightnessG", 20).toDouble());
    ui->spinBrigthnessR->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/brightnessR", 20).toDouble());
    ui->spinBackground->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/background", 100).toDouble());
    ui->spinBackgroundNoise->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/backgroundNoise", 2).toDouble());
    ui->spinDeltaX->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/deltax", 0).toDouble());
    ui->spinDeltaY->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/deltay", 0).toDouble());




    ui->chkBottomG->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DG", false).toBool());
    ui->chkBottomR->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DR", false).toBool());
    ui->chkBottomRG->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DRG", false).toBool());
    ui->chkBottomG_2->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DG_2", false).toBool());
    ui->chkBottomR_2->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DR_2", false).toBool());
    ui->chkBottomRG_2->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DRG_2", false).toBool());
    ui->chkFlowOnlyHalf->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/FlowOnlyHalf", false).toBool());

    ui->spinDeltaY->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DG_2", 15).toDouble());
    ui->spinDeltaY->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DR_2", 15).toDouble());
    ui->spinDeltaY->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DRG_2", 15).toDouble());
    ui->spinDeltaY->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/brightnessG_2", 100).toDouble());
    ui->spinDeltaY->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/brightnessR_2", 100).toDouble());

    ui->spinWalkersR_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/walkersr_2", 0).toInt());
    ui->spinWalkersG_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/walkersg_2", 0).toInt());
    ui->spinWalkersRG_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/walkersrg_2", 0).toInt());
    ui->spinDG_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DG_2", 1).toDouble());
    ui->spinDR_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DR_2", 1).toDouble());
    ui->spinDRG_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DRG_2", 1).toDouble());
    ui->spinBrightnessG_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/brightnessG_2", 20).toDouble());
    ui->spinBrigthnessR_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/brightnessR_2", 20).toDouble());


}

void QFRDRImagingFCSSimulator::on_btnRun_clicked()
{
    if (dstate==dsParameterInput) {
        // START SIMULATION
        sim->set_brightnessG(100);
        sim->set_brightnessR(100);
        sim->set_DG(ui->spinDG->value());
        sim->set_DR(ui->spinDR->value());
        sim->set_DRG(ui->spinDRG->value());
        sim->set_DG2(ui->spinDG_2->value());
        sim->set_DR2(ui->spinDR_2->value());
        sim->set_DRG2(ui->spinDRG_2->value());
        sim->set_dualView(ui->chkDualView->isChecked());
        sim->set_filename(ui->edtFilename->text());
        sim->set_frames(ui->spinFrames->value());
        sim->set_warmup(ui->spinWarmup->value());
        sim->set_frametime(ui->spinFrametime->value());
        sim->set_height(ui->spinHeight->value());
        sim->set_width(ui->spinWidth->value());
        sim->set_pixel_size(ui->spinPixelSize->value());
        sim->set_psf_size_g(ui->spinPSFSizeGreen->value());
        sim->set_psf_size_r(ui->spinPSFSizeRed->value());
        sim->set_crosstalk(ui->spinCrosstalk->value()/100.0);
        sim->set_walkersG(ui->spinWalkersG->value());
        sim->set_walkersR(ui->spinWalkersR->value());
        sim->set_walkersRG(ui->spinWalkersRG->value());
        sim->set_walkersG2(ui->spinWalkersG_2->value());
        sim->set_walkersR2(ui->spinWalkersR_2->value());
        sim->set_walkersRG2(ui->spinWalkersRG_2->value());
        sim->set_VX(ui->spinVX->value());
        sim->set_VY(ui->spinVY->value());
        sim->set_FlowEeverywhere(!ui->chkFlowOnlyHalf->isChecked());
        sim->set_deltax(ui->spinDeltaX->value());
        sim->set_deltay(ui->spinDeltaY->value());
        sim->set_brightnessG(ui->spinBrightnessG->value());
        sim->set_brightnessR(ui->spinBrigthnessR->value());
        sim->set_background(ui->spinBackground->value());
        sim->set_backgroundNoise(ui->spinBackgroundNoise->value());

        sim->set_onlyHalf_DG(ui->chkBottomG->isChecked());
        sim->set_onlyHalf_DR(ui->chkBottomR->isChecked());
        sim->set_onlyHalf_DRG(ui->chkBottomRG->isChecked());
        sim->set_onlyHalf_DG2(ui->chkBottomG_2->isChecked());
        sim->set_onlyHalf_DR2(ui->chkBottomR_2->isChecked());
        sim->set_onlyHalf_DRG2(ui->chkBottomRG_2->isChecked());
        sim->set_brightnessG2(ui->spinBrightnessG_2->value());
        sim->set_brightnessR2(ui->spinBrigthnessR_2->value());

        sim->start();
        writeSettings();
        setState(dsRunning);
    } else if (dstate==dsFinished) { writeSettings(); setState(dsParameterInput); accept(); }
}

void QFRDRImagingFCSSimulator::on_btnCancel_clicked()
{
    if (dstate==dsParameterInput) { writeSettings(); lastSimFile=""; reject(); }
    else if (dstate==dsFinished) {  writeSettings(); lastSimFile=""; setState(dsParameterInput); reject(); }
    else if (dstate==dsRunning) {
        writeSettings();
        sim->cancel();
        sim->waitForFinish();
        setState(dsParameterInput);
    }
}

void QFRDRImagingFCSSimulator::threadFinished()
{
    if (sim->getCurrentFrame()>=sim->get_frames()-1) {
        setState(dsFinished);
    } else {
        setState(dsParameterInput);
    }
    lastSimFile=sim->get_filename();
    accept();
}

void QFRDRImagingFCSSimulator::setState(QFRDRImagingFCSSimulator::DialogState new_dstate)
{
    if (dstate==dsParameterInput) {
        if (new_dstate==dsRunning) {
            ui->grpParams->setEnabled(false);
            ui->groupBox->setEnabled(true);
            ui->progressBar->setRange(0, ui->spinFrames->value()+ui->spinWarmup->value());
            ui->progressBar->setValue(0);
            ui->labProgress->setText(tr("preparing simulation ..."));
        } else {
            new_dstate=dsParameterInput;
        }
    } else if (dstate==dsRunning) {
        if (new_dstate==dsFinished) {
            ui->grpParams->setEnabled(false);
            ui->groupBox->setEnabled(false);
            ui->progressBar->setValue(ui->progressBar->maximum());
            ui->labProgress->setText(tr("simulation complete !"));
        } else if (new_dstate==dsParameterInput) {
            ui->grpParams->setEnabled(true);
            ui->groupBox->setEnabled(false);
            ui->progressBar->setRange(0, ui->spinFrames->value());
            ui->progressBar->setValue(0);
            ui->labProgress->setText("");
        }
    }

    if (new_dstate==dsParameterInput) {
        ui->btnCancel->setText(tr("&Close"));
        ui->btnCancel->setEnabled(true);
        ui->btnRun->setText(tr("&Start Simulation"));
        ui->btnRun->setEnabled(true);
        ui->grpParams->setEnabled(true);
        ui->groupBox->setVisible(false);
    } else if (new_dstate==dsRunning) {
        ui->btnCancel->setText(tr("&Cancel Simulation"));
        ui->btnCancel->setEnabled(true);
        ui->btnRun->setText(tr("&Start Simulation"));
        ui->btnRun->setEnabled(false);
        ui->grpParams->setEnabled(false);
        ui->groupBox->setVisible(true);
    } else if (new_dstate==dsFinished) {
        ui->btnCancel->setText(tr("&Close"));
        ui->btnCancel->setEnabled(true);
        ui->btnRun->setText(tr("&Close & Insert"));
        ui->btnRun->setEnabled(true);
        ui->grpParams->setEnabled(false);
        ui->groupBox->setVisible(false);
    }

    dstate=new_dstate;
}
