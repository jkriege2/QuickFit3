#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "nidriver.h"
#include "nichannelsmodel.h"
#include "jkqtplotter.h"
#include <QVector>
#include <QElapsedTimer>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

        // helper functions for NI event handlers/driver funtions, to access the current task configuration
        NIChannelsModel* getChannels() const;
        bool getStartTrigger() const;
        bool getStartTriggerRetriggerable() const;
        int getStartTriggerEdge() const;
        QString getStartTriggerChannel() const;
        bool getRefTrigger() const;
        int getRefTriggerEdge() const;
        QString getRefTriggerChannel() const;
        double getSampleClock() const;
        double getPreviewSampleClock() const;
        int getUpdatePlotSamples() const;
        int getUpdatePreviewPlotSamples() const;
        int getSamples() const;
        int getPretriggerSamples() const;
        QAction* getAcquisitionAction() const;

        // helper functions for the NI event handler (send block of data, send LOG message, enable/disable acquisition
        void sendSetAcquisitionEnabled(bool en);
        void sendLogMessage(const QString& message);
        void sendData(QList<QVector<double> > data);
    signals:
        void addedLogMessage(const QString& message);
        void sentSetAcquisitionEnabled(bool en);
        void sentData(const QList<QVector<double> > data);
    public slots:
        void addLogMessage(const QString& message);
        void setAcquisitionEnabled(bool en);
        void addData(const QList<QVector<double> > data);
    protected:
        void closeEvent(QCloseEvent *event);
    protected slots:
        void on_btnRemoveChannel_clicked();
        void on_btnAddChannel_clicked();
        void loadSettings(const QString &fn=QString(), const QString &prefix=QString());
        void storeSettings(const QString& fn=QString(), const QString &prefix=QString());
        void loadINI();
        void saveINI();
        void startAcquisition(bool start);
        void startPreview(bool start);
        void on_btnClearGraph_clicked();

        void on_cmbCTransform_currentIndexChanged(int index);


    private:
        Ui::MainWindow *ui;

        QAction* actStartAcquisition;
        QAction* actStartPreview;
        QAction* actExit;
        QAction* actLoadINI;
        QAction* actSaveINI;
        QString lastINIDir;

        /** \brief Handle to the currently running NI taks (0, if no task is referenced) */
        TaskHandle NITaskHandle;

        /** \brief model, which manages the channels (and their metadata: names, physical, type, ...) within the currently running NI task */
        NIChannelsModel* channels;

        /** \brief list of plot objects for the plotter widgets
         *
         * each entry represents one graph within the plot
         **/
        QList<JKQTPxyLineGraph*> plteData;
        QElapsedTimer lastPlotUpdate;

        QToolBar* tbQF3;
};

#endif // MAINWINDOW_H
