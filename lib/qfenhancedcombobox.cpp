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

#include "qfenhancedcombobox.h"
#include <QDebug>
#include <QLineEdit>
#include <QAbstractItemView>
#include <QListView>
QFEnhancedComboBox::QFEnhancedComboBox(QWidget *parent) :
    QComboBox(parent)
{
    m_readonly=false;
    m_oldEditable=isEditable();
}

void QFEnhancedComboBox::setReadOnly(bool readonly) {
    m_readonly=readonly;
    if (lineEdit()) lineEdit()->setReadOnly(readonly);
    if (isEditable() && readonly) {
        m_oldEditable=true;
        setEditable(false);
    }
    if (!readonly && (m_oldEditable!=isEditable())) {
        setEditable(m_oldEditable);
    }
}

void QFEnhancedComboBox::showPopup() {
    if (!m_readonly) {
        QComboBox::showPopup();



        QFontMetrics fm(view()->font());
        int width=view()->size().width();
        for (int i=0; i<qMin(count(), 100); i++) {
            int w=fm.width(itemText(i));
            if (w>width) width=w;
        }

        view()->setMinimumWidth(qMax(width, this->width()));
        view()->setGeometry(0,0,width, view()->size().height());

    }
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

void QFEnhancedComboBox::findAndSelectText(const QString &text)
{
    findAndSelectText(text, currentIndex());
}

void QFEnhancedComboBox::findAndSelectText(const QString &text, int defaultIdx)
{
    int idx=findText(text);
    if (idx>=0) setCurrentIndex(idx);
    else setCurrentIndex(defaultIdx);
}

void QFEnhancedComboBox::findAndSelectContainedLCText(const QString &text)
{
    findAndSelectContainedLCText(text, currentIndex());
}

void QFEnhancedComboBox::findAndSelectContainedLCText(const QString &text, int defaultIdx)
{
    int idx=findText(text, Qt::MatchContains|Qt::MatchFixedString);
    if (idx>=0) setCurrentIndex(idx);
    else setCurrentIndex(defaultIdx);

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
    if (!m_readonly) {
        QComboBox::wheelEvent(e);
        correctCurrentItem();
    }
}

void QFEnhancedComboBox::keyPressEvent(QKeyEvent *e)
{
    if (e->key()==Qt::Key_Enter || e->key()==Qt::Key_Return) emit editingFinished();
    if (!m_readonly) {
        QComboBox::keyPressEvent(e);
        correctCurrentItem();
    }
}

void QFEnhancedComboBox::keyReleaseEvent ( QKeyEvent * e ) {
    if (!m_readonly) QComboBox::keyReleaseEvent(e);
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
