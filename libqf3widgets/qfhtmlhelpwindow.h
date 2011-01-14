#ifndef QFHTMLHelpWindow_H
#define QFHTMLHelpWindow_H

#include <QTextBrowser>
#include <QLabel>
#include <QFont>
#include <QString>
#include <QSettings>
#include <QStringList>

/*! \brief this is a window that displays information from a given HTML file
    \ingroup qf3lib_widgets
 */

class QFHTMLHelpWindow : public QWidget {
        Q_OBJECT

    public:
        /** \brief class constructor */
        QFHTMLHelpWindow(QWidget* parent=0);

        /** \brief read widget size and position from a QSettings object where prefix is prepended to all keys */
        void readSettings(QSettings& settings, QString prefix=QString(""));

        /** \brief write widget size and position to a QSettings object where prefix is prepended to all keys */
        void writeSettings(QSettings& settings, QString prefix=QString(""));

        /** \brief set the current search directory */
        void setSearchPath(QStringList path) { searchPath=path; };

        /** \brief return the current search path */
        QStringList& getSearchPath() { return searchPath; }

        /** \brief return a reference to the font of the title line */
        QFont& titleFont() { return m_titleFont; }

        /** \brief return a reference to the base font of the main text */
        QFont& baseFont() { return m_baseFont; }

    public slots:
        /** \brief updates the information in the window */
        void updateHelp(QString title, QString filename);
    private:
        /** \brief a label for the model name */
        QLabel* labelTitle;

        /** \brief a text edit for the model information/description */
        QTextBrowser* descriptionBrowser;

        /** \brief current searchPath for assets */
        QStringList searchPath;

        /** \brief font of the title line */
        QFont m_titleFont;
        /** \brief base font of the main text */
        QFont m_baseFont;
};

#endif // QFHTMLHelpWindow_H



