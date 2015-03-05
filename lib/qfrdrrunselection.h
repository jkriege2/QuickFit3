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

#ifndef QFRDRRUNSELECTIONINTERFACE_H
#define QFRDRRUNSELECTIONINTERFACE_H

#include <QtPlugin>
#include <QString>
#include <QList>
#include <QChar>
#include <QVector>

/*! \brief interface for run selection/leaveout
    \ingroup qf3rdrdp_fcs

*/
class QFRDRRunSelectionsInterface {
    public:
        virtual ~QFRDRRunSelectionsInterface() {};


        /** \brief returns the number of runs in the record */
        virtual int leaveoutGetRunCount() const=0;
        /** \brief returns whether to leave out a run */
        virtual bool leaveoutRun(int run) const=0 ;
        /** \brief add a run to the leaveouts */
        virtual void leaveoutAddRun(int run)=0;
        /** \brief remove a run from the leaveouts */
        virtual void leaveoutRemoveRun(int run)=0;
        /** \brief clear all leaveouts */
        virtual void leaveoutClear()=0;




        virtual void leaveoutLoad(const QString& filename)=0;
        virtual void leaveoutLoadFromString(const QString &leaveoutstring)=0;
        virtual void leaveoutSave(const QString& filename) const =0;
        virtual QString leaveoutToString() const=0;
        virtual QString leaveoutToIndexString(QChar separator=QChar(',')) const=0;
        virtual void leaveoutLoadFromIndexString(const QString& data, QChar separator=QChar(','))=0;
        virtual void leaveoutSetAll()=0;
        virtual void leaveoutInvert()=0;
        virtual QList<int> leaveoutToIndexList()=0;
        virtual void leaveoutChangedEvent()=0;


};


inline QVector<bool> QFRDRRunSelectionsInterface_getRunSelectionAsBoolVec(QFRDRRunSelectionsInterface* rs) {
    QVector<bool> b;
    if (rs) {
        for (int i=0; i<rs->leaveoutGetRunCount(); i++) {
            b.append(!rs->leaveoutRun(i));
        }
    }
    return b;
}

inline QVector<bool> QFRDRRunSelectionsInterface_setRunSelectionFromBoolVec(QFRDRRunSelectionsInterface* rs, bool* mask) {
    QVector<bool> b;
    if (rs) {
        rs->leaveoutClear();
        for (int i=0; i<rs->leaveoutGetRunCount(); i++) {
            if (mask[i]) rs->leaveoutAddRun(i);
        }
    }
    return b;
}

Q_DECLARE_INTERFACE( QFRDRRunSelectionsInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRRunSelectionsInterface/1.1")

#endif // QFRDRRUNSELECTIONINTERFACE_H
