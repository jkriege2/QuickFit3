#ifndef QFSimpleTCSPCImporter_H
#define QFSimpleTCSPCImporter_H
#include "qfpluginimporters.h"
#include "qfimporter.h"
#include "qftcspcreader.h"
#include <stdio.h>
#include <QMap>


/*! \brief QFImporter class for simple TCSPC
    \ingroup qf3importerplugins_importers_picoquant

*/
class QFSimpleTCSPCImporter: public QFTCSPCReader {
    public:
        QFSimpleTCSPCImporter();
        virtual ~QFSimpleTCSPCImporter();
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;




        /** \copydoc QFTCSPCReader:open:() */
        virtual bool open(const QString& filename, const QString& parameters=QString());
        /** \brief QFTCSPCReader::isOpenParametersUsed() */
        virtual bool isOpenParametersUsed(QString *optionsDescription=NULL) const;

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

        fpos_t fileResetPos;

        uint64_t currentTTTRRecordNum;
        uint64_t recordsInTTTRFile;
        uint64_t lastTTTRMacroTime;
        uint64_t initialTTTRMacroTime;
        double temporalResolutionSeconds;
        double averageInterphotonTime;

        QFTCSPCRecord current;
        QMap<uint16_t, double> cr;
};

#endif // QFSimpleTCSPCImporter_H
