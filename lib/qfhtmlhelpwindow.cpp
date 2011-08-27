#include "qfhtmlhelpwindow.h"

#include <QtGui>
#include <iostream>
#include <QDir>
#include <QDesktopServices>

QString removeHTMLComments(const QString& data) {
     QRegExp rxComments("<!--(.*)-->", Qt::CaseInsensitive);
     rxComments.setMinimal(true);
     QString data1=data;
     data1.remove(rxComments);
     return data1;
}

QFHTMLHelpWindow::QFHTMLHelpWindow(QWidget* parent):
    QWidget(parent)
{
    searchPath=QApplication::applicationDirPath();
    replaces=NULL;
    m_baseFont=font();
    m_titleFont=font();
    m_titleFont.setPointSizeF(m_titleFont.pointSizeF()*2);
    m_titleFont.setBold(true);
    m_home="";
    history_idx=-1;

    labelTitle=new QLabel(this);
    descriptionBrowser=new QTextBrowser(this);
    connect(descriptionBrowser, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(anchorClicked(const QUrl&)));

    QVBoxLayout* layout=new QVBoxLayout;//(this);

    QHBoxLayout* layButtons=new QHBoxLayout;//(this);

    btnPrevious=new QPushButton(QIcon(":/lib/help_previous.png"), tr("&Previous"), this);
    //connect(descriptionBrowser, SIGNAL(backwardAvailable(bool)), btnPrevious, SLOT(setEnabled(bool)));
    //connect(btnPrevious, SIGNAL(clicked()), descriptionBrowser, SLOT(backward()));
    connect(btnPrevious, SIGNAL(clicked()), this, SLOT(previous()));
    layButtons->addWidget(btnPrevious);
    btnHome=new QPushButton(QIcon(":/lib/help_home.png"), tr("&Home"), this);
    layButtons->addWidget(btnHome);
    //connect(btnHome, SIGNAL(clicked()), descriptionBrowser, SLOT(home()));
    connect(btnHome, SIGNAL(clicked()), this, SLOT(home()));
    btnNext=new QPushButton(QIcon(":/lib/help_next.png"), tr("&Next"), this);
    //connect(descriptionBrowser, SIGNAL(forwardAvailable(bool)), btnNext, SLOT(setEnabled(bool)));
    //connect(btnNext, SIGNAL(clicked()), descriptionBrowser, SLOT(forward()));
    connect(btnNext, SIGNAL(clicked()), this, SLOT(next()));
    layButtons->addWidget(btnNext);

    btnPrint=new QPushButton(QIcon(":/lib/print.png"), tr("&Print this help page"), this);
    layButtons->addWidget(btnPrint);
    connect(btnPrint, SIGNAL(clicked()), this, SLOT(print()));


    layButtons->addStretch();

    layout->addLayout(layButtons);
    layout->addWidget(labelTitle);
    layout->setStretchFactor(labelTitle, 0);
    layout->addWidget(descriptionBrowser);
    layout->setStretchFactor(descriptionBrowser, 6);
    setLayout(layout);

    Qt::WindowFlags flags = Qt::Tool | Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);
}

void QFHTMLHelpWindow::readSettings(QSettings& settings, QString prefix) {
     resize(settings.value(prefix+"htmlhelpwin.size", QSize(400, 400)).toSize());
     move(settings.value(prefix+"htmlhelpwin.pos", QPoint(200, 200)).toPoint());
}

void QFHTMLHelpWindow::writeSettings(QSettings& settings, QString prefix) {
     settings.setValue(prefix+"htmlhelpwin.size", size());
     settings.setValue(prefix+"htmlhelpwin.pos", pos());
}

