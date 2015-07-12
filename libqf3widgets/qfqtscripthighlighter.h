/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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


#ifndef SDFFHIGHLIGHTER_H
#define SDFFHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QColor>
#include <QSettings>

#include <QHash>
#include <QTextCharFormat>
#include "libwid_imexport.h"

class QTextDocument;

/** \brief a highlighter class that can highlight QtScript files
  * \ingroup qf3lib_widgets
  *
  * This is derived from QSyntaxHighlighter and implements a custom syntax highlighter for the
  * SDFF language. To do so this class overwrites the highlightBlock() method, as it is described
  * in the according QT example and documentation of QSytaxHighlighter class.
  */
class QFWIDLIB_EXPORT QFQtScriptHighlighter : public QSyntaxHighlighter {
        Q_OBJECT

    public:
        /** \brief class constructor
        *
        * Here the class is constructed. In addition the rules (regular expressions) for the SDFF keywords and
        * structures are beeing created. The rules (and the text formats that apply to them) are stored in the
        * private properties of this class.
        */
        QFQtScriptHighlighter(const QString& settingsDir, QTextDocument *parent = 0);

        void setSpecialFunctions(const QStringList& specialfunctions);

    protected:
        /** \brief implements the actual highlighting scheme */
        void highlightBlock(const QString &text);

    private:
        /** \brief is used to define a syntax highlighting rule that consists of a pattern (regular expression)
        *         to be recognized and a text format to be applied. The formats are set to their default values
        *         or they are read from a configuration file.
        */
        struct HighlightingRule
        {
         QRegExp pattern;
         QTextCharFormat format;
        };

        /** \brief load the format settings, saved in the supplied \a key in the \a settings object into \a format.
        *        The rest of the parameters are the default format specifiers
        */
        void loadFormat(QSettings& settings, QString key, QTextCharFormat& format, QString default_fcolor="black", bool default_bold=false, bool default_italic=false, bool default_underlined=false);

        /** \brief this vector contains all available highlighting rules */
        QVector<HighlightingRule> highlightingRules;
        QVector<HighlightingRule> sfhighlightingRules;

        /** \brief this regular expression defines the start of a comment in SDFF */
        QRegExp commentStartExpression;

        /** \brief this regular expression defines the end of a (block) comment in SDFF */
        QRegExp commentEndExpression;

        /** \brief format for keywords */
        QTextCharFormat keywordFormat;

        /** \brief format for operators */
        QTextCharFormat operatorFormat;

        /** \brief format for single-line comments */
        QTextCharFormat singleLineCommentFormat;

        /** \brief format for multi-line comments */
        QTextCharFormat multiLineCommentFormat;

        /** \brief format for (quoted) string constants */
        QTextCharFormat quotationFormat;

        /** \brief format for function names */
        QTextCharFormat functionFormat;


        /** \brief format for special function names */
        QTextCharFormat specialfunctionFormat;


        /** \brief format for variable names */
        QTextCharFormat variableFormat;
};

#endif
