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
    ui->lstCache->clear();
    QList<QPair<QString, QFMathParser::qfmpVariable> > vars= parser->getVariables();
    for (size_t i=0; i<vars.size(); i++) {
        QString v=QString("%1 = ").arg(vars[i].first);
            if (vars[i].second.type==QFMathParser::qfmpBool) {
                v=v+tr("%1 [boolean]").arg(boolToQString(*(vars[i].second.boolean)));
            } else if (vars[i].second.type==QFMathParser::qfmpDouble) {
                v=v+tr("%1 [float]").arg(*(vars[i].second.num));
            } else if (vars[i].second.type==QFMathParser::qfmpString) {
                v=v+tr("%1 [string]").arg(*(vars[i].second.str));
            } else {
                v=v+tr("??? [unknown]");
            }
        ui->lstCache->addItem(v);
    }
}


void QFECalculatorDialog::on_btnEvaluate_clicked()
{
    QString expression=ui->edtExpression->text();
    QTextCursor cur(ui->edtHistory->document());
    cur.movePosition(QTextCursor::Start);
    cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&gt;&gt; <i>%1</i></tt><br>").arg(expression)));
    parser->resetErrors();
        QFMathParser::qfmpResult r=parser->evaluate(expression);

        QString result;
        if (r.isValid) {
            if (r.type==QFMathParser::qfmpBool) {
                result=tr("<font color=\"blue\">[boolean] %1</font>").arg(boolToQString(r.boolean));
            } else if (r.type==QFMathParser::qfmpDouble) {
                result=tr("<font color=\"blue\">[float] %1</font>").arg(r.num);
            } else if (r.type==QFMathParser::qfmpString) {
                result=tr("<font color=\"blue\">[string] %1</font>").arg(r.str);
            } else {
                result=tr("<font color=\"red\">[unknown] ? ? ?</font>");
            }
            QFMathParser::qfmpResult r1=parser->getVariableOrInvalid("ans");
            QFMathParser::qfmpResult r2=parser->getVariableOrInvalid("ans1");
            if (r1.isValid) {
                parser->addVariable("ans1", r1);
            }
            if (r2.isValid) {
                parser->addVariable("ans2", r2);
            }
            parser->addVariable("ans", r);
            parser->addVariable("answer", r);
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
    QFMathParser::qfmpNode* n;
    // parse some numeric expression
    n=mp.parse(text);
    delete n;
    ui->btnEvaluate->setEnabled(true);
    if (mp.hasErrorOccured()) {
        ui->labError->setText(tr("<font color=\"red\">ERROR: %1</font>").arg(mp.getLastErrors().join("<br>ERROR: ")));
        ui->btnEvaluate->setEnabled(false);
    }

}

void QFECalculatorDialog::on_btnClearHistory_clicked()
{
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
