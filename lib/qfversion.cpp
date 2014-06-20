/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

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

#include "qfversion.h"
#include <QSettings>
#include "../version.h"
#include "qftools.h"

QString qfInfoVersion() {
    return QString(QF_VERSION);
}

QString qfInfoVersionFull() {
    return QString(QF_VERSION);
}

QString qfInfoVersionStatus() {
    return QString(VERSION_STATUS);
}

QString qfInfoThanksTo() {
    return QString(QF_THANKS_TO);
}

QString qfInfoCopyright() {
    return QString(QF_COPYRIGHT);
}

QString qfInfoAuthor() {
    return QString(QF_AUTHOR);
}

QString qfInfoEmail() {
    return QString(QF_EMAIL);
}

QString qfInfoMaillist() {
    return QString(QF_MAILLIST);
}

QString qfInfoMaillistRequest() {
    return QString(QF_MAILLIST_REQUEST);
}

QString qfInfoWeblink() {
    return QString(QF_WEBLINK);
}

QString qfInfoLicense() {
    return QString(QF_LICENSE);
}

QString qfInfoSVNVersion() {
    return QString(SVNVERSION);
}

QString qfInfoCompiler() {
    return QString(COMPILER);
}

QString qfInfoCompileDate() {
    return QString(COMPILEDATE);
}


QUrl qfUpdateXMLURL() {
    if (qfIsSpecialVersion())  return QUrl::fromEncoded(QString(QF_UPDATESPECIALXMLURL).toLocal8Bit());
    return QUrl::fromEncoded(QString(QF_UPDATEXMLURL).toLocal8Bit());
}

bool qfIsSpecialVersion() {
    QSettings settings("HKEY_LOCAL_MACHINE\\Software\\QuickFit3_"+QString::number(getApplicationBitDepth()), QSettings::NativeFormat);
    return settings.value("SpecialVersion", "std").toString().toLower().trimmed()=="special";
}
