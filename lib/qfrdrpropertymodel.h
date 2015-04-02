/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef QFRDRPROPERTYMODEL_H
#define QFRDRPROPERTYMODEL_H

#include <QAbstractTableModel>
#include "lib_imexport.h"
#include <QStringList>


// forward declaration
class QFRawDataRecord;


/*! \brief this class serves as a QAbstractTableModel for the properties associated with a
           QFRawDataRecord.
    \ingroup qf3lib_project

    \note SetData() may be called for the second column with an integer (0: string, 1: number, 2: integer, 3: boolean, 4: date/time) to change the property type.
          This will cause the actual data to be cast to the new datatype. Data may be lost, if the types are not compatible!!!). If it is called with a string describing
          the type, this class tries to read the new type from the string.
*/
class QFLIB_EXPORT QFRDRPropertyModel : public QAbstractTableModel {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRPropertyModel();
        /** Default destructor */
        virtual ~QFRDRPropertyModel();

        void init(QFRawDataRecord* record);

        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;

        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &idx, const QVariant &value, int role = Qt::EditRole);

        QString getPropertyForRow(int row) const;

    protected slots:
        void propertiesChanged(const QString& property, bool visible);
    protected:
        QFRawDataRecord* record;

        void updateModel(bool doReset=true);

        QStringList props;
};

#endif // QFRDRPROPERTYMODEL_H
