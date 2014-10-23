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

#ifndef QFFUNCTIONREFERENCETOOL_H
#define QFFUNCTIONREFERENCETOOL_H

#include <QObject>
#include "qfcompleterfromfile.h"
#include <QCompleter>
#include <QThread>
#include "qftablemodel.h"
#include <QStringListModel>
#include <QLabel>
#include <QProgressBar>
#include <QLineEdit>
#include <QPlainTextEdit>
#include "qmodernprogresswidget.h"
#include "libwid_imexport.h"

class QFWIDLIB_EXPORT QFFunctionReferenceToolDocSearchThread: public QThread {
       Q_OBJECT
   public:
       QFFunctionReferenceToolDocSearchThread(QStringList files, QObject* parent=NULL);

       void setRxDefinition(const QRegExp& rxDefinition);
       void setRxTemplate(const QRegExp& rxTemplate);
       void stopThread();
   protected:
       void run();
   signals:
       void foundFunction(QString name, QString templ, QString help, QString helplink);
       void foundFunction(QString name, QStringList templ, QString help, QString helplink);
   protected:
       QStringList files;
       QRegExp rxDefinition;
       QRegExp rxTemplate;
       bool stopped;
};





/*! \brief tool object which searches a help folder for syntax reference
    \ingroup qf3rdrdp_table

    This object parses (async) all HTML files (*.html, *.htm) in the given directoreis.
    In these files it expects a section like this:
    \code
<!-- func:sin -->
    <b><tt><!-- template -->sin(x)<!-- /template --></tt></b> - <i>sine function </i>:<br>
    calculates the sine of the argument (argument in radians)
<!-- /func:sin -->
    \endcode

    Here \c sin (in the \c <!--func:NAME--> tag) is the name of the function. It extracts the contents between this
    tag and the according end-tag as help for the function and the string (NO MARKUP!!!) between the
    \c <!--template--> comment tags as function template for the autocomplete/function list.

    The actual pattern matching in the files is doen using regular expressions, that may be altered
    by the user.

*/
class QFWIDLIB_EXPORT QFFunctionReferenceTool : public QObject
{
        Q_OBJECT
    public:
        explicit QFFunctionReferenceTool(QObject *parent = 0);

        void setRxDefinition(const QRegExp& rxDefinition);
        void setRxTemplate(const QRegExp& rxTemplate);
        void setCompleterFile(const QString filename);

        QString getFunctionTemplate(QString name);
        QString getFunctionHelp(QString name);

        QAbstractItemModel *getHelpModel();
        QCompleter* getCompleter() const;
        QStringList getDefaultWords() const;
        void setDefaultWords(const QStringList& words);
        void addDefaultWords(const QStringList& words);
        void setDefaultWordsMathExpression();
        void setDefaultHelp(const QString& defaultHelp);

        void setLabProgress(QLabel* labProgress);
        void setProgress(QProgressBar* progress);
        void setLabHelp(QLabel* labHelp);
        void setLabTemplate(QLabel* labTemplate);
        void setProgress(QModernProgressWidget *progress);

        /** \brief registers a QLineEdit as formula editor (basically the function showCurrentFunctionHelp() can be used to show help for the word currently under the cursor */
        void registerEditor(QLineEdit* edtFormula);
        void registerEditor(QPlainTextEdit* edtFormula);

        bool hasFunction(const QString name);
        void setCurrentHelpButton(QAbstractButton* button);
    signals:
        
    public slots:
        /** \brief opens an online-help window and displays the help entry for the given function */
        void showFunctionHelp(QString name);
        /** \brief searches for a function definition in the following directories. uses the given filename filters to select files in the directories. The directories are then extended by the directories stored in the global config variable \a globalConfigVar (see QFPluginServices::getGlobalConfigValue() )  */
        void startSearch(const QStringList& directories, const QStringList& filters, const QString& globalConfigVar=QString("QFMathParser_ref"));
        void startSearch(const QStringList& directories, const QString& globalConfigVar=QString("QFMathParser_ref"));
        void startSearch(const QString& directory, const QString& globalConfigVar=QString("QFMathParser_ref"));
        /** \brief opens an online-help window and displays the help entry for the function currentyl under the cursor */
        void showCurrentFunctionHelp();
        void showDefaultHelp();
    protected slots:
        void onCursorPositionChanged(int old=0, int newPos=0);
        void threadFinished();
        void delayedStartSearchThreads();
        void addFunction(QString name, QString templ, QString help, QString helplink);
        void addFunction(QString name, QStringList templ, QString help, QString helplink);
        void stopThreadsWait();
        void updateHelpModel();
        void delayedAddNamesAndTemplates();
    protected:
        QFCompleterFromFile* compExpression;
        struct functionDescription {
            QString fhelp;
            QString ftemplate;
            QString fhelplink;
            QString fhelpfile;
        };

        QMap<QString, functionDescription > functionhelp;


        int threadsFinished;
        int maxThreads;
        QList<QFFunctionReferenceToolDocSearchThread*> threads;
        QStringListModel helpModel;
        QStringList defaultWords;
        QString defaultHelp;

        QLabel* labProgress;
        QProgressBar* progress;
        QModernProgressWidget* mprogress;
        bool searching;

        QRegExp rxDefinition;
        QRegExp rxTemplate;

        QLabel* labHelp;
        QLabel* labTemplate;
        QString currentFunction;
        QAbstractButton* btnCurrentHelp;
        QAction* actDefaultHelp;
        QAction* actCurrentFunctionHelp;
        QStringList namesToAdd, templatesToAdd;
        QTimer timDelayedAddNamesAndTemplates;
};

#endif // QFFUNCTIONREFERENCETOOL_H
