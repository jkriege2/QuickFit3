#include "qffcsfitevaluation.h"
#include "qffcsfitevaluationeditor.h"



QFFCSFitEvaluation::QFFCSFitEvaluation(QFProject* parent):
    QFEvaluationItem(parent, true, true)//false)
{
    //ctor
}

QFFCSFitEvaluation::~QFFCSFitEvaluation()
{
    //dtor
}


void QFFCSFitEvaluation::intWriteXML(QXmlStreamWriter& w) {
}

void QFFCSFitEvaluation::intReadData(QDomElement* e) {
}


QFEvaluationEditor* QFFCSFitEvaluation::createEditor(int i, QWidget* parent) {
    return new QFFCSFitEvaluationEditor(parent);
};
