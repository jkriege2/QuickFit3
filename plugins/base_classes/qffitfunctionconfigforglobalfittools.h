#ifndef QFFITFUNCTIONCONFIGFORGLOBALFITTOOLS_H
#define QFFITFUNCTIONCONFIGFORGLOBALFITTOOLS_H

#include "qffitfunctionconfigforglobalfitinterface.h"
#include <QString>

void storeGlobalFitConfig(const QString& filename, const QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig& config);

bool openGlobalFitConfig(const QString& filename, QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig& config);


#endif // QFFITFUNCTIONCONFIGFORGLOBALFITTOOLS_H
