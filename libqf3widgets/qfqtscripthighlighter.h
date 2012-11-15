/*
  Name: sdffhighlighter.h
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/

*/


/** \file sdffhighlighter.h
  * \ingroup sdff_editor
  *
  * This file implements a Qt Highlighter for the SDFF language.
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
