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

#ifndef QFRDRSETTINGSDATA_H
#define QFRDRSETTINGSDATA_H

#include <QList>
#include <QObject>
#include <QMap>
#include <QProgressBar>
#include <QStatusBar>
#include <QWidget>
#include <QMenu>
#include <QPair>
#include <QList>
#include <QAction>
#include "qfrawdatarecord.h"
#include "qfrdrsettings_dataeditor.h"
#include "qfrawdatarecordfactory.h"


/*! \brief QFRawDataRecord implementation
    \ingroup qf3rdrdp_rdr_settings

*/
class QFRDRSettingsData : public QFRawDataRecord {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRSettingsData(QFProject* parent);
        /** Default destructor */
        virtual ~QFRDRSettingsData();

        /** \brief return type (short type string) */
        virtual QString getType() const { return "rdr_settings"; }
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("configuration file"); }
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/rdr_settings/qfrdrsettings_small.png"); }
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("configuration/ini file"); }
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/rdr_settings/qfrdrsettings.png"); }
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount() { return 1; }
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i) {
            if (i==0) return tr("configuration file editor");
            return QString("");
        }
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(QFPluginServices* services,  QFRawDataPropertyEditor *propEditor, int i=0, QWidget* parent=NULL) {
            if (i==0) return new QFRDRSettingsDataEditor(services, propEditor, parent);
            return NULL;
        }
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename)const ;
        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportFiletypes() */
        QStringList getExportFiletypes() const {
            QStringList sl;
            return sl;
        }
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle() const { return tr(""); }
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes() const { return tr(""); }

        /** \brief indicates, whether the next of same role/previous of same role buttons should be displayed */
        virtual bool showNextPreviousOfSameRoleButton() const;


		/** \brief some example method that returns data */
        QSettings* getData() const;
    protected:
        /** \brief write the contents of the object to a XML file */
        virtual void intWriteData(QXmlStreamWriter& w) const;
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL);

		/** \brief load data file */
        bool loadDataFile(QString filename);

    private:
		/** \brief example: stores some data */
        QSettings* data;
};


#endif // QFRDRSETTINGSDATA_H
