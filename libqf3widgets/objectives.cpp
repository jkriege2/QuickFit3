#include "objectives.h"
#include "ui_qf3objectiveeditor.h"

ObjectiveDescription::ObjectiveDescription() {
    manufacturer="";
    name="invalid";
    NA=1;
    magnification=1;
}


QF3ObjectiveEditor::QF3ObjectiveEditor(const ObjectiveDescription& objective, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QF3ObjectiveEditor)
{
    ui->setupUi(this);
    init(objective);
}

QF3ObjectiveEditor::~QF3ObjectiveEditor()
{
    delete ui;
}

void QF3ObjectiveEditor::init(const ObjectiveDescription& objective) {
    ui->edtManufacturer->setText(objective.manufacturer);
    ui->edtName->setText(objective.name);
    ui->spinMagnification->setValue(objective.magnification);
    ui->spinNA->setValue(objective.NA);
}

ObjectiveDescription QF3ObjectiveEditor::getData() {
    ObjectiveDescription o;
    o.name=ui->edtName->text();
    o.manufacturer=ui->edtManufacturer->text();
    o.magnification=ui->spinMagnification->value();
    o.NA=ui->spinNA->value();
    return o;
}
