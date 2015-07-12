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

#ifndef FLEX_SIN_TOOLS_H
#define FLEX_SIN_TOOLS_H

#include <QDate>
#include <QTime>
#include <QString>
#include <QFileInfo>

#include <ostream>
#include <exception>
#include <cstdio>
#include <QVector>
#include <iostream>
#include <QFile>
#include <QMap>
#include "cpptools.h"
#include "datatools.h"


/*!
    \defgroup quickfit2FLEX FLEX Data File (ACSII) Import
    \ingroup qf3rdrdp_fcs

    This group contains methods, structs and enums that are used to import data files created by correlator.com correlators (FLEX...).
    The files generally have the extension .sin.

 */


/** \brief allowed tokens for the FLEX file parser
 *  \ingroup quickfit2FLEX
 */
enum FLEX_TOKENTYPE {
    FLEX_EOF = 'e',
    FLEX_LINEBREAK = 'l',
    FLEX_EQUALS ='=',
    FLEX_NAME = 'n',
    FLEX_QUOTED ='"',
    FLEX_VALUE ='v',
    FLEX_GROUP = 'g'
};

/** \brief represents one token (incl. type and value) for the FLEX file parser
 *  \ingroup quickfit2FLEX
 */
typedef struct {
    FLEX_TOKENTYPE type;
    QString value;
    double doubleValue;
} FLEX_TOKEN;

/** \brief an exception class used to report errors while parsing FLEX files
 *  \ingroup quickfit2FLEX
 */
class FLEX_exception : public std::exception {
    private:
        std::string error;
    public:
        FLEX_exception(std::string error_desc) throw() { error=error_desc; std::cout<<error.c_str()<<std::endl; };
        virtual ~FLEX_exception() throw() {};
        virtual const char* what() const throw() { return error.c_str(); };
};


/** \brief reads the next token from an FLEX datafile
 *  \ingroup quickfit2FLEX
 *
 * This method implements a simple tokenizer for FLEX files
 *    \param FLEX_file      The file object to read from
 *    \param readingHeader Do we currently read from the file header? If yes names may contain whitespaces (but no lineabreaks)!
 */
FLEX_TOKEN FLEX_getToken(FILE* FLEX_file);

/** \brief reads until the next \a quotationMark and returns the read string
 *  \ingroup quickfit2FLEX
 *
 * this method will swallow all white spaces bevore the quoted string. Then it expects a \a quotationMark.
 * Now it reads until the next quotationMark, or the end of the line.
 */
QString FLEX_ReadQuoted(FILE* FLEX_file, char quotationMark='"');

/** \brief converts a date in the format "<tt>22-Aug-08</tt>" (FLEX date format) into a QDate object
 *  \ingroup quickfit2FLEX
 */
QDate FLEX_toDate(QString d);

/** \brief converts a time in the format "<tt>15:09:10</tt>" (FLEX date format) into a QDate object
 *  \ingroup quickfit2FLEX
 */
QTime FLEX_toTime(QString d);

/** \brief read a matrix of numbers Result is written into \a datm.
 *  \ingroup quickfit2FLEX
 *
 * This function returns the last read token which is the first token which does not belong to the
 * correlation data. You can use this return value in a token evaluation loop!
 */
FLEX_TOKEN FLEX_readNumberMatrix(FILE* FLEX_file, QList<QVector<double> >* datm, bool getNextToken=true, FLEX_TOKEN lastToken=FLEX_TOKEN());

/** \brief read some parameters from the given FLEX .sin file
 *  \ingroup quickfit2FLEX
 *
 * \param filename filename of the FLEX .sin file to be loaded
 * \param mode the mode string stored in the FLEX .sin file
 * \param channelCount number of channels in the file
 * \param runCount number of runs in the file
 * \param crossCorrelation \c true if the file contains a cross correlation curve
 */
void FLEX_analyze(QString filename, QString& mode, unsigned int& channelCount, unsigned int& runCount, bool& crossCorrelation, bool &autoCorrelation);

struct FLEX_DATA {
    QString filename;
    QString mode;
    unsigned int channelCount;
    unsigned int runCount;
    bool crossCorrelation;
    bool autoCorrelation;
    QList<QVector<double> > corrs;
    QList<QVector<double> > counts;
    QVector<double> corr_tau;
    QVector<double> counts_time;
    QMap<QString, QVariant> properties;
};

/** \brief read contents opf a FLEX .sin file
 *  \ingroup quickfit2FLEX
 *
 * \param filename filename of the FLEX .sin file to be loaded
 * \param mode the mode string stored in the FLEX .sin file
 * \param channelCount number of channels in the file
 * \param runCount number of runs in the file
 * \param crossCorrelation \c true if the file contains a cross correlation curve
 */
void FLEX_readFile(QString filename, FLEX_DATA& data);



#endif // FLEX_SIN_TOOLS_H
