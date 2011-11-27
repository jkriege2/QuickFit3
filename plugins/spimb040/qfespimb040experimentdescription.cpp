#include "qfespimb040experimentdescription.h"
#include "ui_qfespimb040experimentdescription.h"

QFESPIMB040ExperimentDescription::QFESPIMB040ExperimentDescription(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040ExperimentDescription)
{
    ui->setupUi(this);
    ui->edtTime->setTime(QTime::currentTime());
    ui->edtDate->setDate(QDate::currentDate());
}

QFESPIMB040ExperimentDescription::~QFESPIMB040ExperimentDescription()
{
    delete ui;
}

void QFESPIMB040ExperimentDescription::loadSettings(QSettings& settings, QString prefix) {

}

void QFESPIMB040ExperimentDescription::storeSettings(QSettings& settings, QString prefix) {

}

QMap<QString, QVariant> QFESPIMB040ExperimentDescription::getDescription() const {
    QMap<QString, QVariant> description;
    description["title"]=ui->edtTitle->text();
    description["experimenter"]=ui->edtExperimenter->text();
    description["date"]=ui->edtDate->date();
    description["time"]=ui->edtTime->time();
    description["sample"]=ui->edtSample->toPlainText();
    description["description"]=ui->edtDescription->toPlainText();

    return description;
}

void QFESPIMB040ExperimentDescription::on_btnNow_clicked() {
    ui->edtTime->setTime(QTime::currentTime());
    ui->edtDate->setDate(QDate::currentDate());
}
