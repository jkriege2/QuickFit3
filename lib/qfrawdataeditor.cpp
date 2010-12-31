#include "qfrawdataeditor.h"
#include "qfrawdatarecord.h"
#include <QCloseEvent>

QFRawDataEditor::QFRawDataEditor(QWidget* parent):
    QWidget(parent)
{
    current=NULL;
    peID=0;
}

QFRawDataEditor::~QFRawDataEditor()
{
}

void QFRawDataEditor::setCurrent(QFRawDataRecord* c, int peID) {
    this->peID=peID;
    QFRawDataRecord* old=current;
    if (current) {
        writeSettings();
        disconnect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    }
    current=c;
    //std::cout<<"connecting widgets ... \n";
    connectWidgets(current, old);
    //std::cout<<"connecting widgets ... done\n";
    if (current) {
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        rawDataChanged();
    }
}

void QFRawDataEditor::closeEvent( QCloseEvent * event ) {
    if (settings) writeSettings();
    event->accept();
}

void QFRawDataEditor::setSettings(ProgramOptions* s, int peID) {
    std::cout<<"QFRawDataEditor::setSettings("<<s<<", "<<peID<<")\n";
    settings=s;
    this->peID=peID;
    if (settings) readSettings();
}


