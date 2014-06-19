#include "qfhtmlhelptools.h"
#include "qfpluginservices.h"
#include "qfplugin.h"
#include "qffitalgorithm.h"
#include "qffitfunction.h"
#include "qffitalgorithmmanager.h"
#include "qffitfunctionmanager.h"


QString removeHTMLComments(const QString& data) {
     QRegExp rxComments("<!--(.*)-->", Qt::CaseInsensitive);
     rxComments.setMinimal(true);
     QString data1=data;
     data1.remove(rxComments);
     return data1;
}

QString transformQF3HelpHTMLFile(const QString& filename, const QString& defaultText, bool removeNonReplaced, const QFHelpReplacesList& more_replaces, bool insertTooltips, bool dontCreatePics, bool isMainHelp) {
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream in(&f);
        return transformQF3HelpHTML(in.readAll(), filename, removeNonReplaced, more_replaces, insertTooltips, dontCreatePics, isMainHelp);
    } else {
        return transformQF3HelpHTML(defaultText, filename, removeNonReplaced, more_replaces, insertTooltips, dontCreatePics, isMainHelp);
    }
}

QString transformQF3HelpHTML(const QString& input_html, const QString& filename, bool removeNonReplaced, const QList<QPair<QString, QString> >& more_replaces, bool insertTooltips, bool dontCreatePics, bool isMainHelp) {
    return QFPluginServices::getInstance()->transformQF3HelpHTML(input_html, filename, removeNonReplaced, more_replaces, insertTooltips,  dontCreatePics, isMainHelp);
}
