#include "qfrdrcombobox.h"

QFRDRComboBox::QFRDRComboBox(QWidget *parent) :
    QComboBox(parent)
{
    project=NULL;
    matchFunctor=NULL;
}

void QFRDRComboBox::init(QFProject *project, QFMatchRDRFunctor *matchFunctor)
{
    bool idxOK=true;
    int rdrID=itemData(currentIndex()).toInt(&idxOK);
    if (currentIndex()<0) idxOK=false;
    setUpdatesEnabled(false);

    clear();

    this->project=project;
    this->matchFunctor=matchFunctor;

    QList<QPointer<QFRawDataRecord> > lst;
    if (matchFunctor && project) lst=matchFunctor->getFilteredList(project);
    else if (project) {
        QList<QFRawDataRecord*> rl=project->getRawDataList();
        for (int i=0; i<rl.size(); i++) {
            lst<<rl[i];
        }
    }

    for (int i=0; i<lst.size(); i++) {
        if (lst[i]) addItem(lst[i]->getSmallIcon(), lst[i]->getName(), lst[i]->getID());
    }

    int idxFound=findData(rdrID);
    if (idxOK && (idxFound>=0)) setCurrentIndex(idxFound);
    else setCurrentIndex(0);
    setUpdatesEnabled(true);
}

QFRawDataRecord *QFRDRComboBox::currentRDR() const
{
    int ID=itemData(currentIndex()).toInt();
    if (project) {
        return project->getRawDataByID(ID);
    }
    return NULL;
}


void QFRDRComboBox::setCurrentRDR(const QFRawDataRecord *record)
{
    if (!record) {
        setCurrentIndex(-1);
    } else {
        int idx=findData(record->getID());
        setCurrentIndex(idx);
    }
}
