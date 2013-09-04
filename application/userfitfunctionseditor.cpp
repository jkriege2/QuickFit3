#include "userfitfunctionseditor.h"
#include "ui_userfitfunctionseditor.h"

UserFitFunctionsEditor::UserFitFunctionsEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserFitFunctionsEditor)
{
    ui->setupUi(this);
}

UserFitFunctionsEditor::~UserFitFunctionsEditor()
{
    delete ui;
}
