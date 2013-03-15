#ifndef QFRDRIMAGINGFCS_H
#define QFRDRIMAGINGFCS_H
#include "qftools.h"
#include "qfpluginrawdata.h"
#include "qfrdrimagingfcscorrelationdialog.h"
#include "qfrdrimagingfcssimulator.h"

#include <QPointer>
/*!
    \defgroup qf3rdrdp_imaging_fcs Raw Data Record Plugin
    \ingroup qf3rawdataplugins
*/

/*! \brief raw data record plugin class
    \ingroup qf3rdrdp_imaging_fcs
    */

class QFRDRImagingFCSPlugin : public QObject, public QFPluginRawDataRecordBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginRawDataRecord)
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
    protected:
        /*! \brief add a video_correlator file to the current project

            \param filename filename of the input file
            \param filename_overview filename of a file containing an overview image
        */
        void insertVideoCorrelatorFile(const QString& filename, const QString& filename_overview=QString(""), bool binary=false, const QString& role="", int internalDualViewMode=0, int dualViewID=0);
        /*! \brief add a Radhard2 file to the current project

            \param filename filename of the input file
        */
        void insertRH2CorFile(const QString& filename, const QString &role=QString(""));

        /** \brief returns the number of columns of the supplied CSV file (searches for the first row with more than 0 columns!) */
        int checkColumns(QString filename);

        QFRDRImagingFCSCorrelationDialog* dlgCorrelate;
        QFRDRImagingFCSSimulator* dlgSimulate;

        /** \brief insert record, if it is not yet contained in the project! */
        void insertProjectRecord(const QString& type, const QString& name, const QString& filename, const QString& role, const QString& description=QString(""), const QString& directory=QString(""), const QMap<QString,QVariant>& init_params=QFStringVariantMap(), const QStringList& init_params_readonly=QStringList());
        void insertProjectRecordFiles(const QString &type, const QString &name, const QStringList &filename, const QString& role, const QStringList &filetypes, const QStringList &filedescriptions, const QString &description=QString(""), const QString &directory=QString(""), const QMap<QString,QVariant>& init_params=QFStringVariantMap(), const QStringList& init_params_readonly=QStringList());

        bool parseSPIMSettings(const QString& filename_settings, QString& description, QMap<QString,QVariant>& initParams, QStringList& paramsReadonly, QStringList& files, QStringList& files_types, QStringList& files_descriptions);

};

#endif // QFRDRIMAGINGFCS_H
