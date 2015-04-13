/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include<QDebug>
#include "userfitfunctionseditor.h"
#include "ui_userfitfunctionseditor.h"
#include "qffitfunction.h"
#include "qffitfunctionmanager.h"
#include "userfitfunctiondelegate.h"
#include "qfmathparser.h"
#include "qffitfunctionparsed.h"


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
    ui->spinPrevRangeMin->setCheckBounds(false, false);
    ui->spinPrevRangeMax->setCheckBounds(false, false);
    //ui->spinPrevRangeMin->setRange(0,1e38);
    ui->spinPrevRangeMin->setValue(0);
    //ui->spinPrevRangeMax->setRange(0,1e38);
    ui->spinPrevRangeMax->setValue(10);
    //connect(ui->chkPrevLogX, SIGNAL(toggled(bool)), ui->spinPrevRangeMin, SLOT(setCheckMinimum(bool)));
    //connect(ui->chkPrevLogX, SIGNAL(toggled(bool)), ui->spinPrevRangeMax, SLOT(setCheckMinimum(bool)));
    connect(ui->chkPrevLogX, SIGNAL(toggled(bool)), ui->spinPrevRangeMin, SLOT(setLogScale(bool)));
    connect(ui->chkPrevLogX, SIGNAL(toggled(bool)), ui->spinPrevRangeMax, SLOT(setLogScale(bool)));
    ui->splitter->setStretchFactor(0,0);
    ui->splitter->setStretchFactor(1,1);
    ui->splitter_3->setStretchFactor(0,1);
    ui->splitter_3->setStretchFactor(1,1);

    ProgramOptions::getConfigWindowGeometry(this, "UserFitFunctionsEditor/geometry/");
    ProgramOptions::getConfigQSplitter(ui->splitter, "UserFitFunctionsEditor/splitter/");
    ProgramOptions::getConfigQSplitter(ui->splitter_3, "UserFitFunctionsEditor/splitter_3/");
    //connect(ui->spinPrevRangeMin, SIGNAL(valueChanged(double)), ui->spinPrevRangeMax, SLOT(setMinimum(double)));
    init();

}

UserFitFunctionsEditor::~UserFitFunctionsEditor()
{
    ProgramOptions::setConfigWindowGeometry(this, "UserFitFunctionsEditor/geometry/");
    ProgramOptions::setConfigQSplitter(ui->splitter, "UserFitFunctionsEditor/splitter/");
    ProgramOptions::setConfigQSplitter(ui->splitter_3, "UserFitFunctionsEditor/splitter_3/");
    if (simplePlot) {
        simplePlot->close();
    }
    delete ui;
}

void UserFitFunctionsEditor::clearFF()
{
    ui->groupBox->setEnabled(false);
    ui->edtExpression->clear();
    ui->edtID->clear();
    ui->edtName->clear();
    ui->edtCategory->clear();
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
    QString name=tr("new name"), expression="x^2+2", shortname="", category=tr("User Fit Functions (parsed)");
    bool logX=false;
    bool logY=false;
    double rangemin=0;
    double rangemax=10;
    if (QFile::exists(fn)) {
        QSettings settings(fn, QSettings::IniFormat);
        expression=settings.value("function/expression", expression).toString();
        id=settings.value("function/id", id).toString();
        name=settings.value("function/name", name).toString();
        shortname=settings.value("function/short_name", shortname).toString();
        category=settings.value("function/category", category).toString();
        model.loadSettings(settings);
        if (id.startsWith("fcs_") << id.startsWith("dls_")|| id.startsWith("fccs_")) {
            logX=true;
            logY=false;
            rangemin=1e-6;
            rangemax=10;
        }
        settings.value("test/log_x", logX);
        settings.value("test/log_y", logY);
        settings.value("test/range_min", rangemin);
        settings.value("test/range_max", rangemax);
    }

    ui->chkPrevLogX->setChecked(logX);
    ui->chkPrevLogY->setChecked(logY);
    ui->spinPrevRangeMin->setValue(rangemin);
    ui->spinPrevRangeMax->setValue(rangemax);

    ui->groupBox->setEnabled(false);
    ui->edtExpression->setText(expression);
    ui->edtID->setText(id);
    ui->edtName->setText(name);
    ui->edtCategory->setText(category);
    ui->edtShortName->setText(shortname);
    ui->labFilename->setText(fn);
    currentID=id;
    ui->widList->setEnabled(false);
    ui->groupBox->setEnabled(true);
    ui->buttonBox->setEnabled(false);
}

