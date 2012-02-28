#include "qffcsmaxentevaluation_item.h"
#include "qffcsmaxentevaluation_editor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"


QFFCSMaxEntEvaluationItem::QFFCSMaxEntEvaluationItem(QFProject* parent):
    QFEvaluationItem(parent, true, false)
{

}

QFFCSMaxEntEvaluationItem::~QFFCSMaxEntEvaluationItem() {

}


void QFFCSMaxEntEvaluationItem::intWriteData(QXmlStreamWriter& w) {
    /* store global evaluation parameters / initial values or anything else to the project tree here */
}

void QFFCSMaxEntEvaluationItem::intReadData(QDomElement* e) {
    /* read back anything you've written in intWriteData() in this function */
}


QFEvaluationEditor* QFFCSMaxEntEvaluationItem::createEditor(QFPluginServices* services, QWidget* parent) {
    return new QFFCSMaxEntEvaluationEditor(services, parent);
}

bool QFFCSMaxEntEvaluationItem::isApplicable(QFRawDataRecord* record) {
    return record->inherits("QFRDRFCSDataInterface");
}

bool QFFCSMaxEntEvaluationItem::hasEvaluation(QFRawDataRecord* r1) {
    QFRawDataRecord* r=r1;
    QString rsid=getEvaluationResultID();
    return r->resultsExistsFromEvaluation(rsid);
}


