#include "qfhtmlhelpwindow.h"

#include <QtGui>
#include <iostream>
#include <QDir>
#include <QDesktopServices>


QFHTMLHelpWindow::QFHTMLHelpWindow(QWidget* parent):
    QWidget(parent)
{
    searchPath=QApplication::applicationDirPath();

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
    history.append(filename);
    btnNext->setEnabled(false);
    btnPrevious->setEnabled(false);
    //history.clear();
    //QStringList(QApplication::applicationDirPath()+"/models/help/"+QFileInfo(model->getFilename()).completeBaseName()+"/")
    //std::cout<<QString(QApplication::applicationDirPath()+"/models/help/"+QFileInfo(model->getFilename()).completeBaseName()+"/").toStdString()<<std::endl;
    descriptionBrowser->setSource(QFileInfo(filename).fileName());
    descriptionBrowser->reload();
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
    descriptionBrowser->setSource(QFileInfo(filename).fileName());
    descriptionBrowser->reload();
    descriptionBrowser->clearHistory();
    history.clear();
    history_idx=0;
    history.append(filename);
    btnNext->setEnabled(false);
    btnPrevious->setEnabled(false);

    QString title=descriptionBrowser->documentTitle();
    labelTitle->setFont(m_titleFont);
    labelTitle->setText(title);
    labelTitle->setVisible(!title.isEmpty());
    connect(descriptionBrowser, SIGNAL(textChanged()), this, SLOT(displayTitle()));
}

void QFHTMLHelpWindow::clear() {
    labelTitle->setText("");
    descriptionBrowser->clear();
}

void QFHTMLHelpWindow::displayTitle() {
    QString title=descriptionBrowser->documentTitle();
    labelTitle->setFont(m_titleFont);
    labelTitle->setText(title);
    labelTitle->setVisible(!title.isEmpty());
    //QStringList sp;
    //std::cout<<descriptionBrowser->source().toString().toStdString()<<std::endl;
    //descriptionBrowser->setSearchPaths(sp);
}

void QFHTMLHelpWindow::anchorClicked(const QUrl& link) {
    //std::cout<<"scheme = "<<link.scheme().toStdString()<<std::endl;
    if (link.scheme().isEmpty()) {
        QDir spd(searchPath);
        QString cl=spd.cleanPath(spd.absoluteFilePath(link.toString()));
        QString s=spd.absoluteFilePath(cl); //absoluteFilePath
        searchPath=QFileInfo(s).absolutePath();
        //std::cout<<"anchorClicked("<<link.toString().toStdString()<<")   spd="<<spd.canonicalPath().toStdString()<<"   cl="<<cl.toStdString()<<"   s="<<s.toStdString()<<"   searchPath="<<searchPath.toStdString()<<"  src="<<QFileInfo(s).fileName().toStdString()<<"\n";
        descriptionBrowser->setSearchPaths(QStringList(searchPath));
        descriptionBrowser->setSource(QFileInfo(s).fileName());
        descriptionBrowser->reload();

        history.push(s);
        history_idx=history.size()-1;
        btnNext->setEnabled(history.size()>1);
        btnPrevious->setEnabled(history_idx>0);
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
    descriptionBrowser->setSource(QFileInfo(s).fileName());
    descriptionBrowser->reload();
}

void QFHTMLHelpWindow::previous() {
    if (history_idx>=1) {
        history_idx--;
        showFile(history[history_idx]);
        btnNext->setEnabled(history.size()>1);
        btnPrevious->setEnabled(history_idx>0);
    }
}

void QFHTMLHelpWindow::next() {
    if (history_idx<history.size()-1) {
        history_idx++;
        showFile(history[history_idx]);
        btnNext->setEnabled(history.size()>1);
        btnPrevious->setEnabled(history_idx>0);
    }
}

void QFHTMLHelpWindow::home() {
    showFile(m_home);
    history.push(m_home);
    history_idx=history.size()-1;
    btnNext->setEnabled(history.size()>1);
    btnPrevious->setEnabled(history_idx>0);
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
