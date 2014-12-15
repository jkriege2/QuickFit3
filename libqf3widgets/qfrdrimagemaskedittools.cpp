/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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


#include "qfrdrimagemaskedittools.h"
#include "programoptions.h"
#include "qftools.h"
#include <QClipboard>
#include <QButtonGroup>
#include "qfrdrmaskbyoverviewimagedlg.h"
#define sqr(x) ((x)*(x))
#include "qfselectrdrdialog.h"

#define CLICK_UPDATE_TIMEOUT 500

QFRDRImageMaskEditTools::QFRDRImageMaskEditTools(QWidget *parentWidget, const QString& settingsPrefix):
    QObject(parentWidget)
{
    this->parentWidget=parentWidget;
    this->settingsPrefix=settingsPrefix;
    rdr=NULL;
    useDelay=false;

    selection=NULL;
    selectionWidth=0;
    selectionHeight=0;
    selectionEditing=false;

    actMaskSelected=new QAction(QIcon(":/qfrdrmaskeditor/mask.png"), tr("mask &selected pixels"), this);
    actMaskSelected->setToolTip(tr("mask all currently selected pixels"));
    connect(actMaskSelected, SIGNAL(triggered()), this, SLOT(maskSelected()));

    actUnmaskSelected=new QAction(QIcon(":/qfrdrmaskeditor/unmask.png"), tr("unmask &selected pixels"), this);
    actUnmaskSelected->setToolTip(tr("unmask all currently selected pixels"));
    connect(actUnmaskSelected, SIGNAL(triggered()), this, SLOT(unmaskSelected()));

    actSaveMask=new QAction(QIcon(":/qfrdrmaskeditor/savemask.png"), tr("&save mask"), parentWidget);
    actSaveMask->setToolTip(tr("save the mask to harddisk"));
    connect(actSaveMask, SIGNAL(triggered()), this, SLOT(saveMask()));
    actLoadMask=new QAction(QIcon(":/qfrdrmaskeditor/loadmask.png"), tr("&load mask"), parentWidget);
    actLoadMask->setToolTip(tr("load a mask from harddisk"));
    connect(actLoadMask, SIGNAL(triggered()), this, SLOT(loadMask()));
    actCopyMask=new QAction(QIcon(":/qfrdrmaskeditor/copymask.png"), tr("&copy mask"), parentWidget);
    actCopyMask->setToolTip(tr("copy the mask to clipboard"));
    connect(actCopyMask, SIGNAL(triggered()), this, SLOT(copyMask()));
    actPasteMask=new QAction(QIcon(":/qfrdrmaskeditor/pastemask.png"), tr("&paste mask"), parentWidget);
    actPasteMask->setToolTip(tr("paste a mask from clipboard"));
    connect(actPasteMask, SIGNAL(triggered()), this, SLOT(pasteMask()));

    actClearMask=new QAction(QIcon(":/qfrdrmaskeditor/clearmask.png"), tr("&clear mask"), parentWidget);
    actClearMask->setToolTip(tr("clear the current mask"));
    connect(actClearMask, SIGNAL(triggered()), this, SLOT(clearMask()));
    actInvertMask=new QAction(QIcon(":/qfrdrmaskeditor/invertmask.png"), tr("&invert mask"), parentWidget);
    actInvertMask->setToolTip(tr("invert the current mask"));
    connect(actInvertMask, SIGNAL(triggered()), this, SLOT(invertMask()));

    actUndoMask=new QAction(QIcon(":/qfrdrmaskeditor/undomask.png"), tr("&undo"), parentWidget);
    actUndoMask->setToolTip(tr("undo last change to mask"));
    connect(actUndoMask, SIGNAL(triggered()), this, SLOT(undoMask()));
    actRedoMask=new QAction(QIcon(":/qfrdrmaskeditor/redomask.png"), tr("&redo"), parentWidget);
    actRedoMask->setToolTip(tr("redo peviously undone change to mask"));
    connect(actRedoMask, SIGNAL(triggered()), this, SLOT(redoMask()));

    actMaskBorder=new QAction(QIcon(":/qfrdrmaskeditor/maskborder.png"), tr("mask &border"), parentWidget);
    actMaskBorder->setToolTip(tr("mask a border around the image"));
    connect(actMaskBorder, SIGNAL(triggered()), this, SLOT(maskBorder()));

    actMaskByImage=new QAction(tr("mask by image"), parentWidget);
    actMaskByImage->setToolTip(tr("create a mask by segmenting an overview image"));
    connect(actMaskByImage, SIGNAL(triggered()), this, SLOT(maskByImage()));

    actCopyMaskToGroup=new QAction(tr("copy mask to all RDRs in same &group"), parentWidget);
    actCopyMaskToGroup->setToolTip(tr(""));
    connect(actCopyMaskToGroup, SIGNAL(triggered()), this, SLOT(copyMaskToGroup()));

    actCopyMaskToFiles=new QAction(tr("copy mask to other RDRs"), parentWidget);
    actCopyMaskToFiles->setToolTip(tr(""));
    connect(actCopyMaskToFiles, SIGNAL(triggered()), this, SLOT(copyMaskToFiles()));

    actCopyMaskToFilesOfSameType=new QAction(tr("copy mask to other RDRs of the same type"), parentWidget);
    actCopyMaskToFilesOfSameType->setToolTip(tr(""));
    connect(actCopyMaskToFilesOfSameType, SIGNAL(triggered()), this, SLOT(copyMaskToFilesOfSameType()));


    cmbMode=new QComboBox(parentWidget);
    cmbMode->setVisible(false);
    cmbMode->addItem(tr("edit mask"));
    cmbMode->addItem(tr("edit selection"));
    connect(cmbMode, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbModeChanged(int)));
    actMode=new QWidgetAction(parentWidget);
    actMode->setDefaultWidget(cmbMode);
    actMode->setVisible(false);

    actImagesZoom=new QAction(QIcon(":/qfrdrmaskeditor/zoom.png"), tr("zoom"), this);
    actImagesZoom->setToolTip(tr("in this mode the user may zoom into a plot by drawing a rectangle (draging with the left mouse key)\nA click toggles the current selection/mask position."));
    actImagesZoom->setCheckable(true);
    actImagesDrawPoints=new QAction(QIcon(":/qfrdrmaskeditor/draw_point.png"), tr("point-wise mask editing"), this);
    actImagesDrawPoints->setToolTip(tr("in this mode the user may click on single points.<br>"
                                 "to add and remove them to/from the mask. A click<br>"
                                 "will toggle the state of the clicked pixel<ul>"
                                 "<li>CTRL: pixel will be added to current mask</li>"
                                 "<li>SHIFT: pixel will be removed from current mask</li>"
                                 "</ul>"));
    actImagesDrawPoints->setCheckable(true);
    actImagesDrawRectangle=new QAction(QIcon(":/qfrdrmaskeditor/draw_rectangle.png"), tr("rectangular mask editing"), this);
    actImagesDrawRectangle->setToolTip(tr("in this mode the user may draw a rectangle.<br>"
                                 "All pixels inside the rectangle will be selected<br>"
                                 "when the user releases the left mouse key. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: pixels will be added to current mask</li>"
                                 "<li>SHIFT: pixels will be removed from current mask</li>"
                                 "</ul>"));
    actImagesDrawRectangle->setCheckable(true);

    actImagesDrawCircle=new QAction(QIcon(":/qfrdrmaskeditor/draw_circle.png"), tr("circular mask editing"), this);
    actImagesDrawCircle->setToolTip(tr("in this mode the user may draw a circle.<br>"
                                 "All pixels inside the cirle will be selected<br>"
                                 "when the user releases the left mouse key. The point of first<br>"
                                 "click will be the center of the circle. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: pixels will be added to current mask</li>"
                                 "<li>SHIFT: pixels will be removed from current mask</li>"
                                 "</ul>"));
    actImagesDrawCircle->setCheckable(true);
    actImagesDrawEllipse=new QAction(QIcon(":/qfrdrmaskeditor/draw_ellipse.png"), tr("elliptical mask editing"), this);
    actImagesDrawEllipse->setToolTip(tr("in this mode the user may draw a ellipse.<br>"
                                 "All pixels inside the ellipse will be selected<br>"
                                "when the user releases the left mouse key. The point of first<br>"
                                "click will be the center of the ellipse. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: pixels will be added to current mask</li>"
                                 "<li>SHIFT: pixels will be removed from current mask</li>"
                                 "</ul>"));
    actImagesDrawEllipse->setCheckable(true);
    actImagesDrawLine=new QAction(QIcon(":/qfrdrmaskeditor/draw_line.png"), tr("line mask editing"), this);
    actImagesDrawLine->setToolTip(tr("in this mode the user may draw a line.<br>"
                                 "All pixels on the line will be selected<br>"
                                 "when the user releases the left mouse key. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: pixels will be added to current mask</li>"
                                 "<li>SHIFT: pixels will be removed from current mask</li>"
                                 "</ul>"));
    actImagesDrawLine->setCheckable(true);

    actImagesScribble=new QAction(QIcon(":/qfrdrmaskeditor/draw_scribble.png"), tr("scribble mask editing"), this);
    actImagesScribble->setToolTip(tr("in this mode the user may select/deselect pixels by.<br>"
                                 "keeping the left mouse button pressed and moving the mouse<br>"
                                 "over the image. Depending on the key pressed on the keyboard,<br>"
                                 "different actions are executed:<ul>"
                                 "<li>CTRL: the old mask is not deleted, so the result is appended to it.</li>"
                                 "<li>SHIFT: pixels will be removed from current mask</li>"
                                 "</ul>"));
    actImagesScribble->setCheckable(true);
    agImageSelectionActions=new QActionGroup(this);
    agImageSelectionActions->setExclusive(true);
    agImageSelectionActions->addAction(actImagesZoom);
    agImageSelectionActions->addAction(actImagesDrawPoints);
    agImageSelectionActions->addAction(actImagesDrawRectangle);
    agImageSelectionActions->addAction(actImagesScribble);
    agImageSelectionActions->addAction(actImagesDrawLine);
    agImageSelectionActions->addAction(actImagesDrawCircle);
    agImageSelectionActions->addAction(actImagesDrawEllipse);
    connect(agImageSelectionActions, SIGNAL(triggered(QAction*)), this, SLOT(setImageEditMode()));

    actImagesZoom->setChecked(true);

    menuSpecials=new QMenu(tr("specials"), parentWidget);
    menuSpecials->addAction(actUndoMask);
    menuSpecials->addAction(actRedoMask);
    menuSpecials->addAction(actCopyMaskToGroup);
    menuSpecials->addAction(actCopyMaskToFiles);
    menuSpecials->addAction(actCopyMaskToFilesOfSameType);
    menuSpecials->addAction(actMaskBorder);
    menuSpecials->addAction(actMaskByImage);

    timUpdateAfterClick=new QTimer(this);
    timUpdateAfterClick->setSingleShot(true);
    timUpdateAfterClick->setInterval(CLICK_UPDATE_TIMEOUT);
    timUpdateAfterClick->stop();
    connect(timUpdateAfterClick, SIGNAL(timeout()), this, SLOT(updateAfterClick()));

    setRDR(rdr);

}

