/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef QFTCSPCREADERPICOQUANT_H
#define QFTCSPCREADERPICOQUANT_H
#include "qfpluginimporters.h"
#include "qfimporter.h"
#include "qftcspcreader.h"
#include <stdio.h>
#include <QMap>
#include "picoquant_t3r_tools.h"


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
        virtual bool open(const QString& filename, const QString& parameters=QString());
        /** \brief QFTCSPCReader::isOpenParametersUsed() */
        virtual bool isOpenParametersUsed( QString* optionsDescription=NULL) const;

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
