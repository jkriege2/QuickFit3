/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:42:55 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3509 $)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFRDRTABLECOMBOBOX_H
#define QFRDRTABLECOMBOBOX_H

#include "qfrdrcombobox.h"
#include "qfrdrtableinterface.h"
#include "lib_imexport.h"
#include "qfmatchrdrfunctor.h"
#include "qfrdrcolumngraphsinterface.h"


/** \brief a special QFMatchRDRFunctor which only matches records that implement QFRDRTableInterface
  * \ingroup qf3lib_widgets
  *
  */
class QFLIB_EXPORT QFRDRTableComboBoxMatchFunctor: public QFMatchRDRFunctor {
    public:
        QFRDRTableComboBoxMatchFunctor(bool requireColumnGraphics=false, bool requireNotReadonly=false);
        virtual bool matches(const QFRawDataRecord* record) const ;
        void setRequireColumnGraphics(bool require);
        void setRequireNotReadonly(bool require);
    protected:
        bool requireColumnGraphics;
        bool requireNotReadonly;
};



/** \brief a special QFRDRComboBox which only displays records that implement QFRDRTableInterface
  * \ingroup qf3lib_widgets
  *
  */
class QFLIB_EXPORT QFRDRTableComboBox : public QFRDRComboBox
{
        Q_OBJECT
    public:
        explicit QFRDRTableComboBox(QWidget *parent = 0);
        virtual ~QFRDRTableComboBox();
        virtual void init(QFProject* project);
        QFRDRColumnGraphsInterface* currentColumnGraphics() const;
        QFRDRTableInterface* currentTable() const;
    signals:
        void currentColumnGraphicsChanged(QFRDRColumnGraphsInterface* graphics);
        void currentTableChanged(QFRDRTableInterface* table);

    public slots:
        void setRequireColumnGraphics(bool require);
        void setRequireNotReadonly(bool require);
    protected slots:
        virtual void myCurrentIndexChanged(int i);
    protected:
        QFRDRTableComboBoxMatchFunctor* tabFunctor;
};



#endif // QFRDRTABLECOMBOBOX_H
