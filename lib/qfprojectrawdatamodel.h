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

#ifndef QFPROJECTRAWDATAMODEL_H
#define QFPROJECTRAWDATAMODEL_H

#include "lib_imexport.h"
#include <QAbstractListModel>
#include "qfproject.h"


/*! \brief this class implements a data model which contains all
           raw data items in the project. This is used by the QFEvaluationPropertyEditor
           to display a list of all files that may be added to the evaluation item.
    \ingroup qf3lib_project

    This data model is drag/drop enabled. Items may be draged out and will be exported as
    a list of their IDs (MIME: "application/qf2.ids.int.list"). If data is droped into this
    model simply nothing happens. This allows to use two widgets next to each other:
      - Drag from this model to the other allows to include items there.
      - Drag to this model allows to delete selected items from the other list.
    .

    Also this model returns in the first Qt::UserRole the ID of the raw data record in
    every row.

    \note You can use QFMultiListEvaluationPropertyEditor::QFProjectRawDataModelMultiListEvaluationProxy
          to filter out records that are not usable together with a given QFMultiListEvaluation.
 */
class QFLIB_EXPORT QFProjectRawDataModel : public QAbstractListModel {
        Q_OBJECT
    public:
        /** Default constructor */
        QFProjectRawDataModel(QFProject* parent);
        /** Default destructor */
        virtual ~QFProjectRawDataModel();

        void setProject(QFProject* parent);

        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    protected:
        /** \brief pointer to the owning evaluation item */
        QPointer<QFProject> item;
        /** \brief name of the MIME type used for Drag&Drop actions */
        QString defaultMimeType;
    protected slots:
        /** \brief connected to the propertiesChanged() slot of the owning QFEvaluationItem. Used to
         *         tell the model that something has changed */
        void projectChanged(bool data=true);
    private:
};


#endif // QFPROJECTRAWDATAMODEL_H
