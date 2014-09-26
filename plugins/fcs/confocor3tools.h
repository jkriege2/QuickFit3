/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef CONFOCOR3TOOLS_H
#define CONFOCOR3TOOLS_H
#include <QString>
#include <QFile>
#include <QStringList>
#include <QMap>
#include "qftools.h"

/*! \brief Zeiss Confocor3 .fcs import utility
    \ingroup qf3rdrdp_fcs

    \note Each Confocor3 .fcs file contains auto- and crosscorrelation at possibly several positions in a cell. So we can/have to create several RDRs from
          each file. Thus this class has a static store of old parsed data (with currently at most 5 items). When creating a new object with a given filename,
          This class first checks whether the file has already been parsed ... and if so uses the old data. Otherwise the file is parsed using loadFile().


*/
class Confocor3Tools
{
    public:
        enum FCSDatasetType {
            fdtACF=0,
            fdtCCF=1,
            fdtUnkown=-1
        };

        struct FCSDataSet {

            /** \brief correlation function - lag time axis */
            QVector<double> tau;
            /** \brief correlation function - correlation data */
            QList<QVector<double> > corr;
            /** \brief count rates - time axis */
            QVector<double> time;
            /** \brief count rates - data */
            QList<QVector<double> > rate;
            /** \brief PCH - x-axis */
            QVector<double> pch_photons;
            /** \brief PCH - <-axis */
            QList<QVector<double> > pch;
            /** \brief position index */
            int position;
            /** \brief kinetic index */
            int kinetic;
            /** \brief repetition number (interpreted as run/index in QF3) */
            int repetition;
            /** \brief chanel name as in FCS file */
            QString channel;
            /** \brief raw data file */
            QString rawdata;
            /** \brief acquisition timestamp */
            QString acqtime;
            /** \brief all additionally extracted properties */
            QMap<QString, QVariant> props;
            /** \brief the ID after the BEGIN (ignore!) */
            QString id;

            /** \brief channel number (in ACF, or channel No. 1 in FCCS) */
            int channelNo;
            /** \brief channel Number 2 in FCCS */
            int channelNo2;
            /** \brief data in the record */
            FCSDatasetType type;
            /** \brief the group, used by QF3 */
            QString group;
            /** \brief the role, used by QF3 */
            QString role;
            /** \brief points to a record, which contains the countrates for channel 1 */
            int recCnt1;
            /** \brief points to a record, which contains the countrates for channel 1 */
            int recCnt2;
            /** \brief indicates whether this is the reverse FCS */
            bool reverseFCCS;

            FCSDataSet() {
                reverseFCCS=false;
                position=-1;
                kinetic=-1;
                repetition=-1;
                channelNo=-1;
                channelNo2=-1;
                type=fdtUnkown;
                recCnt1=-1;
                recCnt2=-1;
            }
        };

        struct ConfocorDataset {
            QString filename;
            QString name;
            QString comment;
            QString sortorder;
            QList<FCSDataSet> fcsdatasets;
            QStringList getGroups() const;
            QStringList getRoles() const;
            QList<QVector<int> > getRepetitions() const;
            QList<int> getPositions() const;
            QList<int> getKinetics() const;
            void clear();
            ConfocorDataset() {
                clear();
            }

        };


        Confocor3Tools(const QString& filename);
        Confocor3Tools();
        bool loadFile(const QString& filename);


        static void clearStore();

        const ConfocorDataset& getData() const { return data; }

        QStringList getLastErrors() const { return lastErrors; }
        bool wasError() const { return lastErrors.size()>0; }

    protected:
        void clearErrors() { lastErrors.clear(); }
        void setError(const QString& message) {
            lastErrors.append(message);
        }

        ConfocorDataset data;
        bool loadFile(ConfocorDataset& data, const QString& filename);
        void readBlock(int level, ConfocorDataset& data, QIODevice &f, bool readNewLine=true, const QString lastLine=QString(), Confocor3Tools::FCSDataSet *fcsds=NULL);
        QString readArray(QIODevice& f, int lines, int cols, bool &readNextLine, QList<QVector<double> >& dataout);
        static QList<ConfocorDataset> oldData;

        QStringList lastErrors;
};

#endif // CONFOCOR3TOOLS_H
