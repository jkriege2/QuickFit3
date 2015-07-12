/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#ifndef QFRDRSETTINGSEDITOR_H
#define QFRDRSETTINGSEDITOR_H

#include "qfrawdataeditor.h"
#include <QLabel>
#include <QTreeView>
#include "qsettingsmodel.h"
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include "qfenhancedlineedit.h"
#include "qfcompleterfromfile.h"
#include "qfstyledbutton.h"

/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_rdr_settings
*/
class QFRDRSettingsDataEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRSettingsDataEditor(QFPluginServices* services, QFRawDataPropertyEditor* propEditor, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRSettingsDataEditor();
    protected slots:
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);

        void nameFilterChanged(const QString& filter);
    protected:
        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        /** \brief label for test */
        QTreeView* tree;
        QLabel* labFilename;
        QSettingsModel* model;
        QFEnhancedLineEdit* edtFilterName;
        QSortFilterProxyModel* filterModel;
    private:
};

#endif // QFRDRSETTINGSEDITOR_H
