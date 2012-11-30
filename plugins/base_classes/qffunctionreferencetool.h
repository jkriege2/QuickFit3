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

class QFFunctionReferenceToolDocSearchThread: public QThread {
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
class QFFunctionReferenceTool : public QObject
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

        void registerEditor(QLineEdit* edtFormula);

        bool hasFunction(const QString name);
        void setCurrentHelpButton(QAbstractButton* button);
    signals:
        
    public slots:
        void showFunctionHelp(QString name);
        void startSearch(const QStringList& directories, const QStringList& filters);
        void startSearch(const QStringList& directories);
        void startSearch(const QString& directory);
        void showCurrentFunctionHelp();
    protected slots:
        void onCursorPositionChanged(int old, int newPos);
        void threadFinished();
        void delayedStartSearchThreads();
        void addFunction(QString name, QString templ, QString help, QString helplink);
        void stopThreadsWait();
        void updateHelpModel();
    protected:
        QFCompleterFromFile* compExpression;
        struct functionDescription {
            QString fhelp;
            QString ftemplate;
            QString fhelplink;
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
};

#endif // QFFUNCTIONREFERENCETOOL_H
