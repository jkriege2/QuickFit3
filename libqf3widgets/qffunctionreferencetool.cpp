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

#include "qffunctionreferencetool.h"
#include "qfpluginservices.h"
#include "qfhtmlhelptools.h"
#include "qfenhancedlineedit.h"
#include <QTextCursor>
#include "qfenhancedplaintextedit.h"
#include "programoptions.h"
QFFunctionReferenceToolDocSearchThread::QFFunctionReferenceToolDocSearchThread(QStringList files, QObject *parent):
    QThread(parent)
{
    this->files=files;

    stopped=false;
    rxDefinition=QRegExp("<!--\\s*func:([\\w]*)\\s*-->(.*)<!--\\s*/func:\\1\\s*-->");
    rxDefinition.setCaseSensitivity(Qt::CaseInsensitive);
    rxTemplate=QRegExp("<!--\\s*template\\s*-->(.*)<!--\\s*/template\\s*-->");
    rxTemplate.setMinimal(true);
    rxTemplate.setCaseSensitivity(Qt::CaseInsensitive);
}

void QFFunctionReferenceToolDocSearchThread::setRxDefinition(const QRegExp &rxDefinition)
{
    this->rxDefinition=rxDefinition;
}

void QFFunctionReferenceToolDocSearchThread::setRxTemplate(const QRegExp &rxTemplate)
{
    this->rxTemplate=rxTemplate;
}

void QFFunctionReferenceToolDocSearchThread::stopThread()
{
    stopped=true;
}

void QFFunctionReferenceToolDocSearchThread::run()
{
    //qDebug()<<"t.starting thread "<<files<<rxDefinition<<rxTemplate;
    for (int i=0; i<files.size(); i++) {
        QFile f(files[i]);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString contents=f.readAll();
            f.close();
            int pos=0;
            while ((pos = rxDefinition.indexIn(contents, pos)) != -1) {
                QString name=rxDefinition.cap(1).trimmed();
                QString help=rxDefinition.cap(2).trimmed();
                QStringList templ;
                //qDebug()<<"t.def "<<pos<<name;
                int pos1=0;
                while ((pos1=rxTemplate.indexIn(help,pos1)) != -1) {
                    //qDebug()<<"  t.templ "<<rxTemplate.cap(1);
                    templ<<rxTemplate.cap(1);
                    if (stopped) break;
                    pos1 += rxTemplate.matchedLength();
                }
                emit foundFunction(name, templ, help, QFileInfo(files[i]).absoluteFilePath()+QString("#")+name);
                //qDebug()<<"t.found "<<name<<templ;
                pos += rxDefinition.matchedLength();
                if (stopped) break;
             }
        }
        if (stopped) break;
    }
    //qDebug()<<"t.done thread ";
}













QFFunctionReferenceTool::QFFunctionReferenceTool(QObject *parent) :
    QObject(parent)
{
    currentFunction="";
    maxThreads=2;
    threadsFinished=0;
    labProgress=NULL;
    progress=NULL;
    mprogress=NULL;
    searching=false;
    compExpression=new QFCompleterFromFile(this);
    defaultHelp=ProgramOptions::getInstance()->getMainHelpDirectory()+QString("/mathparser.html");
    rxDefinition=QRegExp("<!--\\s*func:([\\w]*)\\s*-->(.*)<!--\\s*/func:\\1\\s*-->");
    rxDefinition.setMinimal(false);
    rxDefinition.setCaseSensitivity(Qt::CaseInsensitive);
    rxTemplate=QRegExp("<!--\\s*template\\s*-->(.*)<!--\\s*/template\\s*-->");
    rxTemplate.setMinimal(true);
    rxTemplate.setCaseSensitivity(Qt::CaseInsensitive);
    btnCurrentHelp=NULL;
    labHelp=NULL;
    labTemplate=NULL;

    actDefaultHelp=new QFActionWithNoMenuRole(tr("show parser help"), this);
    actCurrentFunctionHelp=new QFActionWithNoMenuRole(tr("help for function under cursor"), this);
    connect(actDefaultHelp, SIGNAL(triggered()), SLOT(showDefaultHelp()));
    connect(actCurrentFunctionHelp, SIGNAL(triggered()), SLOT(showCurrentFunctionHelp()));
    connect(&timDelayedAddNamesAndTemplates, SIGNAL(timeout()), this, SLOT(delayedAddNamesAndTemplates()));
    connect(&timDelayedCursorChange, SIGNAL(timeout()), this, SLOT(onCursorPositionChanged()));
}

void QFFunctionReferenceTool::setRxDefinition(const QRegExp &rxDefinition)
{
    this->rxDefinition=rxDefinition;
}

void QFFunctionReferenceTool::setRxTemplate(const QRegExp &rxTemplate)
{
    this->rxTemplate=rxTemplate;
}

