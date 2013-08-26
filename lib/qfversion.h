#ifndef QFVERSION_H
#define QFVERSION_H

#include <QString>
#include <QUrl>
#include "lib_imexport.h"

QFLIB_EXPORT QString qfInfoVersion();
QFLIB_EXPORT QString qfInfoVersionFull();
QFLIB_EXPORT QString qfInfoVersionStatus();
QFLIB_EXPORT QString qfInfoThanksTo();
QFLIB_EXPORT QString qfInfoCopyright();
QFLIB_EXPORT QString qfInfoAuthor();
QFLIB_EXPORT QString qfInfoEmail();
QFLIB_EXPORT QString qfInfoMaillist();
QFLIB_EXPORT QString qfInfoMaillistRequest();
QFLIB_EXPORT QString qfInfoWeblink();
QFLIB_EXPORT QString qfInfoLicense();
QFLIB_EXPORT QString qfInfoSVNVersion();
QFLIB_EXPORT QString qfInfoCompiler();
QFLIB_EXPORT QString qfInfoCompileDate();
QFLIB_EXPORT QUrl qfUpdateXMLURL();

#endif //QFVERSION_H
