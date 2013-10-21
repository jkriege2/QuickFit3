#include "qfrdrtablecolumneditor.h"
#include "ui_qfrdrtablecolumneditor.h"
#include "programoptions.h"
#include "qfmathparser.h"
#include "qfpluginservices.h"
#include "qfrdrtableparserfunctions.h"

qfmpResult QFRDRTableColumnEditor_dummy(const qfmpResult* params, unsigned int n, QFMathParser* p) {
    qfmpResult res;

    return res;
}

QFRDRTableColumnEditor::QFRDRTableColumnEditor(QFTablePluginModel *model, int col, QWidget *parent):
    QDialog(parent),
    ui(new Ui::QFRDRTableColumnEditor)
{
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
        ui->edtFormula->addInsertContextMenuEntry(tr("insert 'column(###)';;&%1: %2").arg(i+1).arg(model->columnTitle(i)), QString("column(%1)").arg(i+1));
        ui->edtFormula->addInsertContextMenuEntry(tr("insert column id;;&%1: %2").arg(i+1).arg(model->columnTitle(i)), QString("%1").arg(i+1));
    }

    QTimer::singleShot(10, this, SLOT(delayedStartSearch()));

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

void QFRDRTableColumnEditor::on_edtFormula_textChanged(QString text) {




    QFMathParser mp; // instanciate
    addQFRDRTableFunctions(&mp, &defaultWords, true);
    mp.addVariableDouble("col", col+1);
    mp.addVariableDouble("columns", model->columnCount());
    mp.addVariableDouble("rows", model->rowCount());
    mp.set_data(&mpdata);
    QFMathParser::qfmpNode* n;
    // parse some numeric expression
    n=mp.parse(getExpression());
    if (mp.hasErrorOccured()) {
        ui->labError->setText(tr("<font color=\"red\">ERROR:<br>&nbsp;&nbsp;&nbsp;&nbsp;%1</font>").arg(mp.getLastErrors().join("<br>&nbsp;&nbsp;&nbsp;&nbsp;")));
    } else {
        QString testout=qfShortenString(n->evaluate().toTypeString(), 250, 30, tr(" ... "));

        ui->labError->setText(tr("<font color=\"darkgreen\">OK</font>&nbsp;&nbsp;&nbsp;&nbsp;<i>expression result preview:</i>&nbsp;&nbsp;%1").arg(testout));
    }
    delete n;

}


void QFRDRTableColumnEditor::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"mathparser.html");
}

void QFRDRTableColumnEditor::on_lstFunctions_doubleClicked(const QModelIndex &index)
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


