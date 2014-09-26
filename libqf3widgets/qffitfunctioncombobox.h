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


#ifndef QFFITFUNCTIONCOMBOBOX_H
#define QFFITFUNCTIONCOMBOBOX_H

#include <QComboBox>
#include "qffitfunction.h"
#include "libwid_imexport.h"

class QFWIDLIB_EXPORT QFFitFunctionComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFFitFunctionComboBox(QWidget *parent = 0);
        QString currentFitFunctionID() const;
        QFFitFunction* createCurrentInstance(QObject *parent=NULL) const;
        QAction* getHelpAction() const { return actHelp; }
        QAction* getSelectAction() const { return actSelectModel; }

    signals:
        
    public slots:
        void updateFitFunctions(const QString& filter=QString(""));
        void updateFitFunctions(const QStringList& availableFF);
        void setCurrentFitFunction(const QString& id);
        void showHelpCurrent();
        void selectModelDialog();
    protected slots:
        void reloadFitFunctions();
    protected:
        QString m_filter;
        QStringList m_availableFuncs;
        QAction* actHelp;
        QAction* actSelectModel;

};

#  include "programoptions.h"
CREATE_WIDGET_PROGRAMOPTIONS_QFLOADSTORE(QFFitFunctionComboBox,setCurrentFitFunction,currentFitFunctionID,QString)

#endif // QFFITFUNCTIONCOMBOBOX_H
