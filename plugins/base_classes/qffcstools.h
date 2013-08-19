#ifndef QFFCSTOOLS_H
#define QFFCSTOOLS_H
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include <QString>

bool qfFCSHasSpecial(const QFRawDataRecord *r, int index, const QString &paramid, double &value, double &error) ;


bool qfFCSOverrideFitFunctionPreset(const QFEvaluationItem* eval, const QFRawDataRecord* r, QString paramid, double &value, bool checkPositive=false);
bool qfFCSOverrideFitFunctionPresetError(const QFEvaluationItem *eval, const QFRawDataRecord* r, QString paramid, double &value, bool checkPositive=false);
bool qfFCSOverrideFitFunctionPresetFix(const QFEvaluationItem* eval, const QFRawDataRecord* r, QString paramid, bool &value);

/** \brief correct FCCS crosstalk, according to see Bacia, Petrasek, Schwille, "Correcting Spectral Cross-Talk in Dual-Color FCCS", DOI: 10.1002/cphc.201100801 */
void qfFCCSCrosstalkCorrection(double& acf0, double& acf1, double& ccf, double I0, double I1, double crosstalk, int crosstalk_direction=0);
#endif // QFFCSTOOLS_H