void QFRDRImageMaskEditTools::setRDR(QFRawDataRecord *rdr)
{
    if (this->rdr)  {
        this->rdr->setQFProperty(settingsPrefix+"QFRDRImageMaskEditTools_undostack", undos, false, false);
        this->rdr->setQFProperty(settingsPrefix+"QFRDRImageMaskEditTools_undopos", undoPos, false, false);
    }

    this->rdr=rdr;
    imagemask=qobject_cast<QFRDRImageMaskInterface*>(rdr);
    runselection=qobject_cast<QFRDRRunSelectionsInterface*>(rdr);
    overviewimages=qobject_cast<QFRDROverviewImagesInterface*>(rdr);
    actSaveMask->setVisible(imagemask||runselection);
    actLoadMask->setVisible(imagemask||runselection);
    actCopyMask->setVisible(imagemask||runselection);
    actPasteMask->setVisible(imagemask||runselection);
    actClearMask->setVisible(imagemask||runselection);
    actInvertMask->setVisible(imagemask||runselection);
    actMaskBorder->setVisible(imagemask);
    actCopyMaskToGroup->setVisible((imagemask||runselection) && rdr && rdr->getGroup()>=0);
    actCopyMaskToFiles->setVisible((imagemask||runselection) && rdr );
    actCopyMaskToFilesOfSameType->setVisible((imagemask||runselection) && rdr );
    actMaskSelected->setVisible((imagemask||runselection)&&maskEditing&&selectionEditing);
    actUnmaskSelected->setVisible((imagemask||runselection)&&maskEditing&&selectionEditing);
    actMaskByImage->setVisible(imagemask && overviewimages);

    if (rdr)  {
        undos=rdr->getProperty(settingsPrefix+"QFRDRImageMaskEditTools_undostack",QStringList()).toStringList();
        undoPos=rdr->getProperty(settingsPrefix+"QFRDRImageMaskEditTools_undopos", -1).toInt();
        if (undos.size()<0) undoPos=-1;

        if (imagemask)  {
            undos.append(imagemask->maskToString());
            undoPos=undos.size()-1;
        }
    }
    updateUndoActions();

}


