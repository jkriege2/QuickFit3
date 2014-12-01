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
    m_formats[QFHTMLHighlighter::Special2]=loadFormat(settings, prefix+"style/special2",  "darkblue", true, false, false);



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

void QFHTMLHighlighter::highlightBlock(const QString &text)
{
    int state = previousBlockState();
    int len = text.length();
    int start = 0;
    int pos = 0;
    while (pos < len) {
        switch (state) {
        case NormalState:
            default:{
                    int txtstart=-1, txtend=-1;
                    while (pos < len) {
                        QChar ch = text.at(pos);
                        if (ch == '<') {
                            if (text.mid(pos, 4) == "<!--") {
                                lastStates.push(state);
                                state = InComment;
                            } else {
                                lastStates.push(state);
                                state = InTag;
                            }
                            start=pos;
                            break;
                        } else if (ch == '&') {
                            setFormat(start, pos - start, m_formats[Text]);
                            start = pos;
                            while (pos < len
                                   && text.at(pos++) != ';')
                                ;
                            setFormat(start, pos - start, m_formats[Entity]);
                            start=pos+1;
                        } else if (special1Start.size()>0 && text.mid(pos, special1Start.size()) == special1Start) {
                            start = pos;
                            pos=pos+special1Start.size();
                            lastStates.push(state);
                            state = InSpecial1;
                            break;
                        } else if (special2Start.size()>0 && text.mid(pos, special2Start.size()) == special2Start) {
                            start = pos;
                            pos=pos+special2Start.size();
                            lastStates.push(state);
                            state = InSpecial2;
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
                        setFormat(start, pos - start, m_formats[Text]);
                    }
                    if (txtstart>=0 && txtend>=0) {
                        setFormat(txtstart, txtend - txtstart, m_formats[Text]);
                    }
                }
            break;

        case InComment:
            start = pos;
            while (pos < len) {
                if (text.mid(pos, 3) == "-->") {
                    pos += 3;
                    state = lastStates.pop();//NormalState;
                    break;
                } else {
                    ++pos;
                }
            }
            setFormat(start, pos - start, m_formats[Comment]);
            break;
        case InTag: {
                    QChar quote = QChar::Null;
                    start = pos;
                    while (pos < len) {
                        QChar ch = text.at(pos);
                        if (quote.isNull()) {
                            if (ch == '\'' || ch == '"') {
                                quote = ch;
                            } else if (ch == '>') {
                                ++pos;
                                state = lastStates.pop();//NormalState;
                                break;
                            }
                        } else if (ch == quote) {
                            quote = QChar::Null;
                        }
                        ++pos;
                    }
                    setFormat(start, pos - start, m_formats[Tag]);
                }
            break;
        case InSpecial1:
            //start = pos;
            while (pos < len) {
                if (text.mid(pos, special1End.size()) == special1End) {
                    pos += special1End.size();
                    state = lastStates.pop();//NormalState;
                    break;
                } else {
                    ++pos;
                }
            }
            setFormat(start, pos - start, m_formats[Special1]);
            break;
        case InSpecial2:
            //start = pos;
            while (pos < len) {
                if (text.mid(pos, special2End.size()) == special2End) {
                    pos += special2End.size();
                    state = lastStates.pop();//NormalState;
                    break;
                } else {
                    ++pos;
                }
            }
            setFormat(start, pos - start, m_formats[Special2]);
            break;
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
