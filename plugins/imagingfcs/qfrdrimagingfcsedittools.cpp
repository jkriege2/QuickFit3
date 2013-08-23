#include "qfrdrimagingfcsedittools.h"
#include "qfrdrimagingfcs_data.h"
#include "qfselectionlistdialog.h"

QFRDRImagingFCSEditTools::QFRDRImagingFCSEditTools(QObject *parent) :
    QObject(parent)
{
    imFCS=NULL;

    actExcludeSegments=new QAction(tr("exclude segments"), this);
    connect(actExcludeSegments, SIGNAL(triggered()), this, SLOT(excludeSegments()));

    setRDR(imFCS);
}

void QFRDRImagingFCSEditTools::registerToMenu(QMenu *menu)
{
    menu->addAction(actExcludeSegments);
}

void QFRDRImagingFCSEditTools::registerToToolbar(QToolBar *menu)
{
}

void QFRDRImagingFCSEditTools::setRDR(QFRawDataRecord *record)
{
    imFCS=qobject_cast<QFRDRImagingFCSData*>(record);
    actExcludeSegments->setEnabled(imFCS && record->getProperty("SEGMENTS_USABLE", false).toBool());
}

void QFRDRImagingFCSEditTools::excludeSegments() {
    if (!imFCS) return;
    QFSelectionListDialog* dlg=new QFSelectionListDialog(NULL, false);
    QStringList segments;
    QList<QColor> cols;
    QList<bool> checked;
    double duration=imFCS->getSegmentDuration();
    for (int i=0; i<imFCS->getSegmentCount(); i++) {
        segments<<tr("segment %1 (t = %2s..%3s)").arg(i+1).arg(double(i)*duration).arg(double(i+1)*duration);
        if (imFCS->segmentUsed(i)) {
            cols<<dlg->palette().color(QPalette::WindowText);
            checked<<true;
        } else {
            cols<<dlg->palette().color(QPalette::Disabled, QPalette::WindowText);
            checked<<false;
        }
    }
    dlg->init(segments, segments, cols);
    dlg->selectItems(checked);
    dlg->setLabel(tr("checked segments will be used,\nunchecked will be excluded from the averaged CFs"));
    dlg->setWindowTitle(tr("imFCS: Ex-/Include Segments"));
    if (dlg->exec()) {
        QList<bool> used=dlg->getSelectedBoolList();
        for (int i=0; i<used.size(); i++) {
            imFCS->setSegmentUsed(i, used[i]);
        }
        imFCS->recalcSegmentedAverages();

        QList<QFRDRImagingFCSData*> grp= filterListForClass<QFRawDataRecord, QFRDRImagingFCSData>(imFCS->getGroupMembers());
        QList<QFRDRImagingFCSData*> inpt=filterListForClass<QFRawDataRecord, QFRDRImagingFCSData>(imFCS->getProject()->getRawDataList());
        if (imFCS->getFilesForType("input").size()<=0) inpt.clear();
        else {
            for (int i=inpt.size()-1; i>=0; i--) {
                if (!inpt[i]->getFilesForType("input").contains(imFCS->getFilesForType("input").first())) {
                    inpt.removeAt(i);
                }
            }
        }

        if (grp.size()>1 || inpt.size()>1) {
            bool ok=false;
            QStringList sl;
            sl<<tr("in same group (%1 more files)").arg(grp.size()-1);
            sl<<tr("with same input dataset (%1 more files)").arg(inpt.size()-1);
            sl<<tr("do not set in other files");
            int option=sl.indexOf(QInputDialog::getItem(NULL, tr("Exclude Segments"), tr("exclude these segments also in the selected files"), sl, 0, false, &ok));
            if (ok && option>=0 && option<=1) {
                if (option==0) { // in group
                    for (int j=0; j<grp.size(); j++) {
                        if (grp[j]!=imFCS && grp[j]) {
                            QList<bool> used=dlg->getSelectedBoolList();
                            for (int i=0; i<used.size(); i++) {
                                grp[j]->setSegmentUsed(i, used[i]);
                            }
                            grp[j]->recalcSegmentedAverages();
                        }
                    }
                } else if (option==1) { // same input
                    for (int j=0; j<inpt.size(); j++) {
                        if (inpt[j]!=imFCS && inpt[j]) {
                            QList<bool> used=dlg->getSelectedBoolList();
                            for (int i=0; i<used.size(); i++) {
                                inpt[j]->setSegmentUsed(i, used[i]);
                            }
                            inpt[j]->recalcSegmentedAverages();
                        }
                    }
                }
            }
        }

    }
}
