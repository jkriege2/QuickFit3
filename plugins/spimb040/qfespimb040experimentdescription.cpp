#include "qfespimb040experimentdescription.h"
#include "ui_qfespimb040experimentdescription.h"
#include <QTimer>
#include "qfpluginservices.h"
#include "programoptions.h"
#include "qftools.h"

QFESPIMB040ExperimentDescription::QFESPIMB040ExperimentDescription(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040ExperimentDescription)
{
    ui->setupUi(this);
    updateTime();
}

QFESPIMB040ExperimentDescription::~QFESPIMB040ExperimentDescription()
{
    delete ui;
}

void QFESPIMB040ExperimentDescription::loadSettings(QSettings& settings, QString prefix) {
    ui->edtTitle->setText(settings.value(prefix+"title", "").toString());
    ui->edtExperimenter->setText(settings.value(prefix+"experimenter", "").toString());
    ui->edtSample->setPlainText(settings.value(prefix+"sample", "").toString());
    ui->edtDescription->setPlainText(settings.value(prefix+"description", "").toString());
    ui->edtLabBook->setText(settings.value(prefix+"labbook", "").toString());
    ui->edtID->setText(settings.value(prefix+"id", "").toString());
}

void QFESPIMB040ExperimentDescription::storeSettings(QSettings& settings, QString prefix) const {
    settings.setValue(prefix+"id", ui->edtID->text());
    settings.setValue(prefix+"title", ui->edtTitle->text());
    settings.setValue(prefix+"experimenter", ui->edtExperimenter->text());
    settings.setValue(prefix+"sample", ui->edtSample->toPlainText());
    settings.setValue(prefix+"description", ui->edtDescription->toPlainText());
    settings.setValue(prefix+"labbook", ui->edtLabBook->text());

}

QString QFESPIMB040ExperimentDescription::getID() const
{
    return ui->edtID->text();
}

QString QFESPIMB040ExperimentDescription::getTitle() const
{
    return ui->edtTitle->text();
}

QString QFESPIMB040ExperimentDescription::getExperimenter() const
{
    return ui->edtExperimenter->text();
}

QMap<QString, QVariant> QFESPIMB040ExperimentDescription::getDescription() const {
    QMap<QString, QVariant> description;
    description["id"]=ui->edtID->text();
    description["title"]=ui->edtTitle->text();
    description["experimenter"]=ui->edtExperimenter->text();
    //description["date"]=QDate::currentDate();
    //description["time"]=QTime::currentTime();
    description["sample"]=ui->edtSample->toPlainText();
    description["description"]=ui->edtDescription->toPlainText();
    description["labbook"]=ui->edtLabBook->text();

    return description;
}

void QFESPIMB040ExperimentDescription::on_btnClearAll_clicked() {
    ui->edtTitle->setText("");
    ui->edtExperimenter->setText("");
    ui->edtSample->setPlainText("");
    ui->edtDescription->setPlainText("");
    ui->edtLabBook->setText("");
    ui->edtID->setText("");
}

void QFESPIMB040ExperimentDescription::updateTime() {
    bool updt=updatesEnabled();
    setUpdatesEnabled(false);
    ui->edtTime->setTime(QTime::currentTime());
    ui->edtDate->setDate(QDate::currentDate());
    setUpdatesEnabled(updt);
    QTimer::singleShot(1013, this, SLOT(updateTime()));
}

void QFESPIMB040ExperimentDescription::on_btnSaveTemplate_clicked() {
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/ed_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040ExperimentDescription/lasttemplatedir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/ed_templates/").toString();
    QString filename=qfGetSaveFileName(this, tr("save as template ..."), dir, tr("exp. description template (*.edt)"))    ;
    if (!filename.isEmpty()) {
        bool ok=true;
        if (QFile::exists(filename)) {
            ok=false;
            if (QMessageBox::question(this, tr("save as template ..."), tr("The file\n  '%1'\nalready exists. Overwrite?").arg(filename), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
                ok=true;
            }
        }
        if (ok) {
            QSettings set(filename, QSettings::IniFormat);
            storeSettings(set, "experiment_settings/");
        }
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040ExperimentDescription/lasttemplatedir", dir);
}

void QFESPIMB040ExperimentDescription::on_btnLoadTemplate_clicked() {
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/ed_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040ExperimentDescription/lasttemplatedir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/ed_templates/").toString();
    QString filename=qfGetOpenFileName(this, tr("open template ..."), dir, tr("exp. description template (*.edt)"))    ;
    if (!filename.isEmpty()) {
        QSettings set(filename, QSettings::IniFormat);
        loadSettings(set, "experiment_settings/");
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040ExperimentDescription/lasttemplatedir", dir);

}
