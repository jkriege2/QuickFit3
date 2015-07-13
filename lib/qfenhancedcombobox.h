/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#ifndef QFENHANCEDCOMBOBOX_H
#define QFENHANCEDCOMBOBOX_H

#include <QComboBox>
#include "lib_imexport.h"
#include <QVariant>
#include <QKeyEvent>
#include <QWheelEvent>


/*! \brief enhanced QComboBox
    \ingroup qf3lib_widgets

 */
class QFLIB_EXPORT QFEnhancedComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFEnhancedComboBox(QWidget *parent = 0);
        QVariant currentData(int role=Qt::UserRole) const;

        virtual QVariant getCurrentIndexData(int role) const;
        virtual void setCurrentFromModelData(const QVariant& data, int role=Qt::UserRole) ;
        virtual void findAndSelectText(const QString& text);
        virtual void findAndSelectText(const QString& text, int defaultIdx);
        virtual void findAndSelectContainedLCText(const QString& text);
        virtual void findAndSelectContainedLCText(const QString& text, int defaultIdx);
        void addItemsAndVariantdata(const QStringList& items, const QVariantList& data);
        void addItemsAndStringData(const QStringList& items, const QStringList& data);

        virtual void showPopup();
    signals:
        void editingFinished();
    public slots:
        virtual void selectIndex(const QModelIndex&index);
        void setCurrentData(const QVariant& data, int role=Qt::UserRole);
        void setReadOnly(bool readonly);

    signals:

    protected:
        bool m_readonly;
        bool m_oldEditable;
        virtual void keyReleaseEvent ( QKeyEvent * e );
        virtual void wheelEvent(QWheelEvent *e);
        virtual void keyPressEvent(QKeyEvent *e);
        virtual void correctCurrentItem();
};

#endif // QFENHANCEDCOMBOBOX_H
