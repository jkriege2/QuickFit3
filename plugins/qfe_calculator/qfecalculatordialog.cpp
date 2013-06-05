#include "qfecalculatordialog.h"
#include "ui_qfecalculatordialog.h"
#include "qfecalculator.h"

QFECalculatorDialog::QFECalculatorDialog(QFECalculator *calc, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFECalculatorDialog)
{
    this->calc=calc;
    parser=new QFMathParser();

    functionRef=new QFFunctionReferenceTool(NULL);
    functionRef->setCompleterFile(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+calc->getID()+"/table_expression.txt");
    functionRef->setDefaultWordsMathExpression();

    ui->setupUi(this);
    ui->edtExpression->setUseHistory(true);

    functionRef->registerEditor(ui->edtExpression);
    functionRef->setCurrentHelpButton(ui->btnFunctionHelp);
    functionRef->setLabHelp(ui->labHelp);
    functionRef->setDefaultHelp(QFPluginServices::getInstance()->getPluginHelp(calc->getID()));
    //functionRef->setLabProgress(ui->labProgress);
    //functionRef->setLabTemplate(ui->labTemplate);
    //functionRef->setProgress(ui->progress);



    QStringList defaultWords;


    functionRef->addDefaultWords(defaultWords);

    QTimer::singleShot(10, this, SLOT(delayedStartSearch()));
}

QFECalculatorDialog::~QFECalculatorDialog()
{
    delete ui;
    delete parser;
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
        v+=vars[i].second.toResult().toTypeString();
        ui->lstCache->addItem(v);
    }
    QList<QPair<QString, QFMathParser::qfmpFunctionDescriptor> > funs= parser->getFunctions();
    for (int i=0; i<funs.size(); i++) {
        ui->lstFunctions->addItem(funs[i].second.toDefString());
    }
    ui->lstCache->setUpdatesEnabled(true);
    ui->lstFunctions->setUpdatesEnabled(true);
}


void QFECalculatorDialog::on_btnEvaluate_clicked()
{
    QString expression=ui->edtExpression->text();
    QTextCursor cur(ui->edtHistory->document());
    cur.movePosition(QTextCursor::Start);
    cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&gt;&gt; <i>%1</i></tt><br>").arg(expression)));
    parser->resetErrors();
    qfmpResult r=parser->evaluate(expression);

    QString result;
    if (r.isValid) {
        if (r.type==qfmpBool) {
            result=tr("<font color=\"blue\">[boolean] %1</font>").arg(boolToQString(r.boolean));
        } else if (r.type==qfmpDouble) {
            result=tr("<font color=\"blue\">[float] %1</font>").arg(r.num);
        } else if (r.type==qfmpDoubleVector) {
            result=tr("<font color=\"blue\">[float vector] %1</font>").arg(listToString(r.numVec));
        } else if (r.type==qfmpStringVector) {
            result=tr("<font color=\"blue\">[string vector] %1</font>").arg(listToString(r.strVec));
        } else if (r.type==qfmpBoolVector) {
            result=tr("<font color=\"blue\">[boolean vector] %1</font>").arg(listToString(r.boolVec));
        } else if (r.type==qfmpString) {
            result=tr("<font color=\"blue\">[string] %1</font>").arg(r.str);
        } else if (r.type==qfmpVoid) {
            result=tr("<font color=\"blue\">[void]</font>");
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
        ui->edtExpression->setFocus();
        history.append(expression);
    } else {
        result=tr("<font color=\"red\">invalid result</font>");
    }

    cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&nbsp;&nbsp;&nbsp;&nbsp; = %1</tt><br>").arg(result)));
    if (parser->hasErrorOccured()) {
        cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&nbsp;&nbsp;&nbsp;&nbsp; <font color=\"red\">ERROR: %1</font></tt><br>").arg(parser->getLastErrors().join("<br>ERROR: "))));
    }
    calc->setHistory(ui->edtHistory->document()->toHtml("UTF-8"));
    ui->edtHistory->moveCursor(QTextCursor::Start);
    showCache();
    ui->edtExpression->setFocus();
}

void QFECalculatorDialog::on_edtExpression_textChanged(QString text) {
    ui->labError->setText(tr("<font color=\"darkgreen\">OK</font>"));
    QFMathParser mp; // instanciate
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
