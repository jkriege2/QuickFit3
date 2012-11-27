#include "qfecalculatordialog.h"
#include "ui_qfecalculatordialog.h"
#include "qfecalculator.h"

QFECalculatorDialog::QFECalculatorDialog(QFECalculator *calc, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFECalculatorDialog)
{
    this->calc=calc;
    parser=new jkMathParser();
    ui->setupUi(this);
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
    cur.movePosition(QTextCursor::End);
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
                result=tr("<font color=\"red\">[unknown] ???</font>");
            }
        } else {
            result=tr("<font color=\"red\">invalid result</font>");
        }

        cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&nbsp;&nbsp;&nbsp;&nbsp; = %1</tt><br>").arg(result)));
    } catch(std::exception& E) {
        cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&nbsp;&nbsp;&nbsp;&nbsp; <font color=\"red\">ERROR: %1</font></tt><br>").arg(E.what())));
    }
    calc->setHistory(ui->edtHistory->document()->toHtml("UTF-8"));
    ui->edtHistory->moveCursor(QTextCursor::End);
    showCache();
    ui->edtExpression->setFocus();
}

void QFECalculatorDialog::on_btnClearHistory_clicked()
{
    ui->edtHistory->clear();
    calc->setHistory(ui->edtHistory->document()->toHtml("UTF-8"));
}

void QFECalculatorDialog::on_btnCLearCache_clicked()
{
    delete parser;
    parser=new jkMathParser();
    showCache();
}
