#ifndef QFHTMLHelpWindow_H
#define QFHTMLHelpWindow_H

#include <QTextBrowser>
#include "libwid_imexport.h"
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

/*! \brief this is a window that displays information from a given HTML file
    \ingroup qf3lib_widgets

    There is a mechanism to reformat the HTML before display. This is doen in the loadHTML() method and uses
    two lists \f$( \mathtt{<key>}, \mathtt{<replaceBy>} )_i\f$ of replacements:
      - an internal list may be managed by clearInternalReplaces() and addInternalReplace()
      - an external (e.g. global) list is provided as pointer, using setHtmlReplacementList()
    .
    The mechanism replace every occurence of \c $$key by the according \c replaceBy string.
 */

class QFWIDLIB_EXPORT QFHTMLHelpWindow : public QWidget {
        Q_OBJECT

    public:



        /** \brief class constructor */
        QFHTMLHelpWindow(QWidget* parent=0, Qt::WindowFlags flags= 0);

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

        /** \brief init with a list of plugin help directories */
        void setPluginDirList(QList<QFPluginServices::HelpDirectoryInfo>* pluginList);

        /** \brief init object from plugin servies object (then you don't need setHtmlReplacementList() and setPluginDirList() ), also stores the services object for further use */
        void initFromPluginServices(QFPluginServices* services);

        /** \brief clear the INTERNAL list of replacements */
        void clearInternalReplaces() { internal_replaces.clear(); }

        /** \brief add internal replacement */
        void addInternalReplace(QString key, QString replaceBy) {
            internal_replaces.append(qMakePair(key, replaceBy));
        }
    public slots:
        /** \brief updates the information in the window */
        void updateHelp(QString title, QString filename);
        /** \brief updates the information in the window. The title is taken from the HTML page */
        void updateHelp(QString filename);
        /** \brief clear contents of window */
        void clear();
    private:
        QList<QPair<QString, QString> >* replaces;

        QList<QFPluginServices::HelpDirectoryInfo>* pluginList;

        QList<QPair<QString, QString> > internal_replaces;

        QList<QPair<QString, QString> > fromHTML_replaces;

        QString loadHTML(QString filename);

        /** \brief a label for the model name */
        QLabel* labelTitle;

        QFPluginServices* m_pluginServices;

        QPushButton* btnPrevious;
        QPushButton* btnNext;
        QPushButton* btnHome;
        QPushButton* btnPrint;


        /** \brief a text edit for the model information/description */
        QTextBrowser* descriptionBrowser;

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

    private slots:
        void displayTitle();
        void anchorClicked(const QUrl& link);
        void previous();
        void next();
        void home();
        void showFile(QString filename);
        void print();
        void updateButtons();
};

#endif // QFHTMLHelpWindow_H