void QFHTMLHelpWindow::updateHelp(QString title, QString filename) {
    m_home=filename;
    searchPath=QFileInfo(filename).canonicalPath();
    //std::cout<<"updateHelp("<<filename.toStdString()<<")   sp="<<searchPath.toStdString()<<"  src="<<QFileInfo(filename).fileName().toStdString()<<"\n";
    disconnect(descriptionBrowser, SIGNAL(textChanged()), this, SLOT(displayTitle()));
    labelTitle->setFont(m_titleFont);
    labelTitle->setText(title);
    labelTitle->setVisible(!title.isEmpty());
    descriptionBrowser->setOpenLinks(false);
    descriptionBrowser->setOpenExternalLinks(true);
    descriptionBrowser->setSearchPaths(QStringList(searchPath));
    descriptionBrowser->setFrameShape(QFrame::Box);
    descriptionBrowser->setReadOnly(true);
    descriptionBrowser->clearHistory();
    history.clear();
    history_idx=0;
    history.append(HistoryEntry(filename, title));
    updateButtons();
    //history.clear();
    //QStringList(QApplication::applicationDirPath()+"/models/help/"+QFileInfo(model->getFilename()).completeBaseName()+"/")
    //std::cout<<QString(QApplication::applicationDirPath()+"/models/help/"+QFileInfo(model->getFilename()).completeBaseName()+"/").toStdString()<<std::endl;
    descriptionBrowser->setHtml(loadHTML(QFileInfo(filename).absoluteFilePath()));


    //descriptionBrowser->setSource(QFileInfo(filename).fileName());
    //descriptionBrowser->reload();
    connect(descriptionBrowser, SIGNAL(textChanged()), this, SLOT(displayTitle()));
}


void QFHTMLHelpWindow::updateHelp(QString filename) {
    m_home=filename;
    searchPath=QFileInfo(filename).canonicalPath();
    //std::cout<<"updateHelp("<<filename.toStdString()<<")   sp="<<searchPath.toStdString()<<"  src="<<QFileInfo(filename).fileName().toStdString()<<"\n";
    disconnect(descriptionBrowser, SIGNAL(textChanged()), this, SLOT(displayTitle()));
    descriptionBrowser->setOpenLinks(false);
    descriptionBrowser->setOpenExternalLinks(true);
    descriptionBrowser->setSearchPaths(QStringList(searchPath));
    descriptionBrowser->setFrameShape(QFrame::Box);
    descriptionBrowser->setReadOnly(true);
    //descriptionBrowser->setSource(QFileInfo(filename).fileName());
    //descriptionBrowser->reload();
    descriptionBrowser->setHtml(loadHTML(QFileInfo(filename).absoluteFilePath()));

    descriptionBrowser->clearHistory();
    history.clear();
    history_idx=0;
    history.append(HistoryEntry(filename, descriptionBrowser->documentTitle()));
    updateButtons();

    QString title=descriptionBrowser->documentTitle();
    labelTitle->setFont(m_titleFont);
    labelTitle->setText(title);
    labelTitle->setVisible(!title.isEmpty());

    connect(descriptionBrowser, SIGNAL(textChanged()), this, SLOT(displayTitle()));
}

void QFHTMLHelpWindow::clear() {
    labelTitle->setText("");
    descriptionBrowser->clear();
    updateButtons();
}

void QFHTMLHelpWindow::displayTitle() {
    QString title=descriptionBrowser->documentTitle();
    labelTitle->setFont(m_titleFont);
    labelTitle->setText(title);
    labelTitle->setVisible(!title.isEmpty());
    updateButtons();
    //QStringList sp;
    //std::cout<<descriptionBrowser->source().toString().toStdString()<<std::endl;
    //descriptionBrowser->setSearchPaths(sp);
}

