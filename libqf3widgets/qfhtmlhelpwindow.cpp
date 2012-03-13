#include "qfhtmlhelpwindow.h"

#include <QtGui>
#include <iostream>
#include <QDir>
#include <QDesktopServices>
#include "qfpluginservices.h"
#include "qffitalgorithmmanager.h"
#include "qffitfunctionmanager.h"
#include "qfextensionmanager.h"
#include "qffitalgorithm.h"
#include "qffitfunction.h"
#include "jkqtmathtext.h"

QString removeHTMLComments(const QString& data) {
     QRegExp rxComments("<!--(.*)-->", Qt::CaseInsensitive);
     rxComments.setMinimal(true);
     QString data1=data;
     data1.remove(rxComments);
     return data1;
}

QFHTMLHelpWindow::QFHTMLHelpWindow(QWidget* parent, Qt::WindowFlags flags):
    QWidget(parent, flags)
{
    m_pluginServices=NULL;
    searchPath="";
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

    setWindowFlags(flags);
    setWindowTitle(tr("QuickFit Online-Help"));
    setWindowIcon(QIcon(":/lib/help.png"));
}

void QFHTMLHelpWindow::readSettings(QSettings& settings, QString prefix) {
     resize(settings.value(prefix+"htmlhelpwin.size", QSize(400, 400)).toSize());
     move(settings.value(prefix+"htmlhelpwin.pos", QPoint(200, 200)).toPoint());
}

void QFHTMLHelpWindow::writeSettings(QSettings& settings, QString prefix) {
     settings.setValue(prefix+"htmlhelpwin.size", size());
     settings.setValue(prefix+"htmlhelpwin.pos", pos());
}

void QFHTMLHelpWindow::updateHelp(QString title, QString filename1) {
    QString filename=filename1;
    QString fragment="";
    int hashpos=filename1.indexOf("#");
    if (hashpos>=0) {
        filename=filename1.left(hashpos);
        fragment=filename1.mid(hashpos);
    }
    //qDebug()<<filename<<fragment;

    m_home=filename;
    searchPath=QFileInfo(filename).canonicalPath();
    //std::cout<<"updateHelp("<<filename.toStdString()<<")   sp="<<searchPath.toStdString()<<"  src="<<QFileInfo(filename).fileName().toStdString()<<"\n";
    disconnect(descriptionBrowser, SIGNAL(textChanged()), this, SLOT(displayTitle()));
    labelTitle->setFont(m_titleFont);
    labelTitle->setText(title);
    labelTitle->setVisible(!title.isEmpty());
    descriptionBrowser->setOpenLinks(false);
    descriptionBrowser->setOpenExternalLinks(true);
    descriptionBrowser->setSearchPaths(QStringList(searchPath)<<"./");
    descriptionBrowser->setFrameShape(QFrame::Box);
    descriptionBrowser->setReadOnly(true);
    descriptionBrowser->clearHistory();
    history.clear();
    history_idx=0;
    history.append(HistoryEntry(filename, title));
    updateButtons();
    //history.clear();
    descriptionBrowser->setHtml(loadHTML(QFileInfo(filename).absoluteFilePath()));
    if (!fragment.isEmpty()) {
        descriptionBrowser->scrollToAnchor(fragment);
    }


    //descriptionBrowser->setSource(QFileInfo(filename).fileName());
    //descriptionBrowser->reload();
    connect(descriptionBrowser, SIGNAL(textChanged()), this, SLOT(displayTitle()));
}


