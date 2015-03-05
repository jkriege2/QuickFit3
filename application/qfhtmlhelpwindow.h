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

#ifndef QFHTMLHelpWindow_H
#define QFHTMLHelpWindow_H
#include <QToolTip>
#include <QDir>
#include <QShortcut>
#include <QListWidget>
#include <QTextBrowser>
#include <QLabel>
#include <QFont>
#include <QString>
#include <QSettings>
#include <QStringList>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStack>
#include <QUrl>
#include "qfplugin.h"
#include "qfpluginservices.h"
#include "qfenhancedlineedit.h"
#include "qfstyledbutton.h"
#include <QCheckBox>
#include <QToolButton>
#include <QMenuBar>
#include "qvisiblehandlesplitter.h"
#include "qfhtmlhelptools.h"
#ifdef QF3_USE_WEBKIT
#  include <QWebView>
#  include <QtWebkit>
#  include <QWebPage>
#  include <QWebFrame>
#  include <QWebSettings>
#endif
#include <QPrintPreviewDialog>

/*! \brief this is a window that displays information from a given HTML file
    \ingroup qf3lib_widgets

    There is a mechanism to reformat the HTML before display. This is doen in the loadHTML() method and uses
    two lists \f$( \mathtt{<key>}, \mathtt{<replaceBy>} )_i\f$ of replacements:
      - an internal list may be managed by clearInternalReplaces() and addInternalReplace()
      - an external (e.g. global) list is provided as pointer, using setHtmlReplacementList()
    .
    The mechanism replace every occurence of \c $$key by the according \c replaceBy string.
 */

class QFHTMLHelpWindow : public QWidget {
        Q_OBJECT

    public:



        /** \brief class constructor */
        QFHTMLHelpWindow(QWidget* parent=0, Qt::WindowFlags flags= Qt::Window|Qt::WindowTitleHint|Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint);

        /** \brief read widget size and position from a QSettings object where prefix is prepended to all keys */
        void readSettings(QSettings& settings, QString prefix=QString(""));

        /** \brief write widget size and position to a QSettings object where prefix is prepended to all keys */
        void writeSettings(QSettings& settings, QString prefix=QString(""));


        /** \brief return a reference to the font of the title line */
        QFont& titleFont() { return m_titleFont; }

        /** \brief return a reference to the base font of the main text */
        QFont& baseFont() { return m_baseFont; }

        /** \brief set a pointer to an external (global) replacement list */
        void setHtmlReplacementList(QList<QPair<QString, QString> >* list);
        /** \brief set a pointer to an external (global) replacement list */
        void setTooltips(const QMap<QString, QFToolTipsData> &list);

        /** \brief init with a list of plugin help directories */
        void setPluginDirList(QList<QFHelpDirectoryInfo>* pluginList);

        /** \brief init object from plugin servies object (then you don't need setHtmlReplacementList() and setPluginDirList() ), also stores the services object for further use */
        void initFromPluginServices(QFPluginServices* services);

        /** \brief clear the INTERNAL list of replacements */
        void clearInternalReplaces() { internal_replaces.clear(); }

        /** \brief add internal replacement */
        void addInternalReplace(QString key, QString replaceBy) {
            internal_replaces.append(qMakePair(key, replaceBy));
        }

        void setContentsMenuActions(const QList<QAction*>& items);

    public slots:
        /** \brief updates the information in the window */
        //void updateHelp(QString title, QString filename);
        /** \brief updates the information in the window. The title is taken from the HTML page */
        void updateHelp(QString filename);
        /** \brief clear contents of window */
        void clear();
        void showAndSearchInAll();

        void helpOnHelp();
    protected:
        QMap<QString, QFToolTipsData> tooltips;

        QList<QPair<QString, QString> >* replaces;

        QList<QFHelpDirectoryInfo>* pluginList;

        QList<QPair<QString, QString> > internal_replaces;

        QList<QPair<QString, QString> > fromHTML_replaces;

        QString loadHTML(QString filename, bool noPics=false);

        /** \brief a label for the model name */
        QLabel* labelTitle;

        QFPluginServices* m_pluginServices;

        QToolButton* btnPrevious;
        QToolButton* btnNext;
        QToolButton* btnHome;
        QToolButton* btnPrint;
        QToolButton* btnFind;
        QToolButton* btnFindInAll;
        QToolButton* btnFindInAllFind;
        QWidget* widFind;
        QWidget* widFindInAll;
        QFEnhancedLineEdit* edtFind;
        QFEnhancedLineEdit* edtFindInAll;
        QToolButton* btnFindNext;
        QToolButton* btnFindPrev;
        QCheckBox* chkCaseSensitive;
        QCheckBox* chkFindWholeWord;
        QCheckBox* chkCaseSensitiveInAll;
        QCheckBox* chkFindWholeWordInAll;
        QMenuBar* menuBar;
        QMenu* menuPage;
        QMenu* menuContents;
        QAction* actFind;
        QAction* actFindNext;
        QAction* actFindPrev;
        QAction* actHome;
        QAction* actNext;
        QAction* actPrevious;
        QAction* actPrint;
        QAction* actSave;
        QAction* actClose;
        QAction* actFindInAll;
        QAction* actHelpHelp;
        QVisibleHandleSplitter* splitterSearch;
        QListWidget* listFindInAll;
        QShortcut* scFindInAllFind;


        /** \brief a text edit for the model information/description */
#ifdef QF3_USE_WEBKIT
        QWebView* descriptionBrowser;
#else
        QTextBrowser* descriptionBrowser;
#endif

        /** \brief current searchPath for assets */
        QString searchPath;

        /** \brief font of the title line */
        QFont m_titleFont;
        /** \brief base font of the main text */
        QFont m_baseFont;

        QString m_home;

        struct HistoryEntry {
            QString url;
            QString name;

            HistoryEntry(QString url, QString name) {
                this->url=url;
                this->name=name;
            }

            HistoryEntry() {
                url="";
                name="";
            }
        };

        QStack<HistoryEntry> history;

        int history_idx;

        void searchHelpDir(const QDir& dir, QStringList& files);

        struct ContentsEntry {
            QList<int> num;
            QString header;
            QString id;
            QString prefix;
            ContentsEntry();
        };

    private slots:
        void displayTitle();
        void anchorClicked(const QUrl& link);
        void previous();
        void next();
        void home();
        void showFile(QString filename);
        void print();
        void save();
        void updateButtons();
        void find(bool checked=true);
        void findInAll(bool checked=true);
        void searchInAll();
        void findNext();
        void findPrev();
        void searchAllItemDoubleClicked(QListWidgetItem* item);
};

#endif // QFHTMLHelpWindow_H