bool UserFitFunctionsEditor::storeCurrentFF(const QString& filename)
{
    //qDebug()<<"storeCurrentFF()";
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    if (ui->groupBox->isEnabled()) {
        if (ui->edtID->text().isEmpty() || ui->edtExpression->text().isEmpty()) {
            QMessageBox::critical(this, tr("User Fit Function Editor"), tr("You have to specify at least an ID and an expression!"), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }
        QString fn=getFFFilename();
        if (!filename.isEmpty()) fn=filename;
        //qDebug()<<"storeCurrentFF(): fn="<<fn;
        QSettings settings(fn, QSettings::IniFormat);
        settings.setValue("function/id", ui->edtID->text());
        settings.setValue("function/name", ui->edtName->text());
        settings.setValue("function/category", ui->edtCategory->text());
        settings.setValue("function/short_name", ui->edtShortName->text());
        settings.setValue("function/expression", ui->edtExpression->text());
        settings.setValue("test/log_x", ui->chkPrevLogX->isChecked());
        settings.setValue("test/log_y", ui->chkPrevLogY->isChecked());
        settings.setValue("test/range_min", ui->spinPrevRangeMin->value());
        settings.setValue("test/range_max", ui->spinPrevRangeMax->value());
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

QString UserFitFunctionsEditor::getFFFilename()
{
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    QDir d(ProgramOptions::getInstance()->getConfigValue("quickfit/user_fitfunctions", ProgramOptions::getInstance()->getHomeQFDirectory()+"/userfitfunctions").toString()+"/");
    QString fn=d.absoluteFilePath(ui->edtID->text()+".qff");
    if (QFile::exists(manager->getUserFitFunctionFile(currentID))) {
        fn=manager->getUserFitFunctionFile(currentID);
    }
    return fn;
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

void UserFitFunctionsEditor::on_btnUpdatePreview_clicked()
{
    if (!simplePlot) {
        simplePlot=new QFSimplePlotDialog();
        simplePlot->setAxesEditable(false);
        simplePlot->setWindowTitle("QuickFit 3.0: User fit-function preview");
        simplePlot->setModal(false);
    }
    if (simplePlot) {
        simplePlot->show();
        simplePlot->raise();
        QFSimplePlotWidget* wid=simplePlot->getSimplePlot();
        wid->startAddingPlots();
        wid->clearPlots();
        wid->setLog(ui->chkPrevLogX->isChecked(), ui->chkPrevLogY->isChecked());
        wid->setXRange(ui->spinPrevRangeMin->value(), ui->spinPrevRangeMax->value());

        QString fn=qfGetTempFilename("qf_ffeditor_temp_XXXXXX");
        storeCurrentFF(fn);
        QFFitFunctionParsed* ff=new QFFitFunctionParsed(fn);

        QFile::remove(fn);
        QVector<double> params=model.getInitParams();

        double rmi=ui->spinPrevRangeMin->value();
        double rma=ui->spinPrevRangeMax->value();
        if (rma<rmi) qSwap(rmi,rma);
        if (ff) {
            if (ff->isValid()) {
                double ymin=0;
                double ymax=0;
                if (!ui->chkPrevLogX->isChecked()) {
                    ymin=ymax=ff->evaluate(rmi, params.data());
                    for (double v=rmi; v<=rma; v=v+(rma-rmi)/50.0) {
                        double e=ff->evaluate(v, params.data());
                        if (e<ymin) ymin=e;
                        if (e>ymax) ymax=e;
                    }
                } else {
                    ymin=ymax=ff->evaluate(rmi, params.data());
                    for (double v=log(rmi); v<=log(rma); v=v+(log(rma)-log(rmi))/50.0) {
                        double e=ff->evaluate(exp(v), params.data());
                        if (e<ymin) ymin=e;
                        if (e>ymax) ymax=e;
                    }
                }
                wid->addFunctionPlot(ff, "r-lw(1.5)", ui->edtID->text(), params);
                wid->setYRange(ymin, ymax);
            } else {
                delete ff;
            }
        }
        wid->endAddingPlots();
    }
}
