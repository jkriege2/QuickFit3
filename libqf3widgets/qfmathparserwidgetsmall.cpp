/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
	
	last modification: $LastChangedDate: 2014-09-02 10:24:20 +0200 (Di, 02 Sep 2014) $  (revision $Rev: 3422 $)

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

#include "qfmathparserwidgetsmall.h"
#include "ui_qfmathparserwidgetsmall.h"
#include "programoptions.h"
#include "qfmathparser.h"
#include "qfpluginservices.h"

qfmpResult QFMathParserWidgetSmall_dummy(const qfmpResult* params, unsigned char n, QFMathParser* p) {
    qfmpResult res;
    Q_UNUSED(params);
    Q_UNUSED(n);
    Q_UNUSED(p);
    return res;
}

QFMathParserWidgetSmall::QFMathParserWidgetSmall(QWidget *parent, const QStringList& moreRefDirs) :
    QFrame(parent),
    ui(new Ui::QFMathParserWidgetSmall)
{
    mp=new QFMathParser();
    functionRef=new QFFunctionReferenceTool(NULL);
    functionRef->setCompleterFile(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/table/table_expression.txt");
    functionRef->setDefaultWordsMathExpression();

    ui->setupUi(this);

    functionRef->registerEditor(ui->edtFormula);
    /*functionRef->setLabHelp(ui->labHelp);
    functionRef->setLabProgress(ui->labProgress);
    functionRef->setLabTemplate(ui->labTemplate);
    functionRef->setProgress(ui->progress);*/
    functionRef->setDefaultHelp(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"mathparser.html");



    QStringList defaultWords;
    //defaultWords<<"A";
    //defaultWords<<"B";



    functionRef->addDefaultWords(defaultWords);


    ui->edtFormula->setText(ProgramOptions::getConfigValue("QFMathParserWidgetSmall/expression", "").toString());

    this->moreRefDirs=moreRefDirs;
    QTimer::singleShot(10, this, SLOT(delayedStartSearch()));

}

QFMathParserWidgetSmall::~QFMathParserWidgetSmall()
{
    delete functionRef;
    ProgramOptions::setConfigValue("QFMathParserWidgetSmall/expression", ui->edtFormula->text());
    delete ui;
    delete mp;
}

QString QFMathParserWidgetSmall::getExpression() const
{
    return ui->edtFormula->text();
}

void QFMathParserWidgetSmall::setExpression(const QString &exp)
{
    ui->edtFormula->setText(exp);
}

void QFMathParserWidgetSmall::addExpressionDefaultWords(const QStringList &words)
{
    functionRef->addDefaultWords(words);
}

QFEnhancedLineEdit *QFMathParserWidgetSmall::getEdit() const
{
    return ui->edtFormula;
}

void QFMathParserWidgetSmall::addFunctionReferenceDirectory(const QString &directory)
{
    QStringList sl;
    sl<<directory;
    functionRef->startSearch(sl);
}

void QFMathParserWidgetSmall::on_edtFormula_textChanged(QString text) {
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
    emit expressionChanged(text);
}


void QFMathParserWidgetSmall::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"mathparser.html");
}

void QFMathParserWidgetSmall::delayedStartSearch()
{
    QStringList sl=QFPluginServices::getInstance()->getQFMathParserRefernceDirs();
    sl<<moreRefDirs;
    functionRef->startSearch(sl);
}


QString QFMathParserWidgetSmall::getFunctionTemplate(QString name)
{
    return functionRef->getFunctionTemplate(name);
}

QString QFMathParserWidgetSmall::getFunctionHelp(QString name)
{
    return functionRef->getFunctionHelp(name);
}


