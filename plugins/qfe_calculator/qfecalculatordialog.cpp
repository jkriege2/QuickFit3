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


#include "qfecalculatordialog.h"
#include "ui_qfecalculatordialog.h"
#include "qfecalculator.h"
#include "qfecalculatorlibrary.h"


QFECalculatorDialog::QFECalculatorDialog(QFECalculator *calc, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFECalculatorDialog)
{
    this->calc=calc;
    parser=new QFMathParser();
    setupParser(parser);

    functionRef=new QFFunctionReferenceTool(NULL);
    functionRef->setCompleterFile(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+calc->getID()+"/table_expression.txt");
    functionRef->setDefaultWordsMathExpression();

    ui->setupUi(this);
    ui->edtExpression->setUseHistory(true);

    functionRef->registerEditor(ui->edtExpression);
    functionRef->registerEditor(ui->pteExpression);
    functionRef->setCurrentHelpButton(ui->btnFunctionHelp);
    functionRef->setLabHelp(ui->labHelp);
    functionRef->setDefaultHelp(QFPluginServices::getInstance()->getPluginHelp(calc->getID()));
    //functionRef->setLabProgress(ui->labProgress);
    //functionRef->setLabTemplate(ui->labTemplate);
    //functionRef->setProgress(ui->progress);



    QStringList defaultWords;


    functionRef->addDefaultWords(defaultWords);

    ui->spinDigits->setValue(ProgramOptions::getConfigValue("QFECalculatorDialog/digits", 10).toDouble());
    ProgramOptions::getConfigQSplitter(ui->splitter, "QFECalculatorDialog/splitter");
    ProgramOptions::getConfigWindowGeometry(this, "QFECalculatorDialog/window");

    QTimer::singleShot(10, this, SLOT(delayedStartSearch()));
    //on_chkMultiline_toggled(ui->chkMultiline->isChecked());
    ui->btnLoad->clearFocus();
    ui->btnSave->clearFocus();
    ui->btnEvaluate->clearFocus();
    ui->btnEvaluateScript->clearFocus();
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
    ui->edtExpression->setFocus(Qt::MouseFocusReason);

    QFont f(ProgramOptions::getConfigValue("quickfit/code_font", "Hack").toString(), ProgramOptions::getConfigValue("quickfit/code_pointsize", 10).toInt());
    ui->edtExpression->setFont(f)    ;
    ui->pteExpression->setFont(f);
}

void QFECalculatorDialog::setupParser(QFMathParser *parser) const
{
    parser->setGeneralData("QFECalculatorDialog/pointer", (qlonglong)this);
    parser->setGeneralData("QFECalculatorDialog/current_figure", int(-1));
    registerQFECalculatorFunctions(parser);
}

QFECalculatorDialog::~QFECalculatorDialog()
{
    ProgramOptions::setConfigQSplitter(ui->splitter, "QFECalculatorDialog/splitter");
    ProgramOptions::setConfigWindowGeometry(this, "QFECalculatorDialog/window");
    QMapIterator<int, QPointer<QFECalculatorPlotDialog> > i(plots);
    while (i.hasNext()) {
        i.next();
        if (i.value()) {
            i.value()->close();
            delete i.value();
        }
    }
    plots.clear();
    delete ui;
    delete parser;
}

int QFECalculatorDialog::getPrecision() const
{
    return ui->spinDigits->value();
}

QPlainTextEdit *QFECalculatorDialog::getHistory() const
{
    return ui->edtHistory;
}

QFECalculatorPlotDialog *QFECalculatorDialog::getPlot(int ii, int* index)
{
    int i=ii;
    if (ii==-1) {
        i=parser->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();
    }
    if (i>=0 && plots.contains(i) && plots[i]) {
        if (index) *index=i;
        plots[i]->show();
        plots[i]->raise();
        return plots[i];
    }

    if (i<0) i=qfstatisticsMax(plots.keys())+1;
    QFECalculatorPlotDialog* dlg=new QFECalculatorPlotDialog(NULL);
    plots[i]=dlg;
    dlg->setWindowTitle(tr("QFECalculator figure %1").arg(i));
    if (index) *index=i;
    dlg->show();
    dlg->raise();
    return dlg;
}

