#include "qfcorrelationmasktools.h"
#include "programoptions.h"
#include "qftools.h"
#include <QClipboard>

QFCorrelationMaskTools::QFCorrelationMaskTools(QWidget *parentWidget, const QString& settingsPrefix) :
    QFRDRImageMaskEditTools(parentWidget, settingsPrefix)
{
    actFindErroneousPixels=new QAction(tr("Filter CFs for &outliers ..."), this);
    connect(actFindErroneousPixels, SIGNAL(triggered()), this, SLOT(findErroneousPixels()));

    actMaskAllZero=new QAction(tr("mask CFs that are all 0 ..."), this);
    connect(actMaskAllZero, SIGNAL(triggered()), this, SLOT(maskAllZeroCorrelations()));
}

void QFCorrelationMaskTools::setRDR(QFRawDataRecord *rdr)
{
    QFRDRImageMaskEditTools::setRDR(rdr);
    fcs=qobject_cast<QFRDRFCSDataInterface*>(rdr);
    actFindErroneousPixels->setEnabled(runselection && fcs);
    actMaskAllZero->setEnabled(runselection && fcs);
}


void QFCorrelationMaskTools::findErroneousPixels()
{
    if (runselection && fcs) {
        QFFindErroneousCorrelationDlg* dlg=new QFFindErroneousCorrelationDlg(NULL);
        connect(dlg, SIGNAL(setSelection(QList<int>,bool)), this, SLOT(setLeaveout(QList<int>,bool)));
        dlg->init(runselection, fcs);
        if (dlg->exec()) {
            QList<int> mask=dlg->getMaskedRuns();
            setLeaveout(dlg->getInitSelection(), true);
            setLeaveout(mask, dlg->doClearOldMask());

        } else {
            setLeaveout(dlg->getInitSelection(), true);
        }
        disconnect(dlg, SIGNAL(setSelection(QList<int>,bool)), this, SLOT(setLeaveout(QList<int>,bool)));
        delete dlg;
    }
}

void QFCorrelationMaskTools::maskAllZeroCorrelations()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (runselection && fcs) {
        int N=fcs->getCorrelationN();
        for (int r=0; r<fcs->getCorrelationRuns(); r++) {
            double* cf=fcs->getCorrelationRun(r);
            bool allZero=true;
            for (int i=0; i<N; i++) {
                if (cf[i]!=0) {
                    allZero=false;
                    break;
                }
            }
            if (allZero) runselection->leaveoutAddRun(r);
            else runselection->leaveoutRemoveRun(r);
        }
    }
    QApplication::restoreOverrideCursor();
}



void QFCorrelationMaskTools::registerCorrelationToolsToMenu(QMenu *menu) const
{
    menu->addAction(actFindErroneousPixels);
    menu->addAction(actMaskAllZero);
}
