#ifndef QFRDRCURVESINTERFACE_H
#define QFRDRCURVESINTERFACE_H


#include <QString>
#include <QVector>

/*! \brief a simple interface for 1D data curves
    \ingroup qf3rdrdp_imaging_fcs

    \note \b IMPLEMENTATION: There is a general possibility to specify curves made up of two vector-results in any RDR by using "compound-rests"
          (see e.g QFRawDataRecord::evaluationCompoundResult and  QFRawDataRecord::resultsCompoundSet() ). Therefore, any QFRawDataRecord implements this interface.
          If you want to implement a curves-interface in a user-defined RDR with custom addition(!!!) curves, implement QFRDRUserCurvesInterface.
          The implementation of QFRDRCurvesInterface will then summarize the curves from QFRDRUserCurvesInterface and the
          compound results in the RDR!

    \note \b ACCESS: If you want to access the curves in an RDR, use the QFRDRCurvesInterface interface!


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


/*! \brief a simple interface for user-defined 1D data curves
    \ingroup qf3rdrdp_imaging_fcs

    \note \b IMPLEMENTATION: There is a general possibility to specify curves made up of two vector-results in any RDR by using "compound-rests"
          (see e.g QFRawDataRecord::evaluationCompoundResult and  QFRawDataRecord::resultsCompoundSet() ). Therefore, any QFRawDataRecord implements this interface.
          If you want to implement a curves-interface in a user-defined RDR with custom addition(!!!) curves, implement QFRDRUserCurvesInterface.
          The implementation of QFRDRCurvesInterface will then summarize the curves from QFRDRUserCurvesInterface and the
          compound results in the RDR!

    \note \b ACCESS: If you want to access the curves in an RDR, use the QFRDRCurvesInterface interface!


*/class QFRDRUserCurvesInterface {
    public:
        virtual ~QFRDRUserCurvesInterface() {}
        virtual int userCurvesGetCount() const=0;
        virtual QString userCurvesGetName(int index) const=0;
        virtual QVector<double> userCurvesGetX(int index) const=0;
        virtual QVector<double> userCurvesGetXError(int index) const=0;
        virtual QVector<double> userCurvesGetY(int index) const=0;
        virtual QVector<double> userCurvesGetYError(int index) const=0;
        virtual bool userCurvesGetLogX(int index) const=0;
        virtual bool userCurvesGetLogY(int index) const=0;
        virtual QFRDRCurvesInterface::CurveType userCurvesGetType(int index) const=0;
        virtual QString userCurvesGetXLabel(int index) const=0;
        virtual QString userCurvesGetYLabel(int index) const=0;


};

Q_DECLARE_INTERFACE( QFRDRUserCurvesInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRUserCurvesInterface/1.0")


/*! \brief a simple interface for 1D data curves
    \ingroup qf3rdrdp_imaging_fcs

*/
class QFRDRUserCurvesPropertiesEditInterface {
    public:
        virtual ~QFRDRUserCurvesPropertiesEditInterface() {}


        virtual void userCurvesSetName(int index, const QString& name) =0;
        virtual void userCurvesSetLogX(int index, bool l) =0;
        virtual void userCurvesSetLogY(int index, bool l) =0;
        virtual void userCurvesSetType(int index, QFRDRCurvesInterface::CurveType type) =0;
        virtual void userCurvesSetXLabel(int index, const QString& label) =0;
        virtual void userCurvesSetYLabel(int index, const QString& label) =0;

        inline void userCurvesSetTypeI(int index, int type) {
            userCurvesSetType(index, QFRDRCurvesInterface::CurveType(type));
        }


};

Q_DECLARE_INTERFACE( QFRDRUserCurvesPropertiesEditInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRUserCurvesPropertiesEditInterface/1.0")


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

