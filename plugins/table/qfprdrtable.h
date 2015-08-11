/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFPRDRTABLE_H
#define QFPRDRTABLE_H

#include "qfpluginrawdata.h"

/*!
    \defgroup qf3rdrdp_table Arbitrary Table Raw Data Record Plugin
    \ingroup qf3rawdataplugins
*/

/*! \brief table raw data record plugin class
    \ingroup qf3rdrdp_table
*/
class QFPRDRTable : public QObject, public QFPluginRawDataRecordBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginRawDataRecord)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFPRDRTable")
#endif

    public:
        /** Default constructor */
        QFPRDRTable(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFPRDRTable();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFRawDataRecord* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("table"); };

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("Data Table Plugin"); };

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("manages a table with arbitrary data in the cells"); };

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan Krieger"); };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2010-2013 by Jan Krieger"); };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr(""); };

        /** \brief icon file for the plugin (from resource system or a separate file) */
        virtual QString getIconFilename() const  { return QString(":/table/table_insert.png"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=3;
        };

    protected slots:
        /** \brief insert an editable table */
        void insertTable();

        /** \brief insert a file as read-only table */
        void insertTableFile();

    private:
};

#endif // QFPRDRTABLE_H
