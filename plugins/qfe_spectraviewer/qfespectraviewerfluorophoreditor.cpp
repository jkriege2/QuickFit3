/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "qfespectraviewerfluorophoreditor.h"
#include "ui_qfespectraviewerfluorophoreditor.h"
#include "qftools.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qfcompleterfromfile.h"
#include "qfstyledbutton.h"

QFESpectraViewerFluorophoreEditor::QFESpectraViewerFluorophoreEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESpectraVieweFluorophoreEditor)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);

    QFCompleterFromFile* c=NULL;
    ui->edtDescription->setCompleter(c=new QFCompleterFromFile);
    ui->edtDescription->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter,ui->edtDescription,ui->edtDescription));
    c->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfe_spectraviewer/description.txt");
    ui->edtManufacturer->setCompleter(c=new QFCompleterFromFile);
    ui->edtManufacturer->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter,ui->edtManufacturer,ui->edtManufacturer));
    c->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfe_spectraviewer/manufacturer.txt");
    ui->edtName->setCompleter(c=new QFCompleterFromFile);
    ui->edtName->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter,ui->edtName,ui->edtName));
    c->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfe_spectraviewer/name.txt");
    ui->edtReference->setCompleter(c=new QFCompleterFromFile);
    ui->edtReference->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter,ui->edtReference,ui->edtReference));
    c->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfe_spectraviewer/ref.txt");
    ui->edtSpectrumReference->setCompleter(c=new QFCompleterFromFile);
    ui->edtSpectrumReference->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter,ui->edtSpectrumReference,ui->edtSpectrumReference));
    ui->edtSpectrumReference_2->setCompleter(c);
    ui->edtSpectrumReference_2->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter,ui->edtSpectrumReference,ui->edtSpectrumReference));
    c->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfe_spectraviewer/specref.txt");
    ui->edtFolder->setCompleter(c=new QFCompleterFromFile);
    ui->edtFolder->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter,ui->edtFolder,ui->edtFolder));
    c->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfe_spectraviewer/flfolder.txt");


    ui->edtSpectrumReference->setCompleter(c=new QFCompleterFromFile);
    ui->edtSpectrumReference_2->setCompleter(c);
    c->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfe_spectraviewer/specref.txt");


    modSpectrum.setReadonly(false);
    modSpectrum.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum.setColumnTitleCreate(1,"intensity/spectrum");
    connect(&modSpectrum, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(setSpectrumChanged()));
    connect(&modSpectrum, SIGNAL(layoutChanged()), this, SLOT(setSpectrumChanged()));
    ui->tableView->setModel(&modSpectrum);

    modSpectrum_2.setReadonly(false);
    modSpectrum_2.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum_2.setColumnTitleCreate(1,"intensity/spectrum");
    connect(&modSpectrum_2, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(setSpectrumChanged2()));
    connect(&modSpectrum_2, SIGNAL(layoutChanged()), this, SLOT(setSpectrumChanged2()));
    ui->tableView_2->setModel(&modSpectrum_2);

    manager=NULL;
    oldspectrum=-1;
    oldspectrum_2=-1;
    spectrumchanged=false;
    spectrumchanged_2=false;
}

QFESpectraViewerFluorophoreEditor::~QFESpectraViewerFluorophoreEditor()
{
    delete ui;
}

void QFESpectraViewerFluorophoreEditor::setFromData(const QString &ID, const SpectrumManager::FluorophoreData &data, SpectrumManager* manager)
{
    ui->edtID->setText(ID);
    ui->edtManufacturer->setText(data.manufacturer);
    ui->edtDescription->setText(data.description);
    ui->edtName->setText(data.name);
    ui->edtOrder->setText(data.orderNo);
    ui->edtReference->setText(data.reference);
    ui->spinAbsMax->setValue(data.excitation_maxwavelength);
    ui->spinEmMax->setValue(data.emission_maxwavelength);
    ui->spinExtinction->setValue(data.extiction_coefficient);
    ui->spinExtinctionWL->setValue(data.extiction_coefficient_wavelength);
    ui->spinFlEff->setValue(data.fluorescence_efficiency);
    ui->spinFlEffWL->setValue(data.fluorescence_efficiency_wavelength);
    ui->spinLifetime->setValue(data.fluorescence_lifetime);
    ui->edtFolder->setText(data.folder);
    this->oldspectrum=data.spectrum_abs;
    this->oldspectrum_2=data.spectrum_fl;
    this->manager=manager;
    this->spectrumchanged=false;
    this->spectrumchanged_2=false;
    if (manager&&manager->spectrumExists(data.spectrum_abs)) {
        modSpectrum.clear();
        modSpectrum.setColumnCreate(0, manager->getSpectrum(data.spectrum_abs)->getWavelength(),  manager->getSpectrum(data.spectrum_abs)->getN());
        modSpectrum.setColumnCreate(1, manager->getSpectrum(data.spectrum_abs)->getSpectrum(),  manager->getSpectrum(data.spectrum_abs)->getN());
        ui->edtSpectrumReference->setText(manager->getSpectrum(data.spectrum_abs)->reference);
    } else {
        ui->edtSpectrumReference->setText("");
    }
    if (manager&&manager->spectrumExists(data.spectrum_fl)) {
        modSpectrum_2.clear();
        modSpectrum_2.setColumnCreate(0, manager->getSpectrum(data.spectrum_fl)->getWavelength(),  manager->getSpectrum(data.spectrum_fl)->getN());
        modSpectrum_2.setColumnCreate(1, manager->getSpectrum(data.spectrum_fl)->getSpectrum(),  manager->getSpectrum(data.spectrum_fl)->getN());
        ui->edtSpectrumReference_2->setText(manager->getSpectrum(data.spectrum_fl)->reference);
    } else {
        ui->edtSpectrumReference->setText("");
    }
    modSpectrum.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum.setColumnTitleCreate(1,"intensity/spectrum");
    modSpectrum_2.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum_2.setColumnTitleCreate(1,"intensity/spectrum");
    ui->tableView->resizeColumnsToContents();
    ui->tableView_2->resizeColumnsToContents();
}

