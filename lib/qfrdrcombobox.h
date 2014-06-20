/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

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

#ifndef QFRDRCOMBOBOX_H
#define QFRDRCOMBOBOX_H

#include <QComboBox>
#include "lib_imexport.h"
#include "qfrawdatarecord.h"
#include "qfproject.h"
#include "qfmatchrdrfunctor.h"


/*! \brief combobox that displays a list of all raw data records matching a given QFMatchRDRFunctor in a given project
    \ingroup qf3lib_tools

  */
class QFLIB_EXPORT QFRDRComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFRDRComboBox(QWidget *parent = 0);
        ~QFRDRComboBox();
        
        /** \brief initialises/fills the combobox from the given project and using the given functor.
          *
          * If \a functorPrivate is \c true the functor belongs to this object and will be deleted if no longer needed
          */
        virtual void init(QFProject* project, QFMatchRDRFunctor* matchFunctor, bool functorPrivate=false);
        QFRawDataRecord* currentRDR() const;
        int currentRDRID() const;
    signals:
        void currentRDRChanged(QFRawDataRecord* record);
        void refilledEmpty();
        void refilled(bool full);
    public slots:
        void setCurrentRDR(const QFRawDataRecord* record);
        void setCurrentRDRID(int record);
        void refill();

    protected slots:
        virtual void myCurrentIndexChanged(int i);
    protected:
        QFProject* project;
        QFMatchRDRFunctor* matchFunctor;
        bool functorPrivate;
        
};

#endif // QFRDRCOMBOBOX_H
