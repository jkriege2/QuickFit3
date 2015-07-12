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

#include "qfhtmlhelptools.h"
#include "qfpluginservices.h"
#include "qfplugin.h"
#include "qffitalgorithm.h"
#include "qffitfunction.h"
#include "qffitalgorithmmanager.h"
#include "qffitfunctionmanager.h"
#include "qftools.h"

QString HTMLGetTitle(const QString& data) {
    QRegExp rxTitle("<title>(.*)</title>", Qt::CaseInsensitive);
    if (rxTitle.indexIn(data) != -1) return rxTitle.cap(1);
    return "";
}

QString removeHTMLComments(const QString& data) {
     QRegExp rxComments("<!--(.*)-->", Qt::CaseInsensitive);
     rxComments.setMinimal(true);
     QString data1=data;
     data1.remove(rxComments);
     return data1;
}

QString removeHTML(const QString& data) {
    QRegExp rxMarkup("<[^>]*>", Qt::CaseInsensitive);
    QRegExp rxBR("<\\s*br\\s*[\\/]\\s*>", Qt::CaseInsensitive);
     rxMarkup.setMinimal(true);
     QString data1=data;
     data1=data1.replace(rxBR, "\n");
     data1=data1.remove(rxMarkup);
     return data1;
}

QString transformQF3HelpHTMLFile(const QString& filename, const QString& defaultText, bool removeNonReplaced, const QFHelpReplacesList& more_replaces, bool insertTooltips, bool dontCreatePics, bool isMainHelp, const QString& texfilenameaddition) {
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream in(&f);
        in.setCodec(QTextCodec::codecForName("UTF-8"));
        return transformQF3HelpHTML(in.readAll(), filename, removeNonReplaced, more_replaces, insertTooltips, dontCreatePics, isMainHelp, texfilenameaddition);
    } else {
        return transformQF3HelpHTML(defaultText, filename, removeNonReplaced, more_replaces, insertTooltips, dontCreatePics, isMainHelp, texfilenameaddition);
    }
}

QString transformQF3HelpHTML(const QString& input_html, const QString& filename, bool removeNonReplaced, const QList<QPair<QString, QString> >& more_replaces, bool insertTooltips, bool dontCreatePics, bool isMainHelp, const QString& texfilenameaddition) {
    return QFPluginServices::getInstance()->transformQF3HelpHTML(input_html, filename, removeNonReplaced, more_replaces, insertTooltips,  dontCreatePics, isMainHelp, texfilenameaddition);
}

struct QFHTMLEntity {
    QString entity;
    QChar character;
};

