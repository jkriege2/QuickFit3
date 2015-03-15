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

#include "qfenhancedcombobox.h"
#include <QDebug>
QFEnhancedComboBox::QFEnhancedComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

QVariant QFEnhancedComboBox::currentData(int role) const
{
    return itemData(currentIndex(), role);
}

QVariant QFEnhancedComboBox::getCurrentIndexData(int role) const
{
    return currentData(role);
}

void QFEnhancedComboBox::setCurrentFromModelData(const QVariant &data, int role)
{
    setCurrentData(data, role);
}

void QFEnhancedComboBox::selectIndex(const QModelIndex &index)
{
    setCurrentIndex(index.row());
    correctCurrentItem();
}

void QFEnhancedComboBox::setCurrentData(const QVariant &data, int role)
{
    int idx=findData(data, role);
    setCurrentIndex(idx);
    correctCurrentItem();
}

void QFEnhancedComboBox::wheelEvent(QWheelEvent *e)
{
    QComboBox::wheelEvent(e);
    correctCurrentItem();
}

void QFEnhancedComboBox::keyPressEvent(QKeyEvent *e)
{
    QComboBox::keyPressEvent(e);
    correctCurrentItem();
}

void QFEnhancedComboBox::correctCurrentItem()
{
    int r=currentIndex();
    Qt::ItemFlags flags=model()->flags(model()->index(r, 0, rootModelIndex()));
    //qDebug()<<model()->flags(model()->index(r, 0, rootModelIndex()))<<model()->data(model()->index(r, 0, rootModelIndex()));
    if (((flags&Qt::ItemIsSelectable)==0) || ((flags&Qt::ItemIsEnabled)==0)) {
        Qt::ItemFlags f=model()->flags(model()->index(r, 0, rootModelIndex()));
        int rows=model()->rowCount();
        while (r<rows && (((f&Qt::ItemIsSelectable)==0) || ((f&Qt::ItemIsEnabled)==0))) {
            r++;
            f=model()->flags(model()->index(r, 0, rootModelIndex()));
        }
        if (r>=rows)
            setCurrentIndex(-1);
        else
            setCurrentIndex(r);
    }
}
