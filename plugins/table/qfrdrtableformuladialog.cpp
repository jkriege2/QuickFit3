#include "qfrdrtableformuladialog.h"
#include "ui_qfrdrtableformuladialog.h"
#include "programoptions.h"
#include "jkmathparser.h"
#include "qfpluginservices.h"

jkMathParser::jkmpResult QFRDRTableFormulaDialog_dummy(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;

    return res;
}

QFRDRTableFormulaDialog::QFRDRTableFormulaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableFormulaDialog)
{
    ui->setupUi(this);
    compExpression=new QFCompleterFromFile(this);
    ui->edtFormula->setCompleter(compExpression);

    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/table/");
    compExpression->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/table/"+"table_expression.txt");
    QStringList sl=compExpression->stringlistModel()->stringList();


    defaultWords<<"pi";
    defaultWords<<"e";
    defaultWords<<"sqrt2";
    defaultWords<<"version";
    defaultWords<<"log2e";
    defaultWords<<"log10e";
    defaultWords<<"ln2";
    defaultWords<<"ln10";
    defaultWords<<"h";
    defaultWords<<"hbar";
    defaultWords<<"epsilon0";
    defaultWords<<"mu0";
    defaultWords<<"c";
    defaultWords<<"ce";
    defaultWords<<"muB";
    defaultWords<<"muB_eV";
    defaultWords<<"muN";
    defaultWords<<"muN_eV";
    defaultWords<<"me";
    defaultWords<<"mp";
    defaultWords<<"mn";
    defaultWords<<"NA";
    defaultWords<<"kB";
    defaultWords<<"kB_eV";
    defaultWords<<"sinc";
    defaultWords<<"asin";
    defaultWords<<"acos";
    defaultWords<<"atan";
    defaultWords<<"atan2";
    defaultWords<<"sin";
    defaultWords<<"cos";
    defaultWords<<"tan";
    defaultWords<<"sinh";
    defaultWords<<"cosh";
    defaultWords<<"tanh";
    defaultWords<<"log";
    defaultWords<<"log2";
    defaultWords<<"log10";
    defaultWords<<"exp";
    defaultWords<<"sqrt";
    defaultWords<<"sqr";
    defaultWords<<"abs";
    defaultWords<<"if";
    defaultWords<<"erf";
    defaultWords<<"erfc";
    defaultWords<<"lgamma";
    defaultWords<<"tgamma";
    defaultWords<<"j0";
    defaultWords<<"j1";
    defaultWords<<"jn";
    defaultWords<<"y0";
    defaultWords<<"y1";
    defaultWords<<"yn";
    defaultWords<<"rand";
    defaultWords<<"srand";
    defaultWords<<"ceil";
    defaultWords<<"floor";
    defaultWords<<"trunc";
    defaultWords<<"round";
    defaultWords<<"fmod";
    defaultWords<<"min";
    defaultWords<<"max";
    defaultWords<<"floattostr";
    defaultWords<<"booltostr";
    defaultWords<<"gauss";
    defaultWords<<"slit";
    defaultWords<<"theta";
    defaultWords<<"tanc";
    defaultWords<<"sigmoid";
    defaultWords<<"sign";


    defaultWords<<"rows";
    defaultWords<<"columns";
    defaultWords<<"row";
    defaultWords<<"column";
    defaultWords<<"col";
    defaultWords<<"data";
    defaultWords<<"dataleft";

    for (int i=0; i<defaultWords.size(); i++) {
        if (!sl.contains(defaultWords[i])) sl.append(defaultWords[i]);
    }
    qSort(sl);
    compExpression->stringlistModel()->setStringList(sl);


    ui->edtFormula->setText(ProgramOptions::getConfigValue("QFRDRTableFormulaDialog/expression", "sin(row/10*2*pi)").toString());
    ui->lstFunctions->setModel(&helpModel);

    QDir d(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"parserreference/");
    QStringList filter;
    filter<<"*.html"<<"*.htm"<<"*.txt";
    QStringList files=d.entryList(filter, QDir::Files);
    threadsFinished=0;
    maxThreads=2;
    QList<QStringList> absFiles;

    for (int i=0; i<maxThreads; i++) {
        QStringList sl;
        absFiles.push_back(sl);
    }
    for (int i=0; i<files.size(); i++) {
        QString file=d.absoluteFilePath(files[i]);
        absFiles[i%maxThreads].append(file);
    }
    for (int i=0; i<maxThreads; i++) {
        threads.append(new QFRDRTableFormulaDialogDocSearchThread(absFiles[i], this));
        connect(threads[i], SIGNAL(finished()), this, SLOT(threadFinished()));
        connect(threads[i], SIGNAL(foundFunction(QString,QString,QString)), this, SLOT(addFunction(QString,QString,QString)));
    }
    QTimer::singleShot(10, this, SLOT(delayedStartSearchThreads()));

}

