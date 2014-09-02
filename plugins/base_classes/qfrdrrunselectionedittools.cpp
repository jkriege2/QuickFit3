/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center
	
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

#include "qfrdrrunselectionedittools.h"

#include "programoptions.h"
#include "qftools.h"
#include <QClipboard>
#include <QButtonGroup>
#define sqr(x) ((x)*(x))

#define CLICK_UPDATE_TIMEOUT 500

QFRDRRunSelectionEditTools::QFRDRRunSelectionEditTools(QWidget *parentWidget, const QString& settingsPrefix):
    QObject(parentWidget)
{
    this->parentWidget=parentWidget;
    this->settingsPrefix=settingsPrefix;
    rdr=NULL;


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


    actCopyMaskToGroup=new QAction(tr("copy mask to all files in same &group"), parentWidget);
    actCopyMaskToGroup->setToolTip(tr(""));
    connect(actCopyMaskToGroup, SIGNAL(triggered()), this, SLOT(copyMaskToGroup()));




    setRDR(rdr);

}

void QFRDRRunSelectionEditTools::setRDR(QFRawDataRecord *rdr)
{
    if (this->rdr)  {
        this->rdr->setQFProperty(settingsPrefix+"QFRDRRunSelectionEditTools_undostack", undos, false, false);
        this->rdr->setQFProperty(settingsPrefix+"QFRDRRunSelectionEditTools_undopos", undoPos, false, false);
    }

    this->rdr=rdr;
    runselection=qobject_cast<QFRDRRunSelectionsInterface*>(rdr);
    actSaveMask->setEnabled(imagemask);
    actLoadMask->setEnabled(imagemask);
    actCopyMask->setEnabled(imagemask);
    actPasteMask->setEnabled(imagemask);
    actClearMask->setEnabled(imagemask);
    actInvertMask->setEnabled(imagemask);
    actCopyMaskToGroup->setEnabled(rdr && rdr->getGroup()>=0);

    if (rdr)  {
        undos=rdr->getProperty(settingsPrefix+"QFRDRRunSelectionEditTools_undostack",QStringList()).toStringList();
        undoPos=rdr->getProperty(settingsPrefix+"QFRDRRunSelectionEditTools_undopos", -1).toInt();
        if (undos.size()<0) undoPos=-1;

        if (imagemask)  {
            undos.append(imagemask->maskToString());
            undoPos=undos.size()-1;
        }
    }
    updateUndoActions();

}


void QFRDRRunSelectionEditTools::setLeaveout(QList<int> leaveout, bool clearOld)
{
    if (runselection) {
        QSet<int> set=leaveout.toSet();
        for (int i=0; i<runselection->leaveoutGetRunCount(); i++) {
            if (set.contains(i)) runselection->leaveoutAddRun(i);
            else if (clearOld) runselection->leaveoutRemoveRun(i);
        }
        signalMaskChanged(false);
    }

}

void QFRDRRunSelectionEditTools::signalMaskChanged(bool delayed, bool updateUndoRedo)
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

    if (delayed && useDelay) {
        timUpdateAfterClick->setSingleShot(true);
        timUpdateAfterClick->stop();
        timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);
    } else {
        //qDebug()<<"###### emitting QFRDRRunSelectionEditTools::rawDataChanged()";
        emit rawDataChanged();
    }
}

void QFRDRRunSelectionEditTools::updateUndoActions()
{
    actUndoMask->setEnabled(imagemask&&(undoPos>=0)&&(undoPos<undos.size()) );
    actRedoMask->setEnabled(imagemask&&(undoPos<undos.size()-1) );
}

void QFRDRRunSelectionEditTools::updateAfterClick()
{
    //qDebug()<<"### emitting QFRDRRunSelectionEditTools::rawDataChanged()";
    emit rawDataChanged();
}



