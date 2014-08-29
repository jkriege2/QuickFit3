#include "functionreferencedialog.h"
#include "ui_functionreferencedialog.h"

FunctionReferenceDialog::FunctionReferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FunctionReferenceDialog)
{
    ui->setupUi(this);
}

FunctionReferenceDialog::~FunctionReferenceDialog()
{
    delete ui;
}

QString FunctionReferenceDialog::insertText() const
{
    return QString("$$funcref_start$$<a name=\"%1\"/><!-- func:%1 -->\n  <b><tt><!-- template -->%2<!-- /template --></tt> - <i> %3 </i>:</b>\n$$funcref_description$$\n    %4  <!-- /func:%1 -->\n$$funcref_end$$").arg(ui->edtName->text()).arg(ui->edtTemplate->text()).arg(ui->edtShort->text()).arg(ui->edtLong->toPlainText());
}
