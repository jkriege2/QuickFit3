#include "qfcorrelationmasktools.h"
#include "programoptions.h"
#include "qftools.h"
#include <QClipboard>

QFCorrelationMaskTools::QFCorrelationMaskTools(QWidget *parentWidget) :
    QObject(parentWidget)
{
    this->parentWidget=parentWidget;
    rdr=NULL;
    actFindErroneousPixels=new QAction(tr("Filter CFs for &outliers ..."), this);
    connect(actFindErroneousPixels, SIGNAL(triggered()), this, SLOT(findErroneousPixels()));

    actSaveMask=new QAction(QIcon(":/imaging_fcs/savemask.png"), tr("&save"), parentWidget);
    actSaveMask->setToolTip(tr("save the mask to harddisk"));
    connect(actSaveMask, SIGNAL(triggered()), this, SLOT(saveMask()));
    actLoadMask=new QAction(QIcon(":/imaging_fcs/loadmask.png"), tr("&load"), parentWidget);
    actLoadMask->setToolTip(tr("load a mask from harddisk"));
    connect(actLoadMask, SIGNAL(triggered()), this, SLOT(loadMask()));
    actCopyMask=new QAction(QIcon(":/imaging_fcs/copymask.png"), tr("&copy"), parentWidget);
    actCopyMask->setToolTip(tr("copy the mask to clipboard"));
    connect(actCopyMask, SIGNAL(triggered()), this, SLOT(copyMask()));
    actPasteMask=new QAction(QIcon(":/imaging_fcs/pastemask.png"), tr("&paste"), parentWidget);
    actPasteMask->setToolTip(tr("paste a mask from clipboard"));
    connect(actPasteMask, SIGNAL(triggered()), this, SLOT(pasteMask()));


    setRDR(rdr);
}

void QFCorrelationMaskTools::setRDR(QFRawDataRecord *rdr)
{
    this->rdr=rdr;
    imagemask=qobject_cast<QFRDRImageMaskInterface*>(rdr);
    runselection=qobject_cast<QFRDRRunSelectionsInterface*>(rdr);
    fcs=qobject_cast<QFRDRFCSDataInterface*>(rdr);
    actFindErroneousPixels->setEnabled(runselection && fcs);
    actSaveMask->setEnabled(imagemask);
    actLoadMask->setEnabled(imagemask);
    actCopyMask->setEnabled(imagemask);
    actPasteMask->setEnabled(imagemask);
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

void QFCorrelationMaskTools::setLeaveout(QList<int> leaveout, bool clearOld)
{
    if (runselection) {
        QSet<int> set=leaveout.toSet();
        for (int i=0; i<runselection->leaveoutGetRunCount(); i++) {
            if (set.contains(i)) runselection->leaveoutAddRun(i);
            else if (clearOld) runselection->leaveoutRemoveRun(i);
        }
        emit rawDataChanged();
    }
}

void QFCorrelationMaskTools::loadMask() {
    if (!imagemask) return;
    QString lastMaskDir=ProgramOptions::getConfigValue("QFCorrelationMaskTools/lastmaskdir", "").toString();
    QString filename= qfGetOpenFileName(parentWidget, tr("select mask file to open ..."), lastMaskDir, tr("mask files (*.msk)"));
    if (QFile::exists(filename)) {
        if (imagemask) {
            imagemask->maskLoad(filename);
        }
        ProgramOptions::setConfigValue("QFCorrelationMaskTools/lastmaskdir", lastMaskDir);
    }

    emit rawDataChanged();
}

void QFCorrelationMaskTools::pasteMask()
{
    if (!imagemask) return;

    QClipboard* clipboard=QApplication::clipboard();

    const QMimeData* mime=clipboard->mimeData();
    if (mime->hasFormat("quickfit3/pixelselection")) {
        imagemask->maskLoadFromString(QString::fromUtf8(mime->data("quickfit3/pixelselection")));
    }

    emit rawDataChanged();
}

void QFCorrelationMaskTools::saveMask()
{
    if (!imagemask) return;
    QString lastMaskDir=ProgramOptions::getConfigValue("QFCorrelationMaskTools/lastmaskdir", "").toString();
    QString filename= qfGetSaveFileName(parentWidget, tr("save mask as ..."), lastMaskDir, tr("mask files (*.msk)"));
    if (!filename.isEmpty()) {
        imagemask->maskSave(filename);
        ProgramOptions::setConfigValue("QFCorrelationMaskTools/lastmaskdir", lastMaskDir);
    }
}

void QFCorrelationMaskTools::copyMask()
{
    if (!imagemask) return;
    QString mask=imagemask->maskToString();
    QClipboard* clipboard=QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setText(mask);
    mime->setData("quickfit3/pixelselection", mask.toUtf8());
    clipboard->setMimeData(mime);
}

void QFCorrelationMaskTools::registerCorrelationToolsToMenu(QMenu *menu) const
{
    menu->addAction(actFindErroneousPixels);
}

void QFCorrelationMaskTools::registerMaskToolsToMenu(QMenu *menu) const
{
    menu->addAction(actLoadMask);
    menu->addAction(actSaveMask);
    menu->addAction(actCopyMask);
    menu->addAction(actPasteMask);
}
