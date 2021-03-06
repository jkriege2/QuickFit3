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

#ifndef QFTABLEDELEGATE_H
#define QFTABLEDELEGATE_H

#include <QItemDelegate>
#include <climits>
#include <QEvent>
#include <QKeyEvent>
#include "lib_imexport.h"

/*! \brief Special item delegate for the QuickFit tables
    \ingroup qf3lib_widgets
*/
class QFLIB_EXPORT QFTableDelegate : public QItemDelegate
{
        Q_OBJECT
    public:
        explicit QFTableDelegate(QObject *parent = 0);
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,  const QModelIndex &index) const;
         void setEditorData(QWidget *editor, const QModelIndex &index) const;
         void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

         void paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

         enum SpecialEndEditHint {
             EditOneRowUp=20,
             EditOneRowDown=21,
             EditNextRow=22,
             EditPreviousRow=23,
             EditClearExpression=24,
             EditEditExpression=25
         };

    signals:
         void closeEditorEnhanced(QWidget* editor, QFTableDelegate::SpecialEndEditHint hint);
    private slots:
         void commitAndCloseEditor();

    protected:
         virtual bool eventFilter(QObject* editor, QEvent* event);

};

#endif // QFTABLEDELEGATE_H
