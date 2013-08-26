#include "qfversion.h"

#include "../version.h"

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
    return QUrl::fromEncoded(QString(QF_UPDATEXMLURL).toLocal8Bit());
}
