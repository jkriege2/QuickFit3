#ifndef QFRDRSIMPLECOUNTRATEINTERFACE_H
#define QFRDRSIMPLECOUNTRATEINTERFACE_H

#include <QtPlugin>

/*! \brief interface for simple count rate information
    \ingroup qf3rdrdp_fcs

*/
class QFRDRSimpleCountRatesInterface {
    public:
        virtual ~QFRDRSimpleCountRatesInterface() {};
        /** \brief returns the average count rate for the given run (or a common value for index<0) */
        virtual double getSimpleCountrateAverage(int run=-1, int channel=0) const=0;
        /** \brief returns the variance of the count rate for the given run (or a common value for index<0) */
        virtual double getSimpleCountrateVariance(int run=-1, int channel=0) const=0;
        /** \brief returns the number of countrate channels */
        virtual int getSimpleCountrateChannels() const=0;
};

Q_DECLARE_INTERFACE( QFRDRSimpleCountRatesInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRSimpleCountRatesInterface/1.1")

#endif // QFRDRSIMPLECOUNTRATEINTERFACE_H
