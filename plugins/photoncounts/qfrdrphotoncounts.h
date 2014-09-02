/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef QFRDRPHOTONCOUNTS_H
#define QFRDRPHOTONCOUNTS_H

#include "qfpluginrawdata.h"
/*!
    \defgroup qf3rdrdp_photoncounts Raw Data Record Plugin
    \ingroup qf3rawdataplugins
*/

/*! \brief raw data record plugin class
    \ingroup qf3rdrdp_photoncounts
    */

class QFRDRPhotonCountsPlugin : public QObject, public QFPluginRawDataRecordBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginRawDataRecord)
    public:
        /** Default constructor */
        QFRDRPhotonCountsPlugin(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFRDRPhotonCountsPlugin();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFRawDataRecord* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("photoncounts"); }

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("photon counts"); }

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("multi-channel photon count representation"); }

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan Krieger"); }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2012 by Jan Krieger"); }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

        /** \brief icon for the plugin */
        virtual QString getIconFilename() const  { return QString(":/photoncounts/qfrdrphotoncounts.png"); }
    protected slots:
        /** \brief insertdata from file*/
        void insertRecord();
};

#endif // QFRDRPHOTONCOUNTS_H
