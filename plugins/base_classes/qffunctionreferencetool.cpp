#include "qffunctionreferencetool.h"
#include "qfpluginservices.h"
#include "qfhtmlhelptools.h"

QFFunctionReferenceToolDocSearchThread::QFFunctionReferenceToolDocSearchThread(QStringList files, QObject *parent):
    QThread(parent)
{
    this->files=files;

    stopped=false;
    rxDefinition=QRegExp("<!--\\s*func:([\\w]*)\\s*-->(.*)<!--\\s*/func:\\1\\s*-->");
    rxTemplate=QRegExp("<!--\\s*template\\s*-->(.*)<!--\\s*/template\\s*-->");
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
    for (int i=0; i<files.size(); i++) {
        QFile f(files[i]);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString contents=f.readAll();
            f.close();
            int pos=0;
            while ((pos = rxDefinition.indexIn(contents, pos)) != -1) {
                QString name=rxDefinition.cap(1).trimmed();
                QString help=rxDefinition.cap(2).trimmed();
                QString templ="";
                if (rxTemplate.indexIn(help,0)!=-1) {
                    templ=rxTemplate.cap(1);
                }
                emit foundFunction(name, templ, help, QFileInfo(files[i]).absoluteFilePath()+QString("#")+name);
                pos += rxDefinition.matchedLength();
                if (stopped) break;
             }
        }
        if (stopped) break;
    }
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
    defaultHelp="";
    rxDefinition=QRegExp("<!--\\s*func:([\\w]*)\\s*-->(.*)<!--\\s*/func:\\1\\s*-->");
    rxTemplate=QRegExp("<!--\\s*template\\s*-->(.*)<!--\\s*/template\\s*-->");
    btnCurrentHelp=NULL;
    labHelp=NULL;
    labTemplate=NULL;
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
    QFFunctionReferenceTool::functionDescription fd;
    fd.fhelp=help;
    fd.ftemplate=templ;
    fd.fhelplink=helplink;
    QUrl url(helplink);
    fd.fhelpfile=url.toString(QUrl::RemoveFragment);
    //qDebug()<<"function help "<<helplink;
    //qDebug()<<"           -> "<<fd.fhelpfile;
    functionhelp[name.toLower()]=fd;
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

void QFFunctionReferenceTool::stopThreadsWait()
{
    if (searching) {
        for (int i=0; i<threads.size(); i++) {
            if (threads[i]->isRunning()) {
                threads[i]->stopThread();
                threads[i]->wait();
            }
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
    connect(edtFormula, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(onCursorPositionChanged(int,int)));
    edtFormula->setCompleter(compExpression);
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
        return transformQF3HelpHTML(functionhelp[name.toLower()].fhelp, functionhelp[name.toLower()].fhelpfile);
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
        connect(threads[i], SIGNAL(finished()), this, SLOT(threadFinished()));
        connect(threads[i], SIGNAL(foundFunction(QString,QString,QString,QString)), this, SLOT(addFunction(QString,QString,QString,QString)));
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


void QFFunctionReferenceTool::onCursorPositionChanged(int /*old*/, int newPos)
{
    QString text="";

    QLineEdit* edit=qobject_cast<QLineEdit*>(sender());
    if (edit) text=edit->text();

    if (text.size()>0) {
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
            } else {
                btnCurrentHelp->setEnabled(true);
                btnCurrentHelp->setToolTip(tr("show help for \"%1\"").arg(currentFunction));
            }
        }

    }
}
