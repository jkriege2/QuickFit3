/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "qfrdrtableenhancedtableview.h"

QFRDRTableEnhancedTableView::QFRDRTableEnhancedTableView(QWidget *parent):
    QEnhancedTableView(parent, true)
{
    QFRDRTableDelegate* del=new QFRDRTableDelegate(this);

    //getActCopyExcelNoHead()->setShortcut(QKeySequence::UnknownKey);

    setItemDelegate(del);
    connect(del, SIGNAL(closeEditorEnhanced(QWidget*,QFRDRTableDelegate::SpecialEndEditHint)), this, SLOT(closeEditorEnhanced(QWidget*,QFRDRTableDelegate::SpecialEndEditHint)));
    QFontMetrics fm(font());
    verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.5));
    setContextMenuPolicy(Qt::ActionsContextMenu);
}


void QFRDRTableEnhancedTableView::closeEditorEnhanced(QWidget *editor, QFRDRTableDelegate::SpecialEndEditHint hint)
{
    //qDebug()<<"closeEditorEnhanced(hint="<<int(hint)<<")";
    QModelIndex index = currentIndex();
    bool tryEditNext=false;
    bool callBaseClass=true;
    if (hint==QFRDRTableDelegate::EditOneRowUp) {
        QEnhancedTableView::closeEditor(editor, QAbstractItemDelegate::NoHint);
        index=moveCursor(MoveUp, Qt::NoModifier);
        tryEditNext=true;
        callBaseClass=false;
    } else if (hint==QFRDRTableDelegate::EditOneRowDown) {
        QEnhancedTableView::closeEditor(editor, QAbstractItemDelegate::NoHint);
        index=moveCursor(MoveDown, Qt::NoModifier);
        tryEditNext=true;
        callBaseClass=false;
    } else if (hint==QFRDRTableDelegate::EditNextRow) {
        if (model()) {
            QEnhancedTableView::closeEditor(editor, QAbstractItemDelegate::NoHint);
            index=model()->index(index.row()+1, 0, index.parent());
            tryEditNext=true;
            callBaseClass=false;
        }
    } else if (hint==QFRDRTableDelegate::EditPreviousRow) {
        if (model()) {
            QEnhancedTableView::closeEditor(editor, QAbstractItemDelegate::NoHint);
            index=model()->index(qMax(0, index.row()-1), 0, index.parent());
            tryEditNext=true;
            callBaseClass=false;
        }
    } else if (hint==QFRDRTableDelegate::EditClearExpression) {
        if (model()) {
            QEnhancedTableView::closeEditor(editor, QAbstractItemDelegate::NoHint);
            int answer = QMessageBox::question(this, tr("Clear Table Expression"), tr("Are you sure that you want to clear the expression from the selected table cell?"), QMessageBox::Yes | QMessageBox::No);
            if (answer == QMessageBox::Yes) {
                model()->setData(index, QVariant(), QFRDRTable::TableExpressionRole);
            }
            tryEditNext=true;
            callBaseClass=false;
        }
    } else if (hint==QFRDRTableDelegate::EditEditExpression) {
        if (model()) {
            QEnhancedTableView::closeEditor(editor, QAbstractItemDelegate::NoHint);
            emit editExpression(index);
            tryEditNext=false;
            callBaseClass=false;
        }
    }

    QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect;
                                                //| selectionBehaviorFlags();

    if (tryEditNext && index.isValid())  {
        QPersistentModelIndex persistent(index);
        selectionModel()->setCurrentIndex(persistent, flags);
        // currentChanged signal would have already started editing
        if (index.flags() & Qt::ItemIsEditable
            && (!(editTriggers() & QAbstractItemView::CurrentChanged)))
            edit(persistent);
    }

    if (callBaseClass) {
        QEnhancedTableView::closeEditor(editor, QAbstractItemDelegate::EditNextItem);
    }
}



bool QFRDRTableEnhancedTableView::event(QEvent *event)
{
    if (event->type()==QEvent::ShortcutOverride) {
        QKeyEvent* keyev=dynamic_cast<QKeyEvent*>(event);
        if (keyev && keyev->matches(QKeySequence::Copy)) {
            //qDebug()<<"COPY";
            emit copyPressed();
            event->accept();
            return true;
        } else if (keyev && keyev->matches(QKeySequence::Delete)) {
            //qDebug()<<"DELETE";
            emit delPressed();
            event->accept();
            return true;
        }
    }
    return QEnhancedTableView::event(event);
}