QFHTMLEntity QFHTMLNamedEntites[] = {
    {"quot", '\"'},
    {"apos", '\''},
    {"amp", '&'},
    {"lt", '<'},
    {"gt", '>'},
    {"euro", '?'},
    {"nbsp", QChar(160)},
    {"iexcl", QChar(161)},
    {"cent", QChar(162)},
    {"pound", QChar(163)},
    {"curren", QChar(164)},
    {"yen", QChar(165)},
    {"brvbar", QChar(166)},
    {"sect", QChar(167)},
    {"uml", QChar(168)},
    {"copy", QChar(169)},
    {"ordf", QChar(170)},
    {"laquo", QChar(171)},
    {"not", QChar(172)},
    {"shy", QChar(173)},
    {"reg", QChar(174)},
    {"macr", QChar(175)},
    {"deg", QChar(176)},
    {"plusmn", QChar(177)},
    {"sup2", QChar(178)},
    {"sup3", QChar(179)},
    {"acute", QChar(180)},
    {"micro", QChar(181)},
    {"para", QChar(182)},
    {"middot", QChar(183)},
    {"cedil", QChar(184)},
    {"sup1", QChar(185)},
    {"ordm", QChar(186)},
    {"raquo", QChar(187)},
    {"frac14", QChar(188)},
    {"frac12", QChar(189)},
    {"frac34", QChar(190)},
    {"iquest", QChar(191)},
    {"Agrave", QChar(192)},
    {"Aacute", QChar(193)},
    {"Acirc", QChar(194)},
    {"Atilde", QChar(195)},
    {"Auml", QChar(196)},
    {"Aring", QChar(197)},
    {"AElig", QChar(198)},
    {"Ccedil", QChar(199)},
    {"Egrave", QChar(200)},
    {"Eacute", QChar(201)},
    {"Ecirc", QChar(202)},
    {"Euml", QChar(203)},
    {"Igrave", QChar(204)},
    {"Iacute", QChar(205)},
    {"Icirc", QChar(206)},
    {"Iuml", QChar(207)},
    {"ETH", QChar(208)},
    {"Ntilde", QChar(209)},
    {"Ograve", QChar(210)},
    {"Oacute", QChar(211)},
    {"Ocirc", QChar(212)},
    {"Otilde", QChar(213)},
    {"Ouml", QChar(214)},
    {"times", QChar(215)},
    {"Oslash", QChar(216)},
    {"Ugrave", QChar(217)},
    {"Uacute", QChar(218)},
    {"Ucirc", QChar(219)},
    {"Uuml", QChar(220)},
    {"Yacute", QChar(221)},
    {"THORN", QChar(222)},
    {"szlig", QChar(223)},
    {"agrave", QChar(224)},
    {"aacute", QChar(225)},
    {"acirc", QChar(226)},
    {"atilde", QChar(227)},
    {"auml", QChar(228)},
    {"aring", QChar(229)},
    {"aelig", QChar(230)},
    {"ccedil", QChar(231)},
    {"egrave", QChar(232)},
    {"eacute", QChar(233)},
    {"ecirc", QChar(234)},
    {"euml", QChar(235)},
    {"igrave", QChar(236)},
    {"iacute", QChar(237)},
    {"icirc", QChar(238)},
    {"iuml", QChar(239)},
    {"eth", QChar(240)},
    {"ntilde", QChar(241)},
    {"ograve", QChar(242)},
    {"oacute", QChar(243)},
    {"ocirc", QChar(244)},
    {"otilde", QChar(245)},
    {"ouml", QChar(246)},
    {"divide", QChar(247)},
    {"oslash", QChar(248)},
    {"ugrave", QChar(249)},
    {"uacute", QChar(250)},
    {"ucirc", QChar(251)},
    {"uuml", QChar(252)},
    {"yacute", QChar(253)},
    {"thorn", QChar(254)},
    {"yuml", QChar(255)},
    {"fnof", QChar(402)},
  /* Greek */
    {"Alpha", QChar(913)},
    {"Beta", QChar(914)},
    {"Gamma", QChar(915)},
    {"Delta", QChar(916)},
    {"Epsilon", QChar(917)},
    {"Zeta", QChar(918)},
    {"Eta", QChar(919)},
    {"Theta", QChar(920)},
    {"Iota", QChar(921)},
    {"Kappa", QChar(922)},
    {"Lambda", QChar(923)},
    {"Mu", QChar(924)},
    {"Nu", QChar(925)},
    {"Xi", QChar(926)},
    {"Omicron", QChar(927)},
    {"Pi", QChar(928)},
    {"Rho", QChar(929)},
    {"Sigma", QChar(931)},
    {"Tau", QChar(932)},
    {"Upsilon", QChar(933)},
    {"Phi", QChar(934)},
    {"Chi", QChar(935)},
    {"Psi", QChar(936)},
    {"Omega", QChar(937)},
    {"alpha", QChar(945)},
    {"beta", QChar(946)},
    {"gamma", QChar(947)},
    {"delta", QChar(948)},
    {"epsilon", QChar(949)},
    {"zeta", QChar(950)},
    {"eta", QChar(951)},
    {"theta", QChar(952)},
    {"iota", QChar(953)},
    {"kappa", QChar(954)},
    {"lambda", QChar(955)},
    {"mu", QChar(956)},
    {"nu", QChar(957)},
    {"xi", QChar(958)},
    {"omicron", QChar(959)},
    {"pi", QChar(960)},
    {"rho", QChar(961)},
    {"sigmaf", QChar(962)},
    {"sigma", QChar(963)},
    {"tau", QChar(964)},
    {"upsilon", QChar(965)},
    {"phi", QChar(966)},
    {"chi", QChar(967)},
    {"psi", QChar(968)},
    {"omega", QChar(969)},
    {"thetasym", QChar(977)},
    {"upsih", QChar(978)},
    {"piv", QChar(982)},
  /* General Punctuation */
    {"bull", QChar(8226)},
    {"hellip", QChar(8230)},
    {"prime", QChar(8242)},
    {"Prime", QChar(8243)},
    {"oline", QChar(8254)},
    {"frasl", QChar(8260)},
  /* Letterlike Symbols */
    {"weierp", QChar(8472)},
    {"image", QChar(8465)},
    {"real", QChar(8476)},
    {"trade", QChar(8482)},
    {"alefsym", QChar(8501)},
  /* Arrows */
    {"larr", QChar(8592)},
    {"uarr", QChar(8593)},
    {"rarr", QChar(8594)},
    {"darr", QChar(8595)},
    {"harr", QChar(8596)},
    {"crarr", QChar(8629)},
    {"lArr", QChar(8656)},
    {"uArr", QChar(8657)},
    {"rArr", QChar(8658)},
    {"dArr", QChar(8659)},
    {"hArr", QChar(8660)},
  /* Mathematical Operators */
    {"forall", QChar(8704)},
    {"part", QChar(8706)},
    {"exist", QChar(8707)},
    {"empty", QChar(8709)},
    {"nabla", QChar(8711)},
    {"isin", QChar(8712)},
    {"notin", QChar(8713)},
    {"ni", QChar(8715)},
    {"prod", QChar(8719)},
    {"sum", QChar(8721)},
    {"minus", QChar(8722)},
    {"lowast", QChar(8727)},
    {"radic", QChar(8730)},
    {"prop", QChar(8733)},
    {"infin", QChar(8734)},
    {"and", QChar(8743)},
    {"or", QChar(8744)},
    {"cap", QChar(8745)},
    {"cup", QChar(8746)},
    {"int", QChar(8747)},
    {"there4", QChar(8756)},
    {"sim", QChar(8764)},
    {"cong", QChar(8773)},
    {"asymp", QChar(8776)},
    {"ne", QChar(8800)},
    {"equiv", QChar(8801)},
    {"le", QChar(8804)},
    {"ge", QChar(8805)},
    {"sub", QChar(8834)},
    {"sup", QChar(8835)},
    {"nsub", QChar(8836)},
    {"sube", QChar(8838)},
    {"supe", QChar(8839)},
    {"oplus", QChar(8853)},
    {"otimes", QChar(8855)},
    {"perp", QChar(8869)},
    {"sdot", QChar(8901)},
  /* Miscellaneous Technical */
    {"lceil", QChar(8968)},
    {"rceil", QChar(8969)},
    {"lfloor", QChar(8970)},
    {"rfloor", QChar(8971)},
    {"lang", QChar(9001)},
    {"rang", QChar(9002)},
  /* Geometric Shapes */
    {"loz", QChar(9674)},
  /* Miscellaneous Symbols */
    {"spades", QChar(9824)},
    {"clubs", QChar(9827)},
    {"hearts", QChar(9829)},
    {"diams", QChar(9830)},
    {"quot", QChar(34)},
    {"amp", QChar(38)},
    {"lt", QChar(60)},
    {"gt", QChar(62)},
  /* Latin Extended-A */
    {"OElig", QChar(338)},
    {"oelig", QChar(339)},
    {"Scaron", QChar(352)},
    {"scaron", QChar(353)},
    {"Yuml", QChar(376)},
  /* Spacing Modifier Letters */
    {"circ", QChar(710)},
    {"tilde", QChar(732)},
  /* General Punctuation */
    {"ensp", QChar(8194)},
    {"emsp", QChar(8195)},
    {"thinsp", QChar(8201)},
    {"zwnj", QChar(8204)},
    {"zwj", QChar(8205)},
    {"lrm", QChar(8206)},
    {"rlm", QChar(8207)},
    {"ndash", QChar(8211)},
    {"mdash", QChar(8212)},
    {"lsquo", QChar(8216)},
    {"rsquo", QChar(8217)},
    {"sbquo", QChar(8218)},
    {"ldquo", QChar(8220)},
    {"rdquo", QChar(8221)},
    {"bdquo", QChar(8222)},
    {"dagger", QChar(8224)},
    {"Dagger", QChar(8225)},
    {"permil", QChar(8240)},
    {"lsaquo", QChar(8249)},
    {"rsaquo", QChar(8250)},
    {"euro", QChar(8364)},
    {"", QChar('\0')}
};

