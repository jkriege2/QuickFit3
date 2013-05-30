#include "qfespimb040acquisitiondescription.h"
#include "ui_qfespimb040acquisitiondescription.h"
#include <QTimer>
#include <QDir>
#include "programoptions.h"
#include "qfcompleterfromfile.h"
#include "qfstyledbutton.h"

QFESPIMB040AcquisitionDescription::QFESPIMB040AcquisitionDescription(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040AcquisitionDescription)
{
    ui->setupUi(this);
    this->expDescription=NULL;
    this->opticsSetup=NULL;

    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/");
    QFCompleterFromFile* c1=new QFCompleterFromFile(this);
    c1->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/acqdescription_prefix.txt");
    QFCompleterFromFile* c2=new QFCompleterFromFile(this);
    c2->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/acqdescription_comment.txt");
    QFCompleterFromFile* c3=new QFCompleterFromFile(this);
    c3->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/acqdescription_currentsample.txt");
    ui->edtPrefix->setCompleter(c1);
    ui->edtComment->setCompleter(c2);
    ui->edtCurrentSampleName->setCompleter(c3);
    ui->edtPrefix->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter, ui->edtPrefix, ui->edtPrefix));
    ui->edtComment->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter, ui->edtComment, ui->edtComment));
    ui->edtCurrentSampleName->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter, ui->edtCurrentSampleName, ui->edtCurrentSampleName));

    updateTime();

}

QFESPIMB040AcquisitionDescription::~QFESPIMB040AcquisitionDescription()
{
    delete ui;
}

void QFESPIMB040AcquisitionDescription::updateReplaces()
{
    setGlobalReplaces(opticsSetup, expDescription, this);
}



void QFESPIMB040AcquisitionDescription::loadSettings(QSettings& settings, QString prefix) {
    ui->edtPrefix->setText(settings.value(prefix+"prefix", QDir::homePath()+"spim_").toString());
    ui->edtComment->setText(settings.value(prefix+"comment", "").toString());
    ui->edtCurrentSampleName->setText(settings.value(prefix+"sample", "").toString());
    ui->spinCell->setValue(settings.value(prefix+"cell", 1).toInt());
    ui->spinPlate->setValue(settings.value(prefix+"plate", 1).toInt());
    ui->spinWell->setValue(settings.value(prefix+"well", 1).toInt());
}

void QFESPIMB040AcquisitionDescription::storeSettings(QSettings& settings, QString prefix) const {
    settings.setValue(prefix+"prefix", ui->edtPrefix->text());
    settings.setValue(prefix+"comment", ui->edtComment->text());
    settings.setValue(prefix+"sample", ui->edtCurrentSampleName->text());
    settings.setValue(prefix+"cell", ui->spinCell->value());
    settings.setValue(prefix+"plate", ui->spinPlate->value());
    settings.setValue(prefix+"well", ui->spinWell->value());

}

QMap<QString, QVariant> QFESPIMB040AcquisitionDescription::getDescription() const {
    QMap<QString, QVariant> description;
    description["prefix"]=ui->edtPrefix->text();
    description["comment"]=ui->edtComment->text();
    description["samplename"]=ui->edtCurrentSampleName->text();
    description["cell"]=ui->spinCell->value();
    description["plate"]=ui->spinPlate->value();
    description["well"]=ui->spinWell->value();
    description["date"]=ui->edtDateTime->dateTime();
    return description;
}

int QFESPIMB040AcquisitionDescription::getCell() const
{
    return ui->spinCell->value();
}

int QFESPIMB040AcquisitionDescription::getWell() const
{
    return ui->spinWell->value();
}

int QFESPIMB040AcquisitionDescription::getPlate() const
{
    return ui->spinPlate->value();
}

QString QFESPIMB040AcquisitionDescription::getComment() const
{
    return ui->edtComment->text();
}

QString QFESPIMB040AcquisitionDescription::getSample() const
{
    return ui->edtCurrentSampleName->text();
}

QString QFESPIMB040AcquisitionDescription::getPrefix() const
{
    return ui->edtPrefix->text();
}

void QFESPIMB040AcquisitionDescription::setOtherSettingWidgets(QFESPIMB040OpticsSetup *setup, QFESPIMB040ExperimentDescription *exp)
{
    this->opticsSetup=setup;
    this->expDescription=exp;
    updateReplaces();
    bindLineEdit(ui->edtPrefix);

}

void QFESPIMB040AcquisitionDescription::on_btnClearAll_clicked() {
    ui->edtComment->setText("");
    ui->spinCell->setValue(1);
    ui->spinPlate->setValue(1);
    ui->spinWell->setValue(1);
}

void QFESPIMB040AcquisitionDescription::on_btnNextCell_clicked()
{
    ui->edtComment->setText("");
    ui->spinCell->setValue(ui->spinCell->value()+1);
}

void QFESPIMB040AcquisitionDescription::updateTime() {
    ui->edtDateTime->setDateTime(QDateTime::currentDateTime());
    QTimer::singleShot(1013, this, SLOT(updateTime()));
}

void QFESPIMB040AcquisitionDescription::spinChanged() {
    ui->edtComment->setText("");
}
