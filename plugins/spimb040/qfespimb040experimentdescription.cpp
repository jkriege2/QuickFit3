#include "qfespimb040experimentdescription.h"
#include "ui_qfespimb040experimentdescription.h"
#include <QTimer>

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
    ui->edtTime->setTime(QTime::currentTime());
    ui->edtDate->setDate(QDate::currentDate());
    QTimer::singleShot(1013, this, SLOT(updateTime()));
}
