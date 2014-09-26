/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#ifndef QFRDRRUNSELECTIONTOOLS_H
#define QFRDRRUNSELECTIONTOOLS_H

#include "qfrdrrunselection.h"

class QFRDRRunSelectionTools : public QFRDRRunSelectionsInterface
{
    public:
        explicit QFRDRRunSelectionTools();
        virtual ~QFRDRRunSelectionTools();
        


        virtual void leaveoutLoad(const QString& filename);
        virtual void leaveoutLoadFromString(const QString &leaveoutstring);
        virtual void leaveoutSave(const QString& filename) const ;
        virtual QString leaveoutToString() const;
        virtual QString leaveoutToIndexString(QChar separator=QChar(',')) const;
        virtual void leaveoutLoadFromIndexString(const QString& data, QChar separator=QChar(','));
        virtual void leaveoutSetAll();
        virtual void leaveoutInvert();
        virtual QList<int> leaveoutToIndexList();
        virtual void leaveoutChangedEvent();


    signals:
        
    public slots:
        
};

#endif // QFRDRRUNSELECTIONTOOLS_H
