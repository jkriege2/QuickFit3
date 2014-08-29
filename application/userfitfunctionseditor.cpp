#include<QDebug>
#include "userfitfunctionseditor.h"
#include "ui_userfitfunctionseditor.h"
#include "qffitfunction.h"
#include "qffitfunctionmanager.h"
#include "userfitfunctiondelegate.h"
#include "qfmathparser.h"

UserFitFunctionsEditor::UserFitFunctionsEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserFitFunctionsEditor)
{
    ui->setupUi(this);
    ui->tvParams->setModel(&model);
    ui->tvParams->setItemDelegate(new UserFitFunctionDelegate(ui->tvParams));
    functionRef=new QFFunctionReferenceTool(NULL);
    functionRef->setCompleterFile(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/userfitfunctions_expression.txt");
    functionRef->setDefaultWordsMathExpression();
    functionRef->registerEditor(ui->edtExpression);
    init();

}

UserFitFunctionsEditor::~UserFitFunctionsEditor()
{
    delete ui;
}

void UserFitFunctionsEditor::clearFF()
{
    ui->groupBox->setEnabled(false);
    ui->edtExpression->clear();
    ui->edtID->clear();
    ui->edtName->clear();
    ui->edtShortName->clear();
    ui->widList->setEnabled(true);
    ui->buttonBox->setEnabled(true);
}

void UserFitFunctionsEditor::showFF(const QString &idd)
{
    QString id=idd;
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();

    model.clear();

    QString fn=manager->getUserFitFunctionFile(id);
    QString name=tr("new name"), expression="x^2+2", shortname="";
    if (QFile::exists(fn)) {
        QSettings settings(fn, QSettings::IniFormat);
        expression=settings.value("function/expression", expression).toString();
        id=settings.value("function/id", id).toString();
        name=settings.value("function/name", name).toString();
        shortname=settings.value("function/short_name", shortname).toString();
        model.loadSettings(settings);
    }


    ui->groupBox->setEnabled(false);
    ui->edtExpression->setText(expression);
    ui->edtID->setText(id);
    ui->edtName->setText(name);
    ui->edtShortName->setText(shortname);
    ui->labFilename->setText(fn);
    currentID=id;
    ui->widList->setEnabled(false);
    ui->groupBox->setEnabled(true);
    ui->buttonBox->setEnabled(false);
}

bool UserFitFunctionsEditor::storeCurrentFF()
{
    //qDebug()<<"storeCurrentFF()";
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    if (ui->groupBox->isEnabled()) {
        if (ui->edtID->text().isEmpty() || ui->edtExpression->text().isEmpty()) {
            QMessageBox::critical(this, tr("User Fit Function Editor"), tr("You have to specify at least an ID and an expression!"), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }
        QDir d(ProgramOptions::getInstance()->getConfigValue("quickfit/user_fitfunctions", ProgramOptions::getInstance()->getHomeQFDirectory()+"/userfitfunctions").toString()+"/");
        QString fn=d.absoluteFilePath(ui->edtID->text()+".qff");
        if (QFile::exists(manager->getUserFitFunctionFile(currentID))) {
            fn=manager->getUserFitFunctionFile(currentID);
        }
        //qDebug()<<"storeCurrentFF(): fn="<<fn;
        QSettings settings(fn, QSettings::IniFormat);
        settings.setValue("function/id", ui->edtID->text());
        settings.setValue("function/name", ui->edtName->text());
        settings.setValue("function/short_name", ui->edtShortName->text());
        settings.setValue("function/expression", ui->edtExpression->text());
        //settings.setValue("function/param_count", model.rowCount());
        model.storeSettings(settings);

    }
    return true;
}

void UserFitFunctionsEditor::init()
{
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    QStringList userIDs=manager->getUserFitFunctionIDs();
    ui->lstFunctions->clear();
    for (int i=0; i<userIDs.size(); i++) {
        QString fn=manager->getUserFitFunctionFile(userIDs[i]);
        QSettings settings(fn, QSettings::IniFormat);
        QString id=settings.value("function/id", userIDs[i]).toString();
        QString name=settings.value("function/name", userIDs[i]).toString();
        //ui->lstFunctions->addItem();
        QListWidgetItem* w=new QListWidgetItem(ui->lstFunctions);
        w->setData(Qt::UserRole, id);
        w->setText(name);
    }
    clearFF();
}

void UserFitFunctionsEditor::on_btnNewF_clicked()
{
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    QStringList userIDs=manager->getUserFitFunctionIDs();
    int cnt=0;
    QString id=tr("new_fitfunction");
    while (userIDs.contains(id)) {
        cnt++;
        id=tr("new_fitfunction_%1").arg(cnt);
    }
    showFF(id);
}

void UserFitFunctionsEditor::on_btnDeleteF_clicked()
{
    QString id=ui->lstFunctions->currentItem()->data(Qt::UserRole).toString();
    int req=QMessageBox::question(this, tr("User Fitfunction Editor"), tr("Do you really want to delete the selected fit function (ID: %1)?").arg(id), QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
    if (req==QMessageBox::Yes) {
        QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
        QString fn=manager->getUserFitFunctionFile(id);
        if (QFile::exists(fn)) {
            QFile::remove(fn);
        }
        manager->reloadUserFitFunctions();
        init();
    }

}

void UserFitFunctionsEditor::on_btnSaveEdit_clicked()
{
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    if (storeCurrentFF()) {
        clearFF();
        manager->reloadUserFitFunctions();
        init();
    }
}

void UserFitFunctionsEditor::on_btnCancelEdit_clicked()
{
    int req=QMessageBox::question(this, tr("User Fitfunction Editor"), tr("You are about to revert any changes made to the current fit function. Is this what you want?"), QMessageBox::Yes|QMessageBox::Cancel|QMessageBox::Save, QMessageBox::Cancel);
    if (req==QMessageBox::Yes) clearFF();
    if (req==QMessageBox::Save) {
        on_btnSaveEdit_clicked();
    }
}

void UserFitFunctionsEditor::on_lstFunctions_itemDoubleClicked(QListWidgetItem *item)
{
    QString id=item->data(Qt::UserRole).toString();
    if (!id.isEmpty()) showFF(id);
}

void UserFitFunctionsEditor::on_btnUp_clicked()
{
    model.moveUp(ui->tvParams->currentIndex().row());
}

void UserFitFunctionsEditor::on_btnDown_clicked()
{
    model.moveDown(ui->tvParams->currentIndex().row());
}

void UserFitFunctionsEditor::on_btnAdd_clicked()
{
    model.addParameter();
}

void UserFitFunctionsEditor::on_btnDelete_clicked()
{
    model.deleteParameter(ui->tvParams->currentIndex().row());
}

void UserFitFunctionsEditor::on_edtExpression_textChanged(const QString &text)
{
    ui->labFunctionOK->setText(tr("<font color=\"darkgreen\">function OK</font>"));
    QFMathParser mp; // instanciate
    QFMathParser::qfmpNode* n;
    mp.addVariableDouble("x", 1);
    QStringList p=model.getIDs();
    for (int i=0; i<p.size(); i++) {
        mp.addVariableDouble(p[i], 1);
    }
    // parse some numeric expression
    n=mp.parse(text);

    delete n;
    if (mp.hasErrorOccured()) {
        ui->labFunctionOK->setText(tr("<font color=\"red\">ERROR in function:<br>&nbsp;&nbsp;&nbsp;&nbsp;%1</font>").arg(mp.getLastErrors().join("<br>&nbsp;&nbsp;&nbsp;&nbsp;")));
    }
}

void UserFitFunctionsEditor::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("userfitfunctions.html");
}

void UserFitFunctionsEditor::on_btnFunctionHelp_clicked()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("mathparser.html");
}