QString QFESpectraViewerFluorophoreEditor::getID() const
{
    return ui->edtID->text();
}

bool QFESpectraViewerFluorophoreEditor::hasSpectrumChanged() const
{
    return spectrumchanged || spectrumchanged_2;
}

void QFESpectraViewerFluorophoreEditor::on_btnClearSpectrum_clicked()
{
    modSpectrum.clear();
    spectrumchanged=true;
    modSpectrum.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum.setColumnTitleCreate(1,"intensity/spectrum");
    ui->tableView->resizeColumnsToContents();
}

void QFESpectraViewerFluorophoreEditor::on_btnPasteSpectrum_clicked()
{
    modSpectrum.paste();
    spectrumchanged=true;
    modSpectrum.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum.setColumnTitleCreate(1,"intensity/spectrum");
    ui->tableView->resizeColumnsToContents();
}

void QFESpectraViewerFluorophoreEditor::on_btnLoadSpectrumFile_clicked()
{
    QStringList filters;
    filters<<tr("Comma-Separated Values (*.csv *.txt *.spec *.dat)");
    filters<<tr("Semicolon-Separated Values (*.csv *.txt *.spec *.dat)");
    filters<<tr("Space/Tab-Separated Values (*.csv *.txt *.spec *.dat)");
    QString filter=filters.first();
    QString filename=qfGetOpenFileName(this, tr("select spectrum file"), ProgramOptions::getConfigValue("QFESpectraVieweFluorophoreEditor/loadSpectrumLastPath", "").toString(), filters.join(";;"), &filter);
    if (QFile::exists(filename)) {
        modSpectrum.clear();
        spectrumchanged=true;
        ProgramOptions::setConfigValue("QFESpectraVieweFluorophoreEditor/loadSpectrumLastPath",QFileInfo(filename).absolutePath());
        if (filters.indexOf(filter)==0) modSpectrum.readCSV(filename);
        else if (filters.indexOf(filter)==1) modSpectrum.readCSV(filename, ';');
        else if (filters.indexOf(filter)==21) modSpectrum.readCSV(filename, ' ');
    }
    modSpectrum.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum.setColumnTitleCreate(1,"intensity/spectrum");
    ui->tableView->resizeColumnsToContents();
}

void QFESpectraViewerFluorophoreEditor::on_btnDeleteRow_clicked()
{
    int c=ui->tableView->currentIndex().row();
    modSpectrum.deleteRow(c);
    spectrumchanged=true;
}

void QFESpectraViewerFluorophoreEditor::on_btnDeleteColumn_clicked()
{
    int c=ui->tableView->currentIndex().column();
    modSpectrum.deleteColumn(c);
    spectrumchanged=true;
}














void QFESpectraViewerFluorophoreEditor::on_btnClearSpectrum_2_clicked()
{
    modSpectrum_2.clear();
    spectrumchanged_2=true;
    modSpectrum_2.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum_2.setColumnTitleCreate(1,"intensity/spectrum");
    ui->tableView->resizeColumnsToContents();
}

void QFESpectraViewerFluorophoreEditor::on_btnPasteSpectrum_2_clicked()
{
    modSpectrum_2.paste();
    spectrumchanged_2=true;
    modSpectrum_2.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum_2.setColumnTitleCreate(1,"intensity/spectrum");
    ui->tableView->resizeColumnsToContents();
}

