/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#ifndef QFRDRTABLEDELEGATE_H
#define QFRDRTABLEDELEGATE_H

#include <QItemDelegate>
#include <climits>
#include "qfrdrtable.h"
#include <QEvent>
#include <QKeyEvent>


/*! \brief Special item delegate for the QuickFit table plugin
    \ingroup qf3rdrdp_table

    \note This only will work correctly when used together with QFRDRTableEnhancedTableView !!!
*/

class QFRDRTableDelegate : public QItemDelegate
{
        Q_OBJECT
    public:
        explicit QFRDRTableDelegate(QObject *parent = 0);
        
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
         void closeEditorEnhanced(QWidget* editor, QFRDRTableDelegate::SpecialEndEditHint hint);
    private slots:
         void commitAndCloseEditor();
         void doClearExpression();
         void doEditExpression();

    protected:
         virtual bool eventFilter(QObject* editor, QEvent* event);
};

#endif // QFRDRTABLEDELEGATE_H
