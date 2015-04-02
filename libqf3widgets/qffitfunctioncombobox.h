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


#ifndef QFFITFUNCTIONCOMBOBOX_H
#define QFFITFUNCTIONCOMBOBOX_H

#include <QComboBox>
#include "qffitfunction.h"
#include "libwid_imexport.h"
#include "qfsimpletreemodel.h"
#include "qftreeviewcombobox.h"
#include "qfpseudotreemodel.h"

/*! \brief a QComboBox which contains a list of (all, or all filtered) QFFitFunctions, available in QuickFit
    \ingroup qf3lib_widgets

  This is usefull, if you want to trigger actions by a keypress ...
*/
class QFWIDLIB_EXPORT QFFitFunctionComboBox : public QFEnhancedComboBox // QFTreeViewComboBox
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
        virtual int currentIndex() const;
        virtual void setCurrentIndex(int idx);
        QString m_filter;
        QStringList m_availableFuncs;
        QAction* actHelp;
        QAction* actSelectModel;
        //QFSimpleTreeModel* m_model;
        QFPseudoTreeModel* m_model;

};

#  include "programoptions.h"
CREATE_WIDGET_PROGRAMOPTIONS_QFLOADSTORE(QFFitFunctionComboBox,setCurrentFitFunction,currentFitFunctionID,QString)

#endif // QFFITFUNCTIONCOMBOBOX_H
