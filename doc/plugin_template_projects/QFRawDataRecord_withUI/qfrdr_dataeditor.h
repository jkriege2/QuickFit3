/*
Copyright (c) 2014
	
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


#ifndef HEADER_HEDITOR_H
#define HEADER_HEDITOR_H

#include "qfrawdataeditor.h"
#include <QLabel>

namespace Ui {
    class QFRDRDataEditor;
}

/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_GROUPNAME
*/
class QFRDRDataEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRDataEditor(QFPluginServices* services,  QFRawDataPropertyEditor* propEditor, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRDataEditor();
    protected slots:
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);
    protected:
		Ui::QFRDRCurveDataEditor *ui;
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
    private:
};

#endif // HEADER_HEDITOR_H
