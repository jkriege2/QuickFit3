#include "qfrdrtablesetcellsdialog.h"
#include "ui_qfrdrtablesetcellsdialog.h"

QFRDRTableSetCellsDialog::QFRDRTableSetCellsDialog(const QVariant &init, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableSetCellsDialog)
{
    ui->setupUi(this);
    ui->edtString->setText(init.toString());
    ui->edtNumber->setValue(init.toDouble());
    ui->chkBoolean->setChecked(init.toBool());
    ui->edtDateTime->setDateTime(init.toDateTime());
    switch(init.type()) {
        case QVariant::String:
            ui->cmbType->setCurrentIndex(0); break;
        case QVariant::Double:
            ui->cmbType->setCurrentIndex(1); break;
        case QVariant::LongLong:
        case QVariant::Int:
        case QVariant::ULongLong:
        case QVariant::UInt:
            ui->cmbType->setCurrentIndex(2); break;
        case QVariant::Date:
        case QVariant::Time:
        case QVariant::DateTime:
            ui->cmbType->setCurrentIndex(4); break;
        case QVariant::Bool:
            ui->cmbType->setCurrentIndex(3); break;
        default: break;
    }
    on_cmbType_currentIndexChanged(ui->cmbType->currentIndex());
}

QFRDRTableSetCellsDialog::~QFRDRTableSetCellsDialog()
{
    delete ui;
}

QVariant QFRDRTableSetCellsDialog::getValue() const
{
    switch(ui->cmbType->currentIndex()) {
        case 0: return ui->edtString->text();
        case 1: return ui->edtNumber->value();
        case 2: return qlonglong(ui->edtNumber->value());
        case 3: return ui->chkBoolean->isChecked();
        case 4: return ui->edtDateTime->dateTime();
        default: return QVariant();
    }
    return QVariant();
}

void QFRDRTableSetCellsDialog::on_cmbType_currentIndexChanged(int index)
{
    ui->edtString->setVisible(index==0);
    ui->edtNumber->setVisible(index==1 || index==2);
    ui->chkBoolean->setVisible(index==3);
    if (index==1) {
        ui->edtNumber->setIntegerWidget(false);
    } else if (index==2){
        ui->edtNumber->setIntegerWidget(true);
    }
    ui->edtDateTime->setVisible(index==4);
}
