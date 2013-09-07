#include "qfecalculatordialog.h"
#include "ui_qfecalculatordialog.h"
#include "qfecalculator.h"

qfmpResult fDisp(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)p->getGeneraldata("QFECalculatorDialog/pointer", NULL).toInt();
  if (dlg) {
      QPlainTextEdit* h=dlg->getHistory();

      QString t="";
      for (int i=0; i<n; i++) {
          t=t+params[i].toString(dlg->getPrecision());
      }

      QTextCursor cur1(h->document());
      cur1.movePosition(QTextCursor::End);
      cur1.insertFragment(QTextDocumentFragment::fromPlainText(t+"\n"));

  }
  return qfmpResult::voidResult();
}

qfmpResult fDispNLB(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)p->getGeneraldata("QFECalculatorDialog/pointer", NULL).toInt();
  if (dlg) {
      QPlainTextEdit* h=dlg->getHistory();

      QString t="";
      for (int i=0; i<n; i++) {
          t=t+params[i].toString(dlg->getPrecision());
      }

      QTextCursor cur1(h->document());
      cur1.movePosition(QTextCursor::End);
      cur1.insertFragment(QTextDocumentFragment::fromPlainText(t));

  }
  return qfmpResult::voidResult();
}

qfmpResult fDispType(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)p->getGeneraldata("QFECalculatorDialog/pointer", NULL).toInt();
  if (dlg) {
      QPlainTextEdit* h=dlg->getHistory();

      QString t="";
      for (int i=0; i<n; i++) {
          t=t+params[i].toTypeString(dlg->getPrecision());
      }

      QTextCursor cur1(h->document());
      cur1.movePosition(QTextCursor::End);
      cur1.insertFragment(QTextDocumentFragment::fromPlainText(t+"\n"));

  }
  return qfmpResult::voidResult();
}



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

    QTimer::singleShot(10, this, SLOT(delayedStartSearch()));
    on_chkMultiline_toggled(ui->chkMultiline->isChecked());
}

void QFECalculatorDialog::setupParser(QFMathParser *parser) const
{
    parser->setGeneralData("QFECalculatorDialog/pointer", (int)this);
    parser->addFunction("write", fDispNLB);
    parser->addFunction("writeln", fDisp);
    parser->addFunction("disp", fDisp);
    parser->addFunction("dispType", fDispType);
}

QFECalculatorDialog::~QFECalculatorDialog()
{

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


void QFECalculatorDialog::on_btnEvaluate_clicked()
{
    ProgramOptions::setConfigValue("QFECalculatorDialog/digits", ui->spinDigits->value());
    QString expression=ui->edtExpression->text();
    if (ui->chkMultiline->isChecked()) expression=ui->pteExpression->toPlainText();
    QTextCursor cur1(ui->edtHistory->document());
    cur1.movePosition(QTextCursor::End);
    cur1.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&gt;&gt; <i>%1</i></tt><br>").arg(expression)));
    parser->resetErrors();
    qfmpResult r=parser->evaluate(expression);

    QString result;
    if (r.isValid) {
        if (r.type==qfmpBool) {
            result=tr("<font color=\"blue\">[boolean] %1</font>").arg(boolToQString(r.boolean));
        } else if (r.type==qfmpDouble) {
            result=tr("<font color=\"blue\">[float] %1</font>").arg(r.num, 0, 'g', ui->spinDigits->value());
        } else if (r.type==qfmpDoubleVector) {
            result=tr("<font color=\"blue\">[float vector] %1</font>").arg(numlistToString(r.numVec));
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

    QTextCursor cur(ui->edtHistory->document());
    cur.movePosition(QTextCursor::End);
    cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&nbsp;&nbsp;&nbsp;&nbsp; = %1</tt><br>").arg(result)));
    if (parser->hasErrorOccured()) {
        cur.insertFragment(QTextDocumentFragment::fromHtml(tr("<tt>&nbsp;&nbsp;&nbsp;&nbsp; <font color=\"red\">ERROR: %1</font></tt><br>").arg(parser->getLastErrors().join("<br>ERROR: "))));
    }
    calc->setHistory(ui->edtHistory->document()->toHtml("UTF-8"));
    ui->edtHistory->moveCursor(QTextCursor::End);
    showCache();

    on_chkMultiline_toggled(ui->chkMultiline->isChecked());
    if (ui->chkMultiline->isChecked()) ui->pteExpression->setFocus();
    else ui->edtExpression->setFocus();
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

void QFECalculatorDialog::on_chkMultiline_toggled(bool enabled)
{
    ui->edtExpression->setVisible(!enabled);
    ui->pteExpression->setVisible(enabled);
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
    setupParser(parser);
    parser->setGeneralData("QFECalculatorDialog/pointer", (int)this);
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
