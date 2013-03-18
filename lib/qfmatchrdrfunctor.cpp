#include "qfmatchrdrfunctor.h"

QFMatchRDRFunctor::QFMatchRDRFunctor()
{
    //qDebug()<<"QFSelectRDRDialogMatchFunctor:constructor ";
}

QFMatchRDRFunctor::~QFMatchRDRFunctor()
{
}

QList<QPointer<QFRawDataRecord > > QFMatchRDRFunctor::getFilteredList(QFProject *project)
{
    QList<QPointer<QFRawDataRecord > >  rdrList;
    if (project) {
        for (int i=0; i<project->getRawDataCount(); i++) {
            QFRawDataRecord* r=project->getRawDataByNum(i);
            if (r && matches(r)) {
                rdrList<<r;
            }
        }
    }
    return rdrList;
}

QFMatchRDRFunctorSelectAll::QFMatchRDRFunctorSelectAll():
    QFMatchRDRFunctor()
{
    //qDebug()<<"QFSelectRDRDialogMatchFunctorSelectAll:constructor ";
}

bool QFMatchRDRFunctorSelectAll::matches(const QFRawDataRecord *record) const
{
    //qDebug()<<"QFSelectRDRDialogMatchFunctorSelectAll";
    return true;
}




QFMatchRDRFunctorSelectType::QFMatchRDRFunctorSelectType(const QString &type)
{
    this->type=type;
}

bool QFMatchRDRFunctorSelectType::matches(const QFRawDataRecord *record) const
{
    return record->getType().toLower()==type;
}

QString QFMatchRDRFunctorSelectType::getType() const
{
    return type;
}

void QFMatchRDRFunctorSelectType::setType(const QString &type)
{
    this->type=type;
}
