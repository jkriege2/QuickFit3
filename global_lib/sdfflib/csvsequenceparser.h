/*
  Name: csvsequenceparser.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file csvsequenceparser.h
 * \ingroup hlseqgen_csv
 *
 */

#ifndef CSVSEQUENCEPARSER_H
#define CSVSEQUENCEPARSER_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstdio>
#include <stdint.h>

#include "tools.h"
#include "datatable.h"
#include "sequencererror.h"
#include "channelmanager.h"
#include "sequencegenerator.h"
#include "baseimporter.h"


namespace sequencer {
/**
 * \brief this class can generate a sequence by linear interpolation between values in a CSV file. It is a high-level sequence generator.
 * \ingroup hlseqgen_csv
 *
 * The CSV files used here have to contain a comment line that names the single columns. The column names have to be
 * the same as the registered channels in the used sequencer::channelManager object. Such a title comment line looks
 * like this:
 * \verbatim
#$ time, AO0, AO1, DO0, DO1
   \endverbatim
 * It begins with \c #$ and then lists the names of the columns separated by commas. The first column is calles time.
 * This has to be so in any CSV file that defines a sequence. It gives the time in seconds. The file then lists
 * the values of the channels at different times. The sequence has to be sorted in time! So it starts at \c t=0 and ends
 * at some time \c tend. You do not have to give all channels in each line. When generating the sequence this class
 * looks at each column separately and then works as follows:
 *  -# save value at the current time
 *  -# look for the next (in time domain) line in CSV file that has a defined value for the channel
 *  -# <b>analog channels:</b> interpolate linearly between the two timesteps. <b>digital channels:</b> leave the signal
 *     on the old value until the found second timestep. There it changes to the new value.
 * .
 * digital channels can only have the value \c 0 and \c 1. Analog channels may have all values that are allowed
 * by the channel properties.
 *
 * At \c t=0 all channels are set to 0, but this may be overwritten by a line for \c t=0 in the CSV file.
 *
 * In the CSV file, use commas to separate columns and \c # for comments.
 *
 * \test test applications for sequencer::CSVsequenceParser can be found in \link niseqout_test.cpp and
 *       \link seqgen_test.cpp.
 */
class CSVsequenceParser: public baseImporter {
  private:
    /** \brief datatable object for CSV import */
    datatable csv;

    /** \brief hidden standard constructor */
    CSVsequenceParser() {};

  public:
    /** \brief class constructor. \c sg is a pointer to a sequencer::sequenceGenerator object that will be used by the class */
    CSVsequenceParser(sequenceGenerator* sg, channelManager* cm);
    /** \brief class destructor */
    virtual ~CSVsequenceParser();


    /** \brief parses a csv file and generstes a sequence from it. \c sample_timestep gives the period between two samples. This value will be set in the sequenceGenerator object accessed by this method/class. If \c timestep=-1 then the timestep from the sequenceGenerator is used (default).
     *
     * \return 0 when there were no warnings and errors, -1 when there were errors and +1 when there were warning but no errors */
    int import(std::string filename, double sample_timestep=-1);

};

}
#endif // CSVSEQUENCEPARSER_H


/*@}*/
