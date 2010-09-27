#include "qfrawdatarecordfactory.h"

#include "qfrdrfcsdata.h"
#include "qfrdrtable.h"

QFRawDataRecordFactory qfrdrRec;

QFRawDataRecordFactory* getRawDataRecordFactory() {
    return &qfrdrRec;
};







QFRawDataRecordFactory::QFRawDataRecordFactory(QObject* parent):
    QObject(parent)
{
    qfrdrFactoryRecord d;


    d.name=tr("FCS");
    d.description=tr("a FCS measurement dataset (correlation curves + count rates)");
    d.create=&createQFRDRFCSData;
    d.registerMenu=&registerQFRDRFCSMenu;
    addRecord("fcs", d);


    d.name=tr("Table");
    d.description=tr("a table of data record");
    d.create=&createQFRDRTable;
    d.registerMenu=&registerQFRDRTableMenu;
    addRecord("table", d);
}

QFRawDataRecordFactory::~QFRawDataRecordFactory()
{
    //dtor
}


bool QFRawDataRecordFactory::addRecord(QString ID, QFRawDataRecordFactory::qfrdrFactoryRecord desc) {
    if (items.contains(ID)) return false;
    items[ID]=desc;
    return true;
}

