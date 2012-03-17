#ifndef QFRDRPHOTONCOUNTSDATA_H
#define QFRDRPHOTONCOUNTSDATA_H

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
#include "qfrawdatarecordfactory.h"
#include "../interfaces/qfrdrphotoncountsinterface.h"


/*! \brief QFRawDataRecord to represent a set of photon counts acquired in a set of channels
    \ingroup qf3rdrdp_photoncounts

*/
class QFRDRPhotonCountsData : public QFRawDataRecord, public QFRDRPhotonCountsInterface {
        Q_OBJECT
        Q_INTERFACES(QFRDRPhotonCountsInterface)
    public:
        /** Default constructor */
        QFRDRPhotonCountsData(QFProject* parent);
        /** Default destructor */
        virtual ~QFRDRPhotonCountsData();

        /** \brief return type (short type string) */
        virtual QString getType() const { return "photoncounts"; }
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("photon counts"); }
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/photoncounts/qfrdrphotoncounts_small.png"); }
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("multi-channel photon count representation"); }
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/photoncounts/qfrdrphotoncounts.png"); }
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount() { return 1; }
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i) {
            if (i==0) return tr("count rate display");
            return QString("");
        };
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(QFPluginServices* services, int i=0, QWidget* parent=NULL);
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename)const ;
        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportFiletypes() */
        QStringList getExportFiletypes() {
            QStringList sl;
            return sl;
        };
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle() { return tr(""); }
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes() { return tr(""); }

        /** \brief name for a channel */
        virtual QString getPhotonCountsChannelName(int channel);
        /** \brief number of countrate channels in this object */
        virtual int getPhotonCountsChannels();
        /** \brief number of datapoints in every count rate curve */
        inline virtual long long getPhotonCountsN() { return rateN; }
        /** \brief sample points (times \f$ \tau \f$ ) of the count rate
         *         This is a 1D array of size rateN */
        inline virtual double* getPhotonCountsT() { return rateT; }
        /** \brief values of the count rate.
         *         This is a 2D array of size channels * rateN
         *
         * access this as \code rate[channel*rateN + n] \endcode
         */
        inline virtual double* getPhotonCounts() { return rate; }
        /** \brief values of the count rate nfor a given channel.
         *         This is a 1D array of length  rateN
         *
         * access this as \code rate[channel*rateN + n] \endcode
         */
        inline virtual double* getPhotonCountsChannel (int channel) { return &(rate[channel*rateN]); }


        /** \brief number of binned count rate channels in this object */
        inline virtual int getPhotonCountsBinnedChannels() { return rateChannels; }
        /** \brief number of datapoints in every binned count rate */
        inline virtual long long getPhotonCountsBinnedN() { return binnedRateN; }
        /** \brief sample points (times \f$ \tau \f$ ) of the binned count rate
         *         This is a 1D array of size binnedRateN */
        inline virtual double* getPhotonCountsBinnedT() { return binnedRateT; }
        /** \brief values of the binned count rate.
         *         This is a 2D array of size channels * binnedRateN
         *
         * access this as \code rate[channel*binnedRateN + n] \endcode
         */
        inline virtual double* getPhotonCountsBinned() { return binnedRate; }
        /** \brief values of the binned count rate for a given channel.
         *         This is a 1D array of length  binnedRateN
         *
         * access this as \code rate[channel*binnedRateN + n] \endcode
         */
        inline virtual double* getPhotonCountsBinnedChannel (int channel) { return &(binnedRate[channel*binnedRateN]); }


        /** \brief calculate the mean value of the count rate */
        virtual double calcPhotonCountsMean(int channel=0);
        /** \brief calculate the standard deviation of the count rate */
        virtual double calcPhotonCountsStdDev(int channel=0);
        /** \brief return the mean value of the count rate, as last calculated by a call to calcRateMean() */
        virtual double getPhotonCountsMean(int channel=0, QFRDRPhotonCountsInterface::PhotonCountsUnits unit=QFRDRPhotonCountsInterface::KiloHertz);
        /** \brief return the standard deviation of the count rate, as last calculated by a call to calcRateStdDev()  */
        virtual double getPhotonCountsStdDev(int channel=0, QFRDRPhotonCountsInterface::PhotonCountsUnits unit=QFRDRPhotonCountsInterface::KiloHertz);

        /** \brief calculate minimum and maximum count rates */
        virtual void calcPhotonCountsMinMax(int channel, double& min, double& max);
        /** \brief calculate minimum and maximum count rates */
        virtual void getPhotonCountsMinMax(int channel, double& min, double& max, QFRDRPhotonCountsInterface::PhotonCountsUnits unit=QFRDRPhotonCountsInterface::KiloHertz);

        /** \brief set the number of datapoints in every binned count rate channel.
         *
         * \note this function does only take effect BEFORE loading a dataset!
         *       If you want to recalculate the binned count rate call calcBinnedRate() afterwards!
         */
        virtual void setPhotonCountsBinnedN(int value);
        virtual int GetPhotonCountsBinnedNFactor();

        /** \brief this function changes the size of the binned count rate and recalculates it
         *         depending on the value of autoCalcRateN. */
        virtual void calcPhotonCountsBinned();
        /** \brief return a factor with which to multiply the data from getPhotonCounts() to achieve a certain unit */
        virtual double getPhotonCountsUnit(QFRDRPhotonCountsInterface::PhotonCountsUnits unit=QFRDRPhotonCountsInterface::KiloHertz);
        /** \brief return a factor with which to multiply the data from getPhotonCountsBinnes() to achieve a certain unit */
        virtual double getPhotonCountsBinnedUnit(QFRDRPhotonCountsInterface::PhotonCountsUnits unit=QFRDRPhotonCountsInterface::KiloHertz);

        inline double getAverageT() const { return averageT; }
        inline double getAverageBinnedT() const { return averageBinnedT; }


    protected:
        /** \brief write the contents of the object to a XML file */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL);

		/** \brief load data file */
        bool loadDataFile(QString filename);

        /** \brief resize the rate curve data. Afterwards the rate curves do not contain
         *         valid adat anymore.
         *
         * \param N number of datapoints in every count rate channel
         * \param channels number of channels of the count rate
         */
        void resizeRates(long long N, int channels);
        /** \brief resize the binned rate curve data. Afterwards the binned rate curves do not contain
         *         valid adat anymore.
         *
         * \param N number of datapoints in every binned count rate channel
         */
        void resizeBinnedRates(long long N);


        /** \brief number of countrate channels in this object */
        int rateChannels;
        /** \brief number of datapoints in every count rate curve */
        long long rateN;
        /** \brief sample points (times \f$ \tau \f$ ) of the count rate.
         *         This is a 1D array of size rateN */
        double* rateT;
        /** \brief values of the count rate curve.
         *         This is a 2D array of size rateChannels * rateN
         *
         * access this as \code rate[channel*rateN + n] \endcode
         */
        double* rate;

        double averageT;
        double averageBinnedT;

        QMap<int, double> rateMean;
        QMap<int, double> rateStdDev;
        QMap<int, double> rateMin;
        QMap<int, double> rateMax;
        QMap<uint16_t, QString> channelName;

        /** \brief if positive this is the number of datapoints to which the countrate
         *         shall be binned in the binnedRate field. */
        int autoCalcRateN;
        /** \brief number of datapoints in every binned count rate curve */
        long long binnedRateN;
        /** \brief sample points (times \f$ \tau \f$ ) of the binned count rate curve.
         *         This is a 1D array of size binnedRateN */
        double* binnedRateT;
        /** \brief values of the binned count rate curve.
         *         This is a 2D array of size rateChannels * binnedRateN
         *
         * access this as \code rate[channel*binnedRateN + n] \endcode
         */
        double* binnedRate;

        /** \brief load a CSV file containing a count rate curve */
        bool loadCountRatesFromCSV(QString filename);
        /** \brief load a CSV file containing a count rate curve */
        bool loadCountRatesFromBinary(QString filename);

    private:


};


#endif // QFRDRPHOTONCOUNTSDATA_H
