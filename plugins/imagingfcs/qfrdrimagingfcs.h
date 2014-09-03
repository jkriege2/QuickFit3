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

#ifndef QFRDRIMAGINGFCS_H
#define QFRDRIMAGINGFCS_H
#include "qftools.h"
#include "qfpluginrawdata.h"
#include "qfrdrimagingfcscorrelationdialog.h"
#include "qfrdrimagingfcssimulator.h"
#include "qfrdrimfcscorrelatorremote.h"
#include <QPointer>
/*!
    \defgroup qf3rdrdp_imaging_fcs Raw Data Record Plugin
    \ingroup qf3rawdataplugins
*/

/*! \brief raw data record plugin class
    \ingroup qf3rdrdp_imaging_fcs
    */

class QFRDRImagingFCSPlugin : public QObject, public QFPluginRawDataRecordBase, public QFRDRIMFCSCorrelatorRemote {
        Q_OBJECT
        Q_INTERFACES(QFPluginRawDataRecord QFRDRIMFCSCorrelatorRemote)
    public:
        /** Default constructor */
        QFRDRImagingFCSPlugin(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFRDRImagingFCSPlugin();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFRawDataRecord* createRecord(QFProject* parent);
        /** \brief set current project */
        virtual void setProject(QFProject* project);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \copydoc QFPluginRawDataRecord::deinit() */
        virtual void deinit();

        virtual void init();

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("imaging_fcs"); };

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("Imaging FCS Data"); };

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("manages FCS correlation curves from an imaging FCS experiment (SPIM-FCS, TIR-FCS ...)"); };

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger, Jan Buchholz"); };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2011-2013 by Jan W. Krieger, Jan Buchholz"); };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=3;
        };

        /** \brief icon for the plugin */
        virtual QString getIconFilename() const  { return QString(":/imaging_fcs/qfrdrimagingfcs.png"); };


        virtual void imfcsCorrRemoteUserSelectFile();
        virtual void imfcsCorrRemoteSelectFile(const QString& filename);
        virtual void imfcsCorrRemoteAddJob();
        virtual void imfcsCorrRemoteAddJobSeries(const QString& parameter, double start, double end, double inc=1);

    protected slots:
        /** \brief insertdata from file*/
        void insertRecord();
        /** \brief correlate image data and insert the result */
        void correlateAndInsert();
        /** \brief called when the correlation dialog was accepted by the user */
        void importCorrelationsFromDialog();
        /** \brief simulates an image series from a 2D random walk */
        void simulateForCorrelation();

        void importCorrelationsFromSimulation();

        void correctOffset();
    protected:
        /*! \brief add correlations from a video_correlator file to the current project

            \param filename filename of the input file
            \param filename_overview filename of a file containing an overview image
        */
        void insertVideoCorrelatorFile(const QString& filename, const QString& filename_overview=QString(""), const QString &filename_evalsettings=QString(""), bool binary=false, const QString &role=QString(""), int internalDualViewMode=0, int dualViewID=0, bool addCorrelations=true, bool adNandB=true, const QString &group=QString(""), const QMap<QString, QVariant>& props=QMap<QString, QVariant>(), const QString& comment=QString(""));
        /*! \brief add a number and brightness record from a video_correlator file to the current project */
        void insertNandBFromVideoCorrelatorFile(const QString& evalFilename, const QString& filename_overvieww, const QString& filename_overviewstdw, const QString& filenameBack, const QString &filenameBackStd=QString(""), int internalDualViewMode=0, int dualViewID=0, const QString &role=QString(""), const QString &group=QString(""), const QMap<QString, QVariant>& props=QMap<QString, QVariant>(), const QString& comment=QString(""));
        /*! \brief find the evaluation seetinsg file for the give data file ... also returns some additional data from filename heuristics */
        void filenameHeuristics(const QString& filename, QString *evalFilename, bool* isCross, bool* isDCCF, int *dccfid, bool *isJanBFile, QString *role);

        /*! \brief read the \c .evalsettings.txt file */
        bool readEvalSettings(const QString& evalFilename, bool isDCCF, QMap<QString, QVariant> &initParams, QStringList &paramsReadonly, int& width, int &height, QStringList& files, QStringList& files_types, QStringList &files_descriptions, QString& description, QString &filename_settings, QString &filename_acquisition, QString filename_overview, QString &filename_overviewstd, QString &filename_background, QString &filename_backgroundstddev, QString &role, int dccfid=-1);

        /*! \brief add a Radhard2 file to the current project

            \param filename filename of the input file
        */
        void insertRH2CorFile(const QString& filename, const QString &role=QString(""));

        /** \brief returns the number of columns of the supplied CSV file (searches for the first row with more than 0 columns!) */
        int checkColumns(QString filename);

        QFRDRImagingFCSCorrelationDialog* dlgCorrelate;
        QFRDRImagingFCSSimulator* dlgSimulate;

        /** \brief insert record, if it is not yet contained in the project! */
        QFRawDataRecord* insertProjectRecord(const QString& type, const QString& name, const QString& filename, const QString& role=QString(""), const QString& group=QString(""), const QString& description=QString(""), const QString &directory=QString(""), const QMap<QString, QVariant> &init_params=QFStringVariantMap(), const QStringList &init_params_readonly=QStringList(), const QMap<QString, QVariant>& inputfile_props=QMap<QString, QVariant>(), const QString& inputfile_comment=QString(""));
        QFRawDataRecord* insertProjectRecordFiles(const QString &type, const QString &name, const QStringList &filename, const QString& role, const QString& group, const QStringList &filetypes, const QStringList &filedescriptions, const QString &description=QString(""), const QString &directory=QString(""), const QMap<QString,QVariant>& init_params=QFStringVariantMap(), const QStringList& init_params_readonly=QStringList(), const QMap<QString, QVariant>& inputfile_props=QMap<QString, QVariant>(), const QString& inputfile_comment=QString(""));

        bool parseSPIMSettings(const QString& filename_settings, QString& description, QMap<QString,QVariant>& initParams, QStringList& paramsReadonly, QStringList& files, QStringList& files_types, QStringList& files_descriptions);

        static QFRDRImagingFCSPlugin* instance;
    public:
        static QFRDRImagingFCSPlugin* getInstance()  { return instance; }
};

#endif // QFRDRIMAGINGFCS_H