void QFRDRImageMaskEditTools::setLeaveout(QList<int> leaveout, bool clearOld)
{
    if (imagemask) {
        if (clearOld) imagemask->maskClear();
        for (int i=0; i<leaveout.size(); i++) {
            int idx=leaveout[i];
            int x=idx%imagemask->maskGetWidth();
            int y=idx/imagemask->maskGetWidth();
            imagemask->maskSet(x, y);
        }
        signalMaskChanged(false, true);
    } else if (runselection) {
        QSet<int> set=leaveout.toSet();
        for (int i=0; i<runselection->leaveoutGetRunCount(); i++) {
            if (set.contains(i)) runselection->leaveoutAddRun(i);
            else if (clearOld) runselection->leaveoutRemoveRun(i);
        }
        signalMaskChanged(false);
    }

}

void QFRDRImageMaskEditTools::signalMaskChanged(bool delayed, bool updateUndoRedo)
{
    if (imagemask && updateUndoRedo)  {
        for (int i=undos.size()-1; i>undoPos; i--) {
            undos.removeAt(i);
        }
        undos.append(imagemask->maskToString());
        undoPos=undos.size()-1;
        //qDebug()<<"undos: size="<<undos.size()<<"pos="<<undoPos;
        updateUndoActions();
    }
    if (runselection && updateUndoRedo)  {
        for (int i=undos.size()-1; i>undoPos; i--) {
            undos.removeAt(i);
        }
        undos.append(runselection->leaveoutToString());
        undoPos=undos.size()-1;
        //qDebug()<<"undos: size="<<undos.size()<<"pos="<<undoPos;
        updateUndoActions();
    }

    if (delayed && useDelay) {
        timUpdateAfterClick->setSingleShot(true);
        timUpdateAfterClick->stop();
        timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);
    } else {
        //qDebug()<<"###### emitting QFRDRImageMaskEditTools::rawDataChanged()";
        emit rawDataChanged();
    }
}

void QFRDRImageMaskEditTools::updateUndoActions()
{
    actUndoMask->setEnabled((imagemask||runselection)&&(undoPos>=0)&&(undoPos<undos.size()) );
    actRedoMask->setEnabled((imagemask||runselection)&&(undoPos<undos.size()-1) );
}

void QFRDRImageMaskEditTools::updateAfterClick()
{
    //qDebug()<<"### emitting QFRDRImageMaskEditTools::rawDataChanged()";
    emit rawDataChanged();
}



void QFRDRImageMaskEditTools::loadMask() {
    if (!imagemask && !runselection) return;
    QString lastMaskDir=ProgramOptions::getConfigValue("QFRDRImageMaskEditTools/lastmaskdir", "").toString();
    QString filename= qfGetOpenFileName(parentWidget, tr("select mask file to open ..."), lastMaskDir, tr("mask files (*.msk)"));
    if (QFile::exists(filename)) {
        if (imagemask) {
            imagemask->maskLoad(filename);
        } else if (runselection) {
            runselection->leaveoutLoad(filename);
        }
        ProgramOptions::setConfigValue("QFRDRImageMaskEditTools/lastmaskdir", lastMaskDir);
    }

    signalMaskChanged(false);
}

void QFRDRImageMaskEditTools::pasteMask()
{
    if (!imagemask && !runselection) return;

    QClipboard* clipboard=QApplication::clipboard();

    const QMimeData* mime=clipboard->mimeData();
    if (mime->hasFormat("quickfit3/pixelselection")) {
        if (imagemask) imagemask->maskLoadFromString(QString::fromUtf8(mime->data("quickfit3/pixelselection")));
    }
    if (mime->hasFormat("quickfit3/runselection")) {
        if (runselection) runselection->leaveoutLoadFromString(QString::fromUtf8(mime->data("quickfit3/runselection")));
    }

    signalMaskChanged(false);
}

void QFRDRImageMaskEditTools::saveMask()
{
    if (!imagemask && !runselection) return;
    QString lastMaskDir=ProgramOptions::getConfigValue("QFRDRImageMaskEditTools/lastmaskdir", "").toString();
    QString filename= qfGetSaveFileName(parentWidget, tr("save mask as ..."), lastMaskDir, tr("mask files (*.msk)"));
    if (!filename.isEmpty()) {
        if (imagemask) imagemask->maskSave(filename);
        else if (runselection) runselection->leaveoutSave(filename);
        ProgramOptions::setConfigValue("QFRDRImageMaskEditTools/lastmaskdir", lastMaskDir);
    }
}

void QFRDRImageMaskEditTools::copyMask()
{
    if (!imagemask && !runselection) return;
    QString mask;
    if (imagemask) mask=imagemask->maskToString();
    else if (runselection) mask=runselection->leaveoutToString();
    QClipboard* clipboard=QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setText(mask);
    if (imagemask) mime->setData("quickfit3/pixelselection", mask.toUtf8());
    else if (runselection) mime->setData("quickfit3/runselection", mask.toUtf8());
    clipboard->setMimeData(mime);
}

void QFRDRImageMaskEditTools::copyMaskToGroup()
{
    if ((!runselection&&!imagemask) || !rdr) return;
    int g=rdr->getGroup();
    QString mask;
    if (imagemask) mask=imagemask->maskToString();
    else if (runselection) mask=runselection->leaveoutToString();

    if (imagemask) {
        for (int r=0; r<rdr->getProject()->getRawDataCount(); r++) {
            QFRawDataRecord* rd=rdr->getProject()->getRawDataByNum(r);
            if (rd && rd->getGroup()==g) {
                QFRDRImageMaskInterface* rm=qobject_cast<QFRDRImageMaskInterface*>(rd);
                if (rm) {
                    rm->maskClear();
                    rm->maskLoadFromString(mask);
                    rm->maskMaskChangedEvent();
                }
            }

        }
    } else if (runselection) {
        for (int r=0; r<rdr->getProject()->getRawDataCount(); r++) {
            QFRawDataRecord* rd=rdr->getProject()->getRawDataByNum(r);
            if (rd && rd->getGroup()==g) {
                QFRDRRunSelectionsInterface* rm=qobject_cast<QFRDRRunSelectionsInterface*>(rd);
                if (rm) {
                    rm->leaveoutClear();
                    rm->leaveoutLoadFromString(mask);
                    rm->leaveoutChangedEvent();
                }
            }

        }

    }
    signalMaskChanged(false, false);

}