void QFESpectraViewerFluorophoreEditor::on_btnLoadSpectrumFile_2_clicked()
{
    QStringList filters;
    filters<<tr("Comma-Separated Values (*.csv *.txt *.spec *.dat)");
    filters<<tr("Semicolon-Separated Values (*.csv *.txt *.spec *.dat)");
    filters<<tr("Space/Tab-Separated Values (*.csv *.txt *.spec *.dat)");
    QString filter=filters.first();
    QString filename=qfGetOpenFileName(this, tr("select spectrum file"), ProgramOptions::getConfigValue("QFESpectraVieweFluorophoreEditor/loadSpectrumLastPath", "").toString(), filters.join(";;"), &filter);
    if (QFile::exists(filename)) {
        modSpectrum_2.clear();
        spectrumchanged_2=true;
        ProgramOptions::setConfigValue("QFESpectraVieweFluorophoreEditor/loadSpectrumLastPath",QFileInfo(filename).absolutePath());
        if (filters.indexOf(filter)==0) modSpectrum_2.readCSV(filename);
        else if (filters.indexOf(filter)==1) modSpectrum_2.readCSV(filename, ';');
        else if (filters.indexOf(filter)==21) modSpectrum_2.readCSV(filename, ' ');
    }
    modSpectrum_2.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum_2.setColumnTitleCreate(1,"intensity/spectrum");
    ui->tableView->resizeColumnsToContents();
}

void QFESpectraViewerFluorophoreEditor::on_btnDeleteRow_2_clicked()
{
    int c=ui->tableView_2->currentIndex().row();
    modSpectrum_2.deleteRow(c);
    spectrumchanged_2=true;
}

void QFESpectraViewerFluorophoreEditor::on_btnDeleteColumn_2_clicked()
{
    int c=ui->tableView_2->currentIndex().column();
    modSpectrum_2.deleteColumn(c);
    spectrumchanged_2=true;
}









void QFESpectraViewerFluorophoreEditor::setSpectrumChanged(bool changed)
{
    spectrumchanged=changed;
}
void QFESpectraViewerFluorophoreEditor::setSpectrumChanged2(bool changed)
{
    spectrumchanged_2=changed;
}


SpectrumManager::FluorophoreData QFESpectraViewerFluorophoreEditor::getData() const
{
    SpectrumManager::FluorophoreData data;
    data.name=ui->edtName->text();
    data.manufacturer=ui->edtManufacturer->text();
    data.orderNo=ui->edtOrder->text();
    data.reference=ui->edtReference->text();
    data.fluorescence_efficiency=ui->spinFlEff->value();
    data.fluorescence_efficiency_wavelength=ui->spinFlEffWL->value();
    data.fluorescence_lifetime=ui->spinLifetime->value();
    data.extiction_coefficient=ui->spinExtinction->value();
    data.extiction_coefficient_wavelength=ui->spinExtinctionWL->value();
    data.excitation_maxwavelength=ui->spinAbsMax->value();
    data.emission_maxwavelength=ui->spinEmMax->value();
    data.spectrum_abs=oldspectrum;
    data.spectrum_fl=oldspectrum_2;
    data.description=ui->edtDescription->text();
    data.folder=ui->edtFolder->text();
    data.condition=ui->edtCondition->text();
    if (spectrumchanged) {
        data.spectrum_abs=-1;
    }
    if (spectrumchanged_2) {
        data.spectrum_fl=-1;
    }
    return data;
}

void QFESpectraViewerFluorophoreEditor::addSpectrum(SpectrumManager *manager, QString* filenameOut, QString* filenameOut_fl, int* spec_abs, int* spec_fl)
{
    addSpectrumAbs(manager, filenameOut, spec_abs);
    addSpectrumFl(manager, filenameOut_fl, spec_fl);
}

void QFESpectraViewerFluorophoreEditor::addSpectrumAbs(SpectrumManager *manager, QString* filenameOut, int* spec_abs)
{
    SpectrumManager *m=manager;
    if (!m) m=this->manager;
    if (m) {
        QDir d(ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString());
        QString filename=cleanStringForFilename(ui->edtID->text().toLower())+"_abs";
        if (filename.isEmpty()) filename=cleanStringForFilename(ui->edtName->text().toLower())+"_abs";
        if (filename.isEmpty()) filename="spectrum_abs";
        filename+=".spec";
        if (QFile::exists(filename)) {
            if (QMessageBox::question(this, tr("overwrite spectrum?"), tr("spectrum\n   %1\nalready exists. Overwrite?"), QMessageBox::Yes||QMessageBox::No, QMessageBox::No)!=QMessageBox::Yes) {
                filename=getNewFilename(filename, d);
            }
        }
        //qDebug()<<"saving spectrum to "<<d.absoluteFilePath(filename);
        modSpectrum.saveCSV(d.absoluteFilePath(filename), ", ", '.', "#");
        if (filenameOut) *filenameOut=filename;
        int idx=m->addSpectrum(filename, 0, false, ui->edtSpectrumReference->text());
        if (spec_abs) *spec_abs=idx;
    }

}

