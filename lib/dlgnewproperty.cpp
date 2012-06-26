#include "dlgnewproperty.h"
#include "ui_dlg_newproperty.h"

dlgNewProperty::dlgNewProperty(QWidget* parent):
    QDialog(parent), ui(new Ui::dlgNewProperty)
{
    ui->setupUi(this);


    ui->cmbName->setFocus(Qt::MouseFocusReason);
    ui->cmbName->lineEdit()->selectAll();
}

dlgNewProperty::~dlgNewProperty()
{
    delete ui;
}


QString dlgNewProperty::propertyName() {
    return ui->cmbName->currentText();
}

QString dlgNewProperty::propertyType() {
    return ui->cmbType->currentText();
}

int dlgNewProperty::propertyTypeInt() {
    return ui->cmbType->currentIndex();
}

QString dlgNewProperty::propertyValue() {
    return ui->edtValue->text();
}

void dlgNewProperty::setPropertyName(QString name) {
    ui->cmbName->setEditText(name);
}

void dlgNewProperty::addPropertyNames(const QStringList& names) {
    ui->cmbName->addItems(names);
}

void dlgNewProperty::setPropertyValue(QString value) {
    ui->edtValue->setText(value);
}

void dlgNewProperty::setPropertyType(int i) {
    ui->cmbType->setCurrentIndex(i);
}
