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


#include "qfrdrcolumngraphicscombobox.h"

QFRDRColumnGraphsComboBox::QFRDRColumnGraphsComboBox(QWidget *parent):
    QComboBox(parent)
{
    cols=NULL;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));
}

QFRDRColumnGraphsComboBox::~QFRDRColumnGraphsComboBox()
{
}

QFRDRColumnGraphsInterface *QFRDRColumnGraphsComboBox::currentColumnGraphics() const
{
    return cols;
}

void QFRDRColumnGraphsComboBox::setColumnGraphs(QFRDRColumnGraphsInterface *cols)
{
    this->cols=cols;
    refill();
}

void QFRDRColumnGraphsComboBox::refill()
{
    QString current=currentText();

    setUpdatesEnabled(false);
    disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));

    clear();
    if (cols) {
        for (int i=0; i<cols->colgraphGetPlotCount(); i++) {
            addItem(cols->colgraphGetPlotTitle(i));
        }
    }

    setUpdatesEnabled(true);

    int idx=findText(current);
    if (idx>=0) setCurrentIndex(idx);
    else setCurrentIndex(0);
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));
    if (count()<=0) emit refilledEmpty();
    emit refilled(count()>0);
}

void QFRDRColumnGraphsComboBox::myCurrentIndexChanged(int i)
{
    //qDebug()<<"myCurrentIndexChanged(index="<<i<<")";
}

