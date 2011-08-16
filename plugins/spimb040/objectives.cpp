#include "objectives.h"
#include "ui_qfespimb040objectiveeditor.h"

ObjectiveDescription::ObjectiveDescription() {
    manufacturer="";
    name="invalid";
    NA=1;
    magnification=1;
}


QFESPIMB040ObjectiveEditor::QFESPIMB040ObjectiveEditor(const ObjectiveDescription& objective, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESPIMB040ObjectiveEditor)
{
    ui->setupUi(this);
    init(objective);
}

QFESPIMB040ObjectiveEditor::~QFESPIMB040ObjectiveEditor()
{
    delete ui;
}

void QFESPIMB040ObjectiveEditor::init(const ObjectiveDescription& objective) {
    ui->edtManufacturer->setText(objective.manufacturer);
    ui->edtName->setText(objective.name);
    ui->spinMagnification->setValue(objective.magnification);
    ui->spinNA->setValue(objective.NA);
}

ObjectiveDescription QFESPIMB040ObjectiveEditor::getData() {
    ObjectiveDescription o;
    o.name=ui->edtName->text();
    o.manufacturer=ui->edtManufacturer->text();
    o.magnification=ui->spinMagnification->value();
    o.NA=ui->spinNA->value();
    return o;
}
