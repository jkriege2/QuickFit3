/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:42:55 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3509 $)

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

#include "qftableview.h"

QFTableView::QFTableView(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle(tr("Data Table View"));
    setWindowIcon(QIcon(":/lib/result_correlation_icon.png"));
    model.setReadonly(false);
    model.resize(0,0);
    model.clear();
    model.setReadonly(true);
    createWidgets();
}

void QFTableView::readSettings(QSettings &settings, const QString &prefix)
{
    connectParameterWidgets(false);
    connectParameterWidgets(true);
}

void QFTableView::writeSettings(QSettings &settings, const QString &prefix)
{

}

void QFTableView::writeQFProperties(QFProperties *current, const QString &prefix, const QString &egroup, const QString &param)
{

}

void QFTableView::readQFProperties(QFProperties *current, const QString &prefix, const QString &egroup, const QString &param)
{

}

void QFTableView::connectParameterWidgets(bool connectTo)
{

}

int QFTableView::addCopiedColumn(QString name, const double *data, int32_t size)
{
    model.setReadonly(false);
    model.disableSignals();
    int c=model.columnCount();
    model.appendColumn();
    model.setColumnTitleCreate(c, name);
    model.setColumnCreate(c, data, size);
    model.setReadonly(true);
    model.enableSignals();
    return c;
}

int QFTableView::addColumn(QString name, const QList<QVariant> &data)
{
    model.setReadonly(false);
    model.disableSignals();
    int c=model.columnCount();
    model.appendColumn();
    model.setColumnTitleCreate(c, name);
    model.setColumnCreate(c, data);
    model.setReadonly(true);
    model.enableSignals();
    return c;
}

void QFTableView::clear()
{
    model.setReadonly(false);
    model.clear();
    model.setReadonly(true);
}

void QFTableView::createWidgets()
{
    QHBoxLayout* lay=new QHBoxLayout();
    setLayout(lay);
    tvMain=new QEnhancedTableView(this);
    tvMain->setAlternatingRowColors(true);
    tvMain->setItemDelegate(new QFHTMLDelegate(tvMain));
    tvMain->setModel(&model);
    tvMain->setContextMenuPolicy(Qt::ActionsContextMenu);
    lay->addWidget(tvMain);
}
