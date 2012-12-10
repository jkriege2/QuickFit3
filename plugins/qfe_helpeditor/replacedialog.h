/*
  Name: replacedialog.h
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/

*/


/** \file replacedialog.h
  * \ingroup sdff_editor
  *
  * This file contains the "Find & Replace Text ..." Dialog for the sdff editor.
  */

#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QString>

#include "ui_replacedialog.h"


/** \brief the sdff-editor's "Find & Replace ..." dialog
 *
 *  \ingroup sdff_editor
 *
 *  \image html editor_findreplace.png
 */
class ReplaceDialog : public QDialog, private Ui::ReplaceDialog
{
    Q_OBJECT
private:
    /** \brief the phrase to match */
    QString phrase;
    /** \brief the phrase to match replace with */
    QString replace;
    /** \brief math case sensitive ? */
    bool matchCase;
    /** \brief begin search at the start of the document ? */
    bool searchFromStart;
    /** \brief match only whole words ? */
    bool wholeWords;
    /** \brief replace all occurences ? */
    bool replaceAll;
    /** \brief ask before replace? */
    bool askBeforeReplace;

public:
    /** \brief class constructor */
    ReplaceDialog(QWidget *parent = 0);

    /** \brief set the phrase to search for */
    void setPhrase(QString phr);
    /** \brief return the phrase to match */
    QString getPhrase() { return phrase; };
    /** \brief return the phrase to replace with */
    QString getReplace() { return replace; };
    /** \brief return whether to match case sensitive */
    bool getMatchCase() {return matchCase; };
    /** \brief return whether to search from start */
    bool getSearchFromStart() {return searchFromStart; };
    /** \brief return whether to match whole words */
    bool getWholeWords() {return wholeWords; };
    /** \brief return whether to replace all occurences */
    bool getReplaceAll() {return replaceAll; };
    /** \brief return whether to ask before replaceing anything */
    bool getAskBeforeReplace() {return askBeforeReplace; };

private slots:
    /** \brief automatically connected slot: called when the state of the matchCase checkbox changes */
    void on_chk_matchCase_stateChanged( int state );
    /** \brief automatically connected slot: called when the state of the searchFromStart checkbox changes */
    void on_chk_searchFromStart_stateChanged(int state);
    /** \brief automatically connected slot: called when the state of the wholeWords checkbox changes */
    void on_chk_wholeWords_stateChanged(int state);
    /** \brief automatically connected slot: called when the state of the replaceAll checkbox changes */
    void on_chk_replaceAll_stateChanged(int state);
    /** \brief automatically connected slot: called when the state of the ask checkbox changes */
    void on_chk_ask_stateChanged(int state);
    /** \brief automatically connected slot: called when the search phrase changes */
    void on_edt_what_textChanged ( const QString & text );
    /** \brief automatically connected slot: called when the replace phrase changes */
    void on_edt_replace_textChanged ( const QString & text );
};

#endif // REPLACEDIALOG_H

