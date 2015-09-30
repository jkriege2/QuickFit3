#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>
#include <QAction>
#include <QKeyEvent>
#include<QSettings>

#include "baseimporter.h"

#include "errormodel.h"

#include "channelmanager.h"
#include "sdffparser.h"
#include "nidriver.h"
#include "nisequencegenerator.h"
#include "qmodernprogresswidget.h"

#define FILENAME_SETTINGS "AlexControl.ini"
namespace Ui {
class controlWidget;
}

class controlWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit controlWidget(QWidget *parent = 0);
    ~controlWidget();
    void saveSettings(QSettings &settings,QString prefix=QString(""));
    void loadSettings(QSettings &settings,QString prefix=QString(""));
protected:
    /// called when the application is closed. It will call the storeSettings() functions of all widgets
    virtual void closeEvent(QCloseEvent* event);

signals:
    /** \brief emitted when the filename of a current sdff file has changed*/
    void fileChanged(QString);

private slots:
    void openFileDialog();
    void setSDFFfile(QString filename);
    /** \brief loads a predefined sequence, e.g. continuous wave */
    void setSequence(int index);
    /** \brief load external sdff file and disable ALEX settings */
    void setExternalSDFF();
    /** \brief for toggle=true alternation times are in ms, toggle=false sets the units to us (standard)*/
    void setSlowAlternation(bool toggle=false);
    /** \brief write last run sequence in sdffSequence to user specified file */
    void saveSDFF();
    void fileDlg_fileSelected(const QString &);
        /** \brief connects the driver to the hardware. Only works, if NI card is installed. */
    void connectToHardware();
    /** \brief play button: run the ALEX sequence on the NI card  */
    void runSDFFOnHardware();
    /** \brief checks if all required .ini files exist */
    void checkIniFiles();

    void on_doubleSpinBoxIntensityBlue_editingFinished();
    void on_doubleSpinBoxIntensityGreen_editingFinished();
    void on_comboBoxSequence_editingFinished();
    void on_pushButtonPlay_clicked();
    void on_pushButtonD_clicked();
    void on_pushButtonA_clicked();
    void on_pushButtonDA_clicked();
    void on_pushButtonLoadDefault_clicked();
    void on_lineEdit_returnPressed();

private:
    Ui::controlWidget *ui;

    enum Mode {
        ALEX=0,
        ALEXslow,
        cw_D,
        cw_A,
        cw_DA,
        externalSDFF
    };

    SDFFLib::SDFFparser* sdffParser;

    /** \brief channel manager object */
    SDFFLib::channelManager* cmanager;

    SDFFLib::NIdriver* ni_driver;

    /** \brief full path (including file name) of \c channels.ini file */
    QString channelsFilename;

    /** \brief Qt model to manage the error messages produced by the SDFFLib::SDFFparser */
    ErrorModel* errorModel;

    /** \brief the name of the currently loaded sdff file */
    QString curFile;
    /** \brief full path (including file name) of \c timing.ini file */
    QString timingFilename;
    /** \brief full path (including file name) of \c ports.ini file */
    QString portFilename;

    /** \brief full path of the default or current directory */
    QString directory;

    /** \brief information on the last run sequence which is written to file by saveSDFF() */
    QString sdffSequence;

};





#endif // CONTROLWIDGET_H
