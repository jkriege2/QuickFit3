#include "qfespectraviewerdetectoreditor.h"
#include "ui_qfespectraviewerdetectoreditor.h"
#include "qftools.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qfstyledbutton.h"
#include "qfcompleterfromfile.h"

QFESpectraViewerDetectorEditor::QFESpectraViewerDetectorEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESpectraViewerDetectorEditor)
{
    ui->setupUi(this);

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
    c->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfe_spectraviewer/specref.txt");
    ui->edtUnit->setCompleter(c=new QFCompleterFromFile);
    ui->edtUnit->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter,ui->edtUnit,ui->edtUnit));
    c->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfe_spectraviewer/effunits.txt");
    ui->edtFolder->setCompleter(c=new QFCompleterFromFile);
    ui->edtFolder->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter,ui->edtFolder,ui->edtFolder));
    c->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/qfe_spectraviewer/detfolder.txt");


    modSpectrum.setReadonly(false);
    modSpectrum.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum.setColumnTitleCreate(1,"sensitivity");
    connect(&modSpectrum, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(setSpectrumChanged()));
    connect(&modSpectrum, SIGNAL(layoutChanged()), this, SLOT(setSpectrumChanged()));
    ui->tableView->setModel(&modSpectrum);
    manager=NULL;
    oldspectrum=-1;
    spectrumchanged=false;
}

QFESpectraViewerDetectorEditor::~QFESpectraViewerDetectorEditor()
{
    delete ui;
}

void QFESpectraViewerDetectorEditor::setFromData(const QString &ID, const SpectrumManager::DetectorData &data, SpectrumManager* manager)
{
    ui->edtID->setText(ID);
    ui->edtManufacturer->setText(data.manufacturer);
    ui->edtDescription->setText(data.description);
    ui->edtName->setText(data.name);
    ui->edtOrder->setText(data.orderNo);
    ui->edtReference->setText(data.reference);
    ui->spinWavelength->setValue(data.peak_wavelength);
    ui->spinSensitivity->setValue(data.peak_sensitivity);
    ui->edtUnit->setText(data.peak_sensitivity_unit);
    ui->edtFolder->setText(data.folder);
    this->oldspectrum=data.spectrum;
    this->manager=manager;
    this->spectrumchanged=false;
    if (manager&&manager->spectrumExists(data.spectrum)) {
        modSpectrum.clear();
        modSpectrum.setColumnCreate(0, manager->getSpectrum(data.spectrum)->getWavelength(),  manager->getSpectrum(data.spectrum)->getN());
        modSpectrum.setColumnCreate(1, manager->getSpectrum(data.spectrum)->getSpectrum(),  manager->getSpectrum(data.spectrum)->getN());
        ui->edtSpectrumReference->setText(manager->getSpectrum(data.spectrum)->reference);
        spectrumchanged=false;
    } else {
        ui->edtSpectrumReference->setText("");
    }
    modSpectrum.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum.setColumnTitleCreate(1,"sensitivity");
    ui->tableView->resizeColumnsToContents();
}

QString QFESpectraViewerDetectorEditor::getID() const
{
    return ui->edtID->text();
}

bool QFESpectraViewerDetectorEditor::hasSpectrumChanged() const
{
    return spectrumchanged;
}

void QFESpectraViewerDetectorEditor::on_btnClearSpectrum_clicked()
{
    modSpectrum.clear();
    spectrumchanged=true;
    modSpectrum.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum.setColumnTitleCreate(1,"sensitivity");
    ui->tableView->resizeColumnsToContents();
}

void QFESpectraViewerDetectorEditor::on_btnPasteSpectrum_clicked()
{
    modSpectrum.paste();
    spectrumchanged=true;
    modSpectrum.setColumnTitleCreate(0,"wavelength [nm]");
    modSpectrum.setColumnTitleCreate(1,"sensitivity");
    ui->tableView->resizeColumnsToContents();
}

void QFESpectraViewerDetectorEditor::on_btnLoadSpectrumFile_clicked()
{
    QStringList filters;
    filters<<tr("Comma-Separated Values (*.csv *.txt *.spec *.dat)");
    filters<<tr("Semicolon-Separated Values (*.csv *.txt *.spec *.dat)");
    filters<<tr("Space/Tab-Separated Values (*.csv *.txt *.spec *.dat)");
    QString filter=filters.first();
    QString filename=qfGetOpenFileName(this, tr("select spectrum file"), ProgramOptions::getConfigValue("QFESpectraViewerDetectorEditor/loadSpectrumLastPath", "").toString(), filters.join(";;"), &filter);
    if (QFile::exists(filename)) {
        modSpectrum.clear();
        spectrumchanged=true;
        ProgramOptions::setConfigValue("QFESpectraViewerDetectorEditor/loadSpectrumLastPath",QFileInfo(filename).absolutePath());
        if (filters.indexOf(filter)==0) modSpectrum.readCSV(filename);
        else if (filters.indexOf(filter)==1) modSpectrum.readCSV(filename, ';');
        else if (filters.indexOf(filter)==21) modSpectrum.readCSV(filename, ' ');
    }
}