void QFFunctionReferenceTool::threadFinished()
{
    //qDebug()<<"threadFinished";
    threadsFinished++;
    if (threadsFinished>=maxThreads) {
        if (progress) {
            /*progress->setSpin(false);
            progress->setVisible(false);*/
        }
        if (mprogress) {
            mprogress->setSpin(false);
            mprogress->setVisible(false);
        }
        if (labProgress) {
            labProgress->setVisible(false);
        }
        searching=false;
    }
    QApplication::processEvents();
    delayedAddNamesAndTemplates();
}

void QFFunctionReferenceTool::delayedStartSearchThreads()
{
    stopThreadsWait();
    functionhelp.clear();
    helpModel.setStringList(defaultWords);
    if (mprogress) {
        mprogress->setVisible(true);
        mprogress->setSpin(true);
    }
    if (progress) {
        /*progress->setVisible(true);
        progress->setSpin(true);*/
    }
    if (labProgress) {
        labProgress->setVisible(true);
    }
    threadsFinished=0;
    searching=true;
    for (int i=0; i<threads.size(); i++) {
        //qDebug()<<"starting thread "<<i+1;
        threads[i]->start();
    }
}

void QFFunctionReferenceTool::addFunction(QString name, QString templ, QString help, QString helplink)
{
    //qDebug()<<"addFunction"<<name<<templ;
    QFFunctionReferenceTool::functionDescription fd;
    fd.fhelp=help;
    fd.ftemplate=templ;
    fd.fhelplink=helplink;
    QUrl url(helplink);
    fd.fhelpfile=url.toString(QUrl::RemoveFragment);
    //qDebug()<<"function help "<<helplink;
    //qDebug()<<"           -> "<<fd.fhelpfile;
    functionhelp[name.toLower()]=fd;
    namesToAdd.append(name);
    templatesToAdd.append(templ);

    timDelayedAddNamesAndTemplates.setSingleShot(true);
    timDelayedAddNamesAndTemplates.setInterval(150);
    timDelayedAddNamesAndTemplates.start();

}

void QFFunctionReferenceTool::addFunction(QString name, QStringList templ, QString help, QString helplink)
{
    //qDebug()<<"addFunction"<<name<<templ;
    QFFunctionReferenceTool::functionDescription fd;
    fd.fhelp=help;
    fd.ftemplate=templ.value(0, "");
    fd.fhelplink=helplink;
    QUrl url(helplink);
    fd.fhelpfile=url.toString(QUrl::RemoveFragment);
    //qDebug()<<"function help "<<helplink;
    //qDebug()<<"           -> "<<fd.fhelpfile;
    functionhelp[name.toLower()]=fd;
    namesToAdd.append(name);
    templatesToAdd<<templ;

    timDelayedAddNamesAndTemplates.setSingleShot(true);
    timDelayedAddNamesAndTemplates.setInterval(150);
    timDelayedAddNamesAndTemplates.start(150);
}

void QFFunctionReferenceTool::delayedAddNamesAndTemplates() {
    //qDebug()<<"delayedAddNamesAndTemplates"<<namesToAdd<<templatesToAdd;
    QStringList sl=compExpression->stringlistModel()->stringList();
    QStringList sl2=helpModel.stringList();
    while (namesToAdd.size()>0) {
        QString name=namesToAdd.first();
        if (!sl.contains(name)) sl.append(name);
        namesToAdd.removeFirst();
    }
    while (templatesToAdd.size()>0) {
        QString templ=templatesToAdd.first();
        if (!sl.contains(templ)) sl.append(templ);
        sl2.removeAll(templ);
        sl2.append(templ);
        templatesToAdd.removeFirst();
    }
    sl.sort();
    sl2.sort();

    compExpression->stringlistModel()->setStringList(sl);
    helpModel.setStringList(sl2);
}

void QFFunctionReferenceTool::stopThreadsWait()
{
    if (searching) {
        for (int i=0; i<threads.size(); i++) {
            if (threads[i]->isRunning()) {
                threads[i]->stopThread();
                threads[i]->wait();
            }
            QApplication::processEvents();
        }
    }
    searching=false;
}

void QFFunctionReferenceTool::updateHelpModel()
{
    QStringList sl=compExpression->stringlistModel()->stringList();
    for (int i=0; i<defaultWords.size(); i++) {
        if (!sl.contains(defaultWords[i])) sl.append(defaultWords[i]);
    }
    qSort(sl);
    compExpression->stringlistModel()->setStringList(sl);

}

QAbstractItemModel *QFFunctionReferenceTool::getHelpModel()
{
    return &helpModel;
}

QCompleter *QFFunctionReferenceTool::getCompleter() const
{
    return compExpression;
}

QStringList QFFunctionReferenceTool::getDefaultWords() const
{
    return defaultWords;
}