QString escapeHTML(const QString& data) {
    QString d;
    for (int i=0; i<data.size(); i++) {
        int j=0;
        bool found=false;
        while (QFHTMLNamedEntites[j].character!='\0') {
            if (data[i]==QFHTMLNamedEntites[j].character) {
                d+=QString(QString("&")+QFHTMLNamedEntites[j].entity+QString(";"));
                found=true;
                break;
            }
            j++;
        }
        if (!found) {
            if (data[i]>127) d+=QString("&#%1;").arg(data[i].unicode());
            else d+=data[i];
        }
    }
    return d;
}



QString deescapeHTML(const QString& data) {
    QString d=data;
    QRegExp rxEntityHex("\\&\\#x([0123456789aAbBcCdDeEfF]+)\\;");
    rxEntityHex.setCaseSensitivity(Qt::CaseInsensitive);
    rxEntityHex.setMinimal(true);
    QRegExp rxEntityNum("\\&\\#([0123456789]+)\\;");
    rxEntityNum.setCaseSensitivity(Qt::CaseInsensitive);
    rxEntityNum.setMinimal(true);
    QRegExp rxEntity("\\&(\\w+)\\;");
    rxEntity.setCaseSensitivity(Qt::CaseInsensitive);
    rxEntity.setMinimal(true);

    int pos = 0;
    while ((pos = rxEntityHex.indexIn(d, pos)) != -1) {
        QString out= QChar(rxEntityHex.cap(1).toUInt(NULL,16));
        d=d.replace(pos,rxEntityHex.matchedLength(),out);
        pos += out.size();
    }

    pos = 0;
    while ((pos = rxEntityNum.indexIn(d, pos)) != -1) {
        QString out= QChar(rxEntityNum.cap(1).toUInt(NULL,10));
        d=d.replace(pos,rxEntityNum.matchedLength(),out);
        pos += out.size();
    }

    pos = 0;
    while ((pos = rxEntity.indexIn(d, pos)) != -1) {
        QString out= rxEntity.cap(1);
        int j=0;
        while (QFHTMLNamedEntites[j].character!='\0') {
            if (out==QFHTMLNamedEntites[j].entity) {
                out=QFHTMLNamedEntites[j].character;
                break;
            }
            j++;
        }
        d=d.replace(pos,rxEntity.matchedLength(),out);
        pos += out.size();
    }
    return d;
}

