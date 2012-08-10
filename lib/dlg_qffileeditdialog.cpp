#include "dlg_qffileeditdialog.h"
#include "ui_dlg_qffileeditdialog.h"
#include "qfstyledbutton.h"
#include "qfcompleterfromfile.h"
#include "programoptions.h"

QFFileEditDialog::QFFileEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFileEditDialog)
{
    ui->setupUi(this);

    QFStyledButton* btn=new QFStyledButton(QFStyledButton::SelectFile, ui->edtFile, ui->edtFile);
    ui->edtFile->addButton(btn);

    QFCompleterFromFile* c1=new QFCompleterFromFile(this);
    c1->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/rdr_fileedit_filetype.txt");
    QFCompleterFromFile* c2=new QFCompleterFromFile(this);
    c2->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"completers/rdr_fileedit_filedescription.txt");

    ui->edtDescription->setCompleter(c2);
    ui->edtType->setCompleter(c1);
}

QFFileEditDialog::~QFFileEditDialog()
{
    delete ui;
}

void QFFileEditDialog::init(const QString &file, const QString &type, const QString &description)
{
    ui->edtFile->setText(file);
    ui->edtType->setText(type);
    ui->edtDescription->setText(description);
}

QString QFFileEditDialog::getFile() const
{
    return ui->edtFile->text();
}

QString QFFileEditDialog::getType() const
{
    return ui->edtType->text();
}

QString QFFileEditDialog::getDescription() const
{
    return ui->edtDescription->text();
}