void QFFunctionReferenceTool::setDefaultWords(const QStringList &words)
{
    defaultWords=words;
    defaultWords.sort();
    updateHelpModel();
}

void QFFunctionReferenceTool::addDefaultWords(const QStringList &words)
{
    defaultWords<<words;
    defaultWords.sort();
    updateHelpModel();
}

void QFFunctionReferenceTool::setDefaultWordsMathExpression()
{
    defaultWords.clear();
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

    defaultWords.sort();

    updateHelpModel();
}

void QFFunctionReferenceTool::setDefaultHelp(const QString &defaultHelp)
{
    this->defaultHelp=defaultHelp;
}

void QFFunctionReferenceTool::setLabProgress(QLabel *labProgress)
{
    this->labProgress=labProgress;
}

void QFFunctionReferenceTool::setProgress(QProgressBar *progress)
{
    this->progress=progress;
}

void QFFunctionReferenceTool::setProgress(QModernProgressWidget *progress)
{
    this->mprogress=progress;
}

void QFFunctionReferenceTool::setLabHelp(QLabel *labHelp)
{
    this->labHelp=labHelp;
}

void QFFunctionReferenceTool::setLabTemplate(QLabel *labTemplate)
{
    this->labTemplate=labTemplate;
}

void QFFunctionReferenceTool::registerEditor(QLineEdit *edtFormula)
{
    connect(edtFormula, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(onCursorPositionChangedDelayed(int,int)));
    edtFormula->setCompleter(compExpression);
    QFEnhancedLineEdit* ed=qobject_cast<QFEnhancedLineEdit*>(edtFormula);
    if (ed) {
        connect(ed, SIGNAL(helpKeyPressed(int)), this, SLOT(showCurrentFunctionHelp()));
    }

    edtFormula->addAction(actDefaultHelp);
    edtFormula->addAction(actCurrentFunctionHelp);
}

void QFFunctionReferenceTool::registerEditor(QPlainTextEdit *edtFormula)
{
    connect(edtFormula, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChangedDelayed()));

    QFEnhancedPlainTextEdit* ed=qobject_cast<QFEnhancedPlainTextEdit*>(edtFormula);
    if (ed) {
        ed->setCompleter(compExpression);
        connect(ed, SIGNAL(helpKeyPressed()), this, SLOT(showCurrentFunctionHelp()));
    }

    //edtFormula->setCompleter(compExpression);
    edtFormula->addAction(actDefaultHelp);
    edtFormula->addAction(actCurrentFunctionHelp);
}

bool QFFunctionReferenceTool::hasFunction(const QString name)
{
    return functionhelp.contains(name.toLower());
}

void QFFunctionReferenceTool::setCurrentHelpButton(QAbstractButton *button)
{
    btnCurrentHelp=button;
    if (btnCurrentHelp) {
        connect(btnCurrentHelp, SIGNAL(clicked()), this, SLOT(showCurrentFunctionHelp()));
        if (currentFunction.isEmpty()) {
            btnCurrentHelp->setEnabled(false);
            btnCurrentHelp->setToolTip(tr("no function selected ..."));
        } else {
            btnCurrentHelp->setEnabled(true);
            btnCurrentHelp->setToolTip(tr("show help for \"%1\"").arg(currentFunction));
        }
    }
}

void QFFunctionReferenceTool::showFunctionHelp(QString name)
{
    QString help=defaultHelp;
    if (functionhelp.contains(name)) {
        help=functionhelp[name.toLower()].fhelplink;
    }
    if (!help.isEmpty()) {
        QFPluginServices::getInstance()->displayHelpWindow(help);
    }
}

void QFFunctionReferenceTool::setCompleterFile(const QString filename)
{
    QDir().mkpath(QFileInfo(filename).absolutePath());
    compExpression->setFilename(filename);

    updateHelpModel();
}

QString QFFunctionReferenceTool::getFunctionTemplate(QString name)
{
    if (functionhelp.contains(name)) {
        return functionhelp[name.toLower()].ftemplate;
    } else {
        return "";
    }

}

QString QFFunctionReferenceTool::getFunctionHelp(QString name)
{
    QString html="";
    if (functionhelp.contains(name)) {
        QString hlp=functionhelp[name.toLower()].fhelp;
        hlp=hlp.remove("$$funcref_start$$");
        hlp=hlp.remove("$$funcref_description$$");
        hlp=hlp.remove("$$funcref_end$$");
        return transformQF3HelpHTML(hlp, functionhelp[name.toLower()].fhelpfile);
    }

    return QString();

}