void QFRDRImageMaskEditTools::copyMaskToFiles()
{
    if ((!runselection&&!imagemask) || !rdr) return;
    int g=rdr->getGroup();
    QString mask;
    if (imagemask) mask=imagemask->maskToString();
    else if (runselection) mask=runselection->leaveoutToString();

    QList<QPointer<QFRawDataRecord> > rdrs;


    QFSelectRDRDialog* dlg=new QFSelectRDRDialog(new QFMatchRDRFunctorSelectAll(), this);
    dlg->setAllowCreateNew(false);
    dlg->setAllowMultiSelect(true);
    dlg->setProject(rdr->getProject());
    dlg->setDescription(tr("Select all RDRs, to which the current evaluation settings should be copied!"));
    if (dlg->exec()) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        rdrs=dlg->getSelectedRDRs();
    }
    delete dlg;



    if (imagemask) {
        for (int r=0; r<rdrs.size(); r++) {
            QFRawDataRecord* rd=rdrs[r];
            if (rd) {
                QFRDRImageMaskInterface* rm=qobject_cast<QFRDRImageMaskInterface*>(rd);
                if (rm) {
                    rm->maskClear();
                    rm->maskLoadFromString(mask);
                    rm->maskMaskChangedEvent();
                }
            }

        }
    } else if (runselection) {
        for (int r=0; r<rdrs.size(); r++) {
            QFRawDataRecord* rd=rdrs[r];
            if (rd) {
                QFRDRRunSelectionsInterface* rm=qobject_cast<QFRDRRunSelectionsInterface*>(rd);
                if (rm) {
                    rm->leaveoutClear();
                    rm->leaveoutLoadFromString(mask);
                    rm->leaveoutChangedEvent();
                }
            }

        }

    }
    signalMaskChanged(false, false);
}

void QFRDRImageMaskEditTools::copyMaskToFilesOfSameType()
{
    if ((!runselection&&!imagemask) || !rdr) return;
    int g=rdr->getGroup();
    QString mask;
    if (imagemask) mask=imagemask->maskToString();
    else if (runselection) mask=runselection->leaveoutToString();

    QList<QPointer<QFRawDataRecord> > rdrs;


    QFSelectRDRDialog* dlg=new QFSelectRDRDialog(new QFMatchRDRFunctorSelectType(rdr->getType()), this);
    dlg->setAllowCreateNew(false);
    dlg->setAllowMultiSelect(true);
    dlg->setProject(rdr->getProject());
    dlg->setDescription(tr("Select all RDRs, to which the current evaluation settings should be copied!"));
    if (dlg->exec()) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        rdrs=dlg->getSelectedRDRs();
    }
    delete dlg;



    if (imagemask) {
        for (int r=0; r<rdrs.size(); r++) {
            QFRawDataRecord* rd=rdrs[r];
            if (rd) {
                QFRDRImageMaskInterface* rm=qobject_cast<QFRDRImageMaskInterface*>(rd);
                if (rm) {
                    rm->maskClear();
                    rm->maskLoadFromString(mask);
                    rm->maskMaskChangedEvent();
                }
            }

        }
    } else if (runselection) {
        for (int r=0; r<rdrs.size(); r++) {
            QFRawDataRecord* rd=rdrs[r];
            if (rd) {
                QFRDRRunSelectionsInterface* rm=qobject_cast<QFRDRRunSelectionsInterface*>(rd);
                if (rm) {
                    rm->leaveoutClear();
                    rm->leaveoutLoadFromString(mask);
                    rm->leaveoutChangedEvent();
                }
            }

        }

    }
    signalMaskChanged(false, false);
}

void QFRDRImageMaskEditTools::maskSelected()
{
    if (!(maskEditing&&selectionEditing&&selection&&selectionWidth>0&&selectionHeight>0)) return;
    if (imagemask) {
        for (int i=0; i<selectionWidth*selectionHeight; i++) {
            if (selection[i]) {
                int x=i%imagemask->maskGetWidth();
                int y=i/imagemask->maskGetWidth();
                imagemask->maskSet(x, y);
            }
        }
        signalMaskChanged(false, true);
    } else if (runselection) {
        for (int i=0; i<selectionWidth*selectionHeight; i++) {
            if (selection[i]) runselection->leaveoutAddRun(i);
        }
        signalMaskChanged(false);
    }
}

void QFRDRImageMaskEditTools::unmaskSelected()
{
    if (!(maskEditing&&selectionEditing&&selection&&selectionWidth>0&&selectionHeight>0)) return;
    if (imagemask) {
        for (int i=0; i<selectionWidth*selectionHeight; i++) {
            if (selection[i]) {
                int x=i%imagemask->maskGetWidth();
                int y=i/imagemask->maskGetWidth();
                imagemask->maskUnset(x, y);
            }
        }
        signalMaskChanged(false, true);
    } else if (runselection) {
        for (int i=0; i<selectionWidth*selectionHeight; i++) {
            if (selection[i]) runselection->leaveoutRemoveRun(i);
        }
        signalMaskChanged(false);
    }
}

void QFRDRImageMaskEditTools::clearSelection()
{
    if (selection && selectionWidth*selectionHeight>0 && selectionEditing) {
        for (int i=0; i<selectionWidth*selectionHeight; i++) {
            selection[i]=false;
        }
        signalMaskChanged(true, false);
    }
}


void QFRDRImageMaskEditTools::clearMask()
{
    if (!imagemask && !runselection) return;
    if (imagemask) imagemask->maskClear();
    else if (runselection) runselection->leaveoutClear();

    signalMaskChanged(false);
}

void QFRDRImageMaskEditTools::invertMask()
{
    if (!imagemask && !runselection) return;
    if (imagemask) imagemask->maskInvert();
    else if (runselection) runselection->leaveoutInvert();

    signalMaskChanged(false);
}

void QFRDRImageMaskEditTools::undoMask()
{
    if (!imagemask && !runselection) return;

    if (undoPos>=0) {
        QString mask=undos.value(undoPos, "");
        if (imagemask) {
            imagemask->maskClear();
            imagemask->maskLoadFromString(mask);
        } else if (runselection){
            runselection->leaveoutClear();
            runselection->leaveoutLoadFromString(mask);

        }
        undoPos--;
    }

    signalMaskChanged(false, false);
    updateUndoActions();
}

