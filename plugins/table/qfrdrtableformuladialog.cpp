/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qfrdrtableformuladialog.h"
#include "ui_qfrdrtableformuladialog.h"
#include "programoptions.h"
#include "qfmathparser.h"
#include "qfpluginservices.h"
#include "qfrdrtableparserfunctions.h"
#include "qfrdrtable.h"

qfmpResult QFRDRTableFormulaDialog_dummy(const qfmpResult* /*params*/, unsigned char /*n*/, QFMathParser* /*p*/) {
    qfmpResult res;

    return res;
}

QFRDRTableFormulaDialog::QFRDRTableFormulaDialog(QFRDRTable *table, QFTablePluginModel *model, int col, int row, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableFormulaDialog)
{
    this->table=table;
    functionRef=new QFFunctionReferenceTool(NULL);
    functionRef->setCompleterFile(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/table/table_expression.txt");
    functionRef->setDefaultWordsMathExpression();

    ui->setupUi(this);

    functionRef->registerEditor(ui->edtFormula);
    functionRef->setLabHelp(ui->labHelp);
    functionRef->setLabProgress(ui->labProgress);
    functionRef->setLabTemplate(ui->labTemplate);
    functionRef->setProgress(ui->progress);
    functionRef->setDefaultHelp(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"mathparser.html");

    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);



    defaultWords<<"rows";
    defaultWords<<"columns";
    defaultWords<<"row";
    defaultWords<<"column";
    defaultWords<<"col";
    defaultWords<<"thisrdr";
    defaultWords<<"data";
    defaultWords<<"dataleft";


    init(model, col, row);


    functionRef->addDefaultWords(defaultWords);

    ui->edtFormula->addInsertContextMenuEntry(tr("insert 'row'"), QString("row"));
    ui->edtFormula->addInsertContextMenuEntry(tr("insert 'column'"), QString("column"));
    ui->edtFormula->addInsertContextMenuEntry(tr("insert row id"), QString("%1").arg(row+1));
    ui->edtFormula->addInsertContextMenuEntry(tr("insert column id"), QString("%1").arg(col+1));
    for (int i=0; i<model->columnCount(); i++) {
        ui->edtFormula->addInsertContextMenuEntry(tr("insert column id;;&%1: %2").arg(i+1).arg(model->columnTitle(i)), QString("%1").arg(i+1));
        ui->edtFormula->addInsertContextMenuEntry(tr("insert 'column(###)';;&%1: %2").arg(i+1).arg(model->columnTitle(i)), QString("column(%1)").arg(i+1));
    }

    ui->edtFormula->setText(ProgramOptions::getConfigValue("QFRDRTableFormulaDialog/expression", "sin(row/10*2*pi)").toString());
    ui->lstFunctions->setModel(functionRef->getHelpModel());

    QTimer::singleShot(10, this, SLOT(delayedStartSearch()));

}

QFRDRTableFormulaDialog::~QFRDRTableFormulaDialog()
{
    delete functionRef;
    ProgramOptions::setConfigValue("QFRDRTableFormulaDialog/expression", ui->edtFormula->text());
    delete ui;
}

void QFRDRTableFormulaDialog::init(QFTablePluginModel *model, int col, int row)
{
    this->model=model;
    this->col=col;
    this->row=row;
    mpdata.column=col;
    mpdata.row=row;
    mpdata.model=model;
}

QString QFRDRTableFormulaDialog::getExpression() const
{
    return ui->edtFormula->text();
}

void QFRDRTableFormulaDialog::setExpression(const QString &exp)
{
    ui->edtFormula->setText(exp);
}

