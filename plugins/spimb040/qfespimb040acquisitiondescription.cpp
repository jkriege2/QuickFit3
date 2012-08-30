#include "qfespimb040acquisitiondescription.h"
#include "ui_qfespimb040acquisitiondescription.h"
#include <QTimer>

QFESPIMB040AcquisitionDescription::QFESPIMB040AcquisitionDescription(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040AcquisitionDescription)
{
    ui->setupUi(this);
    updateTime();
}

QFESPIMB040AcquisitionDescription::~QFESPIMB040AcquisitionDescription()
{
    delete ui;
}


void QFESPIMB040AcquisitionDescription::loadSettings(QSettings& settings, QString prefix) {
    ui->edtComment->setText(settings.value(prefix+"comment", "").toString());
    ui->spinCell->setValue(settings.value(prefix+"cell", 1).toInt());
    ui->spinPlate->setValue(settings.value(prefix+"plate", 1).toInt());
    ui->spinWell->setValue(settings.value(prefix+"well", 1).toInt());
}

void QFESPIMB040AcquisitionDescription::storeSettings(QSettings& settings, QString prefix) const {
    settings.setValue(prefix+"comment", ui->edtComment->text());
    settings.setValue(prefix+"cell", ui->spinCell->value());
    settings.setValue(prefix+"plate", ui->spinPlate->value());
    settings.setValue(prefix+"well", ui->spinWell->value());

}

QMap<QString, QVariant> QFESPIMB040AcquisitionDescription::getDescription() const {
    QMap<QString, QVariant> description;
    description["comment"]=ui->edtComment->text();
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
