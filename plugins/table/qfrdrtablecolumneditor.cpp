/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfrdrtablecolumneditor.h"
#include "ui_qfrdrtablecolumneditor.h"
#include "programoptions.h"
#include "qfmathparser.h"
#include "qfpluginservices.h"
#include "qfrdrtableparserfunctions.h"
#include "qfenhancedlineedit.h"
#include "qfrdrtable.h"

qfmpResult QFRDRTableColumnEditor_dummy(const qfmpResult* /*params*/, unsigned int /*n*/, QFMathParser* /*p*/) {
    qfmpResult res;

    return res;
}

QFRDRTableColumnEditor::QFRDRTableColumnEditor(QFRDRTable *table, QFTablePluginModel *model, int col, QWidget *parent):
    QDialog(parent),
    ui(new Ui::QFRDRTableColumnEditor)
{
    this->table=table;
    this->model=model;
    this->col=col;
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




    defaultWords<<"rows";
    defaultWords<<"columns";
    defaultWords<<"row";
    defaultWords<<"column";
    defaultWords<<"col";
    defaultWords<<"thisrdr";
    defaultWords<<"data";
    defaultWords<<"datacolumn";
    defaultWords<<"dataleft";


    mpdata.column=col;
    mpdata.row=0;
    mpdata.model=model;

    functionRef->addDefaultWords(defaultWords);


    ui->edtFormula->setText(ProgramOptions::getConfigValue("QFRDRTableColumnEditor/expression", "sin(column(0)/10*2*pi)").toString());
    ui->lstFunctions->setModel(functionRef->getHelpModel());

    //ui->edtFormula(QFEnhancedLineEdit)
    for (int i=0; i<model->columnCount(); i++) {
        int newcol=i+1;
        int deltacol=col+1-newcol;
        int deltacol_r=deltacol;
        QString sign="-";
        if (deltacol<0) {
            sign="+";
            deltacol=-deltacol;
        }
        ui->edtFormula->addInsertContextMenuEntry(tr("insert 'column(###)';;&%1: %2").arg(newcol).arg(model->columnTitle(i)), QString("column(%1)").arg(newcol));
        ui->edtFormula->addInsertContextMenuEntry(tr("insert column id;;&%1: %2").arg(newcol).arg(model->columnTitle(i)), QString("%1").arg(newcol));
        ui->edtFormula->addInsertContextMenuEntry(tr("insert rel. 'column(col-###)';;&%1: %2").arg(deltacol_r).arg(model->columnTitle(i)), QString("column(col%1%2)").arg(sign).arg(deltacol));
        ui->edtFormula->addInsertContextMenuEntry(tr("insert rel. column 'col-id'';;&%1: %2").arg(deltacol_r).arg(model->columnTitle(i)), QString("col%1%2").arg(sign).arg(deltacol));
    }

    QTimer::singleShot(10, this, SLOT(delayedStartSearch()));
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);

}



QFRDRTableColumnEditor::~QFRDRTableColumnEditor()
{
    delete functionRef;
    ProgramOptions::setConfigValue("QFRDRTableColumnEditor/expression", ui->edtFormula->text());
    delete ui;
}

void QFRDRTableColumnEditor::setImageWidth(int width)
{
    ui->spinImageWidth->setValue(width);
}

int QFRDRTableColumnEditor::getImageWidth() const
{
    return ui->spinImageWidth->value();
}

void QFRDRTableColumnEditor::setComment(const QString &comment)
{
    ui->edtComment->setText(comment);
}

QString QFRDRTableColumnEditor::getComment() const
{
    return ui->edtComment->text();
}

QString QFRDRTableColumnEditor::getExpression() const
{
    return ui->edtFormula->text();
}

void QFRDRTableColumnEditor::setExpression(bool enabled, const QString &exp)
{
    ui->grpExpression->setChecked(enabled);
    ui->edtFormula->setText(exp);
}

bool QFRDRTableColumnEditor::getExpressionEnabled() const
{
    return ui->grpExpression->isChecked();
}

void QFRDRTableColumnEditor::setColumnTitle(const QString& title)
{
    ui->edtTitle->setText(title);
}

QString QFRDRTableColumnEditor::getColumnTitle() const
{
    return ui->edtTitle->text();
}

void QFRDRTableColumnEditor::on_edtFormula_textChanged(QString /*text*/) {
    on_edtFormula_textChanged();
}

void QFRDRTableColumnEditor::on_edtFormula_textChanged() {




    QFMathParser mp; // instanciate
    mp.addVariableDouble("thisrdr", table->getID());
    mp.addVariableDouble("columns", model->columnCount());
    mp.addVariableDouble("rows", model->rowCount());

    if (table && table->getParserPreScript().size()>0) {
        QFMathParser::qfmpNode* n=mp.parse(table->getParserPreScript());
        if (n) {
            if (!mp.hasErrorOccured()) n->evaluate();
            delete n;
        }
        mp.resetErrors();
    }

    addQFRDRTableFunctions(&mp, &defaultWords, true);
    mp.addVariableDouble("col", col+1);
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


void QFRDRTableColumnEditor::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"mathparser.html");
}

void QFRDRTableColumnEditor::on_btnOK2_clicked()
{
    done(10);
}

void QFRDRTableColumnEditor::on_lstFunctions_doubleClicked(const QModelIndex &index)
{
    QFEnhancedPlainTextEdit* pte=qobject_cast<QFEnhancedPlainTextEdit*>(ui->edtFormula);
    QFEnhancedLineEdit* le=qobject_cast<QFEnhancedLineEdit*>(ui->edtFormula);

    if (le) {
        QString textOld=le->text();
        QString text="";
        QString templ=index.data().toString();
        int cur=le->cursorPosition();
        int newcur=cur;
        int selStart=le->selectionStart();
        int newselStart=selStart;
        int selLen=le->selectedText().size();
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

        le->setText(text);
        le->setFocus(Qt::OtherFocusReason);
        if (newselLen>0 && newselStart>=0) {
            le->setCursorPosition(newselStart);
            le->cursorForward(true, newselLen);
        } else {
            le->setCursorPosition(newcur);
        }
    } else if (pte) {
        QString text="";
        QString templ=index.data().toString();
        pte->textCursor().insertText(templ);
        pte->setFocus(Qt::OtherFocusReason);
    }
}

void QFRDRTableColumnEditor::on_lstFunctions_clicked(const QModelIndex &index)
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

void QFRDRTableColumnEditor::delayedStartSearch()
{
    QStringList sl;
    sl<<QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"parserreference/";
    sl<<QFPluginServices::getInstance()->getMainHelpDirectory()+"/parserreference/";
    functionRef->startSearch(sl);
}


QString QFRDRTableColumnEditor::getFunctionTemplate(QString name)
{
    return functionRef->getFunctionTemplate(name);
}

QString QFRDRTableColumnEditor::getFunctionHelp(QString name)
{
    return functionRef->getFunctionHelp(name);
}

void QFRDRTableColumnEditor::showEvent(QShowEvent *event)
{
    ui->edtTitle->setFocus();
    ui->edtTitle->selectAll();
    QDialog::showEvent(event);
}


