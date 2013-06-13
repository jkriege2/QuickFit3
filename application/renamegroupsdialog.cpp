#include "renamegroupsdialog.h"
#include "ui_renamegroupsdialog.h"

RenameGroupsDialog::RenameGroupsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenameGroupsDialog)
{
    ui->setupUi(this);
    ui->listView->setModel(&model);
}

RenameGroupsDialog::~RenameGroupsDialog()
{
    delete ui;
}

void RenameGroupsDialog::setStrings(const QStringList &list)
{
    model.setStringList(list);
}


QStringList RenameGroupsDialog::getStrings() const
{
    return model.stringList();
}