void QFECalculatorDialog::showHelp()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelp(calc->getID()));
}

void QFECalculatorDialog::showCache()
{
    ui->lstCache->setUpdatesEnabled(false);
    ui->lstFunctions->setUpdatesEnabled(false);
    ui->lstCache->clear();
    ui->lstFunctions->clear();
    QList<QPair<QString, QFMathParser::qfmpVariable> > vars= parser->getVariables();
    for (int i=0; i<vars.size(); i++) {
        QString v=QString("%1 = ").arg(vars[i].first);
        v+=vars[i].second.toResult().toTypeString(ui->spinDigits->value());
        ui->lstCache->addItem(v);
    }
    QList<QPair<QString, QFMathParser::qfmpFunctionDescriptor> > funs= parser->getFunctions();
    for (int i=0; i<funs.size(); i++) {
        ui->lstFunctions->addItem(funs[i].second.toDefString());
    }
    ui->lstCache->setUpdatesEnabled(true);
    ui->lstFunctions->setUpdatesEnabled(true);
}

void QFECalculatorDialog::on_btnLoad_clicked()
{
    QString filename=qfGetOpenFileNameSet("QFECalculatorDialog/", this, tr("load script ..."), QString(), tr("expression parser script (*.esc);;text file (*.txt);;all files (*.*)"));
    if (QFile::exists(filename)) {
        QFile f(filename);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            ui->pteExpression->setPlainText(QString::fromUtf8(f.readAll()));
            f.close();
        }
    }
}

void QFECalculatorDialog::on_btnSave_clicked()
{
    QString filename=qfGetSaveFileNameSet("QFECalculatorDialog/", this, tr("save script ..."), QString(), tr("expression parser script (*.esc);;text file (*.txt);;all files (*.*)"));
    if (!filename.isEmpty()) {
        QFile f(filename);
        if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
            {
                QTextStream s(&f);
                s<<ui->pteExpression->toPlainText().toUtf8();
            }
            f.close();
        }
    }
}


