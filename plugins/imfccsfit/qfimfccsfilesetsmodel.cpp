#include "qfimfccsfilesetsmodel.h"
#include "qfimfccsfitevaluation_item.h"

QFImFCCSFileSetsModel::QFImFCCSFileSetsModel(QFImFCCSFitEvaluationItem *parent) :
    QAbstractListModel(parent)
{
    imfccsfit=parent;
    qRegisterMetaType<QList<QFRawDataRecord*> >("QList<QFRawDataRecord*>");
    rebuildModel();
}

QList<QFRawDataRecord *> QFImFCCSFileSetsModel::getFileSetForIndex(const QModelIndex &idx)
{
    return getFileSetForRow(idx.row());
}

QList<QFRawDataRecord *> QFImFCCSFileSetsModel::getFileSetForRow(int row) const
{
    if (row>=0) {
        if (row<imfccsfit->getFittedFiles().size()) {
            return imfccsfit->getFittedFiles().at(row);
        } else if (row-imfccsfit->getFittedFiles().size()<imfccsfit->getGuessedFiles().size()){
            return imfccsfit->getGuessedFiles().at(row-imfccsfit->getFittedFiles().size());
        }
    }
    return QList<QFRawDataRecord *>();
}

int QFImFCCSFileSetsModel::rowCount(const QModelIndex &parent) const
{
    return fittedSets+guessedSets;
}

Qt::ItemFlags QFImFCCSFileSetsModel::flags(const QModelIndex &/*index*/) const
{
    return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

QVariant QFImFCCSFileSetsModel::data(const QModelIndex &index, int role) const
{
    if (role==Qt::DisplayRole) {
        return names.value(index.row(), tr("???"));
    } else if (role==Qt::TextColorRole) {
        if (index.row()<fittedSets) return QColor("darkblue");
        else return QColor("darkgrey");
    } else if (role==Qt::ToolTipRole) {
        if (index.row()<fittedSets) return tr("there are fit results for this set of files");
        else return tr("this set of files was guessed depending on your last fit");
    }
    return QVariant();
}

void QFImFCCSFileSetsModel::rebuildModel()
{
    names.clear();
    if (imfccsfit) {
        fittedSets=imfccsfit->getFittedFiles().size();
        guessedSets=imfccsfit->getGuessedFiles().size();
        for (int r=0; r<fittedSets+guessedSets; r++) {
            QList<QFRawDataRecord *> set=getFileSetForRow(r);
            if (set.size()>0) {
                QString name="";
                QString groupName=set[0]->getGroupName();
                bool sameGroup=true;
                QString folderName=set[0]->getFolder();
                bool sameFolder=true;
                QString roles="";
                QString n="";
                for (int i=0; i<set.size(); i++) {
                    if (set[i]->getGroupName()!=groupName) sameGroup=false;
                    if (set[i]->getFolder()!=folderName) sameFolder=false;
                    if (roles.size()>0) roles+="+";
                    roles+=set[i]->getRole();
                    if (n.size()>0) n+="+";
                    n+=set[i]->getName();
                }

                if (sameGroup && !groupName.isEmpty()) name=groupName;
                else if (sameFolder && !folderName.isEmpty()) name=folderName;
                else name=n;

                if (!roles.isEmpty()) name+=QString(" - ")+roles;

                if (name.isEmpty()) name=tr("???");
                names<<name;
            } else {
                names<<tr("???");
            }
        }
    } else {
        fittedSets=0;
        guessedSets=0;
    }
    reset();
}

