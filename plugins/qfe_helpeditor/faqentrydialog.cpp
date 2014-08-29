#include "faqentrydialog.h"
#include "ui_faqentrydialog.h"

FAQEntryDialog::FAQEntryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FAQEntryDialog)
{
    ui->setupUi(this);
}

FAQEntryDialog::~FAQEntryDialog()
{
    delete ui;
}

QString FAQEntryDialog::insertText() const
{
    QString ID=QString("<a name=\"%1\">").arg(ui->edtID->text());
    if (ui->edtID->text().isEmpty()) ID="";
    return QString("$$faq_start$$\n  %1<b>%2</b>\n$$faq_answer$$\n  %3\n$$faq_end$$").arg(ID).arg(ui->edtQuestion->toPlainText()).arg(ui->edtAnswer->toPlainText());
}
