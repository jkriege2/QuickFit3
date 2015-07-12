/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
	
	

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

#include "qfmathparserwidget.h"
#include "ui_qfmathparserwidget.h"
#include "programoptions.h"
#include "qfmathparser.h"
#include "qfpluginservices.h"

qfmpResult QFMathParserWidget_dummy(const qfmpResult* params, unsigned char n, QFMathParser* p) {
    qfmpResult res;
    Q_UNUSED(params);
    Q_UNUSED(n);
    Q_UNUSED(p);

    return res;
}

QFMathParserWidget::QFMathParserWidget(QWidget *parent, const QStringList &moreRefDirs) :
    QWidget(parent),
    ui(new Ui::QFMathParserWidget)
{
    mp=new QFMathParser();
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



    QStringList defaultWords;
    //defaultWords<<"A";
    //defaultWords<<"B";



    functionRef->addDefaultWords(defaultWords);


    ui->edtFormula->setText(ProgramOptions::getConfigValue("QFMathParserWidget/expression", "").toString());
    ui->lstFunctions->setModel(functionRef->getHelpModel());

    this->moreRefDirs=moreRefDirs;
    QTimer::singleShot(10, this, SLOT(delayedStartSearch()));

}

QFMathParserWidget::~QFMathParserWidget()
{
    delete functionRef;
    ProgramOptions::setConfigValue("QFMathParserWidget/expression", ui->edtFormula->text());
    delete ui;
    delete mp;
}

QString QFMathParserWidget::getExpression() const
{
    return ui->edtFormula->text();
}

void QFMathParserWidget::setExpression(const QString &exp)
{
    ui->edtFormula->setText(exp);
}

void QFMathParserWidget::addExpressionDefaultWords(const QStringList &words)
{
    functionRef->addDefaultWords(words);
}

void QFMathParserWidget::addFunctionReferenceDirectory(const QString &directory)
{
    QStringList sl;
    sl<<directory;
    functionRef->startSearch(sl);
}

void QFMathParserWidget::on_edtFormula_textChanged(QString text) {
    Q_UNUSED(text);
    QFMathParser::qfmpNode* n;
    mp->resetErrors();
    // parse some numeric expression
    n=mp->parse(getExpression());
    delete n;
    if (mp->hasErrorOccured()) {
        ui->labError->setText(tr("<font color=\"red\">ERROR:</font> %1").arg(mp->getLastError()));
    } else {
        ui->labError->setText(tr("<font color=\"darkgreen\">OK</font>"));
    }

}


void QFMathParserWidget::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"mathparser.html");
}

void QFMathParserWidget::on_lstFunctions_doubleClicked(const QModelIndex &index)
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

void QFMathParserWidget::on_lstFunctions_clicked(const QModelIndex &index)
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

void QFMathParserWidget::delayedStartSearch()
{
    QStringList sl=QFPluginServices::getInstance()->getQFMathParserRefernceDirs();
    sl<<moreRefDirs;
    functionRef->startSearch(sl);
}


QString QFMathParserWidget::getFunctionTemplate(QString name)
{
    return functionRef->getFunctionTemplate(name);
}

QString QFMathParserWidget::getFunctionHelp(QString name)
{
    return functionRef->getFunctionHelp(name);
}



