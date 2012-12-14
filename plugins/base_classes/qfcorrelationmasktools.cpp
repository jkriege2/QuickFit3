#include "qfcorrelationmasktools.h"
#include "programoptions.h"
#include "qftools.h"
#include <QClipboard>

QFCorrelationMaskTools::QFCorrelationMaskTools(QWidget *parentWidget, const QString& settingsPrefix) :
    QFRDRImageMaskEditTools(parentWidget, settingsPrefix)
{
    actFindErroneousPixels=new QAction(tr("Filter CFs for &outliers ..."), this);
    connect(actFindErroneousPixels, SIGNAL(triggered()), this, SLOT(findErroneousPixels()));

}

void QFCorrelationMaskTools::setRDR(QFRawDataRecord *rdr)
{
    QFRDRImageMaskEditTools::setRDR(rdr);
    fcs=qobject_cast<QFRDRFCSDataInterface*>(rdr);
    actFindErroneousPixels->setEnabled(runselection && fcs);
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

        }
        disconnect(dlg, SIGNAL(setSelection(QList<int>,bool)), this, SLOT(setLeaveout(QList<int>,bool)));
        delete dlg;
    }
}



void QFCorrelationMaskTools::registerCorrelationToolsToMenu(QMenu *menu) const
{
    menu->addAction(actFindErroneousPixels);
}
