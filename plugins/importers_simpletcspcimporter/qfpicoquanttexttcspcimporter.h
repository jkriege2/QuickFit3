/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef QFPicoquantTextTCSPCImporter_H
#define QFPicoquantTextTCSPCImporter_H
#include "qfpluginimporters.h"
#include "qfimporter.h"
#include "qftcspcreader.h"
#include <stdio.h>
#include <QMap>
#include <QFile>


/*! \brief QFImporter class for simple text TCSPC as output by PicoQUant demo programs
    \ingroup qf3importerplugins_importers_picoquant

*/
class QFPicoquantTextTCSPCImporter: public QFTCSPCReader {
    public:

        QFPicoquantTextTCSPCImporter();
        virtual ~QFPicoquantTextTCSPCImporter();
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
        /** \copydoc QFTCSPCReader::microtimeChannels() */
        virtual uint32_t microtimeChannels() const;
        /** \copydoc QFTCSPCReader::microtimeChannelsResolutionPicoSeconds() */
        virtual double microtimeChannelsResolutionPicoSeconds() const;

        /** \copydoc QFTCSPCReader::avgCountRate() */
        virtual double avgCountRate(uint16_t channel) const;
        /** \copydoc QFTCSPCReader::getCurrentRecord() */
        virtual QFTCSPCRecord getCurrentRecord() const;

        virtual double percentCompleted()const;

    protected:
        QFile tttrfile;

        uint64_t fileResetLine;
        uint64_t currentTTTRRecordNum;
        uint64_t recordsInTTTRFile;

        double mtResolutionPS;
        uint32_t mtChannels;
        double duration;
        uint16_t inChannels;
        double timeFactorXToS;

        int idxTime;
        int idxChannel;
        int idxRoute;

        QFTCSPCRecord current;
        QMap<uint16_t, double> cr;
};

#endif // QFPicoquantTextTCSPCImporter_H