void QFHTMLHelpWindow::anchorClicked(const QUrl& link) {
    //std::cout<<"scheme = "<<link.scheme().toStdString()<<std::endl;
    //qDebug()<<link.toString();
    QString linkstr=link.toString();
    QString scheme=link.scheme().toLower();
    QDir spd(searchPath);
    QString cl=spd.cleanPath(spd.absoluteFilePath(link.toString()));
    QString s=spd.absoluteFilePath(cl); //absoluteFilePath
    searchPath=QFileInfo(s).absolutePath();

    if (linkstr.startsWith("#")) {
        descriptionBrowser->scrollToAnchor(linkstr.right(linkstr.size()-1));
    }

    if ((scheme!="http") && (scheme!="https") && (scheme!="ftp") && (scheme!="ftps") && (scheme!="mailto") && (scheme!="sftp") && (scheme!="svn") && (scheme!="ssh") && QFile::exists(QFileInfo(s).absoluteFilePath())) {
        //std::cout<<"anchorClicked("<<link.toString().toStdString()<<")   spd="<<spd.canonicalPath().toStdString()<<"   cl="<<cl.toStdString()<<"   s="<<s.toStdString()<<"   searchPath="<<searchPath.toStdString()<<"  src="<<QFileInfo(s).fileName().toStdString()<<"\n";
        descriptionBrowser->setSearchPaths(QStringList(searchPath));
        //descriptionBrowser->setSource(QFileInfo(s).fileName());
        //descriptionBrowser->reload();
        descriptionBrowser->setHtml(loadHTML(QFileInfo(s).absoluteFilePath()));
        for (int i=history.size()-1; i>=qMax(0,history_idx)+1; i--) {
            history.pop();
        }
        history.push(HistoryEntry(s, descriptionBrowser->documentTitle()));
        history_idx=history.size()-1;
        updateButtons();
    } else {
        QDesktopServices::openUrl(link);
    }
}

void QFHTMLHelpWindow::showFile(QString filename) {
    QDir spd(filename);
    QString cl=spd.cleanPath(spd.absoluteFilePath(filename));
    QString s=spd.absoluteFilePath(cl); //absoluteFilePath
    searchPath=QFileInfo(s).absolutePath();

    //std::cout<<"showFile("<<filename.toStdString()<<")   spd="<<spd.canonicalPath().toStdString()<<"   cl="<<cl.toStdString()<<"   s="<<s.toStdString()<<"   searchPath="<<searchPath.toStdString()<<"  src="<<QFileInfo(s).fileName().toStdString()<<"\n";
    descriptionBrowser->setSearchPaths(QStringList(searchPath));
    //descriptionBrowser->setSource(QFileInfo(s).fileName());
    //descriptionBrowser->reload();
    descriptionBrowser->setHtml(loadHTML(QFileInfo(filename).absoluteFilePath()));
}

void QFHTMLHelpWindow::previous() {
    if (history_idx>=1) {
        history_idx--;
        showFile(history[history_idx].url);
        updateButtons();
    }
}

void QFHTMLHelpWindow::next() {
    if (history_idx<history.size()-1) {
        history_idx++;
        showFile(history[history_idx].url);
        updateButtons();
    }
}

void QFHTMLHelpWindow::home() {
    showFile(m_home);
    history.push(HistoryEntry(m_home, descriptionBrowser->documentTitle()));
    history_idx=history.size()-1;
    updateButtons();
}

void QFHTMLHelpWindow::print() {
    QPrinter* p=new QPrinter;

    // select a printer

    QPrintDialog *dialog = new QPrintDialog(p, NULL);
    dialog->setWindowTitle(tr("Print Help Page"));
    if (dialog->exec() != QDialog::Accepted) {
        delete p;
        return;
    }
    descriptionBrowser->print(p);
    delete p;
}



