#ifndef QFTCSPCREADERPICOQUANT_H
#define QFTCSPCREADERPICOQUANT_H
#include "qfpluginimporters.h"
#include "qfimporter.h"
#include "qftcspcreader.h"



/*! \brief QFImporter class for PicoQuant TTTR Files (as TCSPC files)
    \ingroup qf3importerplugins_importers_picoquant

*/
class QFTCSPCReaderPicoquant: public QFTCSPCReader {
    public:
        QFTCSPCReaderPicoquant();
        virtual ~QFTCSPCReaderPicoquant();
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;




        /** \copydoc QFTCSPCReader:open:() */
        virtual bool open(QString filename);
        /** \copydoc QFTCSPCReader::close() */
        virtual void close();
        /** \copydoc QFTCSPCReader::reset() */
        virtual void reset();
        /** \copydoc QFTCSPCReader::nextRecord() */
        virtual bool nextRecord();
        /** \copydoc QFTCSPCReader::measurementDuration() */
        virtual double measurementDuration() const;
        /** \copydoc QFTCSPCReader::inputChannels() */
        virtual int32_t inputChannels() const;
        /** \copydoc QFTCSPCReader::getCurrentRecord() */
        virtual QFTCSPCRecord getCurrentRecord() const;
};

#endif // QFTCSPCREADERPICOQUANT_H
