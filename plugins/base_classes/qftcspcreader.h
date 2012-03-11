#ifndef QFTCSPCREADER_H
#define QFTCSPCREADER_H


#include <stdint.h>
#include <QString>

/*! \brief a single-photon TCSPC record
    \ingroup qf3ext_tcspcimporter

*/
struct QFTCSPCRecord {
    /** \brief photon arrival time in seconds */
    double macrotime;
    /** \brief microtime channel */
    int16_t microtime_channel;


    /** \brief arrival time after macrotime of first microtime channel */
    double microtime_offset;
    /** \brief time difference between two microtime chanels */
    double microtime_deltaT;

    /** \brief input channel in which the photon arrived */
    int32_t input_channel;

    double absoluteTime() const {
        return macrotime+microtime_offset+double(microtime_channel)*microtime_deltaT;
    }

    explicit QFTCSPCRecord(int32_t input_channel, double macrotime=0.0, int16_t microtime_channel=0, double microtime_offset=0.0, double microtime_deltaT=1e-9);
};

/*! \brief interface for TCSPC reader classes
    \ingroup qf3ext_tcspcimporter

*/
class QFTCSPCReader {
    public:


        virtual ~QFTCSPCReader() {}
        /** \brief open the given image sequence file
         *  \param filename name of the image sequence file
         *  \return \c true on success
         */
        virtual bool open(QString filename)=0;
        /** \brief close the currently opened image file */
        virtual void close()=0;
        /** \brief move the reading pointer back to the first frame */
        virtual void reset()=0;
        /** \brief move on to the next frame in the file. return \c false if no further image exists */
        virtual bool nextRecord()=0;
        /** \brief return a description of the last error that occured */
        QString lastError() const {
            return err;
        }
        /** \brief return a filter string for the file format */
        virtual QString filter() const =0;
        /** \brief return a name string for the file format */
        virtual QString formatName() const =0;

        /** \brief returns the duration of the measurement in seconds */
        virtual double measurementDuration() const=0;
        /** \brief returns the number of input channels */
        virtual int32_t inputChannels() const=0;

        /** \brief output the current photon record */
        QFTCSPCRecord getCurrentRecord() const;
    protected:

        void setLastError(QString err) {
            this->err=err;
        }

    private:
        QString err;
};



#endif // QFTCSPCREADER_H
