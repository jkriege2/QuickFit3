#ifndef QFFCSTOOLS_H
#define QFFCSTOOLS_H
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include <QString>

bool qfFCSHasSpecial(const QFRawDataRecord *r, int index, const QString &paramid, double &value, double &error) ;


bool qfFCSOverrideFitFunctionPreset(const QFEvaluationItem* eval, QString paramid, double &value, bool checkPositive=false);
bool qfFCSOverrideFitFunctionPresetError(const QFEvaluationItem *eval, QString paramid, double &value, bool checkPositive=false);
bool qfFCSOverrideFitFunctionPresetFix(const QFEvaluationItem* eval, QString paramid, bool &value);
#endif // QFFCSTOOLS_H
