#include "qfrdrtableformuladialog.h"
#include "ui_qfrdrtableformuladialog.h"
#include "programoptions.h"
#include "jkmathparser.h"
#include "qfpluginservices.h"
#include "qfrdrtableparserfunctions.h"

jkMathParser::jkmpResult QFRDRTableFormulaDialog_dummy(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;

    return res;
}

QFRDRTableFormulaDialog::QFRDRTableFormulaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableFormulaDialog)
{
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
    defaultWords<<"rows";
    defaultWords<<"columns";
    defaultWords<<"row";
    defaultWords<<"column";
    defaultWords<<"col";
    defaultWords<<"data";
    defaultWords<<"dataleft";


    jkMathParser mp; // instanciate
    addQFRDRTableFunctions(&mp, &defaultWords);

    functionRef->addDefaultWords(defaultWords);


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

QString QFRDRTableFormulaDialog::getExpression() const
{
    return ui->edtFormula->text();
}

void QFRDRTableFormulaDialog::setExpression(const QString &exp)
{
    ui->edtFormula->setText(exp);
}

void QFRDRTableFormulaDialog::on_edtFormula_textChanged(QString text) {
    try {
        ui->labError->setText(tr("<font color=\"darkgreen\">OK</font>"));
        jkMathParser mp; // instanciate
        addQFRDRTableFunctions(&mp);
        mp.addVariableDouble("row", 0.0);
        mp.addVariableDouble("rows", 1.0);
        mp.addVariableDouble("col", 0.0);
        mp.addVariableDouble("column", 0.0);
        mp.addVariableDouble("columns", 1.0);
        jkMathParser::jkmpNode* n;
        // parse some numeric expression
        n=mp.parse(getExpression().toStdString());
        delete n;
    } catch(std::exception& E) {
        ui->labError->setText(tr("<font color=\"red\">ERROR:</font> %1").arg(E.what()));
    }

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


