#ifndef QF3CORRELATIONDATAFORMATTOOL_H
#define QF3CORRELATIONDATAFORMATTOOL_H

#include "csvtools.h"
#include <QString>
#include <QMap>
#include <QVector>
#include <QVariant>
#include <QList>
#include "qftools.h"
#include <QTextCodec>

/** \brief This tool reads QF3 ASCII Correlation data, as defined in the FCS-plugin online-help
 *  \ingroup qf3rdrdp_fcs
 *
 */
class QF3CorrelationDataFormatTool
{
    public:
        QF3CorrelationDataFormatTool();
        bool loadFile(const QString& filename, bool propertiesOnly=false);

        QString filename;
        bool ok;
        int runs;
        int channels;
        int correlationTypes;
        QString role;
        QStringList roles;
        QString channel_name;
        QStringList channel_names;
        QString group;
        QString folder;
        QString input_file;
        QMap<QString, QVariant> properties;

        QVector<double> taus;
        /** \brief correlation data, first index: correlationType, second index: run, third index: lag-time */
        QList<QList<QVector<double> > > correlations;

        QVector<double> times;
        /** \brief count-rate data (in Hz), first index: channel, second index: run, third index: time-point */
        QList<QList<QVector<double> > > countrates;

        QString getErrorMessage() const {
            return errormessage;
        }
        bool wasError() const {
            return (errormessage.size()>0);
        }
    protected:
        void reserveCorrelations();
        void reserveRate();
        void clear();
        void setError(const QString& message) {
            errormessage=message;
        }
        void clearErrors() {
            errormessage.clear();
        }

    private:
        QString errormessage="";

};

#endif // QF3CORRELATIONDATAFORMATTOOL_H
