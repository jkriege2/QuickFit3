/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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
QFLIB_EXPORT bool qfIsSpecialVersion();
QFLIB_EXPORT QUrl qfUpdateXMLURL();
QFLIB_EXPORT QString qfInfoCitationHTML();
QFLIB_EXPORT QString qfInfoCitationBiBTeX();

#endif //QFVERSION_H