void QFRDRRunSelectionEditTools::loadMask() {
    if (!imagemask) return;
    QString lastMaskDir=ProgramOptions::getConfigValue("QFRDRRunSelectionEditTools/lastmaskdir", "").toString();
    QString filename= qfGetOpenFileName(parentWidget, tr("select mask file to open ..."), lastMaskDir, tr("mask files (*.msk)"));
    if (QFile::exists(filename)) {
        if (imagemask) {
            imagemask->maskLoad(filename);
        }
        ProgramOptions::setConfigValue("QFRDRRunSelectionEditTools/lastmaskdir", lastMaskDir);
    }

    signalMaskChanged(false);
}

void QFRDRRunSelectionEditTools::pasteMask()
{
    if (!imagemask) return;

    QClipboard* clipboard=QApplication::clipboard();

    const QMimeData* mime=clipboard->mimeData();
    if (mime->hasFormat("quickfit3/pixelselection")) {
        imagemask->maskLoadFromString(QString::fromUtf8(mime->data("quickfit3/pixelselection")));
    }

    signalMaskChanged(false);
}

void QFRDRRunSelectionEditTools::saveMask()
{
    if (!imagemask) return;
    QString lastMaskDir=ProgramOptions::getConfigValue("QFRDRRunSelectionEditTools/lastmaskdir", "").toString();
    QString filename= qfGetSaveFileName(parentWidget, tr("save mask as ..."), lastMaskDir, tr("mask files (*.msk)"));
    if (!filename.isEmpty()) {
        imagemask->maskSave(filename);
        ProgramOptions::setConfigValue("QFRDRRunSelectionEditTools/lastmaskdir", lastMaskDir);
    }
}

void QFRDRRunSelectionEditTools::copyMask()
{
    if (!imagemask) return;
    QString mask=imagemask->maskToString();
    QClipboard* clipboard=QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setText(mask);
    mime->setData("quickfit3/pixelselection", mask.toUtf8());
    clipboard->setMimeData(mime);
}

void QFRDRRunSelectionEditTools::copyMaskToGroup()
{
    if (!imagemask || !rdr) return;
    int g=rdr->getGroup();
    QString mask=imagemask->maskToString();

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
    signalMaskChanged(false, false);

}


void QFRDRRunSelectionEditTools::clearMask()
{
    if (!imagemask) return;
    imagemask->maskClear();

    signalMaskChanged(false);
}

void QFRDRRunSelectionEditTools::invertMask()
{
    //qDebug()<<"invertmask()   imagemask="<<imagemask;
    if (!imagemask) return;
    imagemask->maskInvert();

    signalMaskChanged(false);
}

void QFRDRRunSelectionEditTools::undoMask()
{
    if (!imagemask) return;

    if (undoPos>=0) {
        QString mask=undos.value(undoPos, "");
        imagemask->maskClear();
        imagemask->maskLoadFromString(mask);
        undoPos--;
    }

    signalMaskChanged(false, false);
    updateUndoActions();
}

void QFRDRRunSelectionEditTools::redoMask()
{
    if (!imagemask) return;

    if (undoPos+1<undos.size()) {
        QString mask=undos.value(undoPos+1, "");
        imagemask->maskClear();
        imagemask->maskLoadFromString(mask);
        undoPos++;
    }

    signalMaskChanged(false, false);
    updateUndoActions();
}