void QFFunctionReferenceTool::startSearch(const QStringList &directories_in, const QStringList &filters, const QString &globalConfigVar)
{
    QStringList directories=directories_in;

    QStringList refDirs=QFPluginServices::getInstance()->getGlobalConfigValue(globalConfigVar).toStringList();
    for (int i=0; i<refDirs.size(); i++) {
        if (!directories.contains(refDirs[i])) directories.append(refDirs[i]);
    }

    QStringList files;
    for (int dd=0; dd<directories.size(); dd++) {
        QDir d(directories[dd]);
        QStringList ff=d.entryList(filters, QDir::Files);
        for (int j=0; j<ff.size(); j++) {
            files.append(d.absoluteFilePath(ff[j]));
        }
    }

    QList<QStringList> absFiles;

    for (int i=0; i<maxThreads; i++) {
        QStringList sl;
        absFiles.push_back(sl);
    }
    for (int i=0; i<files.size(); i++) {
        QString file=files[i];
        absFiles[i%maxThreads].append(file);
    }
    for (int i=0; i<maxThreads; i++) {
        threads.append(new QFFunctionReferenceToolDocSearchThread(absFiles[i], this));
        threads[i]->setRxDefinition(rxDefinition);
        threads[i]->setRxTemplate(rxTemplate);
        //qDebug()<<"creating thread "<<absFiles[i]<<rxDefinition<<rxTemplate;
        connect(threads[i], SIGNAL(finished()), this, SLOT(threadFinished()));
        //connect(threads[i], SIGNAL(foundFunction(QString,QString,QString,QString)), this, SLOT(addFunction(QString,QString,QString,QString)));
        connect(threads[i], SIGNAL(foundFunction(QString,QStringList,QString,QString)), this, SLOT(addFunction(QString,QStringList,QString,QString)));
    }
    delayedStartSearchThreads();
}

void QFFunctionReferenceTool::startSearch(const QStringList &directories, const QString& globalConfigVar)
{
    QStringList filter;
    filter<<"*.html"<<"*.htm"<<"*.txt";
    startSearch(directories, filter, globalConfigVar);
}

void QFFunctionReferenceTool::startSearch(const QString &directory, const QString& globalConfigVar)
{
    startSearch(QStringList(directory), globalConfigVar);
}

void QFFunctionReferenceTool::showCurrentFunctionHelp()
{
    showFunctionHelp(currentFunction);
}

void QFFunctionReferenceTool::showDefaultHelp()
{
    QFPluginServices::getInstance()->displayHelpWindow(defaultHelp);
}


void QFFunctionReferenceTool::onCursorPositionChanged()
{
    int old=last_old;
    int newPosIn=last_newPos;

    //qDebug()<<"onCursorPositionChanged("<<last_old<<last_newPos<<") from "<<last_sender;

    QString text="";
    int newPos=-1;

    QLineEdit* edit=qobject_cast<QLineEdit*>(last_sender);
    if (edit) {
        text=edit->text();
        newPos=newPosIn;
    }
    QPlainTextEdit* pte=qobject_cast<QPlainTextEdit*>(last_sender);
    if (pte) {
        //qDebug()<<pte->textCursor().blockNumber()<<pte->textCursor().positionInBlock();
        text=pte->toPlainText();
        text=text.replace("\n\r", "\n");
        text=text.replace("\r\n", "\n");
        QStringList sl=text.split("\n");
        newPos=0;
        for (int i=0; i<pte->textCursor().blockNumber(); i++) {
            newPos+=sl.value(i, "").size()+1;
        }
        newPos=newPos+pte->textCursor().positionInBlock();
    }

    if (text.size()>0) {
        if (newPos>=0 && newPos<text.size()) {
            while (!text[newPos].isLetter() && newPos>0) {
                newPos--;
            }
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
                currentFunction=word;
                if (labHelp) labHelp->setText(getFunctionHelp(word));
                if (labTemplate) labTemplate->setText(tr("<tt>%1</tt>").arg(getFunctionTemplate(word)));
            } else {
                currentFunction="";
            }
        }
        if (btnCurrentHelp) {
            if (currentFunction.isEmpty()) {
                btnCurrentHelp->setEnabled(false);
                btnCurrentHelp->setToolTip(tr("no function selected ..."));
                actCurrentFunctionHelp->setText(tr("help on function under cursor"));
            } else {
                btnCurrentHelp->setEnabled(true);
                btnCurrentHelp->setToolTip(tr("show help for \"%1\"").arg(currentFunction));
                actCurrentFunctionHelp->setText(tr("help on \"%1\"").arg(currentFunction));
            }
        }

    }
}

void QFFunctionReferenceTool::onCursorPositionChangedDelayed(int old, int newPos)
{
    last_sender=sender();
    last_old=old;
    last_newPos=newPos;

    //qDebug()<<"onCursorPositionChangedDelayed("<<old<<newPos<<") from "<<sender();
    timDelayedCursorChange.setSingleShot(true);
    timDelayedCursorChange.setInterval(50);
    timDelayedCursorChange.start();
}

