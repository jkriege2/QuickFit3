/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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
            LastConstruct = Comment
        };
        explicit QFHTMLHighlighter(const QString& settingsDir, QTextDocument *parent = 0);
        void setUseSpecial1(const QString& specialStart, const QString& specialEnd);
        void setUseSpecial2(const QString& specialStart, const QString& specialEnd);

    protected:
        QString special1Start;
        QString special1End;
        QString special2Start;
        QString special2End;
        /** \brief implements the actual highlighting scheme */
        void highlightBlock(const QString &text);

        enum State {
                    NormalState = -1,
                    InComment,
                    InTag,
                    InSpecial1,
                    InSpecial2,
                    Text
                };
        QStack<int> lastStates;
        QTextCharFormat m_formats[LastConstruct + 1];

        /** \brief load the format settings, saved in the supplied \a key in the \a settings object into \a format.
        *        The rest of the parameters are the default format specifiers
        */
        QTextCharFormat loadFormat(QSettings& settings, QString key, QString default_fcolor="black", bool default_bold=false, bool default_italic=false, bool default_underlined=false);

};

#endif // QFHTMLHIGHLIGHTER_H