void parseGlobalreplaces(const QString& directory) {
#ifndef QFMATHPARSER_MATHPARSERTEST
    QDir d(directory);
    if (QFile::exists(d.absoluteFilePath("globalreplaces.ini"))) {
        QSettings setLocalReplace(d.absoluteFilePath("globalreplaces.ini"), QSettings::IniFormat);

        QStringList keys=setLocalReplace.childKeys();
        for (int i=0; i<keys.size(); i++) {

            QFPluginServices::getInstance()->setOrAddHTMLReplacement(keys[i], setLocalReplace.value(keys[i], "").toString());
        }
    }
#endif
}

#ifndef QFMATHPARSER_MATHPARSERTEST
void parseFAQInt(const QString& filename, const QString& fc, const QString& regexp, int capQ, int capL, const QString& pluginID, QMap<QString, QFFAQData> &faqs) {
#ifndef QFMATHPARSER_MATHPARSERTEST
    QRegExp rxFAQ;
    rxFAQ=QRegExp(regexp);
    rxFAQ.setMinimal(true);
    int count = 0;
    int pos = 0;
    while ((pos = rxFAQ.indexIn(fc, pos)) != -1) {
        ++count;
        FAQEntry e;
        e.question=removeHTMLComments(rxFAQ.cap(capQ)).simplified().trimmed();
        if (e.question.startsWith("<b>")) {
            e.question.remove(0,3);
            if (e.question.endsWith("</b>")) e.question=e.question.left(e.question.length()-4);
        }
        e.link=QFileInfo(filename).absoluteFilePath()+"#"+rxFAQ.cap(capL);
//        qDebug()<<"   adding "<<e.question<<e.link;
//        qDebug()<<"          1: "<<rxFAQ.cap(1);
//        qDebug()<<"          2: "<<rxFAQ.cap(2);
//        qDebug()<<"          3: "<<rxFAQ.cap(3);
//        qDebug()<<"          4: "<<rxFAQ.cap(4);
//        qDebug()<<"          5: "<<rxFAQ.cap(5);
        faqs[pluginID].append(e);
        pos += rxFAQ.matchedLength();
    }
#endif
}


