#ifndef QFTCSPCREADERPICOQUANTPT3_H
#define QFTCSPCREADERPICOQUANTPT3_H
#include "qfpluginimporters.h"
#include "qfimporter.h"
#include "qftcspcreader.h"
#include <stdio.h>
#include <QMap>
#include "picoquant_pt3_tools.h"


/*! \brief QFImporter class for PicoQuant PicoHarp 300 Files (as TCSPC files)
    \ingroup qf3importerplugins_importers_picoquant


      - <a href="http://ridl.cis.rit.edu/products/manuals/PicoQuant/PicoHarp%20300%20v2.3/filedemo/pt3/c/pt3demo.c">Demo code</a>
      - <a href="http://wwwmc.bio.uva.nl/cam/manuals/PicoHarp%20300%20Manual%20v2.2.pdf">Documentation</a>
    .
*/
class QFTCSPCReaderPicoquantPT3: public QFTCSPCReader {
    public:
        QFTCSPCReaderPicoquantPT3();
        virtual ~QFTCSPCReaderPicoquantPT3();
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
        PT3TxtHdr txtHeader;
        PT3BinHdr binHeader;
        PT3BoardHdr boardHeader;
        PT3TTTRHdr TTTRHeader;

        fpos_t fileResetPos;

        uint64_t currentTTTRRecordNum;
        uint64_t ofltime;
        uint64_t overflows;
        double syncperiod;

        QFTCSPCRecord current;
        QMap<uint16_t, double> cr;

        double duration;
};

#endif // QFTCSPCREADERPICOQUANTPT3_H
