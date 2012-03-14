#ifndef QFTCSPCREADERPICOQUANT_H
#define QFTCSPCREADERPICOQUANT_H
#include "qfpluginimporters.h"
#include "qfimporter.h"
#include "qftcspcreader.h"
#include <stdio.h>
#include <QMap>
#include "picoquant_tools.h"


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
        virtual uint16_t inputChannels() const;

        /** \copydoc QFTCSPCReader::avgCountRate() */
        virtual double avgCountRate(uint16_t channel) const;
        /** \copydoc QFTCSPCReader::getCurrentRecord() */
        virtual QFTCSPCRecord getCurrentRecord() const;

        virtual double percentCompleted()const;

    protected:
        FILE* tttrfile;
        TTTRTxtHdr txtHeader;
        TTTRBinHdr binHeader;
        TTTRBoardHdr boardHeader;
        TTTRHdr TTTRHeader;

        fpos_t fileResetPos;

        uint64_t currentTTTRRecordNum;
        uint64_t ofltime;
        uint64_t overflows;

        QFTCSPCRecord current;
        QMap<uint16_t, double> cr;
};

#endif // QFTCSPCREADERPICOQUANT_H
