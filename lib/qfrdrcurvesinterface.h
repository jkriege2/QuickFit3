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
            ctPoints=0,
            ctLines=1,
            ctBars=2
        };

        virtual int curvesGetCount() const=0;
        virtual QString curvesGetName(int index) const=0;
        virtual QVector<double> curvesGetX(int index) const=0;
        virtual QVector<double> curvesGetXError(int index) const=0;
        virtual QVector<double> curvesGetY(int index) const=0;
        virtual QVector<double> curvesGetYError(int index) const=0;
        virtual bool curvesGetLogX(int index) const=0;
        virtual bool curvesGetLogY(int index) const=0;
        virtual CurveType curvesGetType(int index) const=0;
        virtual QString curvesGetXLabel(int index) const=0;
        virtual QString curvesGetYLabel(int index) const=0;


};

Q_DECLARE_INTERFACE( QFRDRCurvesInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRCurvesInterface/1.0")


/*! \brief a simple interface for 1D data curves
    \ingroup qf3rdrdp_imaging_fcs

*/
class QFRDRCurvesPropertiesEditInterface {
    public:
        virtual ~QFRDRCurvesPropertiesEditInterface() {}


        virtual void curvesSetName(int index, const QString& name) =0;
        virtual void curvesSetLogX(int index, bool l) =0;
        virtual void curvesSetLogY(int index, bool l) =0;
        virtual void curvesSetType(int index, QFRDRCurvesInterface::CurveType type) =0;
        virtual void curvesSetXLabel(int index, const QString& label) =0;
        virtual void curvesSetYLabel(int index, const QString& label) =0;

        inline void curvesSetTypeI(int index, int type) {
            curvesSetType(index, QFRDRCurvesInterface::CurveType(type));
        }


};

Q_DECLARE_INTERFACE( QFRDRCurvesPropertiesEditInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRCurvesPropertiesEditInterface/1.0")


inline QString QFRDRCurvesInterfaceCurveTypeToString(QFRDRCurvesInterface::CurveType t) {
    switch (t) {
        case QFRDRCurvesInterface::ctPoints: return "points";
        case QFRDRCurvesInterface::ctLines: return "lines";
        case QFRDRCurvesInterface::ctBars: return "histogram";
        default: return "unknown";
    }
}

inline QFRDRCurvesInterface::CurveType QFRDRCurvesInterfaceStringToCurveType(const QString& t) {
    QString tt=t.trimmed().simplified().toLower();
    if (tt=="0" || tt=="points" || tt=="symbols" || tt=="point" || tt=="symbol") return QFRDRCurvesInterface::ctPoints;
    if (tt=="1" || tt=="lines" || tt=="line") return QFRDRCurvesInterface::ctLines;
    if (tt=="2" || tt=="histogram" || tt=="bars") return QFRDRCurvesInterface::ctBars;

    return QFRDRCurvesInterface::ctPoints;
}


#endif // QFRDRCURVESINTERFACE_H

