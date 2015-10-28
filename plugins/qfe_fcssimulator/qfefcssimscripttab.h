#ifndef QFEFCSSIMSCRIPTTAB_H
#define QFEFCSSIMSCRIPTTAB_H

#include <QWidget>
#include <QProcess>
#include <QPointer>
#include <QTimer>
#include <QDir>
#include <QFileInfo>

namespace Ui {
    class QFEFCSSimScriptTab;
}

class QFEFCSSimMainWidnow;//forward

class QFEFCSSimScriptTab : public QWidget
{
        Q_OBJECT

    public:
        explicit QFEFCSSimScriptTab(QFEFCSSimMainWidnow *parent);
        ~QFEFCSSimScriptTab();

        void setRunning(bool running=true);

        bool loadFile(const QString& filename=QString());
        bool loadTemplate(const QString& filename=QString());
        void save();
        bool saveAs(const QString& filename=QString());
        void execute();
        void cancelExecution();
        bool isRunning() const;

        QString getFullFilename() const;
        QString getFilename() const;
        bool hasChanged() const;

    private:
        Ui::QFEFCSSimScriptTab *ui;
    protected:
        QString filename;
        QFEFCSSimMainWidnow* mainWin;
        QPointer<QProcess> proc;
        QStringList resultfiles;
        QTimer updTimer;
        bool isqf3acorr;
        bool txtChanged;
        void setFilename(const QString& filename);
    protected slots:
        void startProcess();
        void editorChanged();
        void dataAvailable();
        void dataAvailableTimed();
        void error(QProcess::ProcessError error);
        void finished(int exitCode, QProcess::ExitStatus exitStatus);
        void on_btnImport_clicked();
    signals:
        void textChanged(bool changed);
        void filenameChanged(const QString& filename);
};

#endif // QFEFCSSIMSCRIPTTAB_H