QString QFHTMLHelpWindow::loadHTML(QString filename) {
    QString result;
    // read HTML file
    QFile file(QFileInfo(filename).absoluteFilePath());
    fromHTML_replaces.clear();
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        result=removeHTMLComments(in.readAll());
    } else {
        result=tr("<html><header><title>Error: Help page does not exist</title></header><body>$$qf_commondoc_header.start$$ $$qf_commondoc_header.simplest$$ $$qf_commondoc_header.end$$<center>The Quickfit online-help page you are trying to access does not exist!<br><br>File was: <i>%1</i></center></body></html>").arg(filename);
    }
    // find special information in file
    QRegExp rxTitle("<title>(.*)</title>", Qt::CaseInsensitive);
    if (rxTitle.indexIn(result) != -1) fromHTML_replaces.append(qMakePair(QString("title"), rxTitle.cap(1)));

    QRegExp rxMeta("(<meta\\s*content\\s*=\\s*\"([^\"']*)\"\\s*name\\s*=\\s*\"([^\"']*)\"[^>]*>|<meta\\s*name\\s*=\\s*\"([^\"']*)\"\\s*content\\s*=\\s*\"([^\"']*)\"[^>]*>)", Qt::CaseInsensitive);
    rxMeta.setMinimal(false);
    int count = 0;
    int pos = 0;
    while ((pos = rxMeta.indexIn(result, pos)) != -1) {

        QString name=rxMeta.cap(2);
        QString content=rxMeta.cap(1);
        if (name.isEmpty() && content.isEmpty()) {
            name=rxMeta.cap(3);
            content=rxMeta.cap(4);
        }
        if ((!name.isEmpty())&&(!content.isEmpty())) {
            fromHTML_replaces.append(qMakePair(QString("meta_")+name,content));
        }

        ++count;
        pos += rxMeta.matchedLength();
    }

    QRegExp rxLink("<link\\s*rel\\s*=\\s*\"([^\"']*)\"[^\\>]*href\\s*=\\s*\"([^\"']*)\"[^>]*>", Qt::CaseInsensitive);
    rxLink.setMinimal(false);
    count = 0;
    pos = 0;
    while ((pos = rxLink.indexIn(result, pos)) != -1) {

        QString rel=rxLink.cap(1).toLower();
        QString href=rxLink.cap(2);
        if (!href.isEmpty()) {
            if (rel=="contents") fromHTML_replaces.append(qMakePair(QString("rel_contents"), tr("<a href=\"%1\">Contents</a> ").arg(href)));
            if (rel=="index") fromHTML_replaces.append(qMakePair(QString("rel_index"), tr("<a href=\"%1\">Index</a> ").arg(href)));
            if (rel=="copyright") fromHTML_replaces.append(qMakePair(QString("rel_copyright"), tr("<a href=\"%1\">Copyright</a> ").arg(href)));
            if (rel=="top") fromHTML_replaces.append(qMakePair(QString("rel_top"), tr("<a href=\"%1\">Top</a> ").arg(href)));
            if (rel=="prev") fromHTML_replaces.append(qMakePair(QString("rel_prev"), tr("<a href=\"%1\"><img src=\":/lib/help/help_prev.png\" border=\"0\"></a><!--&nbsp;<a href=\"%1\">Previous</a>--> ").arg(href)));
            if (rel=="next") fromHTML_replaces.append(qMakePair(QString("rel_next"), tr("<a href=\"%1\"><img src=\":/lib/help/help_next.png\" border=\"0\"></a><!--&nbsp;<a href=\"%1\">Next</a>--> ").arg(href)));
            if (rel=="up") fromHTML_replaces.append(qMakePair(QString("rel_up"), tr("<a href=\"%1\"><img src=\":/lib/help/help_up.png\" border=\"0\"></a><!--&nbsp;<a href=\"%1\">Up</a>--> ").arg(href)));
            //if (rel=="prev") fromHTML_replaces.append(qMakePair(QString("rel_prev"), tr("<a href=\"%1\"><img src=\":/lib/help/help_prev.png\" border=\"0\"></a>&nbsp;<a href=\"%1\">Previous</a> ").arg(href)));
        }

        ++count;
        pos += rxLink.matchedLength();
    }

    QString basepath=QFileInfo(filename).absolutePath();
    QDir basedir=QFileInfo(filename).absoluteDir();
    if (basedir.exists("tutorial.html")) {
        if (QFileInfo(filename).fileName()=="tutorial.html") {
            fromHTML_replaces.append(qMakePair(QString("rel_tutorial"), tr("<b><img src=\":/lib/help/help_tutorial.png\" border=\"0\">&nbsp;Tutorial</b> ")));
        } else {
            fromHTML_replaces.append(qMakePair(QString("rel_tutorial"), tr("<a href=\"%1\"><img src=\":/lib/help/help_tutorial.png\" border=\"0\"></a>&nbsp;<a href=\"%1\">Tutorial</a> ").arg(basedir.absoluteFilePath("tutorial.html"))));
        }
    } else {
        fromHTML_replaces.append(qMakePair(QString("rel_tutorial"), QString(" ")));
    }

    // handle replaces
    if (!result.isEmpty()) {
        bool replaced=true;
        int cnt=0;
        while (replaced && (cnt<15)) {
            replaced=false;
            if (replaces) for (int i=0; i<replaces->size(); i++) {
                //std::cout<<"replace \""<<(QString("$$")+(*replaces)[i].first).toStdString()<<"\" by \""<<(*replaces)[i].second.toStdString()<<"\"\n";
                if (result.contains(QString("$$")+(*replaces)[i].first+QString("$$"))) replaced=true;
                result=result.replace(QString("$$")+(*replaces)[i].first+QString("$$"), (*replaces)[i].second);
            }
            for (int i=0; i<internal_replaces.size(); i++) {
                //std::cout<<"replace \""<<(QString("$$")+internal_replaces[i].first).toStdString()<<"\" by \""<<(*replaces)[i].second.toStdString()<<"\"\n";
                if (result.contains(QString("$$")+internal_replaces[i].first+QString("$$"))) replaced=true;
                result=result.replace(QString("$$")+internal_replaces[i].first+QString("$$"), internal_replaces[i].second);
            }
            for (int i=0; i<fromHTML_replaces.size(); i++) {
                //std::cout<<"replace \""<<(QString("$$")+internal_replaces[i].first).toStdString()<<"\" by \""<<(*replaces)[i].second.toStdString()<<"\"\n";
                if (result.contains(QString("$$")+fromHTML_replaces[i].first+QString("$$"))) replaced=true;
                result=result.replace(QString("$$")+fromHTML_replaces[i].first+QString("$$"), fromHTML_replaces[i].second);
            }
            cnt++;
        }

        // remove all unreplaces $$name$$ sequences
        QRegExp rxSpecials("\\$\\$.+\\$\\$");
        rxSpecials.setMinimal(true);
        result=result.remove(rxSpecials);
    }
    return result;
}

