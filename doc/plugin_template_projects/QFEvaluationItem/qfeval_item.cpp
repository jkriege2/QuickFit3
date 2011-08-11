#include "qfeval_item.h"
#include "qfeval_editor.h"



QFEVALItem::QFEVALItem(QFProject* parent):
    QFEvaluationItem(parent, true, false)
{

}

QFEVALItem::~QFEVALItem() {

}


void QFEVALItem::intWriteData(QXmlStreamWriter& w) {
    /* store global evaluation parameters / initial values or anything else to the project tree here */
}

void QFEVALItem::intReadData(QDomElement* e) {
    /* read back anything you've written in intWriteData() in this function */
}


QFEvaluationEditor* QFEVALItem::createEditor(QFPluginServices* services, QWidget* parent) {
    return new QFEVALEditor(services, parent);
};

bool QFEVALItem::isApplicable(QFRawDataRecord* record) {
    /* CHECK WHETHER A GIVEN RECORD MAY BE USED TOGETHER WITH THE EVALUATION */
    return true;
}

bool QFEVALItem::hasEvaluation(QFRawDataRecord* r1) {
    QFRawDataRecord* r=r1;
    QString rsid=getEvaluationResultID();
    return r->resultsExistsFromEvaluation(rsid);
}