void QFRDRTableFormulaDialog::on_edtFormula_textChanged(QString /*text*/) {

    QFMathParser mp; // instanciate

    mp.addVariableDouble("rows", model->rowCount());
    mp.addVariableDouble("thisrdr", table->getID());
    mp.addVariableDouble("columns", model->columnCount());
    if (table->getParserPreScript().size()>0) {
        QFMathParser::qfmpNode* n=mp.parse(table->getParserPreScript());
        if (n) {
            if (!mp.hasErrorOccured()) n->evaluate();
            delete n;
        }
        mp.resetErrors();
    }

    addQFRDRTableFunctions(&mp);
    mp.addVariableDouble("row", row+1);
    mp.addVariableDouble("col", col+1);
    mp.addVariableDouble("column", col+1);
    mp.set_data(&mpdata);
    QFMathParser::qfmpNode* n;
    // parse some numeric expression
    n=mp.parse(getExpression());
    if (mp.hasErrorOccured()) {
        ui->labError->setText(tr("<font color=\"red\">PARSE ERROR:<br>&nbsp;&nbsp;&nbsp;&nbsp;%1</font>").arg(mp.getLastErrors().join("<br>&nbsp;&nbsp;&nbsp;&nbsp;")));
    } else {
        if (ui->chkPreview->isChecked()) {
            mp.resetErrors();
            QString testout=qfShortenString(n->evaluate().toTypeString(), 250, 30, tr(" ... "));
            if (mp.hasErrorOccured()) {
                ui->labError->setText(tr("<font color=\"red\">EVALUATION ERROR:<br>&nbsp;&nbsp;&nbsp;&nbsp;%1</font>").arg(mp.getLastErrors().join("<br>&nbsp;&nbsp;&nbsp;&nbsp;")));
            } else {
                ui->labError->setText(tr("<font color=\"darkgreen\">OK</font>&nbsp;&nbsp;&nbsp;&nbsp;<i>expression result preview:</i>&nbsp;&nbsp;%1").arg(testout));
            }
        } else {
            ui->labError->setText(tr("<font color=\"darkgreen\">OK</font>"));
        }
    }
    delete n;

}


void QFRDRTableFormulaDialog::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"mathparser.html");
}

void QFRDRTableFormulaDialog::on_lstFunctions_doubleClicked(const QModelIndex &index)
{
    QString textOld=ui->edtFormula->text();
    QString text="";
    QString templ=index.data().toString();
    int cur=ui->edtFormula->cursorPosition();
    int newcur=cur;
    int selStart=ui->edtFormula->selectionStart();
    int newselStart=selStart;
    int selLen=ui->edtFormula->selectedText().size();
    int newselLen=0;

    if (selLen>0) {
        text=textOld.left(selStart);
        text=text+templ;
        text=text+textOld.right(textOld.size()-(selStart+selLen));

        newcur=cur+templ.size()+1;
        newselStart=newcur;
        newselLen=0;

        int ppos=templ.indexOf("(");
        if (ppos>=0) {
            newcur=selStart+ppos+1;
            newselStart=newcur;
            newselLen=0;
            int i=ppos+1;
            while (i<templ.size() && (templ[i].isLetterOrNumber() || templ[i]=='_')) {
                i++;
                newselLen++;
            }
        }
    } else {
        text=textOld;
        text.insert(cur, templ);

        newcur=cur+templ.size()+1;
        newselStart=newcur;
        newselLen=0;

        int ppos=templ.indexOf("(");
        if (ppos>=0) {
            newcur=cur+ppos+1;
            newselStart=newcur;
            newselLen=0;
            int i=ppos+1;
            while (i<templ.size() && (templ[i].isLetterOrNumber() || templ[i]=='_')) {
                i++;
                newselLen++;
            }
        }
    }

    ui->edtFormula->setText(text);
    ui->edtFormula->setFocus(Qt::OtherFocusReason);
    if (newselLen>0 && newselStart>=0) {
        ui->edtFormula->setCursorPosition(newselStart);
        ui->edtFormula->cursorForward(true, newselLen);
    } else {
        ui->edtFormula->setCursorPosition(newcur);
    }
}

void QFRDRTableFormulaDialog::on_lstFunctions_clicked(const QModelIndex &index)
{
    QString txt=index.data().toString();
    QString name="";
    int i=0;
    while (i<txt.size() && (txt[i].isLetterOrNumber() || txt[i]=='_')) {
        name=name+txt[i];
        i++;
    }
    QString word=name.toLower().trimmed();
    if (functionRef->hasFunction(word)) {
        ui->labHelp->setText(getFunctionHelp(word));
        ui->labTemplate->setText(tr("<tt>%1</tt>").arg(getFunctionTemplate(word)));
    }

}

void QFRDRTableFormulaDialog::delayedStartSearch()
{
    QStringList sl;
    sl<<QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"parserreference/";
    sl<<QFPluginServices::getInstance()->getMainHelpDirectory()+"/parserreference/";
    functionRef->startSearch(sl);
}


QString QFRDRTableFormulaDialog::getFunctionTemplate(QString name)
{
    return functionRef->getFunctionTemplate(name);
}

QString QFRDRTableFormulaDialog::getFunctionHelp(QString name)
{
    return functionRef->getFunctionHelp(name);
}


