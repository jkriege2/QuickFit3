#ifndef QFHTMLHIGHLIGHTER_H
#define QFHTMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QColor>
#include <QSettings>

#include <QHash>
#include <QTextCharFormat>
#include "libwid_imexport.h"

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
            LastConstruct = Comment
        };
        explicit QFHTMLHighlighter(const QString& settingsDir, QTextDocument *parent = 0);

    protected:
        /** \brief implements the actual highlighting scheme */
        void highlightBlock(const QString &text);

        enum State {
                    NormalState = -1,
                    InComment,
                    InTag
                };
        QTextCharFormat m_formats[LastConstruct + 1];

        /** \brief load the format settings, saved in the supplied \a key in the \a settings object into \a format.
        *        The rest of the parameters are the default format specifiers
        */
        QTextCharFormat loadFormat(QSettings& settings, QString key, QString default_fcolor="black", bool default_bold=false, bool default_italic=false, bool default_underlined=false);

};

#endif // QFHTMLHIGHLIGHTER_H
