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

    }
}
