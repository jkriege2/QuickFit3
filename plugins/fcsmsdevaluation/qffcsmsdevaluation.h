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

#ifndef QFFCSMAXENTEVALUATION_H
#define QFFCSMAXENTEVALUATION_H

#include "qfpluginevaluation.h"
/*!
    \defgroup qf3rdrdp_GROUPNAME Evaluation Item Plugin
    \ingroup qf3evaluationplugins
*/
/*! \brief Evaluation Item Plugin class
    \ingroup qf3rdrdp_GROUPNAME
*/
class QFFCSMSDEvaluation : public QObject, public QFPluginEvaluationItemBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginEvaluationItem)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFFCSMSDEvaluation")
#endif

    public:
        /** Default constructor */
        QFFCSMSDEvaluation(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFFCSMSDEvaluation();

        /** \brief Create a new QFEvaluationItem in the given project \a parent. Also adds the record to the project. */
        virtual QFEvaluationItem* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("fcs_msd"); }

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("FCS: MSD"); }

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("extracts the mean squared displacement from FCS autocorrelation curves"); }

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2012 by Jan W. Krieger"); }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }

        /** \brief icon file for the plugin (from resource system or a separate file) */
        virtual QString getIconFilename() const  { return QString(":/fcsmsdevaluation/fcsmsdevaluation.png"); }

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=2;
        }

    protected slots:
        /** \brief insert FCS data from file*/
        void insertEvaluation();

    private:
};

#endif // QFFCSMAXENTEVALUATION_H
