#ifndef QFRDRFCSDATA_H
#define QFRDRFCSDATA_H

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
#include "qfrdrfcscorrelationeditor.h"
#include "qfrdrfcsrateeditor.h"
#include "datatable2.h"
#include "tools.h"
#include "qfrawdatarecordfactory.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "../interfaces/qfrdrcountratesinterface.h"
#include "qftools.h"
#include "csvtools.h"
#include "qfrdrfcsfitfunctionsimulator.h"
#include "qfrdrrunselection.h"
#include "qfrdrsimplecountrateinterface.h"
#include "qfrdrrunselectiontools.h"

/*! \brief manages a FCS dataset (set of correlation curves with multiple runs)
    \ingroup qf3rdrdp_fcs

  Every FCS dataset contains these elements:
    - a set of correlation curves ("runs")
    - their average and standard deviation (calculated after loading and by calling recalculateCorrelations().
    - a set of count rate curves plus a second set with the count rates in reduced resolution.
      This second set may be used for quick plotting of the count rates, as for long measurements
      with high timeresolution plotting may otherwise be quite tedious. The property autoCalcRateN
      contains <=0 if this binned count rate should NOT be calculated by the object, but is loaded i.e.
      from a file and otherwise the desired number of count rate datapoints in the binned count rate.
      The binned countrate is calculated only if autoCalcRateN<rateN!
  .

  It is also possible to define a leaveout list which is a list of the runs that shall not be taken into
  account for the correlation average. This may be usefull, if only some few of the runs in a file are damaged
  (e.g. by agregates moving through the focus ...).
*/
class QFRDRFCSData : public QFRawDataRecord, public QFRDRFCSDataInterface, public QFRDRCountRatesInterface, public QFRDRRunSelectionTools, public QFRDRSimpleCountRatesInterface {
        Q_OBJECT
        Q_INTERFACES(QFRDRFCSDataInterface QFRDRCountRatesInterface QFRDRRunSelectionsInterface QFRDRSimpleCountRatesInterface)
    public:
        /** Default constructor */
        QFRDRFCSData(QFProject* parent);
        /** Default destructor */
        virtual ~QFRDRFCSData();

