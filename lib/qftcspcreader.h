#ifndef QFTCSPCREADER_H
#define QFTCSPCREADER_H


#include <stdint.h>
#include <QString>
#include "qfimporter.h"
#include "lib_imexport.h"

/*! \brief a single-photon TCSPC record
    \ingroup qf3ext_tcspcimporter

*/
struct QFLIB_EXPORT QFTCSPCRecord {
    /** \brief photon arrival time in seconds */
    double macrotime;
    /** \brief microtime channel */
    int16_t microtime_channel;


    /** \brief arrival time after macrotime of first microtime channel [nanoseconds] */
    double microtime_offset;
    /** \brief time difference between two microtime chanels [nanoseconds] */
    double microtime_deltaT;

    /** \brief input channel in which the photon arrived */
    uint16_t input_channel;

    double absoluteTime() const ;

    QFTCSPCRecord(uint16_t input_channel=0, double macrotime=0.0, int16_t microtime_channel=0, double microtime_offset=0.0, double microtime_deltaT=1e-9) ;
};

/*! \brief interface for TCSPC reader classes
    \ingroup qf3ext_tcspcimporter

*/
class QFLIB_EXPORT QFTCSPCReader: public QFImporter {
    public:
        QFTCSPCReader();

        virtual ~QFTCSPCReader();
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
        /** \brief return a filter string for the file format */
        virtual QString filter() const =0;
        /** \brief return a name string for the file format */
        virtual QString formatName() const =0;

        /** \brief returns the duration of the measurement in seconds */
        virtual double measurementDuration() const=0;
        /** \brief returns the number of input channels */
        virtual uint16_t inputChannels() const=0;

        /** \brief output the current photon record */
        virtual QFTCSPCRecord getCurrentRecord() const=0;

        /** \brief returns the position inside the input file in % */
        virtual double percentCompleted()const =0;
    protected:

};



#endif // QFTCSPCREADER_H