void QFRDRImageMaskEditTools::redoMask()
{
    if (!imagemask && !runselection) return;

    if (undoPos+1<undos.size()) {
        QString mask=undos.value(undoPos+1, "");
        if (imagemask) {
            imagemask->maskClear();
            imagemask->maskLoadFromString(mask);
        } else if (runselection){
            runselection->leaveoutClear();
            runselection->leaveoutLoadFromString(mask);

        }
        undoPos++;
    }

    signalMaskChanged(false, false);
    updateUndoActions();
}

void QFRDRImageMaskEditTools::maskBorder()
{
    if (!imagemask) return;
    QDialog* dlg=new QDialog(parentWidget);
    QGridLayout* grid=new QGridLayout();
    dlg->setLayout(grid);

    QSpinBox* spinTop=new QSpinBox(dlg);
    spinTop->setRange(0, imagemask->maskGetHeight());
    QSpinBox* spinBot=new QSpinBox(dlg);
    spinBot->setRange(0, imagemask->maskGetHeight());
    QSpinBox* spinLeft=new QSpinBox(dlg);
    spinLeft->setRange(0, imagemask->maskGetHeight());
    QSpinBox* spinRight=new QSpinBox(dlg);
    spinRight->setRange(0, imagemask->maskGetHeight());
    QCheckBox* chkSyncX=new QCheckBox(tr("synchronize left/right"), dlg);
    connect(chkSyncX, SIGNAL(toggled(bool)), spinRight, SLOT(setDisabled(bool)));
    QCheckBox* chkSyncY=new QCheckBox(tr("synchronize top/bottom"), dlg);
    connect(chkSyncY, SIGNAL(toggled(bool)), spinBot, SLOT(setDisabled(bool)));
    QCheckBox* chkSyncXY=new QCheckBox(tr("synchronize all"), dlg);
    connect(chkSyncXY, SIGNAL(toggled(bool)), spinBot, SLOT(setDisabled(bool)));
    connect(chkSyncXY, SIGNAL(toggled(bool)), spinLeft, SLOT(setDisabled(bool)));
    connect(chkSyncXY, SIGNAL(toggled(bool)), spinRight, SLOT(setDisabled(bool)));
    QCheckBox* chkOverwrite=new QCheckBox(tr("overwrite old mask"), dlg);
    QDialogButtonBox* btn=new QDialogButtonBox(dlg);
    btn->addButton(QDialogButtonBox::Ok);
    btn->addButton(QDialogButtonBox::Cancel);
    connect(btn, SIGNAL(accepted()), dlg, SLOT(accept()));
    connect(btn, SIGNAL(rejected()), dlg, SLOT(reject()));
    grid->addWidget(new QLabel(tr("cut top:")), 0, 2);
    grid->addWidget(spinTop, 1, 2);
    grid->addWidget(new QLabel(tr("cut left:")), 2, 0);
    grid->addWidget(spinLeft, 3, 0);
    grid->addWidget(new QLabel(tr("cut right:")), 2, 4);
    grid->addWidget(spinRight, 3, 4);
    grid->addWidget(new QLabel(tr("cut bottom:")), 4, 2);
    grid->addWidget(spinBot, 5, 2);
    grid->addWidget(chkSyncX, 6,0,1,4);
    grid->addWidget(chkSyncY, 7,0,1,4);
    grid->addWidget(chkSyncXY, 8,0,1,4);
    grid->addWidget(chkOverwrite, 9,0,1,4);
    grid->addWidget(btn, 10,0,1,4);
    spinBot->setValue(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_bot", 0).toInt());
    spinTop->setValue(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_top", 0).toInt());
    spinLeft->setValue(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_left", 0).toInt());
    spinRight->setValue(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_right", 0).toInt());
    chkSyncX->setChecked(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_syncX", false).toBool());
    chkSyncY->setChecked(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_syncY", false).toBool());
    chkSyncXY->setChecked(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_syncXY", false).toBool());
    chkOverwrite->setChecked(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_overwrite", true).toBool());

    if (dlg->exec()) {
        if (chkSyncX->isChecked()) spinRight->setValue(spinLeft->value());
        if (chkSyncY->isChecked()) spinBot->setValue(spinTop->value());
        if (chkSyncXY->isChecked()) {
            spinBot->setValue(spinTop->value());
            spinRight->setValue(spinTop->value());
            spinLeft->setValue(spinTop->value());
        }
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_bot", spinBot->value());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_top", spinTop->value());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_left", spinLeft->value());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_right", spinRight->value());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_syncX", chkSyncX->isChecked());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_syncY", chkSyncY->isChecked());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_syncXY", chkSyncXY->isChecked());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRImageMaskEditTools_maskBorder_overwrite", chkOverwrite->isChecked());

        if (chkOverwrite->isChecked()) imagemask->maskClear();
        for (uint32_t x=0; x<imagemask->maskGetWidth(); x++) {
            for (uint32_t y=0; y<imagemask->maskGetHeight(); y++) {
                if (!(   x>=(uint64_t)spinLeft->value() && x<imagemask->maskGetWidth()-(uint64_t)spinRight->value()
                    && y>=(uint64_t)spinBot->value() && y<imagemask->maskGetHeight()-(uint64_t)spinTop->value())) {
                    imagemask->maskSet(x,y);
                }
            }

        }

        signalMaskChanged(false, false);
        updateUndoActions();

    }
    delete dlg;

}

void QFRDRImageMaskEditTools::maskByImage()
{
    if (overviewimages && imagemask && overviewimages->getOverviewImageWidth()==imagemask->maskGetWidth() && overviewimages->getOverviewImageHeight()==imagemask->maskGetHeight()) {
        QFRDRMaskByOverviewImage* dlg=new QFRDRMaskByOverviewImage(parentWidget);
        dlg->init(overviewimages);
        if (dlg->exec()) {
            bool* m=dlg->getMaskWithOld(imagemask->maskGet());
            imagemask->maskSet(m);
            signalMaskChanged(false, false);
            updateUndoActions();
        }

        delete dlg;
    }
}


void QFRDRImageMaskEditTools::registerMaskToolsToMenu(QMenu *menu) const
{
    menu->addAction(actUndoMask);
    menu->addAction(actRedoMask);
    menu->addSeparator();
    menu->addAction(actLoadMask);
    menu->addAction(actSaveMask);
    menu->addAction(actCopyMask);
    menu->addAction(actPasteMask);
    menu->addAction(actCopyMaskToGroup);
    menu->addAction(actCopyMaskToFiles);
    menu->addAction(actCopyMaskToFilesOfSameType);
    menu->addSeparator();
    menu->addAction(actMaskSelected);
    menu->addAction(actUnmaskSelected);
    menu->addSeparator();
    menu->addAction(actClearMask);
    menu->addAction(actInvertMask);
    menu->addAction(actMaskBorder);
    menu->addAction(actMaskByImage);

}

void QFRDRImageMaskEditTools::registerMaskToolsToToolbar(QToolBar *menu) const
{
    menu->addAction(actLoadMask);
    menu->addAction(actSaveMask);
    menu->addAction(actCopyMask);
    menu->addAction(actPasteMask);
    menu->addSeparator();
    menu->addAction(actMaskSelected);
    menu->addAction(actUnmaskSelected);
    menu->addSeparator();
    menu->addAction(actClearMask);
    menu->addAction(actInvertMask);
    menu->addSeparator();
    menu->addAction(menuSpecials->menuAction());
}

void QFRDRImageMaskEditTools::registerPlotter(JKQtPlotter *plot)
{
    plotters.removeAll(plot);
    plotters.append(plot);
    connect(plot, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    connect(plot, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));
}

void QFRDRImageMaskEditTools::unregisterPlotter(JKQtPlotter *plot)
{
    disconnect(plot, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    disconnect(plot, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    disconnect(plot, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    disconnect(plot, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    disconnect(plot, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    disconnect(plot, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));
    plotters.removeAll(plot);
}

void QFRDRImageMaskEditTools::registerPlotterMaskToolsToToolbar(QToolBar *menu) const
{
    menu->addAction(actMode);
    menu->addAction(actImagesZoom);
    menu->addAction(actImagesDrawPoints);
    menu->addAction(actImagesDrawRectangle);
    menu->addAction(actImagesDrawLine);
    menu->addAction(actImagesDrawCircle);
    menu->addAction(actImagesDrawEllipse);
    menu->addAction(actImagesScribble);
}

void QFRDRImageMaskEditTools::setUseDelay(bool use)
{
    useDelay=use;
}

void QFRDRImageMaskEditTools::setAllowEditSelection(bool enabled, bool *selectionArray, int width, int height)
{
    this->selection=selectionArray;
    this->selectionWidth=width;
    this->selectionHeight=height;
    this->selectionEditing=enabled;
    if (selectionEditing && !maskEditing) cmbMode->setCurrentIndex(1);
    if (maskEditing && !selectionEditing) cmbMode->setCurrentIndex(0);
    //if (maskEditing && selectionEditing) cmbMode->setCurrentIndex(1);
    actMode->setVisible(maskEditing&&selectionEditing);
    actMaskSelected->setVisible((imagemask||runselection)&&maskEditing&&selectionEditing);
    actUnmaskSelected->setVisible((imagemask||runselection)&&maskEditing&&selectionEditing);

}

bool QFRDRImageMaskEditTools::getAllowEditSelection() const
{
    return selectionEditing;
}

void QFRDRImageMaskEditTools::setMaskEditing(bool enabled)
{
    maskEditing=enabled;

    if (selectionEditing && !maskEditing) cmbMode->setCurrentIndex(1);
    if (maskEditing && !selectionEditing) cmbMode->setCurrentIndex(0);
    //if (maskEditing && selectionEditing) cmbMode->setCurrentIndex(1);
    actMode->setVisible(maskEditing&&selectionEditing);
    actMaskSelected->setVisible((imagemask||runselection)&&maskEditing&&selectionEditing);
    actUnmaskSelected->setVisible((imagemask||runselection)&&maskEditing&&selectionEditing);
}


void QFRDRImageMaskEditTools::imageClicked(double x, double y, Qt::KeyboardModifiers modifiers)
{
    int xx=(int)floor(x);
    int yy=(int)floor(y);
    if (maskEditing &&( !selectionEditing || (selectionEditing&& cmbMode->currentIndex()==0))) {
        if (!imagemask) return;
        if (xx>=0 && xx<(int64_t)imagemask->maskGetWidth() && yy>=0 && yy<(int64_t)imagemask->maskGetHeight()) {

            if (modifiers==Qt::ControlModifier && !actImagesScribble->isChecked()) {
                imagemask->maskToggle(xx,yy);
            } else if (modifiers==Qt::ShiftModifier) {
                imagemask->maskUnset(xx,yy);
            } else {
                if (!actImagesScribble->isChecked()) imagemask->maskClear();
                imagemask->maskSet(xx,yy);
            }
            //qDebug()<<actImagesScribble->isChecked()<<modifiers;
            signalMaskChanged(true);
        }
    } else if (selectionEditing&& cmbMode->currentIndex()==1 && selection && (xx>=0 && xx<selectionWidth && yy>=0 && yy<selectionHeight)) {

            if (modifiers==Qt::ControlModifier && !actImagesScribble->isChecked()) {
                selection[yy*selectionWidth+xx]=!selection[yy*selectionWidth+xx];
            } else if (modifiers==Qt::ShiftModifier) {
                selection[yy*selectionWidth+xx]=false;
            } else {
                if (!actImagesScribble->isChecked()) {
                    for (int i=0; i<selectionWidth*selectionHeight; i++) selection[i]=false;
                }
                selection[yy*selectionWidth+xx]=true;
            }
            signalMaskChanged(true, false);
    }
}

void QFRDRImageMaskEditTools::imageScribbled(double x, double y, Qt::KeyboardModifiers modifiers, bool first, bool last)
{

    int xx=(int)floor(x);
    int yy=(int)floor(y);

    if (maskEditing && (!selectionEditing || (selectionEditing&& cmbMode->currentIndex()==0))) {
        if (!imagemask) return;
        if (xx>=0 && xx<(int64_t)imagemask->maskGetWidth() && yy>=0 && yy<(int64_t)imagemask->maskGetHeight()) {

            if (first && modifiers==Qt::NoModifier) imagemask->maskClear();
            if (modifiers==Qt::ShiftModifier) {
                imagemask->maskUnset(xx,yy);
            } else {
                imagemask->maskSet(xx,yy);
            }
            signalMaskChanged(true);
        }
    } else if (selectionEditing&& cmbMode->currentIndex()==1 && selection && (xx>=0 && xx<selectionWidth && yy>=0 && yy<selectionHeight)) {
        if (first && modifiers==Qt::NoModifier) {
            for (int i=0; i<selectionWidth*selectionHeight; i++) selection[i]=false;
        };
        if (modifiers==Qt::ShiftModifier) {
            selection[yy*selectionWidth+xx]=false;
        } else {
            selection[yy*selectionWidth+xx]=true;
        }
        signalMaskChanged(true, false);

    }

}


void QFRDRImageMaskEditTools::imageRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers)
{
    //qDebug()<<"QFRDRImageMaskEditTools::imageRectangleFinished("<<x<<y<<width<<height<<")";
    //qDebug()<<"maskEditing="<<maskEditing<<"   selectionEditing="<<selectionEditing<<"   cmbMode->currentIndex()="<<cmbMode->currentIndex();

    if (maskEditing && (!selectionEditing || (selectionEditing&& cmbMode->currentIndex()==0))) {

        if (!imagemask) return;
        int xx1=qBound(0,(int)floor(x), (int)imagemask->maskGetWidth()-1);
        int yy1=qBound(0,(int)floor(y), (int)imagemask->maskGetHeight()-1);
        int xx2=qBound(0,(int)floor(x+width), (int)imagemask->maskGetWidth()-1);
        int yy2=qBound(0,(int)floor(y+height), (int)imagemask->maskGetHeight()-1);

        if (xx1>xx2) qSwap(xx1, xx2);
        if (yy1>yy2) qSwap(yy1, yy2);

        if (modifiers==Qt::ControlModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    imagemask->maskSet(xx,yy);
                    //qDebug()<<"  u"<<xx<<yy;
                }
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    imagemask->maskUnset(xx,yy);
                    //qDebug()<<"  s"<<xx<<yy;
                }
            }
        } else {
            imagemask->maskClear();
            //qDebug()<<"  c";
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    imagemask->maskSet(xx,yy);
                    //qDebug()<<"  u"<<xx<<yy;
                }
            }
        }
        signalMaskChanged(true);
    } else if (selectionEditing&& cmbMode->currentIndex()==1 && selection) {

        int xx1=qBound(0,(int)floor(x), selectionWidth-1);
        int yy1=qBound(0,(int)floor(y), selectionHeight-1);
        int xx2=qBound(0,(int)floor(x+width), selectionWidth-1);
        int yy2=qBound(0,(int)floor(y+height), selectionHeight-1);

        if (xx1>xx2) qSwap(xx1, xx2);
        if (yy1>yy2) qSwap(yy1, yy2);

        if (modifiers==Qt::ControlModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    selection[yy*selectionWidth+xx]=true;
                    //qDebug()<<"  u"<<xx<<yy;
                }
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    selection[yy*selectionWidth+xx]=false;
                    //qDebug()<<"  s"<<xx<<yy;
                }
            }
        } else {
            for (int i=0; i<selectionWidth*selectionHeight; i++) selection[i]=false;
            //qDebug()<<"  c";
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    selection[yy*selectionWidth+xx]=true;
                    //qDebug()<<"  u"<<xx<<yy;
                }
            }
        }
        signalMaskChanged(true, false);
    }

}

