#include "qfevaluationeditor.h"
#include "qfevaluationitem.h"
#include <QCloseEvent>

QFEvaluationEditor::QFEvaluationEditor(QFPluginServices* services, QWidget* parent):
    QWidget(parent)
{
    current=NULL;
    peID=0;
    this->services=services;
}

QFEvaluationEditor::~QFEvaluationEditor()
{
}

void QFEvaluationEditor::setCurrent(QFEvaluationItem* c, int peID) {
    if (current) saveSettings();
    QFEvaluationItem* old=current;
    if (current) {
        disconnect(current, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
    }
    current=c;
    this->peID=peID;
    //std::cout<<"connecting widgets ... \n";
    connectWidgets(current, old);
    //std::cout<<"connecting widgets ... done\n";
    connect(c, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
}

void QFEvaluationEditor::closeEvent( QCloseEvent * event ) {
    if (settings) writeSettings();
    event->accept();
}

