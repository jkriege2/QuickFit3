/*
  Name: completertextedit.h
  Copyright: (c) 2008-2011
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/


/** \file completertextedit.h
  * \ingroup tools_qt
  *
  * A QTextEdit descendent with completer capabilities
  */

#ifndef COMPLETERTEXTEDIT_H
#define COMPLETERTEXTEDIT_H

#include <QTextEdit>
#include <QTextBlock>
#include <QTextCursor>
#include <iostream>
#include <QCompleter>
#include <QAbstractTextDocumentLayout>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QSettings>


/** \brief displays a line number bar, when used together with a QTextEdit.
  * \ingroup tools_qt
  *
  * This class displays the number bar together with a second bar that holds error/info/warning markers.
  * The markers are stored in a map which contains the line number as key and som marker data (type itemData)
  * as value. So we can easily check whether a marker is stored for a specific line. The markers are displayed as
  * colored rectangles. If the mouse hovers over them they show a tooltip with the provided message. To implement
  * these custom tooltips the class overwrites event(). In there we search through all markers whether the mouse is
  * inside their rect property which is set/reset during paintEvent(). So only visible markers have a really existing
  * rect and we can detect them.
  *
  * This class catches events from QTextEdit.document()->layout() to update its contents.
  *
  * \image html editor_editor.png
  *
  * When experimenting a bit with QTextEdit we find this:
  *  - document()->documentLayout()->documentSize() contains the size of the complete document in pixel
  *    which can be bigger than the size of the viewport
  *  - viewport()->width() / ->height() contains the width and height of the actual viewport, i.e. the widget
  *    we can draw on or the visible area, so we always display a portion with the size of the viewport out
  *    of the complete document's size
  *  - the verticalScrollBar() can take a value between 0 and
  *    document()->documentLayout()->documentSize().height()-viewport()->height()
  *    so it may be interpreted as the left top border of the viewport inside the document
  *  - QAbstractTextDocumentLayout::blockBoundingRect(block) returns the bounding rectangle of the specified
  *    text block, so we can check whether this block is inside the current viewport
  * .
  */
class CompleterTextEditNumberBar : public QWidget {
        Q_OBJECT
    public:
        /** \brief The possible marker types */
        enum itemType {
            mtWarning,
            mtInfo,
            mtError
        };
        /** \brief a nested struct to store markers that can be shown in the number bar */
        struct itemData {
            QString message;
            int line;
            itemType type;
            QRect rect;
        };
    private:
        /** \brief the editor used in conjunction with this widget */
        QTextEdit* editor;
        /** \brief the width of the additional marker column [pixels] */
        int markerWidth;
        /** \brief font of the line numbers */
        QFont linenumberFont;
        /** \brief color of the line number column */
        QColor linenumberColumnColor;
        /** \brief color of the marker column */
        QColor markerColumnColor;
        /** \brief color of the line numbers */
        QColor linenumberColor;
        /** \brief color of error markers */
        QColor errorMarkerColor;
        /** \brief color of warning markers */
        QColor warningMarkerColor;
        /** \brief color of info markers */
        QColor infoMarkerColor;
        /** \brief a map to store all markers */
        QMap<int, itemData> markers;
    protected:
        /** \brief here we paint the number bar */
        void paintEvent(QPaintEvent *event);

        /** \brief here we cath the ToolTip event to show a message when we hover over a marker */
        bool event(QEvent *event);
    public:
        /** \brief class constructor */
        CompleterTextEditNumberBar(QTextEdit* edtr, QWidget* parent=0);

        /** \brief class destructor */
        ~CompleterTextEditNumberBar();

        /** \brief connect the widget to a new QTextEdit */
        void setEditor(QTextEdit* edtr);

        /** \brief read settings */
        void readSettings(QSettings& settings);

        /** \brief clear all stored markers */
        inline void clearMarkers() { markers.clear(); update(); }

        /** \brief add an additional marker */
        void addMarker(int line, itemType type, QString message);

        /** \brief add an additional error marker */
        inline void addErrorMarker(int line, QString message) { addMarker(line, mtError, message); };

        /** \brief add an additional warning marker */
        inline void addWarningMarker(int line, QString message) { addMarker(line, mtWarning, message); };

        /** \brief add an additional info marker */
        inline void addInfoMarker(int line, QString message) { addMarker(line, mtInfo, message); };
};



/** \brief a text editor class with sytax highlighting and advanced editing functionality
  * \ingroup tools_qt
  *
  * This text editor class enhances the capabilities of QTextEdit. It adds advanced find and
  * find/replace function. Also it implements advanced editing capabilities, like smart indention
  * and commenting/uncommenting lines.
  *
  * \image html editor_editor.png
  */
class CompleterTextEditWidget : public QTextEdit
{
    Q_OBJECT
    private:


