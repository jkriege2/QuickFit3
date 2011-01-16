#include "qfhtmlhelpwindow.h"

#include <QtGui>


QFHTMLHelpWindow::QFHTMLHelpWindow(QWidget* parent) {
    searchPath=QStringList(QApplication::applicationDirPath());

    m_baseFont=font();
    m_titleFont=font();
    m_titleFont.setPointSizeF(m_titleFont.pointSizeF()*2);
    m_titleFont.setBold(true);

    labelTitle=new QLabel(this);
    descriptionBrowser=new QTextBrowser(this);

    QVBoxLayout* layout=new QVBoxLayout;
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
    labelTitle->setFont(m_titleFont);
    labelTitle->setText(title);
    labelTitle->setVisible(!title.isEmpty());
    descriptionBrowser->setOpenLinks(true);
    descriptionBrowser->setOpenExternalLinks(true);
    descriptionBrowser->setSearchPaths(searchPath);
    descriptionBrowser->setFrameShape(QFrame::Box);
    descriptionBrowser->setReadOnly(true);
    //QStringList(QApplication::applicationDirPath()+"/models/help/"+QFileInfo(model->getFilename()).completeBaseName()+"/")
    //std::cout<<QString(QApplication::applicationDirPath()+"/models/help/"+QFileInfo(model->getFilename()).completeBaseName()+"/").toStdString()<<std::endl;
    descriptionBrowser->setSource(filename);
    descriptionBrowser->reload();
}


void QFHTMLHelpWindow::clear() {
    labelTitle->setText("");
    descriptionBrowser->clear();
}
