/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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
    QFDialog(parent),
    ui(new Ui::QFRDRImagingFCSSimulator)
{
    dstate=dsParameterInput;
    lastSimFile="";
    ui->setupUi(this);
    ui->edtFilename->addInsertContextMenuEntry(tr("insert %counter%"), QString("%counter%"));
    ui->edtFilename->addButton(new QFStyledButton(QFStyledButton::SelectNewFile, ui->edtFilename, ui->edtFilename));

    ui->edtBarrierJumpProb->setRange(1e-10,1);
    ui->edtBarrierJumpProb->setValue(1e-3);
    ui->edtBarrierJumpProb->setCheckBounds(true, true);
    ui->edtBarrierJumpProb->setLogScale(true);


    ui->edtTrapSlowdown->setRange(1e-10,1e10);
    ui->edtTrapSlowdown->setValue(1e-1);
    ui->edtTrapSlowdown->setDecimals(3);
    ui->edtTrapSlowdown->setCheckBounds(true, true);
    ui->edtTrapSlowdown->setLogScale(true);


    ui->edtTrapJumpOut->setRange(0,1);
    ui->edtTrapJumpOut->setValue(1e-1);
    ui->edtTrapJumpOut->setDecimals(4);
    ui->edtTrapJumpOut->setCheckBounds(true, true);
    ui->edtTrapJumpOut->setLogScale(true);

    ui->edtTrapJumpIn->setRange(0,1);
    ui->edtTrapJumpIn->setValue(1e-1);
    ui->edtTrapJumpIn->setDecimals(4);
    ui->edtTrapJumpIn->setCheckBounds(true, true);
    ui->edtTrapJumpIn->setLogScale(true);

    readSettings();
    ui->groupBox->setVisible(false);
    sim=new QFRDRImagingFCSSimulationThread(this);
    connect(sim, SIGNAL(progress(int)), this, SLOT(updateSimStatus(int)));
    connect(sim, SIGNAL(statusMessage(QString)), ui->labProgress, SLOT(setText(QString)));
    connect(sim, SIGNAL(finished()), this, SLOT(threadFinished()));

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

QStringList QFRDRImagingFCSSimulator::getLastMSDNames() const
{
    return lastMSDNames;
}

QStringList QFRDRImagingFCSSimulator::getLastTrajNames() const
{
    return lastTrajNames;
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
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/boundary", ui->cmbBoundary->currentIndex());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/psf", ui->cmbPSF->currentIndex());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/psf_cutoff", ui->spinPSFCutoff->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/save_msd", ui->chkSaveMSD->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/msd_max_steps", ui->spinMsdMaxSteps->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/save_traj", ui->chkSaveTrajectories->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/traj_max", ui->spinTrajMax->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/traj_maxsteps", ui->spinTrajMaxSteps->value());

    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/DG_2", ui->spinDG_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/DG_3", ui->spinDG_3->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/DR_2", ui->spinDR_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/DRG_2", ui->spinDRG_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/walkersr_2", ui->spinWalkersR_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/walkersg_2", ui->spinWalkersG_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/walkersg_3", ui->spinWalkersG_3->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/walkersrg_2", ui->spinWalkersRG_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/brightnessG_2", ui->spinBrightnessG_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/brightnessG_3", ui->spinBrightnessG_3->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/brightnessR_2", ui->spinBrigthnessR_2->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DG_2", ui->chkBottomG_2->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DG_3", ui->chkBottomG_3->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DR_2", ui->chkBottomR_2->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DRG_2", ui->chkBottomRG_2->isChecked());

    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/FlowOnlyHalf", ui->chkFlowOnlyHalf->isCheckable());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/height", ui->spinHeight->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/width", ui->spinWidth->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/pixel_size", ui->spinPixelSize->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/frames", ui->spinFrames->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/frametime", ui->spinFrametime->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/timestep", ui->spinTimestep->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/timestepEqFrametime", ui->chkTimeStepEqFrametime->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/warmup", ui->spinWarmup->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/crosstalk", ui->spinCrosstalk->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/psfSizeGreen", ui->spinPSFSizeGreen->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/psfSizeRed", ui->spinPSFSizeRed->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/VX", ui->spinVX->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/VY", ui->spinVY->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/DualView", ui->chkDualView->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/filename", ui->edtFilename->text());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/filename_add", ui->edtFilenameAdd->text());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/background", ui->spinBackground->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/backgroundNoise", ui->spinBackgroundNoise->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/deltax", ui->spinDeltaX->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/deltay", ui->spinDeltaY->value());

    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/sizeinc", ui->spinSizeIncrease->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/barrier_spacing", ui->spinBarrierGridSpacing->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/barrier_jump_prob", ui->edtBarrierJumpProb->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/barrier_onlyhalf", ui->chkBarrierOnlyHalf->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/barriers", ui->radBarriers->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/free_diffusion", ui->radUnhinderedDiff->isChecked());

    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/trap_spacing", ui->spinTrapGridSpacing->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/trap_slowdown", ui->edtTrapSlowdown->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/trap_diameter", ui->spinTrapDiameter->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/trap_onlyhalf", ui->chkTrapOnlyHalf->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/traps", ui->radTraps->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/trap_jump_out", ui->edtTrapJumpOut->value());
    ProgramOptions::setConfigValue("QFRDRImagingFCSSimulator/trap_jump_in", ui->edtTrapJumpIn->value());
}

