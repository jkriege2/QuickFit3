/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qfpropertyitemdelegate.h"
#include <QLineEdit>
#include <QComboBox>

QFPropertyItemDelegate::QFPropertyItemDelegate(QObject *parent):
    QFItemDelegate(parent)
{
    m_project=NULL;
}

void QFPropertyItemDelegate::setProject(QFProject *project)
{
    m_project=project;
}

QFProject *QFPropertyItemDelegate::project() const
{
    return m_project;
}

QWidget *QFPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (m_project) {
        if (index.column()==0) {
            QComboBox* edit=new QComboBox(parent);
            edit->clear();
            edit->addItems(m_project->getAllPropertyNames());
            edit->setEditable(true);
            edit->setFrame(false);
            return edit;
        } else if (index.column()==1) {
            QComboBox* edit=new QComboBox(parent);
            edit->clear();
            edit->addItem(tr("string"));
            edit->addItem(tr("number"));
            edit->addItem(tr("integer"));
            edit->addItem(tr("boolean"));
            edit->addItem(tr("date/time"));
            edit->setEditable(false);
            edit->setFrame(false);
            return edit;
        }
    }
    return QFItemDelegate::createEditor(parent, option, index);
}

void QFPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column()==0) {
        QComboBox *edit = static_cast<QComboBox*>(editor);
        if (edit) {
            edit->lineEdit()->setText(index.data(Qt::EditRole).toString());
        }
    } else if (index.column()==1) {
        QComboBox *edit = static_cast<QComboBox*>(editor);
        if (edit) {
            QString typeS=index.data(Qt::EditRole).toString().toLower().trimmed();
            if (typeS==tr("string")||typeS==tr("qstring")) { edit->setCurrentIndex(0); }
            else if (typeS==tr("number")||typeS==tr("double")||typeS==tr("float")) { edit->setCurrentIndex(1); }
            else if (typeS==tr("integer")||typeS==tr("long")||typeS==tr("int")||typeS==tr("longlong")||typeS==tr("qlonglong")) { edit->setCurrentIndex(2); }
            else if (typeS==tr("boolean")||typeS==tr("bool")) { edit->setCurrentIndex(3); }
            else if (typeS==tr("date/time")||typeS==tr("date")||typeS==tr("time")) { edit->setCurrentIndex(4); }

            else edit->setCurrentIndex(0);
        }
    } else {
        QFItemDelegate::setEditorData(editor, index);
    }
}

void QFPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column()==0) {
        QComboBox *edit = static_cast<QComboBox*>(editor);
        if (edit) model->setData(index, edit->currentText(), Qt::EditRole);
    } else if (index.column()==1) {
        QComboBox *edit = static_cast<QComboBox*>(editor);
        if (edit) model->setData(index, edit->currentText(), Qt::EditRole);
    } else {
        QFItemDelegate::setModelData(editor, model, index);
    }
}