void QFESpectraViewerDetectorEditor::on_btnDeleteRow_clicked()
{
    int c=ui->tableView->currentIndex().row();
    modSpectrum.deleteRow(c);
    spectrumchanged=true;
}

void QFESpectraViewerDetectorEditor::on_btnDeleteColumn_clicked()
{
    int c=ui->tableView->currentIndex().column();
    modSpectrum.deleteColumn(c);
    spectrumchanged=true;
}

void QFESpectraViewerDetectorEditor::setSpectrumChanged(bool changed)
{
    spectrumchanged=changed;
}

SpectrumManager::DetectorData QFESpectraViewerDetectorEditor::getData() const
{
    SpectrumManager::DetectorData data;
    data.name=ui->edtName->text();
    data.manufacturer=ui->edtManufacturer->text();
    data.orderNo=ui->edtOrder->text();
    data.reference=ui->edtReference->text();
    data.peak_wavelength=ui->spinWavelength->value();
    data.peak_sensitivity_unit=ui->edtUnit->text();
    data.peak_sensitivity=ui->spinSensitivity->value();
    data.spectrum=oldspectrum;
    data.description=ui->edtDescription->text();
    data.folder=ui->edtFolder->text();
    if (spectrumchanged) data.spectrum=-1;
    return data;
}

int QFESpectraViewerDetectorEditor::addSpectrum(SpectrumManager *manager, QString* filenameOut)
{
    SpectrumManager *m=manager;
    if (!m) m=this->manager;
    if (m) {
        QDir d(ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString());
        QString filename=cleanStringForFilename(ui->edtID->text().toLower());
        if (filename.isEmpty()) filename=cleanStringForFilename(ui->edtName->text().toLower());
        if (filename.isEmpty()) filename="spectrum";
        filename+=".spec";
        if (QFile::exists(filename)) {
            if (QMessageBox::question(this, tr("overwrite spectrum?"), tr("spectrum\n   %1\nalready exists. Overwrite?"), QMessageBox::Yes||QMessageBox::No, QMessageBox::No)!=QMessageBox::Yes) {
                filename=getNewFilename(filename, d);
            }
        }
        //qDebug()<<"saving spectrum to "<<d.absoluteFilePath(filename);
        modSpectrum.saveCSV(d.absoluteFilePath(filename), ", ", '.', "#");
        if (filenameOut) *filenameOut=filename;
        return m->addSpectrum(filename, 0, false, ui->edtReference->text(), false);
    }
    return -1;
}

QString QFESpectraViewerDetectorEditor::addDataAndSpectrum(QSettings& database, SpectrumManager *manager)
{
    SpectrumManager *m=manager;
    if (!m) m=this->manager;
    QString specFilename="";
    QString ID=ui->edtID->text();
    if (ID.isEmpty()) ID=cleanStringForFilename(ui->edtName->text().toLower().trimmed());
    SpectrumManager::DetectorData data=getData();
    if (m) {
        if (data.spectrum<0) data.spectrum=addSpectrum(m, &specFilename);
        else specFilename=QFileInfo(database.fileName()).absoluteDir().relativeFilePath(m->getSpectrum(data.spectrum)->getFilename());
    }
    database.setValue(QString("%1/name").arg(ID), data.name);
    database.setValue(QString("%1/folder").arg(ID), ui->edtFolder->text());
    database.setValue(QString("%1/manufacturer").arg(ID), data.manufacturer);
    database.setValue(QString("%1/oder_no").arg(ID), data.orderNo);
    database.setValue(QString("%1/reference").arg(ID), data.reference);
    database.setValue(QString("%1/peak_wavelength").arg(ID), data.peak_wavelength);
    database.setValue(QString("%1/peak_sensitivity").arg(ID), data.peak_sensitivity);
    database.setValue(QString("%1/peak_sensitivity_unit").arg(ID), data.peak_sensitivity_unit);
    database.setValue(QString("%1/description").arg(ID), data.description);
    database.setValue(QString("%1/spectrum").arg(ID), specFilename);
    database.setValue(QString("%1/spectrum_id").arg(ID), 0);
    database.setValue(QString("%1/spectrum_separatewavelengths").arg(ID), false);
    database.setValue(QString("%1/spectrum_reference").arg(ID), ui->edtSpectrumReference->text());
    return ID;
}
