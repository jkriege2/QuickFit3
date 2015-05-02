/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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


#ifndef QFFITALGORITHMCOMBOBOX_H
#define QFFITALGORITHMCOMBOBOX_H

#include <QComboBox>
#include "qffitalgorithm.h"
#include "libwid_imexport.h"
#include "qfenhancedcombobox.h"

class QFWIDLIB_EXPORT QFFitAlgorithmComboBox : public QFEnhancedComboBox
{
        Q_OBJECT
    public:
        explicit QFFitAlgorithmComboBox(QWidget *parent = 0);
        QString currentFitAlgorithmID() const;
        QFFitAlgorithm* createCurrentInstance() const;
        QAction* getHelpAction() const { return actHelp; }
    signals:

    public slots:
        void updateFitAlgorithms();
        void setCurrentAlgorithm(const QString& id);
        void showHelpCurrent();
    protected:
        QAction* actHelp;

};

#endif // QFFITALGORITHMCOMBOBOX_H
