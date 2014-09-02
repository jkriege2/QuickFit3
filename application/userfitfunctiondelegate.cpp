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

#include "userfitfunctiondelegate.h"
#include "qfdoubleedit.h"
#include <QComboBox>

UserFitFunctionDelegate::UserFitFunctionDelegate(QObject *parent) :
    QFHTMLDelegate(parent)
{
    setDisplayRichTextEditor(false);
}

QWidget *UserFitFunctionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant dat=index.data(Qt::EditRole);

    int col=index.column();
    if (col==3) {
        QComboBox* cmb=new QComboBox(parent);
        cmb->addItem(tr("float"), QString("float"));
        cmb->addItem(tr("log. float"), QString("log"));
        cmb->addItem(tr("integer"), QString("int"));
        cmb->addItem(tr("integer combo"), QString("int_combo"));
        connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(onEditorFinished()));
        return cmb;
    }
    if (dat.type()==QVariant::Double) {
        QFDoubleEdit* edt=new QFDoubleEdit(parent);
        edt->setCheckBounds(false, false);
        edt->setValue(dat.toDouble());
        return edt;
    }
    return QFHTMLDelegate::createEditor(parent, option, index);
}

void UserFitFunctionDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox* cmb=qobject_cast<QComboBox*>(editor);
    QFDoubleEdit* dbl=qobject_cast<QFDoubleEdit*>(editor);
    int col=index.column();
    QVariant d=index.data(Qt::EditRole);
    if (cmb && col==3) {
        cmb->setCurrentIndex(cmb->findText(d.toString()));
    } else if (dbl) {
        dbl->setValue(d.toDouble());
    } else {
        QFHTMLDelegate::setEditorData(editor, index);
    }
}

void UserFitFunctionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox* cmb=qobject_cast<QComboBox*>(editor);
    QFDoubleEdit* dbl=qobject_cast<QFDoubleEdit*>(editor);
    int col=index.column();
    QVariant d=index.data(Qt::EditRole);
    if (cmb && col==3) {
        model->setData(index, cmb->itemData(cmb->currentIndex()).toString());
    } else if (dbl) {
        model->setData(index, dbl->value());
    } else {
        QFHTMLDelegate::setModelData(editor, model, index);
    }
}

void UserFitFunctionDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QFHTMLDelegate::updateEditorGeometry(editor, option, index);
}

void UserFitFunctionDelegate::onEditorFinished()
{
    QWidget * widEditor = static_cast<QWidget *>(sender());
    if (widEditor) {
        emit commitData(widEditor);
        emit closeEditor(widEditor);
    }
}
