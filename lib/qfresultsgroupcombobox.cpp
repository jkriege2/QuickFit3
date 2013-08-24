#include "qfresultsgroupcombobox.h"

QFResultsGroupComboBox::QFResultsGroupComboBox(QWidget *parent) :
    QComboBox(parent)
{
    rdr=NULL;
    containsParamInGroup="";
    doFilter=false;
    view()->setTextElideMode(Qt::ElideMiddle);
    setMaximumWidth(500);
    //setSizeAdjustPolicy(QComboBox::AdjustToContents);
    setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    setMinimumContentsLength(50);
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
        egroups=rdr->resultsCalcEvalGroups(containsParamInGroup);
        if (doFilter) {
            for (int i=egroups.size()-1; i>=0; i--) {
                if (rxFilter.indexIn(egroups[i])<0) egroups.removeAt(i);
            }
        }
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

void QFResultsGroupComboBox::setGroupFilter(bool enabled, const QRegExp &filter)
{
    doFilter=enabled;
    this->rxFilter=filter;
    refill();

}

void QFResultsGroupComboBox::setContainedParam(const QString &param)
{
    containsParamInGroup=param;
    refill();
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
