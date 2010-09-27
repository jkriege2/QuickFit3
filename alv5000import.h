#ifndef ALV5000IMPORT_H
#define ALV5000IMPORT_H

#include <QDate>
#include <QTime>
#include <QString>
#include <QFileInfo>


#include <exception>
#include <cstdio>
#include <vector>

#include "../lib/tools.h"
#include "quickfitdatastore.h"


/*!
    \defgroup quickfit2ALV ALV-5000 Data File (ACSII) Import
    \ingroup quickfit2

    This group contains methods, structs and enums that are used to import ALV-5000 data files.

    \section Quickfit2_ALVFileFormat File Format of ALV-5000 data files (ASCII)

    The header contains several lines with additional data fields. Each line looks like this:
    \verbatim  <name> : <data> \endverbatim
    Here the \c <name> is a string which specifies what the parameter is. The \c <data> may either be a number (dot '.' as
    decimal separator) or a string which will be enclosed in quotation marks '"'.
    The first line of the header identifies the file format and ALV software version (standard <tt>ALV-5000/E-WIN Data</tt>).

    After the header the file contains the correlation data and the intensity of the last run (for multiple runs). The correlation
    data section is started by a line which contains <tt>"Correlation ..."</tt>. After the word Correlation there may be additional
    words specifying what is saved here. Note the quotation marks around this line. The next line contains the column titles,
    each enclosed in quotation marks. In a standard file (two channel auto correlation, two runs) it reads:
    \verbatim "Lag [ms]"	"Average(0)"	"Run  1"	"Run  2"	"Average(1)"	" 1,   0.000"	" 2,   0.000"\endverbatim
    So as you can see the data is separated by whitespaces. The first column is the lag time in milliseconds. The second column
    is the average over all correlation functions (multiple runs!) for the first channel (CH0). The next two columns contain the
    correlation functions themselves. Then the same data is provided for the second channel (CH1). Note that there is a software bug
    and the titles of the next runs is wrong.

    After the correlation data the file also contains the count rates of the last run. This section is started by a line, containing
    <tt>"Count Rate"</tt>. The count rates are stored in multiple columns. The first column contains the time index in seconds,
    and the next columns contain the countrates for CH0 (column 2) and CH1 (column 3).
 */


/** \brief allowed tokens for the ALV file parser
 *  \ingroup quickfit2ALV
 */
enum ALV_TOKENTYPE {
	ALV_EOF,
    ALV_LINEBREAK,
    ALV_COLON,
    ALV_NAME,
    ALV_QUOTED,
    ALV_VALUE
};

/** \brief represents one token (incl. type and value) for the ALV file parser
 *  \ingroup quickfit2ALV
 */
typedef struct {
    ALV_TOKENTYPE type;
    QString value;
    double doubleValue;
} ALV_TOKEN;

/** \brief read in an ALV data file (created by ALV correlator software) and add the result to the supplied datastore
 *  \ingroup quickfit2ALV
 *
 * This parses and reads the file \a filename. The result will be added to the \a datastore as a new item. If there are
 * multiple channels in the data file, each channel will be added as one separate item to the datastore, So for dual channels
 * the datastore will afterwards contain two new items!
 *
 */
void ALV_LoadFile(QString filename, quickfitDataStore* datastore);

/** \brief an exception class used to report errors while parsing ALV files
 *  \ingroup quickfit2ALV
 */
class alv_exception : public std::exception {
    private:
        std::string error;
    public:
        alv_exception(std::string error_desc) throw() { error=error_desc; std::cout<<error<<std::endl; };
        virtual ~alv_exception() throw() {};
        virtual const char* what() const throw() { return error.c_str(); };
};


/** \brief reads the next token from an ALV datafile
 *  \ingroup quickfit2ALV
 *
 * This method implements a simple tokenizer for ALV files
 *    \param alv_file      The file object to read from
 *    \param readingHeader Do we currently read from the file header? If yes names may contain whitespaces (but no lineabreaks)!
 */
ALV_TOKEN ALV_getToken(FILE* alv_file, bool readingHeader);

/** \brief reads until the next \a quotationMark and returns the read string
 *  \ingroup quickfit2ALV
 *
 * this method will swallow all white spaces bevore the quoted string. Then it expects a \a quotationMark.
 * Now it reads until the next quotationMark, or the end of the line.
 */
QString ALV_ReadQuoted(FILE* alv_file, char quotationMark='"');

/** \brief converts a date in the format "<tt>22-Aug-08</tt>" (ALV date format) into a QDate object
 *  \ingroup quickfit2ALV
 */
QDate ALV_toDate(QString d);

/** \brief converts a time in the format "<tt>15:09:10</tt>" (ALV date format) into a QDate object
 *  \ingroup quickfit2ALV
 */
QTime ALV_toTime(QString d);

/** \brief read correlation data from file with one channel and \a runs runs in it
 *  \ingroup quickfit2ALV
 *
 * This function returns the last read token which is the first token which does not belong to the
 * correlation data. You can use this return value in a token evaluation loop!
 *
 * This method also calculates the overal minimum and maximum over the correlation times and
 * over the correlation function value of all runs. If the file contains multiple runs this function
 * also calculates an averaged run (averaged over all runs in every data point!).
 */
ALV_TOKEN ALV_readCorrelation(FILE* alv_file, quickfitDataStoreItem* item1, long runs);

/** \brief read correlation data from file with two channels and \a runs runs in it
 *  \ingroup quickfit2ALV
 *
 * This function returns the last read token which is the first token which does not belong to the
 * correlation data. You can use this return value in a token evaluation loop!
 *
 * This method also calculates the overal minimum and maximum over the correlation times and
 * over the correlation function value of all runs. If the file contains multiple runs this function
 * also calculates an averaged run (averaged over all runs in every data point!).
 */
ALV_TOKEN ALV_readCorrelation(FILE* alv_file, quickfitDataStoreItem* item1, quickfitDataStoreItem* item2, long runs);

/** \brief read rate data from file with one channel. also calculates the mean rate
 *  \ingroup quickfit2ALV
 *
 * This function returns the last read token which is the first token which does not belong to the
 * correlation data. You can use this return value in a token evaluation loop!
 *
 * This method also calculates the overal minimum and maximum over the rate times and
 * over the rates.
 */
ALV_TOKEN ALV_readRate(FILE* alv_file, quickfitDataStoreItem* item1);

/** \brief read rate data from file with two channels. also calculates the mean rate
 *  \ingroup quickfit2ALV
 *
 * This function returns the last read token which is the first token which does not belong to the
 * correlation data. You can use this return value in a token evaluation loop!
 *
 * This method also calculates the overal minimum and maximum over the rate times and
 * over the rates.
 */
ALV_TOKEN ALV_readRate(FILE* alv_file, quickfitDataStoreItem* item1, quickfitDataStoreItem* item2);

/** \brief read a matrix of numbers, used by ALV_readCorrelation() and ALV_readRate(). The result
 *         is written into \a datm.
 *  \ingroup quickfit2ALV
 *
 * This function returns the last read token which is the first token which does not belong to the
 * correlation data. You can use this return value in a token evaluation loop!
 */
ALV_TOKEN ALV_readNumberMatrix(FILE* alv_file, std::vector<std::vector<double> >* datm);


#endif // ALV5000IMPORT_H