void QFHTMLHelpWindow::updateHelp(QString filename1) {
    QString filename=filename1;
    QString fragment="";
    int hashpos=filename1.indexOf("#");
    if (hashpos>=0) {
        filename=filename1.left(hashpos);
        fragment=filename1.mid(hashpos);
    }
    //qDebug()<<filename<<fragment;

    m_home=filename;
    searchPath=QFileInfo(filename).canonicalPath();
    //qDebug()<<searchPath;
    //std::cout<<"updateHelp("<<filename.toStdString()<<")   sp="<<searchPath.toStdString()<<"  src="<<QFileInfo(filename).fileName().toStdString()<<"\n";
    disconnect(descriptionBrowser, SIGNAL(textChanged()), this, SLOT(displayTitle()));
    descriptionBrowser->setOpenLinks(false);
    descriptionBrowser->setOpenExternalLinks(true);
    descriptionBrowser->setSearchPaths(QStringList(searchPath)<<"./");
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
    if (!fragment.isEmpty()) {
        descriptionBrowser->scrollToAnchor(QString("#")+fragment);
    }

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

    if (linkstr.startsWith("#")) {
        descriptionBrowser->scrollToAnchor(linkstr.right(linkstr.size()-1));
        return;
    }

    if ((scheme!="http") && (scheme!="https") && (scheme!="ftp") && (scheme!="ftps") && (scheme!="mailto") && (scheme!="sftp") && (scheme!="svn") && (scheme!="ssh") /*&& QFile::exists(QFileInfo(s).absoluteFilePath())*/) {

        QDir spd(searchPath);
        QString filename=link.toString(QUrl::RemoveFragment);
        QString fragment=link.fragment();
        QString cl=spd.cleanPath(spd.absoluteFilePath(filename));
        QString s=spd.absoluteFilePath(cl); //absoluteFilePath
        searchPath=QFileInfo(s).absolutePath();
        //qDebug()<<QFileInfo(s).absoluteFilePath()<<searchPath;
        //qDebug()<<filename<<fragment;

        //std::cout<<"anchorClicked("<<link.toString().toStdString()<<")   spd="<<spd.canonicalPath().toStdString()<<"   cl="<<cl.toStdString()<<"   s="<<s.toStdString()<<"   searchPath="<<searchPath.toStdString()<<"  src="<<QFileInfo(s).fileName().toStdString()<<"\n";
        descriptionBrowser->setSearchPaths(QStringList(searchPath)<<"./");
        //qDebug()<<QFileInfo(s).absoluteFilePath()<<searchPath;
        //descriptionBrowser->setSource(QFileInfo(s).fileName());
        //descriptionBrowser->reload();
        descriptionBrowser->setHtml(loadHTML(QFileInfo(s).absoluteFilePath()));
        for (int i=history.size()-1; i>=qMax(0,history_idx)+1; i--) {
            history.pop();
        }
        history.push(HistoryEntry(s, descriptionBrowser->documentTitle()));
        history_idx=history.size()-1;
        updateButtons();
        if (link.hasFragment()) {
            descriptionBrowser->scrollToAnchor(QString("#")+fragment);
        }
    } else {
        QDesktopServices::openUrl(link);
    }
}

