#include "qfspimlightsheetevaluation_item.h"
#include "qfspimlightsheetevaluation_editor.h"
#include "qfrdrimagestackinterface.h"



QFSPIMLightsheetEvaluationItem::QFSPIMLightsheetEvaluationItem(QFProject* parent):
    QFEvaluationItem(parent, true, false)
{

}

QFSPIMLightsheetEvaluationItem::~QFSPIMLightsheetEvaluationItem() {

}


void QFSPIMLightsheetEvaluationItem::intWriteData(QXmlStreamWriter& w) {
    /* store global evaluation parameters / initial values or anything else to the project tree here */
}

void QFSPIMLightsheetEvaluationItem::intReadData(QDomElement* e) {
    /* read back anything you've written in intWriteData() in this function */
}


QFEvaluationEditor* QFSPIMLightsheetEvaluationItem::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFSPIMLightsheetEvaluationEditor(services, propEditor, parent);
}

bool QFSPIMLightsheetEvaluationItem::isApplicable(QFRawDataRecord* record) {
    /* CHECK WHETHER A GIVEN RECORD MAY BE USED TOGETHER WITH THE EVALUATION */
    return qobject_cast<QFRDRImageStackInterface*>(record);
}

bool QFSPIMLightsheetEvaluationItem::hasEvaluation(QFRawDataRecord* r1, int stack, int channel) {
    QFRawDataRecord* r=r1;
    QString rsid=getEvaluationResultID(stack, channel);
    return r->resultsExistsFromEvaluation(rsid);
}

QString QFSPIMLightsheetEvaluationItem::getEvaluationResultID(int stack, int channel)
{
    if (channel<0) {
        return QString("%1_%2_stack%3").arg(getType()).arg(getID()).arg(stack);
    } else {
        return QString("%1_%2_stack%3_ch%4").arg(getType()).arg(getID()).arg(stack).arg(channel);
    }
}