void QFESpectraViewerFluorophoreEditor::addSpectrumFl(SpectrumManager *manager,  QString* filenameOut_fl,  int* spec_fl)
{
    SpectrumManager *m=manager;
    if (!m) m=this->manager;
    if (m) {
        QDir d(ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString());
        QString filename=cleanStringForFilename(ui->edtID->text().toLower())+"_fl";
        if (filename.isEmpty()) filename=cleanStringForFilename(ui->edtName->text().toLower())+"_fl";
        if (filename.isEmpty()) filename="spectrum_fl";
        filename+=".spec";
        if (QFile::exists(filename)) {
            if (QMessageBox::question(this, tr("overwrite spectrum?"), tr("spectrum\n   %1\nalready exists. Overwrite?"), QMessageBox::Yes||QMessageBox::No, QMessageBox::No)!=QMessageBox::Yes) {
                filename=getNewFilename(filename, d);
            }
        }
        //qDebug()<<"saving spectrum to "<<d.absoluteFilePath(filename);
        modSpectrum_2.saveCSV(d.absoluteFilePath(filename), ", ", '.', "#");
        if (filenameOut_fl) *filenameOut_fl=filename;
        int idx=m->addSpectrum(filename, 0, false, ui->edtSpectrumReference_2->text());
        if (spec_fl) *spec_fl=idx;
    }
}

QString QFESpectraViewerFluorophoreEditor::addDataAndSpectrum(QSettings& database, SpectrumManager *manager)
{
    SpectrumManager *m=manager;
    if (!m) m=this->manager;
    QString specFilename="", spec2Filename="";
    QString ID=ui->edtID->text();
    if (ID.isEmpty()) ID=cleanStringForFilename(ui->edtName->text().toLower().trimmed());
    SpectrumManager::FluorophoreData data=getData();
    //qDebug()<<data.spectrum_abs<<data.spectrum_fl;
    if (m) {
        if (data.spectrum_abs<0) addSpectrumAbs(m, &specFilename, &(data.spectrum_abs));
        else specFilename=QFileInfo(database.fileName()).absoluteDir().relativeFilePath(m->getSpectrum(data.spectrum_abs)->getFilename());

        if (data.spectrum_fl<0) addSpectrumFl(m, &spec2Filename, &(data.spectrum_fl));
        else spec2Filename=QFileInfo(database.fileName()).absoluteDir().relativeFilePath(m->getSpectrum(data.spectrum_fl)->getFilename());
    }
    //qDebug()<<data.spectrum_abs<<data.spectrum_fl<<specFilename<<spec2Filename;
    database.setValue(QString("%1/name").arg(ID), data.name);
    database.setValue(QString("%1/folder").arg(ID), ui->edtFolder->text());
    database.setValue(QString("%1/manufacturer").arg(ID), data.manufacturer);
    database.setValue(QString("%1/oder_no").arg(ID), data.orderNo);
    database.setValue(QString("%1/reference").arg(ID), data.reference);
    database.setValue(QString("%1/q_fluor").arg(ID), data.fluorescence_efficiency);
    database.setValue(QString("%1/q_fluor_wavelength").arg(ID), data.fluorescence_efficiency_wavelength);
    database.setValue(QString("%1/molar_extinction").arg(ID), data.extiction_coefficient);
    database.setValue(QString("%1/molar_extinction_wavelength").arg(ID), data.extiction_coefficient_wavelength);
    database.setValue(QString("%1/tau_fluor").arg(ID), data.fluorescence_lifetime);
    database.setValue(QString("%1/excitation_max").arg(ID), data.excitation_maxwavelength);
    database.setValue(QString("%1/emission_max").arg(ID), data.emission_maxwavelength);
    database.setValue(QString("%1/description").arg(ID), data.description);
    database.setValue(QString("%1/condition").arg(ID), data.condition);
    database.setValue(QString("%1/spectrum_abs").arg(ID), specFilename);
    database.setValue(QString("%1/spectrum_abs_id").arg(ID), 0);
    database.setValue(QString("%1/spectrum_abs_separatewavelengths").arg(ID), false);
    database.setValue(QString("%1/spectrum_abs_reference").arg(ID), ui->edtSpectrumReference->text());
    database.setValue(QString("%1/spectrum_fl").arg(ID), spec2Filename);
    database.setValue(QString("%1/spectrum_fl_id").arg(ID), 0);
    database.setValue(QString("%1/spectrum_fl_separatewavelengths").arg(ID), false);
    database.setValue(QString("%1/spectrum_fl_reference").arg(ID), ui->edtSpectrumReference_2->text());
    return ID;
}
