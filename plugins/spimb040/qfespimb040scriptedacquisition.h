/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef QFESPIMB040SCRIPTEDACQUISITION_H
#define QFESPIMB040SCRIPTEDACQUISITION_H


#include <QWidget>
#include <QSettings>
#include <QAction>
#include <QToolButton>
#include <QtScript>
#include "qfextensionlinearstage.h"
#include "qfespimb040opticssetup.h"
#include "qfespimb040filenametool.h"
#include "qtriple.h"
#include "qfespimb040acquisitiontools.h"
#include "qrecentfilesmenu.h"
#include "qfqtscripthighlighter.h"
#include "finddialog.h"
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include "replacedialog.h"

class QFESPIMB040ScriptedAcquisitionDocSearchThread: public QThread {
       Q_OBJECT
   public:
       QFESPIMB040ScriptedAcquisitionDocSearchThread(QStringList files, QObject* parent=NULL);

       void stopThread();
   protected:
       void run();
   signals:
       void foundFunction(QString name, QString templ, QString help);
   protected:
       QStringList files;
       bool stopped;
};


class QFESPIMB040MainWindow2; // forward
class QFPluginServices; // forward
class QFESPIMB040ScriptedAcquisitionTools; // forward
class QFESPIMB040ScriptedAcquisitionInstrumentControl; // forward
class QFESPIMB040ScriptedAcquisitionAcquisitionControl; // forward

namespace Ui {
    class QFESPIMB040ScriptedAcquisition;
}


/*! \brief widget to configure/code scripted acquisitions
    \ingroup qf3ext_spimb040

    This dialog parses (async) all HTML files (*.html, *.htm) in the plugin help directory, subdirectory "parserreference".
    In these files it expects a section like this:
    \code
<!-- func:sin -->
    <b><tt><!-- template -->sin(x)<!-- /template --></tt></b> - <i>sine function </i>:<br>
    calculates the sine of the argument (argument in radians)
<!-- /func:sin -->
    \endcode

    Here \c sin (in the \c <!--func:NAME--> tag) is the name of the function. It extracts the contents between this
    tag and the according end-tag as help for the function (displayed below the expression line edit) and the string
    (NO MARKUP!!!) bewteen the \c <!--template--> comment tags as function template for the autocomplete/function list
    on the rhs of the dialog.
*/
class QFESPIMB040ScriptedAcquisition : public QWidget, public QFESPIMB040FilenameTool {
        Q_OBJECT

    public:
        explicit QFESPIMB040ScriptedAcquisition(QFESPIMB040MainWindow2* mainWindow, QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetupBase* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription, QString configDirectory);
        ~QFESPIMB040ScriptedAcquisition();

        QString getScript() const;

        void loadSettings(QSettings& settings, QString prefix);
        void storeSettings(QSettings& settings, QString prefix) const;

    signals:
        void doAcquisition();

    public slots:
        void performAcquisition();
        void setStatus(const QString& text);
        void setStatusProgressRange(double minimum, double maximum);
        void setStatusProgress(double value);
        void incStatusProgress();
        void incStatusProgress(double value);
        void updateReplaces();

    protected slots:
        void on_btnExecute_clicked();
        void on_btnCancel_clicked();
        void on_btnNew_clicked();
        void on_btnOpen_clicked();
        void on_btnSave_clicked();
        void on_btnSyntaxCheck_clicked();
        void on_btnOpenExample_clicked();
        void on_btnOpenTemplate_clicked();
        void edtScript_cursorPositionChanged();
        void on_btnHelp_clicked();

        void openScript(QString dir, bool saveDir=true);
        void openScriptNoAsk(QString filename);

        void threadFinished();
        void delayedStartSearchThreads();
        void addFunction(QString name, QString templ, QString help);






        /** \brief slot: find the first occurence of the text */
        void findFirst();

        /** \brief slot: find the next occurence of the text */
        void findNext();

        /** \brief slot: replace the first occurence of the text */
        void replaceFirst();

        /** \brief slot: replace the next occurence of the text */
        void replaceNext();

        /** \brief slot: goto a user selectable line in the text */
        void gotoLine();

        /** \brief slot: print the SDFF document */
        void print();

        /** \brief slot: preview print of the SDFF document */
        void printPreviewClick();

        /** \brief slot: used by printPreview() */
        void printPreview(QPrinter *printer);
        /** \brief set the "find next", ... actions to disabled (this is called whenever a new file is loaded or created) */
        void clearFindActions();

    protected:
        bool maybeSave();
        void setScriptFilename(QString filename);

        QString currentScript;
    private:
        Ui::QFESPIMB040ScriptedAcquisition *ui;
        QFPluginServices* m_pluginServices;
        QFESPIMB040OpticsSetupBase* opticsSetup;
        QFESPIMB040AcquisitionDescription* acqDescription;
        QFESPIMB040ExperimentDescription* expDescription;
        QFPluginLogService* log;
        QFESPIMB040AcquisitionTools* acqTools;
        QFESPIMB040MainWindow2* mainWindow;
        QScriptEngine* engine;
        QRecentFilesMenu* recentMaskFiles;

        QMap<QString, QPair<QString, QString> > functionhelp;
        QStringList specialFunctions;

        QString getFunctionTemplate(QString name);
        QString getFunctionHelp(QString name);

        int threadsFinished;
        int maxThreads;
        QList<QFESPIMB040ScriptedAcquisitionDocSearchThread*> threads;
        QStringListModel helpModel;
        QStringList defaultWords;
        QString lastScript;
        QFESPIMB040ScriptedAcquisitionTools* acquisitionTools;
        QFESPIMB040ScriptedAcquisitionInstrumentControl* instrumentControl;
        QFESPIMB040ScriptedAcquisitionAcquisitionControl* acquisitionControl;

        QFQtScriptHighlighter* highlighter;
        QAction *cutAct;
        /** \brief action object: copy the currently selected text */
        QAction *copyAct;
        /** \brief action object: paste some text */
        QAction *pasteAct;
        /** \brief action object: undo text changes */
        QAction *undoAct;
        /** \brief action object: redo text changes */
        QAction *redoAct;
        /** \brief action object: find text */
        QAction *findAct;
        /** \brief action object: find next occurence text */
        QAction *findNextAct;
        /** \brief action object: find and replace text */
        QAction *replaceAct;
        /** \brief action object: run current SDFF script */
        QAction *runSDFFAct;
        /** \brief action object: comment text */
        QAction *commentAct;
        /** \brief action object: uncomment text */
        QAction *unCommentAct;
        /** \brief action object: incease indention */
        QAction *indentAct;
        /** \brief action object: decrease indention */
        QAction *unindentAct;
        /** \brief action object: goto line */
        QAction *gotoLineAct;
        /** \brief action object: print */
        QAction *printAct;

        QCompleter* completer;


        /** \brief the "Find ..." dialog object */
        FindDialog* findDlg;
        /** \brief the "Find & Replace..." dialog object */
        ReplaceDialog* replaceDlg;
        QStringListModel* completermodel;

        /** \brief load a completer model from a file */
        QStringListModel* modelFromFile(const QString& fileName);

};









#endif // QFESPIMB040SCRIPTEDACQUISITION_H
