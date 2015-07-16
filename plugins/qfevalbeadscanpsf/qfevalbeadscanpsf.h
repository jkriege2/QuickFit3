/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-01-23 12:13:35 +0100 (Fr, 23 Jan 2015) $  (revision $Rev: 3746 $)

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


#ifndef QFEVALBEADSCANPSF_H
#define QFEVALBEADSCANPSF_H

#include "qfpluginevaluation.h"
#include "qfpluginfitfunction.h"

/*!
    \defgroup qf3rdrdp_eval_beadscanpsf PSF Evaluation Item Plugin
    \ingroup qf3evaluationplugins
*/
/*! \brief Evaluation Item Plugin class
    \ingroup qf3rdrdp_eval_beadscanpsf
*/
class QFEvalBeadScanPSF : public QObject, public QFPluginEvaluationItemBase, public QFFitFunctionFactory {
        Q_OBJECT
        Q_INTERFACES(QFPluginEvaluationItem QFFitFunctionFactory)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFEvalBeadScanPSF")
#endif		
    public:
        /** Default constructor */
        QFEvalBeadScanPSF(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFEvalBeadScanPSF();

        /** \brief Create a new QFEvaluationItem in the given project \a parent. Also adds the record to the project. */
        virtual QFEvaluationItem* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief init plugin, this function is guaranteed to be called once, before  */
        virtual void init();

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("eval_beadscanpsf"); }

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("Beadscan Evaluation: PSF"); }

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("determines the PSF from a z-stack of fluorescent beads inside a gel (\"beadstack\")"); }

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2015 by Jan W. Krieger"); }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }

        /** \brief icon file for the plugin (from resource system or a separate file) */
        virtual QString getIconFilename() const  { return QString(":/eval_beadscanpsf/eval_beadscanpsf.png"); }

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        }

        /** \brief return list of plugin IDs */
        virtual QStringList fitFunctionFactoryGetIDs() const ;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* fitFunctionFactoryGet(const QString& id) const ;

        virtual QString fitFunctionFactoryGetHelpFile(const QString& id) const ;
    protected slots:
        /** \brief insert FCS data from file*/
        void insertEvaluation();

    private:
};

#endif // QFEVALBEADSCANPSF_H
