#include "qfresultsgroupcombobox.h"

QFResultsGroupComboBox::QFResultsGroupComboBox(QWidget *parent) :
    QComboBox(parent)
{
    rdr=NULL;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));
}

void QFResultsGroupComboBox::setRDR(QFRawDataRecord *rdr)
{

    if (this->rdr) disconnect(this->rdr, 0, this, 0);
    QVariant oldData=itemData(currentIndex());
    setUpdatesEnabled(false);

    this->rdr=rdr;
    if (rdr) {
        QStringList oldEG=egroups;
        QStringList oldEGN=egnames;
        egroups=rdr->resultsCalcEvalGroups();
        egnames.clear();
        for (int i=0; i<egroups.size(); i++) {
            egnames<<QString("%1").arg(rdr->resultsGetLabelForEvaluationGroup(egroups[i]));
        }
        if (egroups!=oldEG || egnames!=oldEGN) {
            clear();


            for (int i=0; i<egroups.size(); i++) {
                addItem(egnames.value(i, ""), egroups[i]);
            }


            int idxFound=findData(oldData);
            if (oldData.isValid() && (idxFound>=0)) setCurrentIndex(idxFound);
            else setCurrentIndex(0);
        }
    } else {
        egroups.clear();
        egnames.clear();
        clear();
    }
    setUpdatesEnabled(true);
    if (rdr) connect(rdr, SIGNAL(rawDataChanged()), this, SLOT(refill()));

}

QString QFResultsGroupComboBox::currentEvaluationGroup() const
{
    return itemData(currentIndex()).toString();
}

void QFResultsGroupComboBox::setCurrentEvaluationGroup(const QString &group)
{
    int idx=findData(group);
    if (idx>=0) setCurrentIndex(idx);
}


void QFResultsGroupComboBox::myCurrentIndexChanged(int i)
{
    emit currentEvaluationGroupChanged(currentEvaluationGroup());
}

void QFResultsGroupComboBox::refill()
{
    if (rdr) setRDR(rdr);
    else clear();
}
