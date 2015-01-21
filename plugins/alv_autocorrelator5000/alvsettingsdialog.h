#ifndef ALVSETTINGSDIALOG_H
#define ALVSETTINGSDIALOG_H

#include <QWidget>
#include <QDialog>
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <limits>
#include "qfpluginservices.h"
#include "alvsma5edatain.h"

class ALVSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ALVSettingsDialog(QSettings& settings, AlvSmA5EDataIn *new_sm_data_in,  QWidget *parent = 0);

//    QPushButton* getConfirmer();
//    QPushButton* getCanceler();



    int32_t getDuration();
    int32_t getNumberOfRuns();
    uint8_t getScalingMode();
    bool getScalingOn0();
    uint8_t getScaleLevel0();
    bool getScalingOn1();
    uint8_t getScaleLevel1();
    bool getDualMode();
    bool getCrossMode();
    bool getIn1Mode();
    bool getFastMode();
    float getScatteringAngle();
    float getWaveLength();
    float getRI();
    float getViscosity();
    float getTemperature();

protected:
    AlvSmA5EDataIn *sm_data_in;
    //QWidget *alvsettings_dialog;
    QGridLayout *layout;
    //QSettings *settings;
    //QPushButton *alvstarter, *memoryallocator, *test_reader, *settings_confirmer;
    //QPushButton *runstartstopper, *test_reader2, *test_reader3, *test_reader4, *test_reader5;

    QSpinBox *duration, *runnumber;
    QGroupBox *scalingmodes;
    QGridLayout *scalinggrid;
    QRadioButton *man,*nor, *con, *sec;
    QCheckBox *scaling0on, *scaling1on;
    QSpinBox *scalelevel0, *scalelevel1;
    QGroupBox *sdmode, *acmode, *inmode;
    QGridLayout *sdgrid, *acgrid, *ingrid;
    QRadioButton *single,*dual, *autoc, *cross, *in0, *in1;
    QCheckBox *fastmode;
    QDoubleValidator *floatvalid;
    QLineEdit *scattangle, *wavel, *ri, *visc, *temp;
    //QGroupBox *runstate;
    QPushButton *confirmer, *canceler;
    //QGridLayout *rungrid;
    //QPushButton *starter, *stopper, *para;



    /** \brief log project text message
     *  \param message the message to log
     */
    void log_text(QString message);
    /** \brief log project warning message
     *  \param message the warning message to log
     */
    void log_warning(QString message);
    /** \brief log project error message
     *  \param message the error message to log
     */
    void log_error(QString message);
signals:

public slots:
    void storeSettings(QSettings& settings);
    //void setValues();
};

#endif // ALVSETTINGSDIALOG_H