void parseFAQ(const QString& filename, const QString& pluginID, QMap<QString, QFFAQData> &faqs) {
    QFile f(filename);
    if (f.open(QFile::ReadOnly)) {
//        qDebug()<<"parse FAQ "<<filename;
        QString fc=f.readAll();
        f.close();
//        qDebug()<<"          "<<fc;
        parseFAQInt(filename, fc, "<!--\\s*faq\\s*-->\\s*<\\s*a\\s*name\\s*\\=\\s*\\\"\\#?([^#]*)\\\"\\s*/?\\s*>\\s*(.*)\\s*<!--\\s*/\\s*faq\\s*-->", 2, 1, pluginID, faqs);
        parseFAQInt(filename, fc, "\\$\\$faq_start\\$\\$\\s*<\\s*a\\s*name\\s*\\=\\s*\\\"\\#?([^#]*)\\\"\\s*/?\\s*>\\s*(.*)\\s*\\$\\$faq_answer\\$\\$", 2, 1, pluginID, faqs);
    }
}



void parseTooltips(const QString& directory, QMap<QString, QFToolTipsData>& tooltips) {
    QDir d(directory);
    if (QFile::exists(d.absoluteFilePath("tooltips.ini"))) {
        QSettings setTooltips(d.absoluteFilePath("tooltips.ini"), QSettings::IniFormat);

        QStringList keys=setTooltips.childKeys();
        for (int i=0; i<keys.size(); i++) {
            tooltips[keys[i]].tooltip=setTooltips.value(keys[i], QObject::tr("<i>no tooltip available</i>")).toString();
            tooltips[keys[i]].tooltipfile=d.absoluteFilePath("tooltips.ini");
            //qDebug()<<keys[i]<<setTooltips.value(keys[i], QObject::tr("<i>no tooltip available</i>")).toString();
        }
        QMapIterator<QString, QFToolTipsData> it(tooltips);
        while (it.hasNext()) {
            it.next();
            if (it.value().tooltip.startsWith("%") && tooltips.contains(it.value().tooltip.mid(1))) {
                //qDebug()<<it.key()<<": "<<it.value().tooltip<<"->"<<tooltips[it.value().tooltip.mid(1)].tooltip<<" ["<<it.value().tooltip.mid(1)<<"]";
                tooltips[it.key()]=tooltips[it.value().tooltip.mid(1)];
            }
        }
    }

}

#endif
void parseAutolinks(const QString& directory, QMap<QString, QString>& autolinks) {
    QDir d(directory);
    if (QFile::exists(d.absoluteFilePath("autolinks.ini"))) {
        QSettings setAutolinks(d.absoluteFilePath("autolinks.ini"), QSettings::IniFormat);

        QStringList keys=setAutolinks.childKeys();
        for (int i=0; i<keys.size(); i++) {
            QString l=setAutolinks.value(keys[i]).toString();
            if (!l.startsWith("$$") && !l.startsWith("http://") && !l.startsWith("https://") && !l.startsWith("ftp://") && !l.startsWith("ftps://")) {
                l=d.absoluteFilePath(l);
            }
            if (!l.isEmpty()) {
                autolinks[keys[i]]=l;
            }
        }

        QMapIterator<QString, QString> ita(autolinks);
        while (ita.hasNext()) {
            ita.next();
            if (ita.value().startsWith("%") && autolinks.contains(ita.value().mid(1))) {
                autolinks[ita.key()]=autolinks[ita.value().mid(1)];
            }
        }
    }
}
