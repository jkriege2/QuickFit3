/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef USERFITFUNCTIONMODEL_H
#define USERFITFUNCTIONMODEL_H

#include <QAbstractTableModel>
#include <QSettings>
#include <QStringList>
#include <QVector>

class UserFitFunctionModel : public QAbstractTableModel
{
        Q_OBJECT
    public:
        explicit UserFitFunctionModel(QObject *parent = 0);
        void loadSettings(QSettings& settings);
        void storeSettings(QSettings& settings) const;
        QVector<double> getInitParams() const;

        void deleteParameter(int idx);
        void addParameter();
        void clear();
        void moveUp(int idx);
        void moveDown(int idx);
        QStringList getIDs() const;


        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    signals:
        
    public slots:
    protected:
        struct FPItem {
            QString id;
            QString name;
            QString label;
            QString unit;
            QString unitLabel;
            QString type;
            bool fit;
            bool userEditable;
            bool userRangeEditable;
            bool initialFix;
            double initialValue;
            double maxValue;
            double minValue;
            double absMaxValue;
            double absMinValue;
            double inc;
        };
        QList<FPItem> items;
        
};

#endif // USERFITFUNCTIONMODEL_H
