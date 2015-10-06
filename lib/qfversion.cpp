/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfversion.h"
#include <QSettings>
#include "../version.h"
#include "qftools.h"
#include <QString>
#include <QDate>

QString qfInfoVersion() {
    return QLatin1String(QF_VERSION);
}

QString qfInfoVersionFull() {
    return QLatin1String(QF_VERSION);
}

QString qfInfoVersionStatus() {
    return QLatin1String(VERSION_STATUS);
}

QString qfInfoThanksTo() {
    return QLatin1String(QF_THANKS_TO);
}

QString qfInfoCopyright() {
    return QF_COPYRIGHT;
}

QString qfInfoAuthor() {
    return QLatin1String(QF_AUTHOR);
}

QString qfInfoEmail() {
    return QLatin1String(QF_EMAIL);
}

QString qfInfoMaillist() {
    return QLatin1String(QF_MAILLIST);
}

QString qfInfoMaillistRequest() {
    return QLatin1String(QF_MAILLIST_REQUEST);
}

QString qfInfoWeblink() {
    return QLatin1String(QF_WEBLINK);
}

QString qfInfoSourceWeblink() {
    return QLatin1String(QF_SOURCEWEBLINK);
}
QString qfInfoLicense() {
    return QLatin1String(QF_LICENSE);
}

QString qfInfoGITVersion() {
    QString s= QLatin1String(GITCOMMITCOUNT);
    if (s.contains("exportiert", Qt::CaseInsensitive) || s.contains("exported", Qt::CaseInsensitive)) {
        s="???";
    } else if (s.endsWith("M")) {
        s=s.left(s.length()-1);
    }
    QRegExp rxc("(\\d+)\\:(\\d+)");
    if (rxc.indexIn(s)>=0) {
        int i1=rxc.cap(1).toInt();
        int i2=rxc.cap(2).toInt();
        s.setNum(qMax(i1,i2));
    }
    bool nOK=false;
    int ver=s.toInt(&nOK);
    if (nOK) s=QString::number(2100+ver); // this offset accounts for the change from our internal subversion repository to GitHub!!!
    return s;
}

QString qfInfoCompiler() {
    QString s=QLatin1String(COMPILER);
    QRegExp rxc("(g\\+\\+\\s\\([^\\)]*\\).*)Copyright.*(\\(MACHINE.*\\))");
    rxc.setCaseSensitivity(Qt::CaseInsensitive);
    if (rxc.indexIn(s)>=0) {
        s=rxc.cap(1)+QString(" ")+rxc.cap(2);
    }
    return s;
}

QString qfInfoCompileDate() {
    QString s=QLatin1String(COMPILEDATE);
    QRegExp rx("\\s*(\\d\\d)\\.(\\d\\d)\\.(\\d\\d\\d\\d)\\s*");
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    if (rx.indexIn(s)==0 && rx.matchedLength()==s.length()) {
        s=QDate(rx.cap(3).toInt(), rx.cap(2).toInt(), rx.cap(1).toInt()).toString("yyyy-MM-dd");
    }
    s=s.replace('/', '-');
    return s;
}


QUrl qfUpdateXMLURL() {
    if (qfIsSpecialVersion())  return QUrl::fromEncoded(QString(QF_UPDATESPECIALXMLURL).toLocal8Bit());
    return QUrl::fromEncoded(QString(QF_UPDATEXMLURL).toLocal8Bit());
}

bool qfIsSpecialVersion() {
    QSettings settings("HKEY_LOCAL_MACHINE\\Software\\QuickFit3_"+QString::number(getApplicationBitDepth()), QSettings::NativeFormat);
    return settings.value("SpecialVersion", "std").toString().toLower().trimmed()=="special";
}

QString qfInfoCitationHTML() {
    return QF_CITATION_HTML;
}

QString qfInfoCitationBiBTeX() {
    return QF_CITATION_BIBTEX;
}








