#ifndef QFRDRCURVESINTERFACE_H
#define QFRDRCURVESINTERFACE_H


#include <QString>
#include <QVector>

/*! \brief a simple interface for 1D data curves
    \ingroup qf3rdrdp_imaging_fcs

*/
class QFRDRCurvesInterface {
    public:
        virtual ~QFRDRCurvesInterface() {}

        enum CurveType {
            ctPoints,
            ctLines,
            ctHistogram
        };

        virtual int curvesGetCount() const=0;
        virtual QString curvesGetName(int index=0) const=0;
        virtual const double* curvesGetX(int index=0) const=0;
        virtual const double* curvesGetXError(int index=0) const=0;
        virtual const double* curvesGetY(int index=0) const=0;
        virtual const double* curvesGetYError(int index=0) const=0;
        virtual bool curvesGetLogX(int index) const=0;
        virtual bool curvesGetLogY(int index) const=0;
        virtual CurveType curvesGetType(int index) const=0;
        virtual QString curvesGetXLabel(int index) const=0;
        virtual QString curvesGetYLabel(int index) const=0;


};

Q_DECLARE_INTERFACE( QFRDRCurvesInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRCurvesInterface/1.0")


#endif // QFRDRCURVESINTERFACE_H