void QFRDRRunSelectionEditTools::maskBorder()
{
    if (!imagemask) return;
    QDialog* dlg=new QDialog(parentWidget);
    QGridLayout* grid=new QGridLayout(dlg);
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
    spinBot->setValue(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_bot", 0).toInt());
    spinTop->setValue(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_top", 0).toInt());
    spinLeft->setValue(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_left", 0).toInt());
    spinRight->setValue(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_right", 0).toInt());
    chkSyncX->setChecked(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_syncX", false).toBool());
    chkSyncY->setChecked(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_syncY", false).toBool());
    chkSyncXY->setChecked(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_syncXY", false).toBool());
    chkOverwrite->setChecked(ProgramOptions::getConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_overwrite", true).toBool());

    if (dlg->exec()) {
        if (chkSyncX->isChecked()) spinRight->setValue(spinLeft->value());
        if (chkSyncY->isChecked()) spinBot->setValue(spinTop->value());
        if (chkSyncXY->isChecked()) {
            spinBot->setValue(spinTop->value());
            spinRight->setValue(spinTop->value());
            spinLeft->setValue(spinTop->value());
        }
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_bot", spinBot->value());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_top", spinTop->value());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_left", spinLeft->value());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_right", spinRight->value());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_syncX", chkSyncX->isChecked());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_syncY", chkSyncY->isChecked());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_syncXY", chkSyncXY->isChecked());
        ProgramOptions::setConfigValue(settingsPrefix+"QFRDRRunSelectionEditTools_maskBorder_overwrite", chkOverwrite->isChecked());

        if (chkOverwrite->isChecked()) imagemask->maskClear();
        for (uint16_t x=0; x<imagemask->maskGetWidth(); x++) {
            for (uint16_t y=0; y<imagemask->maskGetHeight(); y++) {
                if (!(   x>=spinLeft->value() && x<imagemask->maskGetWidth()-spinRight->value()
                    && y>=spinBot->value() && y<imagemask->maskGetHeight()-spinTop->value())) {
                    imagemask->maskSet(x,y);
                }
            }

        }

        signalMaskChanged(false, false);
        updateUndoActions();

    }
    delete dlg;

}


void QFRDRRunSelectionEditTools::registerMaskToolsToMenu(QMenu *menu) const
{
    menu->addAction(actUndoMask);
    menu->addAction(actRedoMask);
    menu->addSeparator();
    menu->addAction(actLoadMask);
    menu->addAction(actSaveMask);
    menu->addAction(actCopyMask);
    menu->addAction(actPasteMask);
    menu->addAction(actCopyMaskToGroup);
    menu->addSeparator();
    menu->addAction(actClearMask);
    menu->addAction(actInvertMask);
    menu->addAction(actMaskBorder);

}

void QFRDRRunSelectionEditTools::registerPlotter(JKQtPlotter *plot)
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

void QFRDRRunSelectionEditTools::unregisterPlotter(JKQtPlotter *plot)
{
    disconnect(plot, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    disconnect(plot, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    disconnect(plot, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    disconnect(plot, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    disconnect(plot, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    disconnect(plot, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));
    plotters.removeAll(plot);
}

void QFRDRRunSelectionEditTools::registerPlotterMaskToolsToToolbar(QToolBar *menu) const
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

void QFRDRRunSelectionEditTools::setUseDelay(bool use)
{
    useDelay=use;
}

void QFRDRRunSelectionEditTools::setAllowEditSelection(bool enabled, bool *selectionArray, int width, int height)
{
    actMode->setVisible(enabled);
    this->selection=selectionArray;
    this->selectionWidth=width;
    this->selectionHeight=height;
    this->selectionEditing=enabled;
}


void QFRDRRunSelectionEditTools::imageClicked(double x, double y, Qt::KeyboardModifiers modifiers)
{
    int xx=(int)floor(x);
    int yy=(int)floor(y);
    if (!selectionEditing || (selectionEditing&& cmbMode->currentIndex()==0)) {
        if (!imagemask) return;
        if (xx>=0 && xx<imagemask->maskGetWidth() && yy>=0 && yy<imagemask->maskGetHeight()) {

            if (modifiers==Qt::ControlModifier && !actImagesScribble->isChecked()) {
                imagemask->maskToggle(xx,yy);
            } else if (modifiers==Qt::ShiftModifier) {
                imagemask->maskUnset(xx,yy);
            } else {
                if (!actImagesScribble->isChecked()) imagemask->maskClear();
                imagemask->maskSet(xx,yy);
            }
            signalMaskChanged(true);
        }
    } else if (selectionEditing&& cmbMode->currentIndex()==1 && selection && (xx>=0 && xx<selectionWidth && yy>=0 && yy<selectionHeight)) {

            if (modifiers==Qt::ControlModifier && !actImagesScribble->isChecked()) {
                selection[yy*selectionWidth+xx]=!selection[yy*selectionWidth+xx];
            } else if (modifiers==Qt::ShiftModifier) {
                selection[yy*selectionWidth+xx]=false;
            } else {
                selection[yy*selectionWidth+xx]=true;
            }
            signalMaskChanged(true, false);
    }
}

void QFRDRRunSelectionEditTools::imageScribbled(double x, double y, Qt::KeyboardModifiers modifiers, bool first, bool last)
{

    int xx=(int)floor(x);
    int yy=(int)floor(y);

    if (!selectionEditing || (selectionEditing&& cmbMode->currentIndex()==0)) {
        if (!imagemask) return;
        if (xx>=0 && xx<imagemask->maskGetWidth() && yy>=0 && yy<imagemask->maskGetHeight()) {

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


void QFRDRRunSelectionEditTools::imageRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers)
{
    //qDebug()<<"QFRDRRunSelectionEditTools::imageRectangleFinished("<<x<<y<<width<<height<<")";

    if (!selectionEditing || (selectionEditing&& cmbMode->currentIndex()==0)) {

        if (!imagemask) return;
        int xx1=qBound(0,(int)floor(x), imagemask->maskGetWidth()-1);
        int yy1=qBound(0,(int)floor(y), imagemask->maskGetHeight()-1);
        int xx2=qBound(0,(int)floor(x+width), imagemask->maskGetWidth()-1);
        int yy2=qBound(0,(int)floor(y+height), imagemask->maskGetHeight()-1);

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

void QFRDRRunSelectionEditTools::imageLineFinished(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers)
{
    if (!selectionEditing || (selectionEditing&& cmbMode->currentIndex()==0)) {
        if (!imagemask) return;

        QLineF line(x1, y1, x2, y2);

        if (modifiers==Qt::ControlModifier) {
            for (double i=0; i<1.0; i=i+0.5/double(qMax(imagemask->maskGetWidth(), imagemask->maskGetHeight()))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), imagemask->maskGetWidth()-1);
                int yy=qBound(0,(int)floor(p.y()), imagemask->maskGetHeight()-1);
                imagemask->maskSet(xx,yy);
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (double i=0; i<1.0; i=i+0.5/double(qMax(imagemask->maskGetWidth(), imagemask->maskGetHeight()))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), imagemask->maskGetWidth()-1);
                int yy=qBound(0,(int)floor(p.y()), imagemask->maskGetHeight()-1);
                imagemask->maskUnset(xx,yy);
            }
        } else {
            imagemask->maskClear();
            for (double i=0; i<1.0; i=i+0.5/double(qMax(imagemask->maskGetWidth(), imagemask->maskGetHeight()))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), imagemask->maskGetWidth()-1);
                int yy=qBound(0,(int)floor(p.y()), imagemask->maskGetHeight()-1);
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

void QFRDRRunSelectionEditTools::imageCircleFinished(double x, double y, double radius, Qt::KeyboardModifiers modifiers)
{
    if (!selectionEditing || (selectionEditing&& cmbMode->currentIndex()==0)) {
        if (!imagemask) return;


        int xx1=qBound(0,(int)floor(x-radius), imagemask->maskGetWidth()-1);
        int yy1=qBound(0,(int)floor(y-radius), imagemask->maskGetHeight()-1);
        int xx2=qBound(0,(int)floor(x+radius), imagemask->maskGetWidth()-1);
        int yy2=qBound(0,(int)floor(y+radius), imagemask->maskGetHeight()-1);


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

void QFRDRRunSelectionEditTools::imageEllipseFinished(double x, double y, double radiusX, double radiusY, Qt::KeyboardModifiers modifiers)
{
    if (!selectionEditing || (selectionEditing&& cmbMode->currentIndex()==0)) {
        if (!imagemask) return;

        int xx1=qBound(0,(int)floor(x-radiusX), imagemask->maskGetWidth()-1);
        int yy1=qBound(0,(int)floor(y-radiusY), imagemask->maskGetHeight()-1);
        int xx2=qBound(0,(int)floor(x+radiusX), imagemask->maskGetWidth()-1);
        int yy2=qBound(0,(int)floor(y+radiusY), imagemask->maskGetHeight()-1);

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



void QFRDRRunSelectionEditTools::setImageEditMode() {
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

void QFRDRRunSelectionEditTools::cmbModeChanged(int index)
{
}
