/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

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

#include "qfstyleditemdelegate.h"
#include "limits.h"
#include "float.h"
#include "qfdoubleedit.h"
#include <QDateEdit>
#include <QTimeEdit>
#include <QDateTimeEdit>
#include <QCheckBox>

QFItemDelegate::QFItemDelegate(QObject *parent):
    QItemDelegate(parent)
{
}

QWidget *QFItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    switch (index.data(Qt::EditRole).type()) {
        case QVariant::Int:
        case QVariant::LongLong: {
            QSpinBox *editor = new QSpinBox(parent);
            editor->setMinimum(INT_MIN);
            editor->setMaximum(INT_MAX);
            editor->setFrame(false);

            return editor;
        }
        case QVariant::UInt:
        case QVariant::ULongLong: {
            QSpinBox *editor = new QSpinBox(parent);
            editor->setMinimum(0);
            editor->setMaximum(UINT_MAX);
            editor->setFrame(false);

            return editor;
        }
        case QVariant::Double: {
            QFDoubleEdit *editor = new QFDoubleEdit(parent);
            editor->setCheckBounds(false, false);
            editor->setDecimals(10);
            editor->setFrame(false);
            return editor;
        }
        case QVariant::Bool: {
            QCheckBox *editor = new QCheckBox(parent);
            editor->setText("");
            return editor;
        }
        case QVariant::Date: {
            QDateEdit *editor = new QDateEdit(parent);
            editor->setFrame(false);
            return editor;
        }
        case QVariant::Time: {
            QTimeEdit *editor = new QTimeEdit(parent);
            editor->setFrame(false);
            return editor;
        }
        case QVariant::DateTime: {
            QDateTimeEdit *editor = new QDateTimeEdit(parent);
            editor->setFrame(false);
            return editor;
        }
        default: return QItemDelegate::createEditor(parent, option, index);

    }
    return QItemDelegate::createEditor(parent, option, index);
}

void QFItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    switch (index.data(Qt::EditRole).type()) {
        case QVariant::UInt:
        case QVariant::ULongLong:
        case QVariant::Int:
        case QVariant::LongLong:
        case QVariant::Double:
        case QVariant::Time:
        case QVariant::Date:
        case QVariant::DateTime:
        case QVariant::Bool:
            editor->setGeometry(option.rect);
            break;
        default:
            QItemDelegate::updateEditorGeometry(editor, option, index);
            break;
    }
}

void QFItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    switch (index.data(Qt::EditRole).type()) {
        case QVariant::UInt:
        case QVariant::ULongLong:
        case QVariant::Int:
        case QVariant::LongLong: {
                QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
                spinBox->interpretText();
                model->setData(index, spinBox->value(), Qt::EditRole);
            }
            break;
        case QVariant::Double: {
                QFDoubleEdit *spinBox = static_cast<QFDoubleEdit*>(editor);
                model->setData(index, spinBox->value(), Qt::EditRole);
            }
            break;
        case QVariant::Bool: {
                QCheckBox *spinBox = static_cast<QCheckBox*>(editor);
                model->setData(index, spinBox->isChecked(), Qt::EditRole);
            }
            break;
        case QVariant::Date: {
                QDateEdit *edit = static_cast<QDateEdit*>(editor);
                edit->interpretText();
                model->setData(index, edit->date(), Qt::EditRole);
            }
            break;
        case QVariant::Time: {
                QTimeEdit *edit = static_cast<QTimeEdit*>(editor);
                edit->interpretText();
                model->setData(index, edit->time(), Qt::EditRole);
            }
            break;
        case QVariant::DateTime: {
                QDateTimeEdit *edit = static_cast<QDateTimeEdit*>(editor);
                edit->interpretText();
                model->setData(index, edit->dateTime(), Qt::EditRole);
            }
            break;

        default:
            QItemDelegate::setModelData(editor, model, index);
            break;

    }
}

void QFItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    switch (index.data(Qt::EditRole).type()) {
        case QVariant::UInt:
        case QVariant::ULongLong:
        case QVariant::Int:
        case QVariant::LongLong: {
                int value = index.model()->data(index, Qt::EditRole).toInt();

                QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
                spinBox->setValue(value);
            }
            break;
        case QVariant::Double:{
                double value = index.model()->data(index, Qt::EditRole).toDouble();

                QFDoubleEdit *spinBox = static_cast<QFDoubleEdit*>(editor);
                spinBox->setValue(value);
            }
            break;
        case QVariant::Bool:{
                bool value = index.model()->data(index, Qt::EditRole).toBool();

                QCheckBox *spinBox = static_cast<QCheckBox*>(editor);
                spinBox->setChecked(value);
            }
            break;
        case QVariant::Date: {
                QDateEdit *edit = static_cast<QDateEdit*>(editor);
                edit->setDate(index.data(Qt::EditRole).toDate());
            }
            break;
        case QVariant::Time: {
                QTimeEdit *edit = static_cast<QTimeEdit*>(editor);
                edit->setTime(index.data(Qt::EditRole).toTime());
            }
            break;
        case QVariant::DateTime: {
                QDateTimeEdit *edit = static_cast<QDateTimeEdit*>(editor);
                edit->setDateTime(index.data(Qt::EditRole).toDateTime());
            }
            break;
        default:
            QItemDelegate::setEditorData(editor, index);
            break;
    }
}
