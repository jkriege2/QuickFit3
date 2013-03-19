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
    m_formats[QFHTMLHighlighter::Entity]=loadFormat(settings, prefix+"style/entity",  "darkgreen", false, false, false);
    m_formats[QFHTMLHighlighter::Tag]=loadFormat(settings, prefix+"style/tag",  "darkred", true, false, false);
    m_formats[QFHTMLHighlighter::Comment]=loadFormat(settings, prefix+"style/comment",  "darkgray", false, true, false);
    m_formats[QFHTMLHighlighter::Text]=loadFormat(settings, prefix+"style/text",  "black", false, false, false);



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
        default:
            while (pos < len) {
                QChar ch = text.at(pos);
                if (ch == '<') {
                    if (text.mid(pos, 4) == "<!--") {
                        state = InComment;
                    } else {
                        state = InTag;
                    }
                    break;
                } else if (ch == '&') {
                    setFormat(start, pos - start,
                              m_formats[Text]);
                    start = pos;
                    while (pos < len
                           && text.at(pos++) != ';')
                        ;
                    setFormat(start, pos - start,
                              m_formats[Entity]);
                } else {
                    ++pos;
                }
                if (state==NormalState) {
                    setFormat(start, pos - start, m_formats[Text]);

                }
            }
            break;

        case InComment:
            start = pos;
            while (pos < len) {
                if (text.mid(pos, 3) == "-->") {
                    pos += 3;
                    state = NormalState;
                    break;
                } else {
                    ++pos;
                }
            }
            setFormat(start, pos - start,
                      m_formats[Comment]);
            break;
        case InTag:
            QChar quote = QChar::Null;
            start = pos;
            while (pos < len) {
                QChar ch = text.at(pos);
                if (quote.isNull()) {
                    if (ch == '\'' || ch == '"') {
                        quote = ch;
                    } else if (ch == '>') {
                        ++pos;
                        state = NormalState;
                        break;
                    }
                } else if (ch == quote) {
                    quote = QChar::Null;
                }
                ++pos;
            }
            setFormat(start, pos - start, m_formats[Tag]);
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