void QFRDRImageMaskEditTools::imageLineFinished(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers)
{
    if (maskEditing && (!selectionEditing || (selectionEditing&& cmbMode->currentIndex()==0))) {
        if (!imagemask) return;

        QLineF line(x1, y1, x2, y2);

        if (modifiers==Qt::ControlModifier) {
            for (double i=0; i<1.0; i=i+0.5/double(qMax(imagemask->maskGetWidth(), imagemask->maskGetHeight()))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), (int)imagemask->maskGetWidth()-1);
                int yy=qBound(0,(int)floor(p.y()), (int)imagemask->maskGetHeight()-1);
                imagemask->maskSet(xx,yy);
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (double i=0; i<1.0; i=i+0.5/double(qMax(imagemask->maskGetWidth(), imagemask->maskGetHeight()))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), (int)imagemask->maskGetWidth()-1);
                int yy=qBound(0,(int)floor(p.y()), (int)imagemask->maskGetHeight()-1);
                imagemask->maskUnset(xx,yy);
            }
        } else {
            imagemask->maskClear();
            for (double i=0; i<1.0; i=i+0.5/double(qMax(imagemask->maskGetWidth(), imagemask->maskGetHeight()))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), (int)imagemask->maskGetWidth()-1);
                int yy=qBound(0,(int)floor(p.y()), (int)imagemask->maskGetHeight()-1);
                imagemask->maskSet(xx,yy);
            }
        }

        signalMaskChanged(true);
    } else if (selectionEditing&& cmbMode->currentIndex()==1 && selection) {
        QLineF line(x1, y1, x2, y2);

        if (modifiers==Qt::ControlModifier) {
            for (double i=0; i<1.0; i=i+0.5/double(qMax(selectionWidth, selectionHeight))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), selectionWidth-1);
                int yy=qBound(0,(int)floor(p.y()), selectionHeight-1);
                selection[yy*selectionWidth+xx]=true;
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (double i=0; i<1.0; i=i+0.5/double(qMax(selectionWidth, selectionHeight))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), selectionWidth-1);
                int yy=qBound(0,(int)floor(p.y()), selectionHeight-1);
                selection[yy*selectionWidth+xx]=false;
            }
        } else {
            for (int i=0; i<selectionWidth*selectionHeight; i++) selection[i]=false;
            for (double i=0; i<1.0; i=i+0.5/double(qMax(selectionWidth, selectionHeight))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), selectionWidth-1);
                int yy=qBound(0,(int)floor(p.y()), selectionHeight-1);
                selection[yy*selectionWidth+xx]=true;
            }
        }

        signalMaskChanged(true, false);
    }


}