void QFHTMLHelpWindow::showFile(QString filename1) {
    QString filename=filename1;
    QString fragment="";
    int hashpos=filename1.indexOf("#");
    if (hashpos>=0) {
        filename=filename1.left(hashpos);
        fragment=filename1.mid(hashpos);
    }
    //qDebug()<<filename<<fragment;

    QDir spd(filename);
    QString cl=spd.cleanPath(spd.absoluteFilePath(filename));
    QString s=spd.absoluteFilePath(cl); //absoluteFilePath
    searchPath=QFileInfo(s).absolutePath();
    //qDebug()<<QFileInfo(filename).absoluteFilePath()<<searchPath;

    //std::cout<<"showFile("<<filename.toStdString()<<")   spd="<<spd.canonicalPath().toStdString()<<"   cl="<<cl.toStdString()<<"   s="<<s.toStdString()<<"   searchPath="<<searchPath.toStdString()<<"  src="<<QFileInfo(s).fileName().toStdString()<<"\n";
    descriptionBrowser->setSearchPaths(QStringList(searchPath)<<"./");
    //descriptionBrowser->setSource(QFileInfo(s).fileName());
    //descriptionBrowser->reload();
    descriptionBrowser->setHtml(loadHTML(QFileInfo(filename).absoluteFilePath()));
    if (!fragment.isEmpty()) {
        descriptionBrowser->scrollToAnchor(QString("#")+fragment);
    }
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

    QPrintDialog *dialog = new QPrintDialog(p, this);
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

    QFontDatabase fontdb;
    QStringList fonts=fontdb.families();

    JKQTmathText mathParser(this);
    mathParser.set_fontSize(12);
    if (fonts.contains("Times New Roman")) {
        //qDebug()<<"using Times New Roman";
        mathParser.set_fontRoman("Times New Roman");
        mathParser.set_fontMathRoman("Times New Roman");
    }
    if (fonts.contains("Arial")) {
        //qDebug()<<"using Arial";
        mathParser.set_fontSans("Arial");
        mathParser.set_fontMathSans("Arial");
    } else if (fonts.contains("Helvetica")) {
        //qDebug()<<"using Helvetica";
        mathParser.set_fontSans("Helvetica");
        mathParser.set_fontMathSans("Helvetica");
    }
    if (!fonts.contains("Symbol") && !fonts.contains("Standard Symbols L")) {
        //qDebug()<<"no Symbol";
        mathParser.useAnyUnicode(mathParser.get_fontRoman(), mathParser.get_fontSans());
    } else  {
        //qDebug()<<"no Symbol";
        //mathParser.useAnyUnicode(mathParser.get_fontRoman(), mathParser.get_fontSans());
    }
    if (fonts.contains("Courier")) {
        //qDebug()<<"using Courier";
        mathParser.set_fontTypewriter("Courier");
    }
    //qDebug()<<fonts;

    // read HTML file
    QFile file(QFileInfo(filename).absoluteFilePath());
    QString fileDir=QFileInfo(filename).absolutePath();
    if (!fileDir.endsWith("/")) fileDir.append("/");
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
            if (rel=="contents") fromHTML_replaces.append(qMakePair(QString("rel_contents"), tr("<a href=\"%1\"><img src=\":/lib/help/help_contents.png\" border=\"0\"><!--Contents--></a> ").arg(href)));
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

    // collectspecial replaces based on current directory
    bool foundPlugin=false;
    if (pluginList) {
        for (int i=0; i<pluginList->size(); i++) {
            if (QDir(pluginList->at(i).directory)==basepath) { // we found the info for this directory
                QString pid=pluginList->at(i).plugin->getID();
                fromHTML_replaces.append(qMakePair(QString("local_plugin_icon"), QString("<img src=\"%1\">").arg(pluginList->at(i).plugin->getIconFilename())));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_iconfilename"), pluginList->at(i).plugin->getIconFilename()));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_name"), pluginList->at(i).plugin->getName()));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_author"), pluginList->at(i).plugin->getAuthor()));
                if (basedir.exists("copyright.html")) {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_copyright"), QString("<a href=\"copyright.html\">%1</a>").arg(pluginList->at(i).plugin->getCopyright())));
                } else {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_copyright"), pluginList->at(i).plugin->getCopyright()));
                }

                fromHTML_replaces.append(qMakePair(QString("local_plugin_weblink_url"), pluginList->at(i).plugin->getWeblink()));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_weblink"), tr("<a href=\"%1\">Plugin Webpage</a>").arg(pluginList->at(i).plugin->getWeblink())));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_id"), pid));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_dllbasename"), pluginList->at(i).pluginDLLbasename));
                fromHTML_replaces.append(qMakePair(QString("local_plugin_dllsuffix"), pluginList->at(i).pluginDLLSuffix));
                if (m_pluginServices) {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_assets"), m_pluginServices->getAssetsDirectory()+"/plugins/"+pluginList->at(i).pluginDLLbasename+"/"));
                }
                if (!pluginList->at(i).tutorial.isEmpty()) {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_tutorial_file"), pluginList->at(i).tutorial));
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_tutorial_link"), tr("$$qf_commondoc_header.separator$$  <a href=\"%1\"><b>Plugin Tutorial</b></a>").arg(pluginList->at(i).tutorial)));
                }
                if (!pluginList->at(i).mainhelp.isEmpty()) {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_mainhelp_file"), pluginList->at(i).mainhelp));
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_mainhelp_link"), tr("$$qf_commondoc_header.separator$$  <a href=\"%1\"><b>Plugin Help</b></a>").arg(pluginList->at(i).mainhelp)));
                }
                int major=0, minor=0;
                pluginList->at(i).plugin->getVersion(major, minor);
                fromHTML_replaces.append(qMakePair(QString("local_plugin_version"), QString("%1.%2").arg(major).arg(minor)));
                if (!pluginList->at(i).plugintypehelp.isEmpty()) {
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_typehelp_file"), pluginList->at(i).plugintypehelp));
                    fromHTML_replaces.append(qMakePair(QString("local_plugin_typehelp_link"), tr("$$qf_commondoc_header.separator$$  <a href=\"%1\">%2</a>").arg(pluginList->at(i).plugintypehelp).arg(pluginList->at(i).plugintypename)));
                }
                foundPlugin=true;
                break;
            }
        }

    }

    if (!foundPlugin) {
        fromHTML_replaces.append(qMakePair(QString("local_plugin_icon"), QString("<img src=\":/icon.png\">")));
        fromHTML_replaces.append(qMakePair(QString("local_plugin_iconfilename"), QString(":/icon.png")));
        fromHTML_replaces.append(qMakePair(QString("local_plugin_name"), tr("QuickFit $$version$$: Online-Help")));
        fromHTML_replaces.append(qMakePair(QString("local_plugin_author"), QString("$$author$$")));
        if (basedir.exists("copyright.html")) {
            fromHTML_replaces.append(qMakePair(QString("local_plugin_copyright"), QString("<a href=\"copyright.html\">$$copyright$$</a>")));
        } else {
            fromHTML_replaces.append(qMakePair(QString("local_plugin_copyright"), QString("$$copyright$$")));
        }
        fromHTML_replaces.append(qMakePair(QString("local_plugin_weblink_url"), QString("$$weblink$$")));
        fromHTML_replaces.append(qMakePair(QString("local_plugin_weblink"), tr("<a href=\"$$weblink$$\">QuickFit Webpage</a>")));
    }

    // handle replaces, also handles special commands, like $$list:...$$
    if (!result.isEmpty()) {
        if (!result.contains("$$qf_commondoc_footer")) {
            result=result.replace(QString("</body>"), QString("<br><br><br><br><br>$$qf_commondoc_footer.start$$ $$qf_commondoc_footer.end$$</body>"));
        }
        if (!result.contains("$$qf_commondoc_header")) {
            result=result.replace(QString("<body>"), QString("<body>$$qf_commondoc_header.start$$  $$qf_commondoc_header.end$$"));
        }
        bool replaced=true;
        int cnt=0;
        while (replaced && (cnt<15)) {
            replaced=false;
            if (replaces) for (int i=0; i<replaces->size(); i++) {
                if (result.contains(QString("$$")+(*replaces)[i].first+QString("$$"))) replaced=true;
                result=result.replace(QString("$$")+(*replaces)[i].first+QString("$$"), (*replaces)[i].second);
            }
            for (int i=0; i<internal_replaces.size(); i++) {
                if (result.contains(QString("$$")+internal_replaces[i].first+QString("$$"))) replaced=true;
                result=result.replace(QString("$$")+internal_replaces[i].first+QString("$$"), internal_replaces[i].second);
            }
            for (int i=0; i<fromHTML_replaces.size(); i++) {
                if (result.contains(QString("$$")+fromHTML_replaces[i].first+QString("$$"))) replaced=true;
                result=result.replace(QString("$$")+fromHTML_replaces[i].first+QString("$$"), fromHTML_replaces[i].second);
            }


            // interpret $$list:<list_name>:<filter>$$ items
            QRegExp rxList("\\$\\$list\\:([a-z]+)\\:([^\\$\\s]*)\\$\\$", Qt::CaseInsensitive);
            rxList.setMinimal(true);
            int count = 0;
            int pos = 0;
            while ((pos = rxList.indexIn(result, pos)) != -1) {
                QString list=rxList.cap(1).toLower().trimmed();
                QString filter=rxList.cap(2).trimmed();
                //qDebug()<<pos<<list<<filter;
                if (m_pluginServices) {
                    if (list=="fitalg") {
                        QString text="";
                        QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
                        QString item_template_nolink=QString("<li><img width=\"16\" height=\"16\" src=\"%1\">&nbsp;%2</li>");
                        // gather information about plugins
                        for (int i=0; i<m_pluginServices->getFitAlgorithmManager()->pluginCount(); i++) {
                            int id=i;
                            QString aID="";
                            QStringList fa=m_pluginServices->getFitAlgorithmManager()->getIDList(id);
                            QString dir=m_pluginServices->getFitAlgorithmManager()->getPluginHelp(id);
                            QString name=m_pluginServices->getFitAlgorithmManager()->getName(id);
                            QString icon=m_pluginServices->getFitAlgorithmManager()->getIconFilename(id);
                            for (int j=0; j<fa.size(); j++) {
                                aID=fa[j];
                                QFFitAlgorithm* a=m_pluginServices->getFitAlgorithmManager()->createAlgorithm(aID);
                                if (a) {
                                    name=a->name();
                                    dir=dir=m_pluginServices->getFitAlgorithmManager()->getPluginHelp(id, aID);
                                    delete a;
                                    if (QFile::exists(dir)) text+=item_template.arg(icon).arg(name).arg(dir);
                                    else text+=item_template_nolink.arg(icon).arg(name);
                                }
                            }
                        }
                        if (!text.isEmpty()) {
                            result=result.replace(rxList.cap(0), QString("<ul>")+text+QString("</ul>"));
                        }
                    } else if (list=="fitfunc") {
                        QString text="";
                        QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
                        QString item_template_nolink=QString("<li><img width=\"16\" height=\"16\" src=\"%1\">&nbsp;%2</li>");
                        // gather information about plugins
                            QMap<QString, QFFitFunction*> models= m_pluginServices->getFitFunctionManager()->getModels(filter);
                            QMapIterator<QString, QFFitFunction*> j(models);
                            while (j.hasNext()) {
                                j.next();
                                int id=m_pluginServices->getFitFunctionManager()->getPluginForID(j.key());
                                QString dir=m_pluginServices->getFitFunctionManager()->getPluginHelp(id);
                                QString name=m_pluginServices->getFitFunctionManager()->getName(id);
                                QString icon=m_pluginServices->getFitFunctionManager()->getIconFilename(id);
                                QFFitFunction* a=j.value();
                                if (a) {
                                    name=a->name();
                                    dir=dir=m_pluginServices->getFitFunctionManager()->getPluginHelp(id, j.key());
                                    delete a;
                                    if (QFile::exists(dir)) text+=item_template.arg(icon).arg(name).arg(dir);
                                    else text+=item_template_nolink.arg(icon).arg(name);
                                }
                            }

                        if (!text.isEmpty()) {
                            result=result.replace(rxList.cap(0), QString("<ul>")+text+QString("</ul>"));
                        }
                    } else if (list=="extension") {
                        QString text="";
                        QString item_template=QString("<li><a href=\"%3\"><img width=\"16\" height=\"16\" src=\"%1\"></a>&nbsp;<a href=\"%3\">%2</a></li>");
                        QString item_template_nolink=QString("<li><img width=\"16\" height=\"16\" src=\"%1\">&nbsp;%2</li>");
                        // gather information about plugins
                        QStringList ids=m_pluginServices->getExtensionManager()->getIDList();
                        for (int i=0; i<ids.size(); i++) {
                            QString id=ids[i];
                            QString dir=m_pluginServices->getExtensionManager()->getPluginHelp(id);
                            QString name=m_pluginServices->getExtensionManager()->getName(id);
                            QString icon=m_pluginServices->getExtensionManager()->getIconFilename(id);
                            if (filter.isEmpty() || ((!filter.isEmpty()) && (m_pluginServices->getExtensionManager()->getQObjectInstance(id)->inherits(filter.toAscii().data())))) {
                                if (QFile::exists(dir)) text+=item_template.arg(icon).arg(name).arg(dir);
                                else text+=item_template_nolink.arg(icon).arg(name);
                            }
                        }
                        if (!text.isEmpty()) {
                            result=result.replace(rxList.cap(0), QString("<ul>")+text+QString("</ul>"));
                        }
                    }
                }

                ++count;
                pos += rxList.matchedLength();
            }




            // interpret $$list:<list_name>:<filter>$$ items
            QRegExp rxInsert("\\$\\$(insert|insertglobal)\\:([^\\$\\s]*)\\$\\$", Qt::CaseInsensitive);
            rxInsert.setMinimal(true);
            count = 0;
            pos = 0;
            while ((pos = rxInsert.indexIn(result, pos)) != -1) {
                QString command=rxInsert.cap(1).toLower().trimmed();
                QString file=rxInsert.cap(2).trimmed();
                //qDebug()<<pos<<list<<filter;

                if (command=="insert") {
                    //qDebug()<<QFileInfo(filename).absoluteDir().absoluteFilePath(file);
                    QFile f(QFileInfo(filename).absoluteDir().absoluteFilePath(file));
                    QString rep="";
                    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        rep=f.readAll();
                    }
                    result=result.replace(rxInsert.cap(0), rep);
                } else if (m_pluginServices&&(command=="insertglobal")) {
                    //qDebug()<<QDir(m_pluginServices->getAssetsDirectory()+"/help/").absoluteFilePath(file);
                    QFile f(QDir(m_pluginServices->getAssetsDirectory()+"/help/").absoluteFilePath(file));
                    QString rep="";
                    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        rep=f.readAll();
                    }
                    result=result.replace(rxInsert.cap(0), rep);
                }

                ++count;
                pos += rxInsert.matchedLength();
            }


            // interpret $$math:<latex>$$ items
            QRegExp rxLaTeX("\\$\\$(math|bmath)\\:([^\\$]*)\\$\\$", Qt::CaseInsensitive);
            rxLaTeX.setMinimal(true);
            count = 0;
            pos = 0;
            while ((pos = rxLaTeX.indexIn(result, pos)) != -1) {
                QString command=rxLaTeX.cap(1).toLower().trimmed();
                QString latex=rxLaTeX.cap(2).trimmed();

                if (command=="math" || command=="bmath") {
                    QPixmap pix(300,100);
                    QPainter p(&pix);
                    p.setRenderHint(QPainter::Antialiasing);
                    p.setRenderHint(QPainter::HighQualityAntialiasing);
                    p.setRenderHint(QPainter::TextAntialiasing);
                    mathParser.parse(latex);
                    QSizeF size=mathParser.getSize(p);
                    p.end();
                    pix=QPixmap(size.width()*1.2, size.height()*1.1);
                    pix.fill(Qt::transparent);
                    p.begin(&pix);
                    p.setRenderHint(QPainter::Antialiasing);
                    p.setRenderHint(QPainter::HighQualityAntialiasing);
                    p.setRenderHint(QPainter::TextAntialiasing);
                    mathParser.draw(p,Qt::AlignTop | Qt::AlignLeft, QRectF(QPointF(0,0), size));
                    p.end();
                    QString texfilename=QDir::tempPath()+"/qf3help_"+QFileInfo(filename).baseName()+"_tex"+QString::number(count)+".png";
                    //qDebug()<<"latex-render: "<<latex<<"\n    size = "<<size<<"  output = "<<texfilename;
                    pix.save(texfilename);

                    if (command=="bmath") {
                        result=result.replace(rxLaTeX.cap(0), QString("<blockquote><img style=\"vertical-align: middle;\" alt=\"%1\" src=\"%2\"></blockquote>").arg(latex).arg(texfilename));
                    } else {
                        result=result.replace(rxLaTeX.cap(0), QString("<img style=\"vertical-align: middle;\" alt=\"%1\" src=\"%2\">").arg(latex).arg(texfilename));
                    }
                }

                ++count;
                pos += rxLaTeX.matchedLength();
            }

            cnt++;
        }



        // remove all unreplaces $$name$$ sequences
        QRegExp rxSpecials("\\$\\$.+\\$\\$");
        rxSpecials.setMinimal(true);
        result=result.remove(rxSpecials);

        // make all image pathes absolute, WORKAROUND FOR PROBLEM IN Qt4.8.0
        QRegExp rxImages("<img\\s.*src\\s*=\\s*\\\"([^\\\"]*)\\\".*>");
        rxImages.setMinimal(true);
        rxImages.setCaseSensitivity(Qt::CaseInsensitive);
        pos = 0;
        while ((pos = rxImages.indexIn(result, pos)) != -1) {
            QString file=rxImages.cap(1).trimmed();
            QString old=QString("\"%1\"").arg(file);
            QString news=QString("\"%1%2\"").arg(fileDir).arg(file);
            if (QFileInfo(file).isRelative()) {
                result.replace(old, news);
            }
            pos += rxImages.matchedLength()+(news.size()-old.size());
        }

    }
    //qDebug()<<result;
    return result;
}

void QFHTMLHelpWindow::setHtmlReplacementList(QList<QPair<QString, QString> >* list) {
    replaces=list;
}

void QFHTMLHelpWindow::setPluginDirList(QList<QFPluginServices::HelpDirectoryInfo>* pluginList) {
    this->pluginList=pluginList;
}

void QFHTMLHelpWindow::initFromPluginServices(QFPluginServices* services) {
    m_pluginServices=services;
    setHtmlReplacementList(services->getHTMLReplacementList());
    setPluginDirList(services->getPluginHelpList());
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
