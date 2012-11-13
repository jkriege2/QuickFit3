#ifndef QFESPIMB040SCRIPTEDACQUISITION_H
#define QFESPIMB040SCRIPTEDACQUISITION_H


#include <QWidget>
#include <QSettings>
#include <QAction>
#include <QToolButton>
#include <QtScript>
#include "../interfaces/qfextensionlinearstage.h"
#include "qfespimb040opticssetup.h"
#include "qfespimb040filenametool.h"
#include "qtriple.h"
#include "qfespimb040acquisitiontools.h"



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
        explicit QFESPIMB040ScriptedAcquisition(QFESPIMB040MainWindow2* mainWindow, QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetup* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription, QString configDirectory);
        ~QFESPIMB040ScriptedAcquisition();

        QString getScript() const;

        void loadSettings(QSettings& settings, QString prefix);
        void storeSettings(QSettings& settings, QString prefix) const;

    signals:
        void doAcquisition();

    public slots:
        void performAcquisition();

    protected slots:
        void on_btnExecute_clicked();
        void on_btnCancel_clicked();
        void updateReplaces();
        void on_btnNew_clicked();
        void on_btnOpen_clicked();
        void on_btnSave_clicked();
        void on_btnSyntaxCheck_clicked();
        void on_btnOpenExample_clicked();
        void on_btnOpenTemplate_clicked();
        void on_edtScript_cursorPositionChanged();
        void on_btnHelp_clicked();

        void openScript(QString dir, bool saveDir=true);

        void threadFinished();
        void delayedStartSearchThreads();
        void addFunction(QString name, QString templ, QString help);
    protected:
        bool maybeSave();
        void setScriptFilename(QString filename);
        QString currentScript;
    private:
        Ui::QFESPIMB040ScriptedAcquisition *ui;
        QFPluginServices* m_pluginServices;
        QFESPIMB040OpticsSetup* opticsSetup;
        QFESPIMB040AcquisitionDescription* acqDescription;
        QFESPIMB040ExperimentDescription* expDescription;
        QFPluginLogService* log;
        QFESPIMB040AcquisitionTools* acqTools;
        QFESPIMB040MainWindow2* mainWindow;
        QScriptEngine* engine;

        QMap<QString, QPair<QString, QString> > functionhelp;

        QString getFunctionTemplate(QString name);
        QString getFunctionHelp(QString name);

        int threadsFinished;
        int maxThreads;
        QList<QFESPIMB040ScriptedAcquisitionDocSearchThread*> threads;
        QStringListModel helpModel;
        QStringList defaultWords;
        QString lastScript;

};

#endif // QFESPIMB040SCRIPTEDACQUISITION_H