        /** \brief internal: this property contains the word currently under the cursor and is
          *        used in cursorChanged() to emit a wordUnderCursorChanged() signal.
          */
        QString currentTextUnderCursor;
        /** \brief internal: \c true when findFirst() was called with a match */
        bool currentlySearching;
        /** \brief internal: \c true when replaceFirst() was called with a match */
        bool currentlyReplacing;
        /** \brief internal: current search/replace phrase */
        QString searchPhrase;
        /** \brief internal: current replace phrase */
        QString replacePhrase;
        /** \brief internal: flag for search/replace action */
        bool searchFromStart;
        /** \brief internal: flag for search/replace action */
        bool matchCase;
        /** \brief internal: flag for search/replace action */
        bool wholeWords;
        /** \brief internal: flag for search/replace action */
        bool replaceAll;
        /** \brief internal: flag for search/replace action */
        bool askBeforeReplace;

        /** \brief internal: this string is used as comment string (standard is the C single-line comment <tt>//</tt>) */
        QString commentString;


        /** \brief removes leading single line comments from string
         *
         * This method is used by uncomment() it takes a string and first searches the first non-space
         * charakter in it. Then it checks whether the first non-space characters are \a commentString and eventually
         * deletes them. Actually it deletes a leading \a commentString (including one space) or if there is no
         * space after \a commentString just \a commentString .
         */
        QString removeComments(QString t);

        /** \brief returns the current word near the cursor */
        QString textUnderCursor() const;

        /** \brief returns the current valid word near the cursor. The valid words are defined by clearValidWords()
         *         and addValidWords().
         */
        QString validTextUnderCursor() const;

        /** \brief the list of valid words */
        QStringList validwords;

        /** \brief internal: width of a tab in characters */
        unsigned int tabwidth;

        /** \brief returns a string that consists of \a tabwidth whitespaces, i.e. that represents one TAB */
        inline QString tabToSpaces() { return (tabwidth==0)?QString('\t'):QString(tabwidth, ' '); };

        /** \brief this method processes a TAB key beeing pressed
         *
         * If Text is selected this method calls indentInc() to increment the indention level
         * of the selected text. If no text is selected, this simply inserts a TAB (i.e. \c tabwidth
         * space characters).
         */
        void processTab(QKeyEvent* e);

        /** \brief points to the QCompleter class, used by this QTextEdit */
        QCompleter *c;

    public:
        /** \brief class constructor */
        CompleterTextEditWidget(QWidget *parent = 0);
        /** \brief class destructor */
        ~CompleterTextEditWidget();

        /** \brief returns true if any text is currently selected */
        inline bool hasSelection() { return textCursor().hasSelection(); };

        /** \brief returns the currently selected text */
        inline QString getSelection() { return textCursor().selectedText(); };

        /** \brief set the used QCompleter object */
        void setCompleter(QCompleter *c);

        /** \brief return the currently used QCompleter object */
        QCompleter* completer() const;

        /** \brief set the tab width in characters */
        void setTabwidth(unsigned int width) {tabwidth=width; };

        /** \brief get the tab width in characters */
        unsigned int getTabwidth() {return tabwidth; };

        /** \brief get the current line number */
        inline long getLineNumber() { return textCursor().blockNumber() + 1; };

        /** \brief get the current line number */
        inline long getColumnNumber() { return textCursor().columnNumber() + 1; };

        /** \brief mark the first occurence of the specified \a phrase
         *
         * \param phrase the phrase to search for
         * \param searchFromStart start the search at the start of the document
         *            (or if \c false at the current cursor position)
         * \param matchCase execute the search case sensitive
         * \param wholeWords only match whole words (if \c true )
         *
         * \return \c true if the \a phrase was found, \c false else
         */
        bool findFirst(QString phrase, bool searchFromStart, bool matchCase, bool wholeWords);

        /** \brief find the next occurence after a previous call of findFirst()
         *
         * \return \c true if the \a phrase was found, \c false else
         */
        bool findNext();

        /** \brief replace the first occurence of the specified \a phrase by \a replaceBy
         *
         * \param phrase the phrase to search for
         * \param replaceBy the phrase to replace by
         * \param searchFromStart start the search at the start of the document
         *            (or if \c false at the current cursor position)
         * \param matchCase execute the search case sensitive
         * \param replaceAll replace all occurences of \a phrase
         * \param askBeforeReplace let the user affirm every replacement
         * \param wholeWords only match whole words (if \c true )
         *
         * \return \c true if the \a phrase was found, \c false else
         */
        bool replaceFirst(QString phrase, QString replaceBy, bool searchFromStart, bool matchCase, bool wholeWords, bool replaceAll, bool askBeforeReplace);