void QFHTMLHelpWindow::setHtmlReplacementList(QList<QPair<QString, QString> >* list) {
    replaces=list;
}

void QFHTMLHelpWindow::updateButtons() {
    //std::cout<<"updateButtons():  history_idx="<<history_idx<<"   history.size()="<<history.size()<<std::endl;
    if(history_idx<history.size()-1) {
        btnNext->setEnabled(true);
        btnNext->setWhatsThis("");
        btnNext->setToolTip("");
        if (history_idx>=0) {
            btnNext->setWhatsThis(tr("move on to help page: \"%1\"").arg(history[history_idx+1].name));
            btnNext->setToolTip(tr("move on to help page: \"%1\"").arg(history[history_idx+1].name));
        }
    } else {
        btnNext->setEnabled(false);
        btnNext->setWhatsThis("");
        btnNext->setToolTip("");
    }

    if(history_idx>0) {
        btnPrevious->setEnabled(true);
        btnPrevious->setWhatsThis("");
        btnPrevious->setToolTip("");
        if (history_idx<history.size()) {
            btnPrevious->setWhatsThis(tr("move back to help page: \"%1\"").arg(history[history_idx-1].name));
            btnPrevious->setToolTip(tr("move back to help page: \"%1\"").arg(history[history_idx-1].name));
        }
    } else {
        btnPrevious->setEnabled(false);
        btnPrevious->setWhatsThis("");
        btnPrevious->setToolTip("");
    }
}
