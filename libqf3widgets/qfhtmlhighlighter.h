/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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


#ifndef QFHTMLHIGHLIGHTER_H
#define QFHTMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QColor>
#include <QSettings>

#include <QHash>
#include <QTextCharFormat>
#include "libwid_imexport.h"
#include <QStack>

class QTextDocument;

/** \brief a highlighter class that can highlight HTML files
  * \ingroup qf3lib_widgets
  *
  *
  * This class is based on http://doc.qt.digia.com/qq/qq21-syntaxhighlighter.html#example
  */
class QFWIDLIB_EXPORT QFHTMLHighlighter : public QSyntaxHighlighter
{
        Q_OBJECT
    public:
        enum Construct {
            Entity=0,
            Tag=1,
            Comment=2,
            Special1=3,
            Special2=4,
            Text=5
        };
        explicit QFHTMLHighlighter(const QString& settingsDir, QTextDocument *parent = 0);
        void setUseSpecial1(const QString& specialStart, const QString& specialEnd);
        void setUseSpecial2(const QString& specialStart, const QString& specialEnd);

    protected:
        void setFormat(int start, int count, const QTextCharFormat &format, int state);
        void setFormat(int start, int count, const QTextCharFormat &format);

        static void setBaseState(int& state, int bState);
        static void setSubState(int& state, int sState, bool enabled);

        QString special1Start;
        QString special1End;
        QString special2Start;
        QString special2End;
        /** \brief implements the actual highlighting scheme */
        void highlightBlock(const QString &text);

        enum State {
                    NormalState = 0x000,
                    InComment=0x001,
                    InTag=0x002,
                    InSpecial1=0x003,
                    InSpecial2=0x004
                };

        enum SubState {
            NoSubState=0x00000,
            TagB    =0x0000100,
            TagI    =0x0000200,
            TagU    =0x0000400,
            TagA    =0x0000800,
            TagH1   =0x0001000,
            TagH2   =0x0002000,
            TagH3   =0x0004000,
            TagH46  =0x0008000,
            TagSMALL=0x0010000,
            TagLI   =0x0020000,
            TagTD   =0x0040000,
            TagTH   =0x0080000,
            TagLIST =0x0100000,
            TagTR   =0x0200000,
            TagTABLE=0x0400000,
            TagDIV  =0x0800000,
            TagCENTER=0x1000000,
            TagP    =0x2000000,
        };


        QStack<int> lastStates;
        QMap<int, QTextCharFormat> m_formats;


        struct additionalFormatStruct {
            bool setBold;
            bool bold;
            bool setItalic;
            bool italic;
            bool setUnderlined;
            bool underlined;
            bool setFontSize;
            double fontSize;
            bool incFontSize;
            double fontSizeInc;
            bool setFontFamily;
            QString fontFamily;
            inline additionalFormatStruct() {
                clear();
            }

            inline void clear() {
                setBold=false;
                bold=false;
                setItalic=false;
                italic=false;
                setUnderlined=false;
                underlined=false;
                setFontSize=false;
                fontSize=-1;
                setFontFamily=false;
                fontFamily="";
                incFontSize=false;
                fontSizeInc=0;
            }
        };
        additionalFormatStruct additionalFormat;
        /** \brief load the format settings, saved in the supplied \a key in the \a settings object into \a format.
        *        The rest of the parameters are the default format specifiers
        */
        QTextCharFormat loadFormat(QSettings& settings, QString key, QString default_fcolor="black", bool default_bold=false, bool default_italic=false, bool default_underlined=false);

};

#endif // QFHTMLHIGHLIGHTER_H
