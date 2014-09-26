/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-25 17:54:41 +0200 (Do, 25 Sep 2014) $  (revision $Rev: 3506 $)

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

#ifndef QFCOMPLETERPROPERTIES_H
#define QFCOMPLETERPROPERTIES_H

#include <QCompleter>
#include "lib_imexport.h"
#include "qfproject.h"
#include <QStringListModel>
#include <QPointer>

/** \brief completer class that may complete a list of all currently available properties
 *  \ingroup qf3lib_widgets
 */
class QFLIB_EXPORT QFCompleterProperties : public QCompleter
{
        Q_OBJECT
    public:
        QFCompleterProperties(QObject * parent = 0);
        virtual ~QFCompleterProperties();
        /** \brief return the QStringListModel that holds the completer list */
        inline QStringListModel* stringlistModel() const { return m_stringlist; }
        inline QFProject* project() const { return m_project; }
        void setProject(QFProject* project);
    protected:
    private:
        QPointer<QFProject> m_project;
        QStringListModel* m_stringlist;
    protected slots:
        void propertiesChanged();

};

#endif // QFCOMPLETERPROPERTIES_H