void QFECalculatorDialog::on_btnEvaluate_clicked()
{
    ProgramOptions::setConfigValue("QFECalculatorDialog/digits", ui->spinDigits->value());
    ProgramOptions::setConfigQSplitter(ui->splitter, "QFECalculatorDialog/splitter");
    ProgramOptions::setConfigWindowGeometry(this, "QFECalculatorDialog/window");
    QString expression=ui->edtExpression->text();
    //if (ui->chkMultiline->isChecked()) expression=ui->pteExpression->toPlainText();
    QTextCursor cur1(ui->edtHistory->document());
    cur1.movePosition(QTextCursor::End);
    cur1.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&gt;&gt; <i>%1</i></tt><br>").arg(qfHTMLEscape(expression))));
    parser->resetErrors();
    qfmpResult r=parser->evaluate(expression);

    QString result;
    if (r.isValid) {
        if (r.type==qfmpBool) {
            result=tr("<font color=\"blue\">[boolean] %1</font>").arg(qfHTMLEscape(boolToQString(r.boolean)));
        } else if (r.type==qfmpDouble) {
            result=tr("<font color=\"blue\">[float] %1</font>").arg(qfHTMLEscape(doubleToQString(r.num, ui->spinDigits->value())));
        } else if (r.type==qfmpDoubleVector) {
            result=tr("<font color=\"blue\">[float vector] [ %1 ]</font>").arg(qfHTMLEscape(doubleVecToQString(r.numVec, ui->spinDigits->value())));
        } else if (r.type==qfmpStringVector) {
            result=tr("<font color=\"blue\">[string vector] %1</font>").arg(qfHTMLEscape(listToString(r.strVec)));
        } else if (r.type==qfmpBoolVector) {
            result=tr("<font color=\"blue\">[boolean vector] %1</font>").arg(qfHTMLEscape(listToString(r.boolVec)));
        } else if (r.type==qfmpString) {
            result=tr("<font color=\"blue\">[string] %1</font>").arg(qfHTMLEscape(r.str));
        } else if (r.type==qfmpVoid) {
            result=tr("<font color=\"blue\">[void]</font>");
        } else if (r.type==qfmpStruct) {
            result=tr("<font color=\"blue\">[struct] %1</font>").arg(r.toString());
        } else {
            result=tr("<font color=\"red\">[unknown] ? ? ?</font>");
        }
        qfmpResult r1=parser->getVariableOrInvalid("ans");
        qfmpResult r2=parser->getVariableOrInvalid("ans1");
        if (r1.isUsableResult()) {
            parser->addVariable("ans1", r1);
        }
        if (r2.isUsableResult()) {
            parser->addVariable("ans2", r2);
        }
        parser->addVariable("ans", r);
        parser->addVariable("answer", r);
        ui->edtExpression->setText("");
        ui->btnLoad->clearFocus();
        ui->btnSave->clearFocus();
        ui->btnEvaluate->clearFocus();
        ui->btnEvaluateScript->clearFocus();
        ui->pteExpression->clearFocus();
        ui->edtExpression->setFocus();
        history.append(expression);
    } else {
        result=tr("<font color=\"red\">invalid result</font>");
    }

    QTextCursor cur(ui->edtHistory->document());
    cur.movePosition(QTextCursor::End);
    cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&nbsp;&nbsp;&nbsp;&nbsp; = %1</tt><br>").arg(result)));
    if (parser->hasErrorOccured()) {
        cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&nbsp;&nbsp;&nbsp;&nbsp; <font color=\"red\">ERROR: %1</font></tt><br>").arg(parser->getLastErrors().join("<br>ERROR: "))));
    }
    calc->setHistory(ui->edtHistory->document()->toHtml("UTF-8"));
    ui->edtHistory->moveCursor(QTextCursor::End);
    showCache();

    //on_chkMultiline_toggled(ui->chkMultiline->isChecked());
    //if (ui->chkMultiline->isChecked()) ui->pteExpression->setFocus();
    //else ui->edtExpression->setFocus();
    ui->edtExpression->setFocus();
}

