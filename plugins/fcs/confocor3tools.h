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
        struct ConfocorDataset {
            QString filename;
            QMap<QString, QVariant> props;
            int positions;
            int kinetics;
            int repetitions;
            int channels;
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
        };

        ConfocorDataset data;

        Confocor3Tools(const QString& filename);

        void loadFile(ConfocorDataset& data, const QString& filename);

        static void clearStore();

    protected:
        static QList<ConfocorDataset> oldData;
};

#endif // CONFOCOR3TOOLS_H