void QFRDRImageMaskEditTools::imageCircleFinished(double x, double y, double radius, Qt::KeyboardModifiers modifiers)
{
    if (maskEditing && (!selectionEditing || (selectionEditing&& cmbMode->currentIndex()==0))) {
        if (!imagemask) return;


        int xx1=qBound(0,(int)floor(x-radius), (int)imagemask->maskGetWidth()-1);
        int yy1=qBound(0,(int)floor(y-radius), (int)imagemask->maskGetHeight()-1);
        int xx2=qBound(0,(int)floor(x+radius), (int)imagemask->maskGetWidth()-1);
        int yy2=qBound(0,(int)floor(y+radius), (int)imagemask->maskGetHeight()-1);


        if (xx1>xx2) qSwap(xx1, xx2);
        if (yy1>yy2) qSwap(yy1, yy2);

        if (modifiers==Qt::ControlModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) imagemask->maskSet(xx,yy);
                }
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) imagemask->maskUnset(xx,yy);
                }
            }
        } else {
            imagemask->maskClear();
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) imagemask->maskSet(xx,yy);
                }
            }
        }

        signalMaskChanged(true);
    } else if (selectionEditing&& cmbMode->currentIndex()==1 && selection) {
        int xx1=qBound(0,(int)floor(x-radius), selectionWidth-1);
        int yy1=qBound(0,(int)floor(y-radius), selectionHeight-1);
        int xx2=qBound(0,(int)floor(x+radius), selectionWidth-1);
        int yy2=qBound(0,(int)floor(y+radius), selectionHeight-1);


        if (xx1>xx2) qSwap(xx1, xx2);
        if (yy1>yy2) qSwap(yy1, yy2);

        if (modifiers==Qt::ControlModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) selection[yy*selectionWidth+xx]=true;
                }
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) selection[yy*selectionWidth+xx]=false;
                }
            }
        } else {
            for (int i=0; i<selectionWidth*selectionHeight; i++) selection[i]=false;
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) selection[yy*selectionWidth+xx]=true;
                }
            }
        }

        signalMaskChanged(true);
    }

}

