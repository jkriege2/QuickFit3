#include "qfevaluationitemfactory.h"
#include "qffcsfitevaluation.h"

QFEvaluationItemFactory qfeiFac;

QFEvaluationItemFactory* getEvaluationItemFactory() {
    std::cout<<"getEvaluationItemFactory()\n";
    return &qfeiFac;
}



QFEvaluationItemFactory::QFEvaluationItemFactory(QObject* parent):
    QObject(parent)
{
    qfeiFactoryRecord d;
    d.name=tr("FCS Fit");
    d.description=tr("least squares fitting for FCS correlation curve data");
    d.registerMenu=registerQFFCSFitMenu;
    d.create=createQFFCSFit;
    addRecord("fcs_fit", d);

    std::cout<<"QFEvaluationItemFactory:  "<<items.size()<<" items\n";
}

QFEvaluationItemFactory::~QFEvaluationItemFactory()
{
    //dtor
}




bool QFEvaluationItemFactory::addRecord(QString ID, QFEvaluationItemFactory::qfeiFactoryRecord desc) {
    if (items.contains(ID)) return false;
    items[ID]=desc;
    return true;
}

