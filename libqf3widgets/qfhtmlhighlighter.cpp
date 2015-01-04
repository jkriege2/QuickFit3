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


#include "qfhtmlhighlighter.h"
#include "programoptions.h"

QFHTMLHighlighter::QFHTMLHighlighter(const QString& settingsDir, QTextDocument *parent):
    QSyntaxHighlighter(parent)
{

    QString sd=settingsDir;
    if (sd.isEmpty()) {
        sd=ProgramOptions::getInstance()->getAssetsDirectory()+"/highlighter/html/";
    }
    QString prefix="";

    QSettings settings(sd+"/highlight.ini", QSettings::IniFormat);
    m_formats[QFHTMLHighlighter::Entity]=loadFormat(settings, prefix+"style/entity",  "darkgreen", false, true, false);
    m_formats[QFHTMLHighlighter::Tag]=loadFormat(settings, prefix+"style/tag",  "darkred", true, false, false);
    m_formats[QFHTMLHighlighter::Comment]=loadFormat(settings, prefix+"style/comment",  "darkgray", false, true, false);
    m_formats[QFHTMLHighlighter::Text]=loadFormat(settings, prefix+"style/text",  "black", false, false, false);
    m_formats[QFHTMLHighlighter::Special1]=loadFormat(settings, prefix+"style/special1",  "darkblue", true, false, false);
    m_formats[QFHTMLHighlighter::Special2]=loadFormat(settings, prefix+"style/special2",  "darkblue", true, true, false);



}

void QFHTMLHighlighter::setUseSpecial1(const QString &specialStart, const QString &specialEnd)
{
    special1Start=specialStart;
    special1End=specialEnd;
}

void QFHTMLHighlighter::setUseSpecial2(const QString &specialStart, const QString &specialEnd)
{
    special2Start=specialStart;
    special2End=specialEnd;
}

void QFHTMLHighlighter::setFormat(int start, int count, const QTextCharFormat &format)
{
    QSyntaxHighlighter::setFormat(start, count, format);
}

void QFHTMLHighlighter::setFormat(int start, int count, const QTextCharFormat &format, int state)
{
    QTextCharFormat f=format;



    if ((state&TagH1)==TagH1) {
        QFont ff=f.font();
        ff.setUnderline(true);
        ff.setBold(true);
        ff.setPointSizeF(ff.pointSizeF()*1.7);
        f.setFont(ff);
    }

    if ((state&TagH2)==TagH2) {
        QFont ff=f.font();
        ff.setUnderline(true);
        ff.setBold(true);
        ff.setPointSizeF(ff.pointSizeF()*1.55);
        f.setFont(ff);
    }

    if ((state&TagH3)==TagH3) {
        QFont ff=f.font();
        ff.setUnderline(true);
        ff.setBold(true);
        ff.setPointSizeF(ff.pointSizeF()*1.35);
        f.setFont(ff);
    }

    if ((state&TagH46)==TagH46) {
        QFont ff=f.font();
        ff.setUnderline(true);
        ff.setPointSizeF(ff.pointSizeF()*1.2);
        f.setFont(ff);
    }

    if ((state&TagP)==TagP) {
        QFont ff=f.font();
        ff.setBold(true);
        f.setFont(ff);
        f.setBackground(QColor(0xF0,0xFA,0xFF));
    }
    if ((state&TagCENTER)==TagCENTER) {
        QFont ff=f.font();
        ff.setBold(true);
        f.setFont(ff);
        f.setBackground(QColor(0xE0,0xF5,0xFF));
    }

    if ((state&TagLIST)==TagLIST) {
        f.setBackground(QColor(0xE6,0xE6,0xB6));
    }
    if ((state&TagLI)==TagLI) {
        f.setBackground(QColor(0xFF,0xFF,0xCC));
    }

     if ((state&TagTABLE)==TagTABLE) {
        QFont ff=f.font();
        ff.setBold(true);
        f.setFont(ff);
        f.setBackground(QColor(0x99,0x99,0x99));
    }
    if ((state&TagTR)==TagTR) {
        QFont ff=f.font();
        ff.setBold(true);
        f.setFont(ff);
        f.setBackground(QColor(0xB2,0xB2,0xB2));
    }
    if ((state&TagTH)==TagTH) {
        QFont ff=f.font();
        ff.setBold(true);
        f.setFont(ff);
        f.setBackground(QColor(0xCC,0xCC,0xCC));
    }
    if ((state&TagTD)==TagTD) {
        QFont ff=f.font();
        ff.setBold(true);
        f.setFont(ff);
        f.setBackground(QColor(0xE6,0xE6,0xE6));
    }

    if ((state&TagDIV)==TagDIV) {
        QFont ff=f.font();
        ff.setBold(true);
        f.setFont(ff);
        f.setBackground(QColor(0xD1,0xF0,0xFF));
    }
    if ((state&TagSMALL)==TagSMALL) {
        QFont ff=f.font();
        ff.setPointSizeF(ff.pointSizeF()*0.75);
        f.setFont(ff);
    }
    if ((state&TagB)==TagB) {
        QFont ff=f.font();
        ff.setBold(true);
        f.setFont(ff);
    }

    if ((state&TagI)==TagI) {
        QFont ff=f.font();
        ff.setItalic(true);
        f.setFont(ff);
    }

    if ((state&TagU)==TagU) {
        QFont ff=f.font();
        ff.setUnderline(true);
        f.setFont(ff);
    }

    if ((state&TagA)==TagA) {
        QFont ff=f.font();
        ff.setUnderline(true);
        f.setFont(ff);
        f.setForeground(QColor("blue"));
    }
    /*
     * if (additionalFormat.setBold) {
        QFont ff=f.font();
        ff.setBold(additionalFormat.bold);
        f.setFont(ff);
    }

    if (additionalFormat.setItalic) {
        QFont ff=f.font();
        ff.setItalic(additionalFormat.italic);
        f.setFont(ff);
    }

    if (additionalFormat.setUnderlined) {
        QFont ff=f.font();
        ff.setUnderline(additionalFormat.underlined);
        f.setFont(ff);
    }

    if (additionalFormat.setFontFamily) {
        QFont ff=f.font();
        ff.setFamily(additionalFormat.fontFamily);
        f.setFont(ff);
    }

    if (additionalFormat.setFontSize) {
        QFont ff=f.font();
        ff.setPointSizeF(additionalFormat.fontSize);
        f.setFont(ff);
    }
    if (additionalFormat.incFontSize) {
        QFont ff=f.font();
        ff.setPointSizeF(ff.pointSizeF()+additionalFormat.fontSizeInc);
        f.setFont(ff);
    }*/

    QSyntaxHighlighter::setFormat(start, count, f);
}