void QFRDRImageMaskEditTools::imageEllipseFinished(double x, double y, double radiusX, double radiusY, Qt::KeyboardModifiers modifiers)
{
    if (maskEditing && (!selectionEditing || (selectionEditing&& cmbMode->currentIndex()==0))) {
        if (!imagemask) return;

        int xx1=qBound(0,(int)floor(x-radiusX), (int)imagemask->maskGetWidth()-1);
        int yy1=qBound(0,(int)floor(y-radiusY), (int)imagemask->maskGetHeight()-1);
        int xx2=qBound(0,(int)floor(x+radiusX), (int)imagemask->maskGetWidth()-1);
        int yy2=qBound(0,(int)floor(y+radiusY), (int)imagemask->maskGetHeight()-1);

        if (xx1>xx2) qSwap(xx1, xx2);
        if (yy1>yy2) qSwap(yy1, yy2);


            if (modifiers==Qt::ControlModifier) {
                for (int yy=yy1; yy<=yy2; yy++) {
                    for (int xx=xx1; xx<=xx2; xx++) {
                        if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) imagemask->maskSet(xx,yy);
                    }
                }
            } else if (modifiers==Qt::ShiftModifier) {
                for (int yy=yy1; yy<=yy2; yy++) {
                    for (int xx=xx1; xx<=xx2; xx++) {
                        if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) imagemask->maskUnset(xx,yy);
                    }
                }
            } else {
                imagemask->maskClear();
                for (int yy=yy1; yy<=yy2; yy++) {
                    for (int xx=xx1; xx<=xx2; xx++) {
                        if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) imagemask->maskSet(xx,yy);
                    }
                }
            }

        signalMaskChanged(true);
    } else if (selectionEditing&& cmbMode->currentIndex()==1 && selection) {

        int xx1=qBound(0,(int)floor(x-radiusX), selectionWidth-1);
        int yy1=qBound(0,(int)floor(y-radiusY), selectionHeight-1);
        int xx2=qBound(0,(int)floor(x+radiusX), selectionWidth-1);
        int yy2=qBound(0,(int)floor(y+radiusY), selectionHeight-1);

        if (xx1>xx2) qSwap(xx1, xx2);
        if (yy1>yy2) qSwap(yy1, yy2);


            if (modifiers==Qt::ControlModifier) {
                for (int yy=yy1; yy<=yy2; yy++) {
                    for (int xx=xx1; xx<=xx2; xx++) {
                        if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) selection[yy*selectionWidth+xx]=true;
                    }
                }
            } else if (modifiers==Qt::ShiftModifier) {
                for (int yy=yy1; yy<=yy2; yy++) {
                    for (int xx=xx1; xx<=xx2; xx++) {
                        if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) selection[yy*selectionWidth+xx]=false;
                    }
                }
            } else {
                for (int i=0; i<selectionWidth*selectionHeight; i++) selection[i]=false;
                for (int yy=yy1; yy<=yy2; yy++) {
                    for (int xx=xx1; xx<=xx2; xx++) {
                        if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) selection[yy*selectionWidth+xx]=true;
                    }
                }
            }

        signalMaskChanged(true, false);
    }

}



void QFRDRImageMaskEditTools::setImageEditMode() {
    for (int i=0; i<plotters.size(); i++) {
        if (plotters[i]) {
            if (actImagesZoom->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::ZoomRectangle);
            } else if (actImagesDrawPoints->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::ClickEvents);
            } else if (actImagesDrawRectangle->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::RectangleEvents);
            } else if (actImagesDrawLine->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::LineEvents);
            } else if (actImagesDrawCircle->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::CircleEvents);
            } else if (actImagesDrawEllipse->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::EllipseEvents);
            } else if (actImagesScribble->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::ScribbleEvents);
            }
        }
    }


}

void QFRDRImageMaskEditTools::cmbModeChanged(int index)
{
}


bool QFRDRImageMaskEditTools::getMaskEditing() const
{
    return maskEditing;
}

void QFRDRImageMaskEditTools::copyMask(bool *data, int maskSize)
{
    if (!(maskEditing)) return;
    if (imagemask) {
        memcpy(data, imagemask->maskGet(), imagemask->maskGetWidth()*imagemask->maskGetHeight()*sizeof(bool));
    } else if (runselection) {
        QList<int> l = runselection->leaveoutToIndexList();
        for (int i=0; i<maskSize; i++) {
            data[i]=l.contains(i);
        }
    }

}

void QFRDRImageMaskEditTools::setMask(bool *data, int maskSize)
{
    if (!(maskEditing)) return;
    if (imagemask) {
        memcpy( imagemask->maskGet(), data, qMin(maskSize, (int)imagemask->maskGetWidth()*(int)imagemask->maskGetHeight())*(int)sizeof(bool));
    } else if (runselection) {
        for (int i=0; i<maskSize; i++) {
            if (data[i]) runselection->leaveoutAddRun(i);
            else runselection->leaveoutRemoveRun(i);
        }
    }
}

void QFRDRImageMaskEditTools::setEditingMode(int idx)
{
    cmbMode->setCurrentIndex(idx);
}
