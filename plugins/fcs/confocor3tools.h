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
        struct FCSDataSet {
            QVector<double> tau;
            QList<QVector<double> > corr;
            QVector<double> time;
            QList<QVector<double> > rate;
            QVector<double> pch_photons;
            QList<QVector<double> > pch;
            int position;
            int kinetic;
            int repetition;
            QString channel;
            QString rawdata;
            QString acqtime;
            QMap<QString, QVariant> props;

            FCSDataSet() {
                position=-1;
                kinetic=-1;
                repetition=-1;
            }
        };

        struct ConfocorDataset {
            QString filename;
            QString name;
            QString comment;
            QString sortorder;
            QList<int> positions;
            QList<int> kinetics;
            QList<int> repetitions;
            QList<int> channels;
            QList<FCSDataSet> fcsdatasets;
            bool hasPCH;
            bool hasFCS;
            bool hasCountrate;
            QStringList channelNames;
            /** \brief all data columns available in the file */
            QList<QVector<double> > datacolumns;
            /** \brief lists of indexes used for a given combination of position, kinetic, ... The ID can be obtained from getID() */
            QMap<QString, QList<int> > datacolumnid;

            enum DataType {
                dtFCS=0,
                dtPCH=1,
                dtCountrate=2
            };
            static QString getID(int position, int kinetic, int repetition, int channel, DataType type);
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
        void readBlock(int level, ConfocorDataset& data, QFile& f, bool readNewLine=true, const QString lastLine=QString(), Confocor3Tools::FCSDataSet *fcsds=NULL);
        static QList<ConfocorDataset> oldData;

        QStringList lastErrors;
};

#endif // CONFOCOR3TOOLS_H
