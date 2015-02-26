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


#ifndef QFRDRCOLUMNGRAPHICSCOMBOBOX_H
#define QFRDRCOLUMNGRAPHICSCOMBOBOX_H

#include "qfrdrcombobox.h"
#include "qfrdrcolumngraphsinterface.h"
#include "qfrdrtableinterface.h"
#include "libwid_imexport.h"
#include "qfmatchrdrfunctor.h"
#include "qfrdrtablecombobox.h"



/** \brief a special QComboBox which displays all graphs available in a QFRDRColumnGraphsInterface
  * \ingroup qf3lib_widgets
  *
  */
class QFWIDLIB_EXPORT QFRDRColumnGraphsComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFRDRColumnGraphsComboBox(QWidget *parent = 0);
        virtual ~QFRDRColumnGraphsComboBox();
        QFRDRColumnGraphsInterface* currentColumnGraphics() const;
    signals:
        void refilledEmpty();
        void refilled(bool full);
    public slots:
        void setColumnGraphs(QFRDRColumnGraphsInterface* cols);
        void refill();
    protected slots:
        virtual void myCurrentIndexChanged(int i);
    protected:
        QFRDRColumnGraphsInterface* cols;
};

#endif // QFRDRCOLUMNGRAPHICSCOMBOBOX_H