void QFRDRImagingFCSSimulator::readSettings()
{
    ui->edtFilename->setText(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/filename", "").toString());
    ui->edtFilenameAdd->setText(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/filename_add", "%counter%").toString());
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
    ui->chkTimeStepEqFrametime->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/timestepEqFrametime", true).toBool());
    ui->spinTimestep->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/timestep", 100).toDouble());
    ui->spinFrametime->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/frametime", 100).toDouble());
    ui->spinWarmup->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/warmup", 10000).toDouble());
    ui->spinPSFSizeGreen->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/psfSizeGreen", 0.6).toDouble());
    ui->spinPSFSizeRed->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/psfSizeRed", 0.7).toDouble());
    ui->spinPSFCutoff->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/psf_cutoff", 1.0).toDouble());
    ui->spinCrosstalk->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/crosstalk", 5).toDouble());
    ui->spinVX->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/VX", 0).toDouble());
    ui->spinVY->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/VY", 0).toDouble());
    ui->chkDualView->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DualView", false).toBool());
    ui->spinBrightnessG->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/brightnessG", 500).toDouble());
    ui->spinBrigthnessR->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/brightnessR", 500).toDouble());
    ui->spinBackground->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/background", 100).toDouble());
    ui->spinBackgroundNoise->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/backgroundNoise", 2).toDouble());
    ui->spinDeltaX->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/deltax", 0).toDouble());
    ui->spinDeltaY->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/deltay", 0).toDouble());

    ui->cmbPSF->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/psf", 0).toInt());
    ui->cmbBoundary->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/boundary", 0).toInt());
    ui->spinMsdMaxSteps->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/msd_max_steps", 0).toInt());
    ui->chkSaveMSD->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/save_msd", false).toBool());
    ui->spinTrajMax->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/traj_max", 10).toInt());
    ui->spinTrajMaxSteps->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/traj_maxsteps", 100).toInt());
    ui->chkSaveTrajectories->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/save_traj", false).toBool());


    ui->chkBottomG->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DG", false).toBool());
    ui->chkBottomR->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DR", false).toBool());
    ui->chkBottomRG->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DRG", false).toBool());
    ui->chkBottomG_2->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DG_2", false).toBool());
    ui->chkBottomG_3->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DG_3", false).toBool());
    ui->chkBottomR_2->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DR_2", false).toBool());
    ui->chkBottomRG_2->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/onlyhalf_DRG_2", false).toBool());
    ui->chkFlowOnlyHalf->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/FlowOnlyHalf", false).toBool());


    ui->spinWalkersR_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/walkersr_2", 0).toInt());
    ui->spinWalkersG_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/walkersg_2", 0).toInt());
    ui->spinWalkersG_3->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/walkersg_3", 0).toInt());
    ui->spinWalkersRG_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/walkersrg_2", 0).toInt());
    ui->spinDG_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DG_2", 1).toDouble());
    ui->spinDG_3->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DG_3", 0.1).toDouble());
    ui->spinDR_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DR_2", 1).toDouble());
    ui->spinDRG_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/DRG_2", 1).toDouble());
    ui->spinBrightnessG_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/brightnessG_2", 500).toDouble());
    ui->spinBrightnessG_3->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/brightnessG_3", 500).toDouble());
    ui->spinBrigthnessR_2->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/brightnessR_2", 500).toDouble());

    ui->spinSizeIncrease->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/sizeinc", 3).toDouble());
    ui->spinBarrierGridSpacing->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/barrier_spacing", 100).toDouble());
    ui->edtBarrierJumpProb->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/barrier_jump_prob", 0.001).toDouble());
    ui->chkBarrierOnlyHalf->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/barrier_onlyhalf", false).toBool());
    ui->radBarriers->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/barriers", false).toBool());
    ui->radUnhinderedDiff->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/free_diffusion", true).toBool());

    ui->spinTrapGridSpacing->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/trap_spacing", 5).toDouble());
    ui->edtTrapSlowdown->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/trap_slowdown", 0.05).toDouble());
    ui->spinTrapDiameter->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/trap_diameter", 3).toDouble());
    ui->edtTrapJumpOut->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/trap_jump_out", 1).toDouble());
    ui->edtTrapJumpIn->setValue(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/trap_jump_in", 1).toDouble());
    ui->chkTrapOnlyHalf->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/trap_onlyhalf", false).toBool());
    ui->radTraps->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSSimulator/traps", false).toBool());

}

void QFRDRImagingFCSSimulator::on_btnRun_clicked()
{
    if (dstate==dsParameterInput) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        // START SIMULATION
        sim->set_DG(ui->spinDG->value());
        sim->set_DR(ui->spinDR->value());
        sim->set_DRG(ui->spinDRG->value());
        sim->set_DG2(ui->spinDG_2->value());
        sim->set_DG3(ui->spinDG_3->value());
        sim->set_DR2(ui->spinDR_2->value());
        sim->set_DRG2(ui->spinDRG_2->value());
        sim->set_dualView(ui->chkDualView->isChecked());

        sim->set_filename(transformedFilename());
        sim->set_frames(ui->spinFrames->value());
        sim->set_warmup(ui->spinWarmup->value());
        sim->set_frametime(ui->spinFrametime->value());
        double timestep=0;
        if (ui->chkTimeStepEqFrametime->isChecked()) {
            sim->set_sim_timestep(timestep=ui->spinFrametime->value());
        } else {
            sim->set_sim_timestep(timestep=ui->spinTimestep->value());
        }
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
        sim->set_walkersG3(ui->spinWalkersG_3->value());
        sim->set_VX(ui->spinVX->value());
        sim->set_VY(ui->spinVY->value());
        sim->set_FlowEeverywhere(!ui->chkFlowOnlyHalf->isChecked());
        sim->set_deltax(ui->spinDeltaX->value());
        sim->set_deltay(ui->spinDeltaY->value());
        sim->set_brightnessG(ui->spinBrightnessG->value()*timestep*1e-6);
        sim->set_brightnessR(ui->spinBrigthnessR->value()*timestep*1e-6);
        sim->set_background(ui->spinBackground->value());
        sim->set_backgroundNoise(ui->spinBackgroundNoise->value());
        sim->set_boundaryConditions(ui->cmbBoundary->currentIndex());

        sim->set_onlyHalf_DG(ui->chkBottomG->isChecked());
        sim->set_onlyHalf_DR(ui->chkBottomR->isChecked());
        sim->set_onlyHalf_DRG(ui->chkBottomRG->isChecked());
        sim->set_onlyHalf_DG2(ui->chkBottomG_2->isChecked());
        sim->set_onlyHalf_DR2(ui->chkBottomR_2->isChecked());
        sim->set_onlyHalf_DRG2(ui->chkBottomRG_2->isChecked());
        sim->set_brightnessG2(ui->spinBrightnessG_2->value()*timestep*1e-6);
        sim->set_brightnessR2(ui->spinBrigthnessR_2->value()*timestep*1e-6);

        sim->set_onlyHalf_DG3(ui->chkBottomG_3->isChecked());
        sim->set_brightnessG3(ui->spinBrightnessG_3->value()*timestep*1e-6);

        sim->set_saveMSD(ui->chkSaveMSD->isChecked());
        sim->set_msdMaxSteps(ui->spinMsdMaxSteps->value());
        sim->set_saveTrajectores(ui->chkSaveTrajectories->isChecked());
        sim->set_maxTrajectores(ui->spinTrajMax->value());
        sim->set_trajectoresMaxSteps(ui->spinTrajMaxSteps->value());

        sim->set_simspace_sizeinc(ui->spinSizeIncrease->value());
        sim->set_boundaryGridJumpProbability(ui->edtBarrierJumpProb->value());
        sim->set_boundaryGridOnlyRight(ui->chkBarrierOnlyHalf->isChecked());
        sim->set_boundaryGridSpacing(ui->spinBarrierGridSpacing->value()/1000.0);

        sim->set_psf_cutoff_factor(ui->spinPSFCutoff->value());


        sim->set_trapDiameter(ui->spinTrapDiameter->value());
        sim->set_trapOnlyRight(ui->chkTrapOnlyHalf->isChecked());
        sim->set_trapGridSpacing(ui->spinTrapGridSpacing->value());
        sim->set_trapJumpOut(ui->edtTrapJumpOut->value());
        sim->set_trapJumpIn(ui->edtTrapJumpIn->value());
        sim->set_trapSlowdown(ui->edtTrapSlowdown->value());
        if (ui->radBarriers->isChecked()) {
            sim->set_environmentMode(SIMENV_GRIDBOUNDARIES);
        } else if (ui->radTraps->isChecked()){
            sim->set_environmentMode(SIMENV_TRAPS);
        } else {
            sim->set_environmentMode(SIMENV_NORMAL);
        }

        QApplication::processEvents();
        sim->start();
        timeSimStart=QDateTime::currentDateTime();
        QApplication::processEvents();
        oldstate=0;
        oldStateTime=timeSimStart;
        oldstate2=0;
        oldStateTime2=timeSimStart;
        writeSettings();
        QApplication::processEvents();
        setState(dsRunning);
        QApplication::restoreOverrideCursor();
        raise();
    } else if (dstate==dsFinished) { writeSettings(); setState(dsParameterInput); accept(); }
}

void QFRDRImagingFCSSimulator::on_btnCancel_clicked()
{
    if (dstate==dsParameterInput) { writeSettings(); lastSimFile=""; reject(); }
    else if (dstate==dsFinished) {  writeSettings(); lastSimFile=""; setState(dsParameterInput); reject(); }
    else if (dstate==dsRunning) {
        writeSettings();
        sim->cancel();
        QApplication::processEvents();
        sim->waitForFinish();
        QApplication::processEvents();
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
    lastMSDNames=sim->get_msdNames();
    lastTrajNames=sim->get_trajNames();
    accept();
}

void QFRDRImagingFCSSimulator::updateSimStatus(int status)
{
    ui->progressBar->setValue(status);

    double runtime=timeSimStart.msecsTo(QDateTime::currentDateTime())/1000.0;
    double finished=double(status)/double(ui->progressBar->maximum()-ui->progressBar->minimum());
    double timeperone=runtime/finished;

    if (timeSimStart!=oldStateTime2 && timeSimStart!=oldStateTime && fabs(oldStateTime2.msecsTo(oldStateTime)/1000.0)>0) {
        timeperone=fabs(oldStateTime2.msecsTo(oldStateTime))/1000.0/(fabs(oldstate-oldstate2)/double(ui->progressBar->maximum()-ui->progressBar->minimum()));
    }

    if (fabs(oldStateTime.msecsTo(QDateTime::currentDateTime())/1000.0)>10 && (abs(status-oldstate)>10)) {
        oldstate2=oldstate;
        oldStateTime2=oldStateTime;
        oldstate=status;
        oldStateTime=QDateTime::currentDateTime();
    }
    double toend=1.0-finished;
    double eta=toend*timeperone;

    //qDebug()<<"timeSimStart="<<timeSimStart<<" status="<<status<<"   oldStateTime="<<oldStateTime<<" oldstate="<<oldstate<<"  rt="<<runtime<<"  finished="<<finished<<"  perone="<<timeperone<<"  toend="<<toend<<"  eta="<<eta;

    ui->labSimTime->setText(tr("runtime: %1 (est. remaining: %2)").arg(qfSecondsDurationToHMSString(runtime)).arg(qfSecondsDurationToHMSString(eta)));
}

void QFRDRImagingFCSSimulator::on_spinFrametime_valueChanged(double /*val*/)
{
    QString txt;

    ui->spinTimestep->setMaximum(ui->spinFrametime->value());

    if (ui->chkTimeStepEqFrametime->isChecked()) {
        ui->spinTimestep->setValue(ui->spinFrametime->value());
    } else {
        if (ui->spinFrametime->value()/ui->spinTimestep->value() != round(ui->spinFrametime->value()/ui->spinTimestep->value())) {
            txt=tr("<font color=\"orange\">WARNING: Frametime should be an integer multiple of the timestep, but is %1*timestep!</font>").arg(ui->spinFrametime->value()/ui->spinTimestep->value());
        } else {
            txt=tr("frametime = %1 * timestep!").arg(ui->spinFrametime->value()/ui->spinTimestep->value());
        }
    }

    txt+=tr("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;runtime = %1 s").arg(double(ui->spinFrames->value())*ui->spinFrametime->value()*1e-6);

    ui->labSimSettings->setText(txt);
}

void QFRDRImagingFCSSimulator::on_spinFrames_valueChanged(int /*val*/)
{
    on_spinFrametime_valueChanged(0);
}

void QFRDRImagingFCSSimulator::on_spinWarmup_valueChanged(int /*val*/)
{
    on_spinFrametime_valueChanged(0);
}

void QFRDRImagingFCSSimulator::on_spinTimestep_valueChanged(double /*val*/)
{
    on_spinFrametime_valueChanged(0);
}

void QFRDRImagingFCSSimulator::on_chkTimeStepEqFrametime_toggled(bool /*val*/)
{
    on_spinFrametime_valueChanged(0);
}

void QFRDRImagingFCSSimulator::on_edtFilename_textChanged(QString /*text*/)
{
    ui->labFilename->setText(transformedFilename());
}

void QFRDRImagingFCSSimulator::on_edtFilenameAdd_textChanged(QString /*text*/)
{
    ui->labFilename->setText(transformedFilename());
}

QString QFRDRImagingFCSSimulator::transformedFilename() const
{
    QFileInfo fi(ui->edtFilename->text());
    QString f=ui->edtFilename->text();
    f=f.left(f.size()-(fi.completeSuffix().size()+1));
    f=f+ui->edtFilenameAdd->text()+"."+fi.completeSuffix();
    return f;
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
    QApplication::processEvents();
}
