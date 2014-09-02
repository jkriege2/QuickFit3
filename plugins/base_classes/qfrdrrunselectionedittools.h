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

#ifndef QFRDRRUNSELECTIONEDITTOOLS_H
#define QFRDRRUNSELECTIONEDITTOOLS_H

#include <QObject>
#include "qfrdrrunselection.h"
#include "qfrdrrunselectiontools.h"
#include <QAction>
#include <QWidget>
#include <QPointer>
#include "qfrawdatarecord.h"


class QFRDRRunSelectionEditTools: public QObject {
        Q_OBJECT
    public:
        QFRDRRunSelectionEditTools(QWidget *parentWidget = 0, const QString& settingsPrefix="");


        virtual void setRDR(QFRawDataRecord* rdr);
        virtual void registerMaskToolsToMenu(QMenu* menu) const;

        QAction* get_actCopyMask() const { return actCopyMask; }
        QAction* get_actPasteMask() const { return actPasteMask; }
        QAction* get_actLoadMask() const { return actLoadMask; }
        QAction* get_actSaveMask() const { return actSaveMask; }
        QAction* get_actClearMask() const { return actClearMask; }
        QAction* get_actInvertMask() const { return actInvertMask; }
        QAction* get_actUndoMask() const { return actUndoMask; }
        QAction* get_actRedoMask() const { return actRedoMask; }
        QAction* get_actMaskBorder() const { return actMaskBorder; }


    public slots:
        void loadMask();
        void pasteMask();
        void saveMask();
        void copyMask();
        void clearMask();
        void invertMask();
        void undoMask();
        void redoMask();
        void copyMaskToGroup();
    signals:
        void rawDataChanged();

    protected slots:
        void setLeaveout(QList<int> leaveout, bool clearOld=false);

        void signalMaskChanged(bool delayed, bool updateUndoRedo=true);
        void updateUndoActions();
    protected:
        QWidget* parentWidget;
        QPointer<QFRawDataRecord> rdr;
        QAction* actCopyMask;
        QAction* actPasteMask;
        QAction* actLoadMask;
        QAction* actSaveMask;
        QAction* actClearMask;
        QAction* actInvertMask;
        QAction* actCopyMaskToGroup;

        QAction* actUndoMask;
        QAction* actRedoMask;



        QFRDRRunSelectionsInterface* runselection;

        QStringList undos;
        int undoPos;
        QString settingsPrefix;

};

#endif // QFRDRRUNSELECTIONEDITTOOLS_H
