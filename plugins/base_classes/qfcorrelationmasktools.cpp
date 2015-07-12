/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



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
