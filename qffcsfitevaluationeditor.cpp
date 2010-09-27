#include "qffcsfitevaluationeditor.h"
#include <QGridLayout>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"

QFFCSFitEvaluationEditor::QFFCSFitEvaluationEditor(QWidget* parent):
    QFEvaluationEditor(parent)
{
    createWidgets();
}

QFFCSFitEvaluationEditor::~QFFCSFitEvaluationEditor()
{
    //dtor
}


void QFFCSFitEvaluationEditor::createWidgets() {
    QGridLayout* mainLayout=new QGridLayout(this);
    setLayout(mainLayout);

    labRecord=new QLabel(this);
    mainLayout->addWidget(labRecord, 0, 0);
}


void QFFCSFitEvaluationEditor::connectWidgets(const QFEvaluationItem* current, const QFEvaluationItem* old) {
    if (old!=NULL) {
        disconnect(old, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
    }
    connect(current, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
}

void QFFCSFitEvaluationEditor::resultsChanged() {
}

void QFFCSFitEvaluationEditor::readSettings() {
}

void QFFCSFitEvaluationEditor::writeSettings() {
}

void QFFCSFitEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    labRecord->setText(tr("<b>current:</b> <i>%1</i>").arg(currentRecord->getName()) );
}
