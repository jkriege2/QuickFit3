#include "qfecalculatordialog.h"
#include "ui_qfecalculatordialog.h"
#include "qfecalculator.h"

QFECalculatorDialog::QFECalculatorDialog(QFECalculator *calc, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFECalculatorDialog)
{
    this->calc=calc;
    parser=new jkMathParser();

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
    std::vector<std::pair<std::string, jkMathParser::jkmpVariable> > vars= parser->getVariables();
    for (size_t i=0; i<vars.size(); i++) {
        QString v=QString("%1 = ").arg(vars[i].first.c_str());
            if (vars[i].second.type==jkMathParser::jkmpBool) {
                v=v+tr("%1 [boolean]").arg(boolToQString(*(vars[i].second.boolean)));
            } else if (vars[i].second.type==jkMathParser::jkmpDouble) {
                v=v+tr("%1 [float]").arg(*(vars[i].second.num));
            } else if (vars[i].second.type==jkMathParser::jkmpString) {
                v=v+tr("%1 [string]").arg(vars[i].second.str->c_str());
            } else {
                v=v+tr("??? [unknown]");
            }
        ui->lstCache->addItem(v);
    }
}


void QFECalculatorDialog::on_btnEvaluate_clicked()
{
    std::string expression=ui->edtExpression->text().toStdString();
    QTextCursor cur(ui->edtHistory->document());
    cur.movePosition(QTextCursor::Start);
    cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&gt;&gt; <i>%1</i></tt><br>").arg(expression.c_str())));
    try {
        jkMathParser::jkmpResult r=parser->evaluate(expression);

        QString result;
        if (r.isValid) {
            if (r.type==jkMathParser::jkmpBool) {
                result=tr("<font color=\"blue\">[boolean] %1</font>").arg(boolToQString(r.boolean));
            } else if (r.type==jkMathParser::jkmpDouble) {
                result=tr("<font color=\"blue\">[float] %1</font>").arg(r.num);
            } else if (r.type==jkMathParser::jkmpString) {
                result=tr("<font color=\"blue\">[string] %1</font>").arg(r.str.c_str());
            } else {
                result=tr("<font color=\"red\">[unknown] ? ? ?</font>");
            }
            jkMathParser::jkmpResult r1=parser->getVariableOrInvalid("ans");
            jkMathParser::jkmpResult r2=parser->getVariableOrInvalid("ans1");
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
    } catch(std::exception& E) {
        cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&nbsp;&nbsp;&nbsp;&nbsp; <font color=\"red\">ERROR: %1</font></tt><br>").arg(E.what())));
    }
    calc->setHistory(ui->edtHistory->document()->toHtml("UTF-8"));
    ui->edtHistory->moveCursor(QTextCursor::Start);
    showCache();
    ui->edtExpression->setFocus();
}

void QFECalculatorDialog::on_edtExpression_textChanged(QString text) {
    try {
        ui->labError->setText(tr("<font color=\"darkgreen\">OK</font>"));
        jkMathParser mp; // instanciate
        jkMathParser::jkmpNode* n;
        // parse some numeric expression
        n=mp.parse(text.toStdString());
        delete n;
        ui->btnEvaluate->setEnabled(true);
    } catch(std::exception& E) {
        ui->labError->setText(tr("<font color=\"red\">ERROR:</font> %1").arg(E.what()));
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
    parser=new jkMathParser();
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
