#include "qfrawdataresultscombobox.h"

QFRawDataResultsComboBox::QFRawDataResultsComboBox(QWidget *parent) :
    QComboBox(parent)
{
    rdr=NULL;
    egroup="";
    resultGroupFilters<<"";
    evaluationIDFilter="";
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));
}

void QFRawDataResultsComboBox::setRDR(QFRawDataRecord *rdr)
{
    if (this->rdr) disconnect(this->rdr, 0, this, 0);
    QVariant oldData=itemData(currentIndex());
    setUpdatesEnabled(false);

    this->rdr=rdr;
    if (rdr) {

        QList<QTriple<QString, QString, QString> > params;
        if (resultGroupFilters.size()<=0) params=rdr->resultsCalcNamesEvalIDsAndLabels(evaluationIDFilter, "", egroup);
        for (int i=0; i<resultGroupFilters.size(); i++) {
            QList<QTriple<QString, QString, QString> > params1=rdr->resultsCalcNamesEvalIDsAndLabels(evaluationIDFilter, resultGroupFilters[i], egroup);
            params.append(params1);
        }

        clear();
        evalIDs.clear();
        for (int i=0; i<params.size(); i++) {
            addItem(params[i].first, params[i].second);
            evalIDs[i]=params[i].third;
        }

        int idxFound=findData(oldData);
        if (oldData.isValid() && (idxFound>=0)) setCurrentIndex(idxFound);
        else setCurrentIndex(0);
    } else {
        evalIDs.clear();
        clear();
    }
    setUpdatesEnabled(true);
    if (rdr) connect(rdr, SIGNAL(rawDataChanged()), this, SLOT(refill()));
}

QString QFRawDataResultsComboBox::currentResult() const
{
    return itemData(currentIndex()).toString();
}

QString QFRawDataResultsComboBox::currentEvaluationID() const
{
    return evalIDs.value(currentIndex(), "");
}


QString QFRawDataResultsComboBox::evaluationGroup() const
{
    return egroup;
}

void QFRawDataResultsComboBox::setResultGroupFilters(QStringList &resultGroupFilters)
{
    this->resultGroupFilters=resultGroupFilters;
    refill();
}

QFRawDataRecord *QFRawDataResultsComboBox::getRDR() const
{
    return rdr;
}


void QFRawDataResultsComboBox::setEvaluationGroup(const QString &group)
{
    this->egroup=group;
    refill();
}

void QFRawDataResultsComboBox::setCurrentResult(const QString &result)
{
    int idx=findData(result);
    if (idx>=0) setCurrentIndex(idx);
}

void QFRawDataResultsComboBox::setEvaluationIDFilter(const QString &evalID)
{
    evaluationIDFilter=evalID;
    refill();
}

void QFRawDataResultsComboBox::myCurrentIndexChanged(int i)
{
    emit currentResultChanged(currentResult(), currentEvaluationID());
}

void QFRawDataResultsComboBox::refill()
{
    if (rdr) setRDR(rdr);
    else clear();
}