        /** \brief return type (short type string) */
        virtual QString getType() const { return "fcs"; };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("FCS Data"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/fcs_rawdata.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("FCS data (correlation curve + opt. counts)"); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/fcs_rawdata_large.png"); };
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount() { return 2; };
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i) {
            if (i==0) return tr("Correlation Curve");
            if (i==1) return tr("Photon Counts");
            return QString("");
        };
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(QFPluginServices* services,  QFRawDataPropertyEditor *propEditor, int i=0, QWidget* parent=NULL) {
            if (i==0) return new QFRDRFCSCorrelationEditor(services, propEditor, parent);
            if (i==1) return new QFRDRFCSRateEditor(services, propEditor, parent);
            return NULL;
        };
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename)const ;
        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportFiletypes() */
        QStringList getExportFiletypes() {
            QStringList sl;
            //sl << "CSV" << "SYLK";
            return sl;
        };
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle() { return tr("Export FCS Data  ..."); };
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes() { return tr("Comma Separated Value Files (*.csv, *.txt);;SYLK File (*.sylk, *.slk);;DIF File (*.dif)"); };


        /** \brief channel represented by this object (a file e.g. from ALV may contain several channels,
         *         but only one channel is represented by one object. This is saved in the property
         *         CHANNEL. */
        inline virtual int getChannel() const { return getProperty("CHANNEL", QVariant((int)0)).toUInt(); }
        /** \brief number of correlation runs in this object */
        inline virtual int getCorrelationRuns() const { return correlationRuns; };
        /** \brief number of datapoints in every correlation curve */
        inline virtual long long getCorrelationN() const { return correlationN; }
        /** \brief sample points (times \f$ \tau \f$ ) of the correlation function.
         *         This is a 1D array of size correlationN */
        inline virtual double* getCorrelationT() const { return correlationT; };
        /** \brief values of the correlation function.
         *         This is a 2D array of size runs * correlationN
         *
         * access this as \code correlation[run*correlationN + n] \endcode
         */
        inline virtual double* getCorrelation() const { return correlation; };
        /** \brief values of the correlation function.
         *         This is a 2D array of size runs * correlationN
         *
         * access this as \code correlation[run*correlationN + n] \endcode
         */
        inline virtual double* getCorrelationRunErrors() const { return correlationErrors; };
        /** \brief values of the correlation function for a specified run.
         *         This is a 1D array of size correlationN
         */
        virtual double* getCorrelationRun(int run) const ;
        /** \brief values of the averaged correlation function (averaged over all runs).
         *         This is a 1D array of size correlationN */
        inline virtual double* getCorrelationMean() const { return correlationMean; };
        /** \brief values of the standard deviation of the correlation function (averaged over all runs).
         *         This is a 1D array of size correlationN */
        inline virtual double* getCorrelationStdDev() const { return correlationStdDev; };
        /** \brief errors, associated with the correlation function for a specified run.
         *         This is a 1D array of size correlationN
         */
        virtual double* getCorrelationRunError(int run) const;

        /** \copydoc QFRDRFCSDataInterface::getCorrelationRunName() */
        virtual QString getCorrelationRunName(int run) const;


        /** \brief returns the number of channels with count rates */
        virtual int getRateChannels() const  { return rateChannels; }

        /** \brief number of countrate runs in this object */
        inline virtual int getRateRuns() const { return rateRuns; };
        /** \brief number of datapoints in every count rate curve */
        inline virtual long long getRateN() const { return rateN; };
        /** \brief sample points (times \f$ \tau \f$ ) of the count rate
         *         This is a 1D array of size rateN */
        inline virtual double* getRateT() const { return rateT; };
        /** \brief values of the count rate.
         *         This is a 2D array of size runs * rateN
         *
         * access this as \code rate[run*rateN + n] \endcode
         */
        virtual double* getRate(int channel=0) const ;
        /** \brief values of the count rate nfor a given run.
         *         This is a 1D array of length  rateN
         *
         * access this as \code rate[run*rateN + n] \endcode
         */
        virtual double* getRateRun(int run, int channel=0) const  ;


        /** \brief number of binned count rate runs in this object */
        inline virtual int getBinnedRateRuns(int channel=0) const { return rateRuns; };
        /** \brief number of datapoints in every binned count rate */
        inline virtual long long getBinnedRateN() const { return binnedRateN; };
        /** \brief sample points (times \f$ \tau \f$ ) of the binned count rate
         *         This is a 1D array of size binnedRateN */
        inline virtual double* getBinnedRateT() const { return binnedRateT; };
        /** \brief values of the binned count rate.
         *         This is a 2D array of size runs * binnedRateN
         *
         * access this as \code rate[run*binnedRateN + n] \endcode
         */
        inline virtual double* getBinnedRate(int channel=0) const { return &binnedRate[channel*binnedRateN*rateRuns]; };
        /** \brief values of the binned count rate for a given run.
         *         This is a 1D array of length  binnedRateN
         *
         * access this as \code rate[run*binnedRateN + n] \endcode
         */
        inline virtual double* getBinnedRateRun (int run, int channel=0) const { return &(binnedRate[channel*binnedRateN*rateRuns + run*binnedRateN]); };

        bool getRateChannelsSwapped() const;

        /** \brief calculate the mean value of the count rate */
        virtual double calcRateMean(int run=0, int channel=0) const;
        /** \brief calculate the standard deviation of the count rate */
        virtual double calcRateStdDev(int run=0, int channel=0) const;
        /** \brief return the mean value of the count rate, as last calculated by a call to calcRateMean() */
        virtual double getRateMean(int run=0, int channel=0) const ;
        /** \brief return the standard deviation of the count rate, as last calculated by a call to calcRateStdDev()  */
        virtual double getRateStdDev(int run=0, int channel=0) const ;


        /** \brief calculate minimum and maximum count rates */
        virtual void calcRateMinMax(int run, double& min, double& max, int channel=0) const;
        /** \brief calculate minimum and maximum count rates */
        virtual void getRateMinMax(int run, double& min, double& max, int channel=0) const ;
        /** \brief recalculate correlation curve mean and standard deviation */
        virtual void recalculateCorrelations();
        /** \brief allows to perform an offset correction of the correlation curves, when loading the data */
        virtual void correctCorrelations();

        /** \brief set the number of datapoints in every binned count rate run.
         *
         * \note this function does only take effect BEFORE loading a dataset!
         *       If you want to recalculate the binned count rate call calcBinnedRate() afterwards!
         */
        inline virtual void setBinnedRateN(int value) { autoCalcRateN=value; setIntProperty("AUTO_BINNED_RATE_N", value, false, false); };

        /** \brief this function changes the size of the binned count rate and recalculates it
         *         depending on the value of autoCalcRateN. */
        virtual void calcBinnedRate();

        /** \brief returns the number of runs in the record */
        virtual int leaveoutGetRunCount() const;

        /** \brief returns whether to leave out a run */
        inline virtual bool leaveoutRun(int run) const { return leaveout.contains(run); };
        /** \brief add a run to the leaveouts */
        inline virtual void leaveoutAddRun(int run) { leaveout.append(run); }
        /** \brief remove a run from the leaveouts */
        inline virtual void leaveoutRemoveRun(int run) { leaveout.removeAll(run); }
        /** \brief clear all leaveouts */
        inline virtual void leaveoutClear() { leaveout.clear(); }

        /** \brief returns true when a given run is visible. the average run is indicated by -1 */
        inline virtual bool isCorrelationRunVisible(int run) const {
            if (run+1<runsVisibleList.size() && run>=-1)
                return runsVisibleList[run+1];
            else return true;
        }
        /** \brief set whether the given run is visible. the average run is indicated by -1 */
        inline virtual void setRunVisible(int run, bool vis) {
            if (run+1<runsVisibleList.size() && run>=-1) {
                runsVisibleList[run+1]=vis;
                //emitPropertiesChanged();
                project->setDataChanged();
            }
        }




        /** \copydoc QFRawDataRecord::isFilesListEditable() */
        virtual QFRawDataRecord::FileListEditOptions isFilesListEditable() const;
        /** \copydoc QFRawDataRecord::selectNewFiles() */
        virtual bool selectNewFiles(QStringList& files, QStringList& types, QStringList& descriptions) const;
        /** \copydoc QFRawDataRecord::mayDeleteFiles() */
        virtual bool mayDeleteFiles(QStringList& files, QStringList& types, QStringList& descriptions) const;
        /** \copydoc QFRawDataRecord::reloadFromFiles() */
        virtual bool reloadFromFiles();

        /** \brief returns a list of the allowed RDR roles, default: an emplty list */
        virtual QStringList getAvailableRoles() const;
        /** \brief if this returns \c true, the user may change the RDRs role in the QFRawDataPropertyEditor, default: \c false */
        virtual bool isRoleUserEditable() const;

        /** \copydoc QFRDRSimpleCountRatesInterface::getSimpleCountrateAverage() */
        virtual double getSimpleCountrateAverage(int run=-1, int channel=0, bool swapChannels=false) const;
        /** \copydoc QFRDRSimpleCountRatesInterface::getSimpleCountrateVariance() */
        virtual double getSimpleCountrateStdDev(int run=-1, int channel=0, bool swapChannels=false) const;
        /** \copydoc QFRDRSimpleCountRatesInterface::getSimpleCountrateChannels() */
        virtual int getSimpleCountrateChannels() const;

    protected:
        /** \brief write the contents of the object to a XML file */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL);

        /** \brief resize the correlation curve data. Afterwards the correlation curves do not contain
         *         valid adat anymore. */
        void resizeCorrelations(long long N, int runs);
        /** \brief resize the rate curve data. Afterwards the rate curves do not contain
         *         valid adat anymore.
         *
         * \param N number of datapoints in every count rate run
         * \param runs number of runs of the count rate
         */
        void resizeRates(long long N, int runs, int channels);
        /** \brief resize the binned rate curve data. Afterwards the binned rate curves do not contain
         *         valid adat anymore.
         *
         * \param N number of datapoints in every binned count rate run
         */
        void resizeBinnedRates(long long N);


        /** \brief the leaveout list */
        QList<int> leaveout;

        /** \brief rund visible */
        QList<bool> runsVisibleList;

        /** \brief number of correlation runs in this object */
        int correlationRuns;
        /** \brief number of datapoints in every correlation curve */
        long long correlationN;
        /** \brief sample points (times \f$ \tau \f$ ) of the correlation function.
         *         This is a 1D array of size correlationN */
        double* correlationT;
        /** \brief values of the correlation function.
         *         This is a 2D array of size correlationRuns * correlationN
         *
         * access this as \code correlation[run*correlationN + n] \endcode
         */
        double* correlation;
        /** \brief values of the correlation function errors (for each run).
         *         This is a 2D array of size correlationRuns * correlationN
         *
         * access this as \code correlation[run*correlationN + n] \endcode
         */
        double* correlationErrors;
        /** \brief values of the averaged correlation function (averaged over all runs).
         *         This is a 1D array of size correlationN */
        double* correlationMean;
        /** \brief values of the standard deviation of the correlation function (averaged over all runs).
         *         This is a 1D array of size correlationN */
        double* correlationStdDev;


        /** \brief number of countrate runs in this object */
        int rateRuns;
        /** \brief number of datapoints in every count rate curve */
        long long rateN;
        /** \brief sample points (times \f$ \tau \f$ ) of the count rate.
         *         This is a 1D array of size rateN */
        double* rateT;
        /** \brief values of the count rate curve.
         *         This is a 2D array of size rateRuns * rateN
         *
         * access this as \code rate[channel*rateRuns*rateN + run*rateN + n] \endcode
         */
        double* rate;

        mutable QMap<int, double> rateMean;
        mutable QMap<int, double> rateStdDev;
        mutable QMap<int, double> rateMin;
        mutable QMap<int, double> rateMax;

        /** \brief if positive this is the number of datapoints to which the countrate
         *         shall be binned in the binnedRate field. */
        int autoCalcRateN;
        /** \brief number of datapoints in every binned count rate curve */
        long long binnedRateN;

        int rateChannels;
        /** \brief sample points (times \f$ \tau \f$ ) of the binned count rate curve.
         *         This is a 1D array of size binnedRateN */
        double* binnedRateT;
        /** \brief values of the binned count rate curve.
         *         This is a 2D array of size rateRuns * binnedRateN
         *
         * access this as \code rate[run*binnedRateN + n] \endcode
         */
        double* binnedRate;

        /** \brief load private data format of Oleg Kriechevsky's group */
        bool loadOlegData(QString filenames);

        /** \brief load  data format of correlator.com (*.sin) */
        bool loadCorrelatorComSIN(QString filename);

        /** \brief load  data format of Zeiss COnfocor3 (*.fcs) */
        bool loadConfocor3(QString filename);

        /** \brief load a CSV file containing a count rate curve */
        bool loadCountRatesFromCSV(QStringList filenames, int rateChannels=1);
        /** \brief load a CSV file containing a correlation curve */
        bool loadCorrelationCurvesFromCSV(QStringList filenames);
        /** \brief load an ISS ALBA file containing a correlation curve */
        bool loadCorrelationCurvesFromALBA(QStringList filenames);
        /** \brief read in an ALV5000 data file (created by ALV correlator software)
         *
         * If multiple channels are present in the file, this method looks for an integer property CHANNEL
         * and imports that channel. If this property is not present, the first channel (CH0) will be loaded.
         *
         * This functions loads all files in the QStringList \a filenames into this single record. The parameters are read from the first
         * file and a warning is printed for most of the parameters in the subsequent files, if they do not match the first file. This is
         * omitted for DATE, TIME etc.
         */
        bool loadFromALV5000Files(QStringList filenames);
        /** \brief read in an ALV6000 data file (created by ALV correlator software)
         *
         * If multiple channels are present in the file, this method looks for an integer property CHANNEL
         * and imports that channel. If this property is not present, the first channel (CH0) will be loaded.
         *
         * This functions loads all files in the QStringList \a filenames into this single record. The parameters are read from the first
         * file and a warning is printed for most of the parameters in the subsequent files, if they do not match the first file. This is
         * omitted for DATE, TIME etc.
         */
        bool loadFromALV6000Files(QStringList filenames);
        /** \brief read in an ALV7000 data file (created by ALV correlator software)
         *
         * If multiple channels are present in the file, this method looks for an integer property CHANNEL
         * and imports that channel. If this property is not present, the first channel (CH0) will be loaded.
         *
         * This functions loads all files in the QStringList \a filenames into this single record. The parameters are read from the first
         * file and a warning is printed for most of the parameters in the subsequent files, if they do not match the first file. This is
         * omitted for DATE, TIME etc.
         */
        bool loadFromALV7000Files(QStringList filenames);

        /** \brief initialisez the data from the project file
         */
        bool loadInternal(QDomElement* e);
        void saveInternal(QXmlStreamWriter& w) const;


        struct dataArr {
            int correlationRuns;
            long long correlationN;
            double* correlationT;
            double* correlation;
            double* correlationErrors;
            int rateRuns;
            int rateChannels;
            long long rateN;
            double* rateT;
            double* rate;
        };




    private:
};


#endif // QFRDRFCSDATA_H