void QFHTMLHighlighter::setBaseState(int &state, int bState)
{
    state=state&0xFFFFFFF0;
    state=state+(bState&0x000000F);
}

void QFHTMLHighlighter::setSubState(int &state, int sState, bool enabled)
{
    if (enabled) {
        state=state|sState;
    } else {
        state=state&(~sState);
    }
}

void QFHTMLHighlighter::highlightBlock(const QString &text)
{    
    int state = previousBlockState();
    if (state<0) state=NormalState;
    int len = text.length();
    int start = 0;
    int pos = 0;
    bool atEnd;
    QString tagName="";
    while (pos < len) {
        if ((state & 0x0F)==InComment) {
            start = pos;
            while (pos < len) {
                if (text.mid(pos, 3) == "-->") {
                    pos += 3;
                    if (lastStates.size()>0) state=lastStates.pop();//NormalState;
                    else setBaseState(state, NormalState);//NormalState;
                    break;
                } else {
                    ++pos;
                }
            }
            setFormat(start, pos - start, m_formats[Comment]);
        } else if ((state & 0x0F)==InTag) {
            QChar quote = QChar::Null;
            start = pos;
            QString parseTag, parseTagOld;
            while (pos < len) {
                QChar ch = text.at(pos);
                tagName=tagName+ch;
                if (quote.isNull()) {
                    if (ch == '\'' || ch == '"') {
                        quote = ch;
                    } else if (ch == '>') {
                        ++pos;
                        if (lastStates.size()>0) state=lastStates.pop();//NormalState;
                        else setBaseState(state, NormalState);//NormalState;

                        if (tagName.size()>0) {
                            QString tagNameOld=tagName.toLower().simplified().trimmed();
                            tagName=tagName.toLower().simplified().trimmed();
                            if (tagName.startsWith('<')) tagName=tagName.right(tagName.size()-1);
                            if (tagName.endsWith('>')) tagName=tagName.left(tagName.size()-1);
                            atEnd=tagName.startsWith('/');
                            if (atEnd) tagName=tagName.right(tagName.size()-1);
                            if (tagName.contains(' ')) {
                                tagName=tagName.split(' ').first();
                            }
                            parseTag=tagName;
                            parseTagOld=tagNameOld;
                            //qDebug()<<tagName<<tagNameOld;
                        }
                        tagName="";
                        break;
                    }
                } else if (ch == quote) {
                    quote = QChar::Null;
                }
                ++pos;
            }

            int oldstate=state;
            bool useOld=atEnd;
            if (parseTag.size()>0) {
                //qDebug()<<parseTag<<parseTagOld;
                if (parseTag=="b" || parseTag=="strong") {
                    setSubState(state, TagB, !atEnd);
                } else if (parseTag=="i" || parseTag=="em") {
                    setSubState(state, TagI, !atEnd);
                } else if (parseTag=="h1" || parseTag=="title") {
                    setSubState(state, TagH1, !atEnd);
                } else if (parseTag=="h2") {
                    setSubState(state, TagH2, !atEnd);
                } else if (parseTag=="h3") {
                    setSubState(state, TagH3, !atEnd);
                } else if (parseTag=="h4" || parseTag=="h5" || parseTag=="h6") {
                    setSubState(state, TagH46, !atEnd);
                } else if (parseTag=="small") {
                    setSubState(state, TagSMALL, !atEnd);
                } else if (parseTag=="td") {
                    setSubState(state, TagTD, !atEnd);
                } else if (parseTag=="th") {
                    setSubState(state, TagTH, !atEnd);
                } else if (parseTag=="tr") {
                    setSubState(state, TagTR, !atEnd);
                } else if (parseTag=="table") {
                    setSubState(state, TagTABLE, !atEnd);
                } else if (parseTag=="li") {
                    setSubState(state, TagLI, !atEnd);
                } else if (parseTag=="ol" || parseTag=="ul") {
                    setSubState(state, TagLIST, !atEnd);
                } else if (parseTag=="u") {
                    setSubState(state, TagU, !atEnd);
                } else if (parseTag=="div") {
                    setSubState(state, TagDIV, !atEnd);
                } else if (parseTag=="p") {
                    setSubState(state, TagP, !atEnd);
                } else if (parseTag=="center") {
                    setSubState(state, TagCENTER, !atEnd);
                } else if (parseTag=="a" && (atEnd || parseTagOld.contains("href"))) {
                    setSubState(state, TagA, !atEnd);
                    //useOld=!atEnd;
                }
                parseTag="";
                parseTagOld="";
            }
            if (useOld) setFormat(start, pos - start, m_formats[Tag], oldstate);
            else setFormat(start, pos - start, m_formats[Tag], state);
        } else if ((state & 0x0F)==InSpecial1) {
            while (pos < len) {
                if (text.mid(pos, special1End.size()) == special1End) {
                    pos += special1End.size();
                    if (lastStates.size()>0) state=lastStates.pop();//NormalState;
                    else setBaseState(state, NormalState);
                    break;
                } else {
                    ++pos;
                }
            }
            setFormat(start, pos - start, m_formats[Special1], state);
        } else if ((state & 0x0F)==InSpecial2) {
            while (pos < len) {
                if (text.mid(pos, special2End.size()) == special2End) {
                    pos += special2End.size();
                    if (lastStates.size()>0) state=lastStates.pop();//NormalState;
                    else setBaseState(state, NormalState);//NormalState;
                    break;
                } else {
                    ++pos;
                }
            }
            setFormat(start, pos - start, m_formats[Special2], state);
        } else {
            int txtstart=-1, txtend=-1;
            while (pos < len) {
                QChar ch = text.at(pos);
                if (ch == '<') {
                    if (text.mid(pos, 4) == "<!--") {
                        lastStates.push(state);
                        setBaseState(state, InComment);
                    } else {
                        lastStates.push(state);
                        setBaseState(state, InTag);
                    }
                    start=pos;
                    tagName="";
                    break;
                } else if (ch == '&') {
                    setFormat(start, pos - start, m_formats[Text], state);
                    start = pos;
                    while (pos < len
                           && text.at(pos++) != ';')
                        ;
                    setFormat(start, pos - start, m_formats[Entity], state);
                    start=pos+1;
                } else if (special1Start.size()>0 && text.mid(pos, special1Start.size()) == special1Start) {
                    start = pos;
                    pos=pos+special1Start.size();
                    lastStates.push(state);
                    setBaseState(state, InSpecial1);
                    break;
                } else if (special2Start.size()>0 && text.mid(pos, special2Start.size()) == special2Start) {
                    start = pos;
                    pos=pos+special2Start.size();
                    lastStates.push(state);
                    setBaseState(state, InSpecial2);
                    break;
                } else {
                    if (txtstart<0) {
                        txtstart=pos;
                        txtend=pos;
                    } else {
                        txtend++;
                    }
                    ++pos;

                }

            }
            if (state==NormalState) {
                setFormat(start, pos - start, m_formats[Text], state);
            }
            if (txtstart>=0 && txtend>=0) {
                setFormat(txtstart, txtend - txtstart+1, m_formats[Text], state);
            }
        }

    }
    setCurrentBlockState(state);

}

QTextCharFormat QFHTMLHighlighter::loadFormat(QSettings &settings, QString key, QString default_fcolor, bool default_bold, bool default_italic, bool default_underlined)
{
    QTextCharFormat format;
    bool bold=settings.value(key+".bold", QVariant(default_bold)).toBool();
    bool italic=settings.value(key+".italic", QVariant(default_italic)).toBool();
    bool underlined=settings.value(key+".underlined", QVariant(default_underlined)).toBool();
    QString col=settings.value(key+".color", default_fcolor).toString();

    format.setForeground(QColor(col));
    format.setFontItalic(italic);
    format.setFontWeight((bold)?(QFont::Bold):(QFont::Normal));
    format.setFontUnderline(underlined);
    format.setFontFixedPitch(settings.value("editor/fixedpitch", true).toBool());
    format.setFontFamily(settings.value("editor/fontname", "Courier New").toString());
    format.setFontPointSize(settings.value("editor/fontsize", 10).toDouble());

    return format;

}
