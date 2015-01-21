/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "qftabledelegate.h"
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfdoubleedit.h"
#include "qftools.h"

QFTableDelegate::QFTableDelegate(QObject *parent) :
    QItemDelegate(parent)
{
    qRegisterMetaType<QFTableDelegate::SpecialEndEditHint>("QFTableDelegate::SpecialEndEditHint");

}

QWidget *QFTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    QVariant dat=index.data(Qt::EditRole);
    if (dat.type()==QVariant::Invalid) {
        dat=index.data(Qt::DisplayRole);
    }
     if ( dat.type() == QVariant::DateTime || dat.type() == QVariant::Time || dat.type() == QVariant::Date ) {
         QDateTimeEdit *editor = new QDateTimeEdit(parent);
         //editor->setDisplayFormat("dd/M/yyyy");
         editor->setCalendarPopup(true);
         return editor;
     }
     if ( !index.isValid() || dat.type() == QVariant::Double ) {
         QFDoubleEdit* editor=new QFDoubleEdit(parent);
         editor->setCheckBounds(false, false);
         editor->setShowUpDown(false);
         return editor;
     }
     if ( dat.type() == QVariant::Int || dat.type() == QVariant::LongLong ) {
         QSpinBox* editor=new QSpinBox(parent);
         editor->setRange(INT_MIN, INT_MAX);
         //editor->setButtonSymbols(QAbstractSpinBox::NoButtons);
         return editor;
     }
     if ( dat.type() == QVariant::UInt || dat.type() == QVariant::ULongLong ) {
         QSpinBox* editor=new QSpinBox(parent);
         editor->setRange(0, UINT_MAX);
         //editor->setButtonSymbols(QAbstractSpinBox::NoButtons);
         return editor;
     }
     if ( dat.type() == QVariant::Bool) {
         QCheckBox* editor=new QCheckBox(parent);
         return editor;
     }

     QLineEdit *editor = new QLineEdit(parent);

     // create a completer with the strings in the column as model
     QStringList allStrings;
     for (int i = 1; i<index.model()->rowCount(); i++) {
         QString strItem(index.model()->data(index.sibling(i, index.column()),
             Qt::EditRole).toString());

         if (!allStrings.contains(strItem))
             allStrings.append(strItem);
     }

     QCompleter *autoComplete = new QCompleter(allStrings);
     editor->setCompleter(autoComplete);
     connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
     return editor;

}

void QFTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
    if (edit) {
        edit->setText(index.model()->data(index, Qt::EditRole).toString());
    } else {
         QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
         if (dateEditor) {
             dateEditor->setDateTime(index.model()->data(index, Qt::EditRole).toDateTime());
         } else {
             QFDoubleEdit *dEditor = qobject_cast<QFDoubleEdit *>(editor);
             if (dEditor) {
                 dEditor->setValue(index.model()->data(index, Qt::EditRole).toDouble());
             } else {
                 QSpinBox *sEditor = qobject_cast<QSpinBox *>(editor);
                 if (sEditor) {
                     sEditor->setValue(index.model()->data(index, Qt::EditRole).toLongLong());
                 } else {
                     QCheckBox *check = qobject_cast<QCheckBox *>(editor);
                     if (check) {
                         check->setChecked(index.model()->data(index, Qt::EditRole).toBool());
                     }
                 }

             }

         }
    }
}

void QFTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QFDoubleEdit *dEditor = qobject_cast<QFDoubleEdit *>(editor);
    if (dEditor) {
        QVariant v( dEditor->value());
        v.convert(QVariant::Double);
        //qDebug()<<v;
        model->setData(index, v);
    } else {
        QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
        if (edit) {
            model->setData(index, edit->text());
        } else {
             QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
             if (dateEditor) {
                 model->setData(index, dateEditor->dateTime());
             } else {
                QSpinBox *sEditor = qobject_cast<QSpinBox *>(editor);
                 if (sEditor) {
                     model->setData(index, sEditor->value());
                 } else {
                     QCheckBox *check = qobject_cast<QCheckBox *>(editor);
                     if (check) {
                          model->setData(index, check->isChecked());
                     }
                 }

             }

         }
    }
}

void QFTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate::paint(painter, option, index);
}

void QFTableDelegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

bool QFTableDelegate::eventFilter(QObject *editor, QEvent *event)
{
    QWidget* w=qobject_cast<QWidget*>(editor);
    QKeyEvent* key=dynamic_cast<QKeyEvent*>(event);
    //qDebug()<<"QFTableDelegate::eventFilter(event="<<event->type()<<")";
    if (w && key && event->type()==QEvent::KeyPress) {
        //qDebug()<<"QFTableDelegate::eventFilter(event=QEvent::KeyPress): key_modifiers="<<key->modifiers()<<"   key="<<key->key();
        if (key->modifiers()==Qt::NoModifier && (key->key()==Qt::Key_Enter || key->key()==Qt::Key_Return)) {
            emit commitData(w);
            emit closeEditorEnhanced(w, QFTableDelegate::EditNextRow);
            return true;
        } else if (key->modifiers()==Qt::ControlModifier && (key->key()==Qt::Key_Enter || key->key()==Qt::Key_Return)) {
            emit commitData(w);
            emit closeEditorEnhanced(w, QFTableDelegate::EditPreviousRow);
            return true;
        } else if (key->modifiers()==Qt::NoModifier && (key->key()==Qt::Key_Down)) {
            emit commitData(w);
            emit closeEditorEnhanced(w, QFTableDelegate::EditOneRowDown);
            return true;
        } else if (key->modifiers()==Qt::NoModifier && (key->key()==Qt::Key_Up)) {
            emit commitData(w);
            emit closeEditorEnhanced(w, QFTableDelegate::EditOneRowUp);
            return true;
        }
    }
    return QItemDelegate::eventFilter(editor, event);
}