        /** \brief find the next occurence after a previous call of findFirst()
         *
         * \return \c true if the \a phrase was found, \c false else
         */
        bool replaceNext();

        /** \brief returns \a phrase of the current search/replace operation */
        inline QString getPhrase() { return searchPhrase; };

        /** \brief sets the comment starting string for the comment() and uncomment() function (e.g. <tt>"//"</tt>
          *        for standard C-like single-line comments
          */
        inline void setCommentString(QString c) { commentString=c; };

        /** \brief clear the list of valid words, used by validTextUnderCursor() */
        inline void clearValidWords() { validwords.clear(); };
        /** \brief add a word \a w to the list of valid words, used by validTextUnderCursor() */
        inline void addValidWord(QString w) { validwords<<w; };
    public slots:
        /** \brief move the cursor to the specified line */
        void gotoLine(int line);

        /** \brief Increases the indention of the current or all the currently selected lines by
         *         \c tabwidth.
         */
        void indentInc();

        /** \brief Reduces the indention of every selected (or the current) line by tabwidth characters.
         *         If there are less whitespaces than tabwidth in the line only the fewer whitespaces are
         *         deleted.
         */
        void indentDec();

        /** \brief comment the current line (by adding single line comments at the line start) */
        void comment();

        /** \brief uncomment the current line (by removing single line comment at the line start) */
        void uncomment();
    protected:
        /** \brief this method implements the reaction on key presses and also displays the
         *         completer. In addition it handles special key press tasks (TAB to spaces ...)
         *
         * - If the completer is displayed, some of the special keys (TAB, RETURN ...) are beeing ignored
         * - in normal mode:
         *    - for TAB indentInc() is called
         *    - for Shift+TAB indentDec() is called
         *    - when RETURN is pressed, the indention is handled propperly:
         *       - the cursor in the new line is set to the same indention level, as the current
         *         line
         *       - if RETURN is pressed after '{' the indention is increased by \c tabwidth characters
         *       - if RETURN is pressed after '}' with only leading space characters, the indention is
         *         reduced to the indention of the matching '{'. Here the indention in the new line AND
         *         the indention of the line, conatining the '}', is set equally!
         *       .
         *    .
         * .
         */
        void keyPressEvent(QKeyEvent *e);

        /** \brief called when the focus of the TextEdit changes */
        void focusInEvent(QFocusEvent *e);

    private slots:
        /** \brief called to insert the selected completion
         *
         * In the selction database file you can use the special character \c ° to specify a
         * position in the inserted text. if there is one \c ° the cursor will be set to this
         * position after insert. If there are two \c ° the text between them will be selected
         * after insert.
         */
        void insertCompletion(const QString &completion);

        /** \brief called when the cursor position changed an subsequently emits cursorMoved() */
        void cursorChanged();

    signals:
        /** \brief emitted every time the cursor position changes. This can be used to implement a
         *         widget that displays the current line and column of the cursor
         */
        void cursorMoved(int line, int column);

        /** \brief emitted whenever when the availability of find/replace next actions is changed */
        void findNextAvailable(bool avail);

        /** \brief emitted whenever the cursor is moved above a new word */
        void wordUnderCursorChanged(QString newWord);


};






/** \brief this class groups a CompleterTextEditWidget and a CompleterTextEditNumberBar to a complete
  *        editor widget
  * \ingroup tools_qt
  *
  * \image html editor_editor.png
  * This class forwards part of the interfaces of its sub widgets. Use getEditor() to access the editor component
  * itself, which provides an enhanced QTextEdit interface (see CompleterTextEditWidget).
  */
class CompleterTextEdit : public QWidget
{
    Q_OBJECT
    private:
        /** \brief the actual editor widget */
        CompleterTextEditWidget* editor;
        /** \brief the number bar widget */
        CompleterTextEditNumberBar* numbers;
        /** \brief a layout to display numberbar and text editor */
        QHBoxLayout* layout;
    protected:

    public:
        /** \brief class constructor */
        CompleterTextEdit(QWidget* parent=0);

        /** \brief returns a pointer to the editor component */
        CompleterTextEditWidget* getEditor() { return editor; };

        /** \brief read some settings from the provided QSettings object (INI file) */
        void readSettings(QSettings& settings);

        /** \brief clear all stored markers */
        inline void clearMarkers() { numbers->clearMarkers(); }

        /** \brief add an additional error marker */
        inline void addErrorMarker(int line, QString message) { numbers->addErrorMarker(line, message); };

        /** \brief add an additional warning marker */
        inline void addWarningMarker(int line, QString message) { numbers->addWarningMarker(line, message); };

        /** \brief add an additional info marker */
        inline void addInfoMarker(int line, QString message) { numbers->addInfoMarker(line, message); };
};


#endif // COMPLETERTEXTEDIT_H



