#include "tableresizedialog.h"
#include "ui_tableresizedialog.h"

TableResizeDialog::TableResizeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableResizeDialog)
{
    ui->setupUi(this);
}

TableResizeDialog::~TableResizeDialog()
{
    delete ui;
}

void TableResizeDialog::init(int width, int height)
{
    ui->labWidth->setText(QString::number(width));
    ui->spinWidth->setValue(width);
    ui->labHeight->setText(QString::number(height));
    ui->spinHeight->setValue(height);
}

int TableResizeDialog::newWidth() const
{
    return ui->spinWidth->value();
}

int TableResizeDialog::newHeight() const
{
    return ui->spinHeight->value();
}