QFRDRTableFormulaDialog::~QFRDRTableFormulaDialog()
{
    for (int i=0; i<threads.size(); i++) {
        if (threads[i]->isRunning()) {
            threads[i]->stopThread();
            threads[i]->wait();
        }
    }
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
        mp.addFunction("data", QFRDRTableFormulaDialog_dummy);
        mp.addFunction("dataleft", QFRDRTableFormulaDialog_dummy);
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

void QFRDRTableFormulaDialog::on_edtFormula_cursorPositionChanged(int /*old*/, int newPos) {
    QString text=ui->edtFormula->text();
    if (newPos>=0 && newPos<text.size()) {
        QString word;
        word+=text[newPos];
        int p=newPos-1;
        while (p>=0 && (text[p].isLetterOrNumber()||text[p]=='_')) {
            word=text[p]+word;
            p--;
        }
        p=newPos+1;
        while (p<text.size() && (text[p].isLetterOrNumber()||text[p]=='_')) {
            word=word+text[p];
            p++;
        }
        word=word.toLower();
        if (functionhelp.contains(word)) {
            ui->labHelp->setText(getFunctionHelp(word));
            ui->labTemplate->setText(tr("<tt>%1</tt>").arg(getFunctionTemplate(word)));
        }
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
    if (functionhelp.contains(word)) {
        ui->labHelp->setText(getFunctionHelp(word));
        ui->labTemplate->setText(tr("<tt>%1</tt>").arg(getFunctionTemplate(word)));
    }

}

void QFRDRTableFormulaDialog::threadFinished()
{
    threadsFinished++;
    if (threadsFinished>=maxThreads) {
        ui->progress->setSpin(false);
        ui->progress->setVisible(false);
        ui->labProgress->setVisible(false);
    }
}

void QFRDRTableFormulaDialog::delayedStartSearchThreads()
{
    helpModel.setStringList(defaultWords);
    ui->progress->setVisible(true);
    ui->progress->setSpin(true);
    ui->labProgress->setVisible(true);
    threadsFinished=0;
    for (int i=0; i<threads.size(); i++) {
        threads[i]->start();
    }
}

void QFRDRTableFormulaDialog::addFunction(QString name, QString templ, QString help)
{
    functionhelp[name.toLower()]=qMakePair(templ, help);
    QStringList sl=compExpression->stringlistModel()->stringList();
    if (!sl.contains(name)) sl.append(name);
    if (!sl.contains(templ)) sl.append(templ);
    sl.sort();
    compExpression->stringlistModel()->setStringList(sl);
    sl=helpModel.stringList();
    sl.removeAll(name);
    sl.removeAll(templ);
    sl.append(templ);
    sl.sort();
    helpModel.setStringList(sl);
}

QString QFRDRTableFormulaDialog::getFunctionTemplate(QString name)
{
    return functionhelp.value(name.toLower(), qMakePair(name+"()", QString())).first;
}

QString QFRDRTableFormulaDialog::getFunctionHelp(QString name)
{
    return functionhelp.value(name.toLower(), qMakePair(QString(""), tr("<b><tt>%1(...)</tt></b> - <i>function </i>:<br>no help available").arg(name))).second;
}





QFRDRTableFormulaDialogDocSearchThread::QFRDRTableFormulaDialogDocSearchThread(QStringList files, QObject *parent):
    QThread(parent)
{
    this->files=files;
    stopped=false;
}

void QFRDRTableFormulaDialogDocSearchThread::stopThread()
{
    stopped=true;
}

void QFRDRTableFormulaDialogDocSearchThread::run()
{
    for (int i=0; i<files.size(); i++) {
        QFile f(files[i]);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString contents=f.readAll();
            f.close();
            int pos=0;
            QRegExp rx("<!--\\s*func:([\\w]*)\\s*-->(.*)<!--\\s*/func:\\1\\s*-->");
            while ((pos = rx.indexIn(contents, pos)) != -1) {
                QString name=rx.cap(1).trimmed();
                QString help=rx.cap(2).trimmed();
                QRegExp rx2("<!--\\s*template\\s*-->(.*)<!--\\s*/template\\s*-->");
                QString templ="";
                if (rx2.indexIn(help,0)!=-1) {
                    templ=rx2.cap(1);
                }
                emit foundFunction(name, templ, help);
                pos += rx.matchedLength();
                if (stopped) break;
             }
        }
        if (stopped) break;
    }
}

