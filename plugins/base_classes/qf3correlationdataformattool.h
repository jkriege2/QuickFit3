/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2015-01-21 11:37:05 +0100 (Mi, 21 Jan 2015) $  (revision $Rev: 3738 $)

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
        /** \brief load the data in the given file into the current object */
        bool loadFile(const QString& filename, bool propertiesOnly=false);

        /** \brief create a QF3 ASCII Correlation data file from the data in the current object */
        void saveFile(const QString& filename) const;

        QString filename;
        bool ok;
        int runs;
        int rateRuns;
        int channels;
        int correlationTypes;
        QString role;
        QMap<int, QString> roles;

        inline QString getRole(int idx) const {
            QString r=role;
            if (r.isEmpty() && correlationTypes==1) r="ACF";
            return roles.value(idx, r);
        }

        QMap<int, int> preferred_channels;
        int getCorrelationTypePreferredChannel(int idx, int defaultVal=0);

        QString channel_name;
        QMap<int, QString> channel_names;
        inline QString getChannelname(int idx) const {
            return channel_names.value(idx, channel_name);
        }



        QString group;
        QString folder;
        QString input_file;
        QMap<QString, QVariant> properties;

        /** \brief correlation data, first index: correlationType, second index: run, third index: lag-time */
        QList<QList<QVector<double> > > correlations;
        /** \brief lag times in seconds */
        QVector<double> taus;

        inline const QVector<double>& getCorrelation(int run, int corrType=0) const {
            if (corrType>=0 && corrType<correlations.size()) {
                const QList<QVector<double> >& ct=correlations[corrType];
                if (run>=0 && run<ct.size()) {
                    const QVector<double>& ru=ct[run];
                    return ru;
                }
            }
            return empty;
        }
        inline double getCorrelationEntry(int idx, int run=0, int corrType=0) const {
            if (corrType>=0 && corrType<correlations.size()) {
                const QList<QVector<double> >& ct=correlations[corrType];
                if (run>=0 && run<ct.size()) {
                    const QVector<double>& ru=ct[run];
                    return ru.value(idx, NAN);
                }
            }
            return NAN;
        }
        inline void setCorrelationEntry(double value, int idx, int run=0, int corrType=0)  {
            if (corrType>=0 && corrType<correlations.size()) {
                QList<QVector<double> >& ct=correlations[corrType];
                if (run>=0 && run<ct.size()) {
                    QVector<double>& ru=ct[run];
                    if (idx>=0 && idx<ru.size()) {
                        ru[idx]=value;
                    }
                }
            }
        }
        inline void addCorrelationEntry(double value, int run=0, int corrType=0)  {
            if (corrType>=0 && corrType<correlations.size()) {
                QList<QVector<double> >& ct=correlations[corrType];
                if (run>=0 && run<ct.size()) {
                    QVector<double>& ru=ct[run];
                    ru.append(value);
                }
            }
        }



        /** \brief times for count rates in seconds */
        QVector<double> times;
        /** \brief count-rate data (in Hz), first index: channel, second index: run, third index: time-point */
        QList<QList<QVector<double> > > countrates;
        inline const QVector<double>& getCountrates(int run, int channel=0) const {
            if (channel>=0 && channel<countrates.size()) {
                const QList<QVector<double> >& ct=countrates[channel];
                if (run>=0 && run<ct.size()) {
                    const QVector<double>& ru=ct[run];
                    return ru;
                }
            }
            return empty;
        }
        inline double getCountrateEntry(int idx, int run=0, int channel=0) const {
            if (channel>=0 && channel<countrates.size()) {
                const QList<QVector<double> >& ct=countrates[channel];
                if (run>=0 && run<ct.size()) {
                    const QVector<double>& ru=ct[run];
                    return ru.value(idx, NAN);
                }
            }
            return NAN;
        }
        inline void setCountrateEntry(double value, int idx, int run=0, int channel=0)  {
            if (channel>=0 && channel<countrates.size()) {
                QList<QVector<double> >& ct=countrates[channel];
                if (run>=0 && run<ct.size()) {
                    QVector<double>& ru=ct[run];
                    if (idx>=0 && idx<ru.size()) {
                        ru[idx]=value;
                    }
                }
            }
        }
        inline void addCountrateEntry(double value, int run=0, int channel=0)  {
            if (channel>=0 && channel<countrates.size()) {
                QList<QVector<double> >& ct=countrates[channel];
                if (run>=0 && run<ct.size()) {
                    QVector<double>& ru=ct[run];
                    ru.append(value);
                }
            }
        }

        inline QString getErrorMessage() const {
            return errormessage;
        }
        inline bool wasError() const {
            return (errormessage.size()>0);
        }

        void reserveCorrelations();
        void reserveRate();
        void clear();

    protected:
        inline void setError(const QString& message) {
            errormessage=message;
        }
        inline void clearErrors() {
            errormessage.clear();
        }


    private:
        QString errormessage;
        QVector<double> empty;

};

#endif // QF3CORRELATIONDATAFORMATTOOL_H