void QFECalculatorDialog::on_btnEvaluateScript_clicked()
{
    ProgramOptions::setConfigValue("QFECalculatorDialog/digits", ui->spinDigits->value());
    ProgramOptions::setConfigQSplitter(ui->splitter, "QFECalculatorDialog/splitter");
    ProgramOptions::setConfigWindowGeometry(this, "QFECalculatorDialog/window");
    QString expression=ui->pteExpression->toPlainText();
    QTextCursor cur1(ui->edtHistory->document());
    cur1.movePosition(QTextCursor::End);
    QStringList ex=expression.split('\n');
    for (int i=0; i<ex.size(); i++) {
        if (i==0) cur1.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&gt;&gt; <i>%1</i></tt><br>").arg(qfHTMLEscape(ex[i]))));
        else cur1.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&nbsp;&nbsp;&nbsp;<i>%1</i></tt><br>").arg(qfHTMLEscape(ex[i]))));
    }
    parser->resetErrors();
    qfmpResult r=parser->evaluate(expression);

    QString result;
    if (r.isValid) {
        if (r.type==qfmpBool) {
            result=tr("<font color=\"blue\">[boolean] %1</font>").arg(qfHTMLEscape(boolToQString(r.boolean)));
        } else if (r.type==qfmpDouble) {
            result=tr("<font color=\"blue\">[float] %1</font>").arg(qfHTMLEscape(doubleToQString(r.num, ui->spinDigits->value())));
        } else if (r.type==qfmpDoubleVector) {
            result=tr("<font color=\"blue\">[float vector] [ %1 ]</font>").arg(qfHTMLEscape(doubleVecToQString(r.numVec, ui->spinDigits->value())));
        } else if (r.type==qfmpStringVector) {
            result=tr("<font color=\"blue\">[string vector] %1</font>").arg(qfHTMLEscape(listToString(r.strVec)));
        } else if (r.type==qfmpBoolVector) {
            result=tr("<font color=\"blue\">[boolean vector] %1</font>").arg(qfHTMLEscape(listToString(r.boolVec)));
        } else if (r.type==qfmpString) {
            result=tr("<font color=\"blue\">[string] %1</font>").arg(qfHTMLEscape(r.str));
        } else if (r.type==qfmpVoid) {
            result=tr("<font color=\"blue\">[void]</font>");
        } else if (r.type==qfmpStruct) {
            result=tr("<font color=\"blue\">[struct] %1</font>").arg(r.toString());
        } else {
            result=tr("<font color=\"red\">[unknown] ? ? ?</font>");
        }
        qfmpResult r1=parser->getVariableOrInvalid("ans");
        qfmpResult r2=parser->getVariableOrInvalid("ans1");
        if (r1.isUsableResult()) {
            parser->addVariable("ans1", r1);
        }
        if (r2.isUsableResult()) {
            parser->addVariable("ans2", r2);
        }
        parser->addVariable("ans", r);
        parser->addVariable("answer", r);
        history.append(expression);
    } else {
        result=tr("<font color=\"red\">invalid result</font>");
    }

    QTextCursor cur(ui->edtHistory->document());
    cur.movePosition(QTextCursor::End);
    cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&nbsp;&nbsp;&nbsp;&nbsp; = <tt>%1</tt></tt><br>").arg(result)));
    if (parser->hasErrorOccured()) {
        cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&nbsp;&nbsp;&nbsp;&nbsp; <font color=\"red\">ERROR: %1</font></tt><br>").arg(parser->getLastErrors().join("<br>ERROR: "))));
    }
    calc->setHistory(ui->edtHistory->document()->toHtml("UTF-8"));
    ui->edtHistory->moveCursor(QTextCursor::End);
    showCache();

    //on_chkMultiline_toggled(ui->chkMultiline->isChecked());
    //if (ui->chkMultiline->isChecked()) ui->pteExpression->setFocus();
    //else ui->edtExpression->setFocus();

}

void QFECalculatorDialog::on_edtExpression_textChanged(QString text) {
    ui->labError->setText(tr("<font color=\"darkgreen\">OK</font>"));
    QFMathParser mp; // instanciate
    setupParser(&mp);
    QFMathParser::qfmpNode* n=NULL;
    // parse some numeric expression
    n=mp.parse(text);
    if (n) delete n;
    ui->btnEvaluate->setEnabled(true);
    if (mp.hasErrorOccured()) {
        ui->labError->setText(tr("<font color=\"red\">ERROR: %1</font>").arg(mp.getLastErrors().join("<br>ERROR: ")));
        ui->btnEvaluate->setEnabled(false);
    }

}

//void QFECalculatorDialog::on_chkMultiline_toggled(bool enabled)
//{
//    ui->edtExpression->setVisible(!enabled);
//    ui->pteExpression->setVisible(enabled);
//}


void QFECalculatorDialog::on_btnClearHistory_clicked()
{
    history.clear();
    ui->edtHistory->clear();
    calc->setHistory(ui->edtHistory->document()->toHtml("UTF-8"));
}

void QFECalculatorDialog::on_btnClearCache_clicked()
{
    delete parser;
    parser=new QFMathParser();
    setupParser(parser);
    parser->setGeneralData("QFECalculatorDialog/pointer", (qlonglong)this);
    showCache();
}

void QFECalculatorDialog::delayedStartSearch()
{
    QStringList sl;

    sl<<QFPluginServices::getInstance()->getPluginHelpDirectory(calc->getID())+"parserreference/";
    sl<<QFPluginServices::getInstance()->getPluginHelpDirectory(calc->getID());
    sl<<QFPluginServices::getInstance()->getMainHelpDirectory()+"/parserreference/";
    functionRef->startSearch(sl);
}
