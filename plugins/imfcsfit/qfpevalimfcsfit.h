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

#ifndef QFPEVALIMFCSFIT_H
#define QFPEVALIMFCSFIT_H

#include "qfpluginevaluation.h"
#include "qfpluginoptionsdialog.h"
#include "qfpluginservices.h"
#include "qfrawdatapropertyeditor.h"
#include "imfcscalibrationwizard.h"

/*!
    \defgroup qf3evalp_imfcsfit FCS Least Squares Curve Fitting Plugin for imaging FCS
    \ingroup qf3evaluationplugins

    This plugin implements more or less the same functionality as the fcsfit plugin (see QFFCSFitEvaluationEditor and QFFCSFitEvaluation). The difference is the
    data storage scheme: Where fcsfit stores data with a separate resultID for every index/run, this plugin stores data as matrices/vectors with one resultID for all
    runs/indices for each parameter.

*/
/*! \brief plugin class for imaging FCS least-square fits
    \ingroup qf3evalp_imfcsfit
*/
class QFPEvalIMFCSFit : public QObject, public QFPluginEvaluationItemBase, public QFPluginOptionsDialogInterface {
        Q_OBJECT
        Q_INTERFACES(QFPluginEvaluationItem QFPluginOptionsDialogInterface)
    public:
        /** Default constructor */
        QFPEvalIMFCSFit(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFPEvalIMFCSFit();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFEvaluationItem* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        virtual void init();

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("imfcs_fit"); }

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("imagingFCS Correlation Curve Fit Plugin"); }

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("fit imaging Fluorescence Correlations Spectroscopy (imagingFCS) curve data."); }

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan Krieger"); }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2012 by Jan Krieger"); }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }

        /** \brief icon file for the plugin (from resource system or a separate file) */
        virtual QString getIconFilename() const  { return QString(":/imfcs_fit_logo.png"); }

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=2;
        }

        QString pluginOptionsName() const;
        QIcon pluginOptionsIcon() const;
        QFPluginOptionsWidget *createOptionsWidget(QWidget *parent);
    protected slots:
        /** \brief insert FCS data from file*/
        void insertFCSFit();

        void insertFCSCalibrationWizard();

        void insertFCSFitForCalibration();
        void imFCSCalibrationSelectFile();
        void imFCSCalibrationCorrelate();

        void imFCSCalibrationTool1();
        void imFCSCalibrationTool2();
        void imFCSCalibrationTool3();
        void imFCSCalibrationTool4();
        void showCalibrationTutorial();


    private:
        QMenu* menuCalibration;
        ImFCSCalibrationWizard* calibrationWizard;
};

#endif // QFPEVALIMFCSFIT_H
