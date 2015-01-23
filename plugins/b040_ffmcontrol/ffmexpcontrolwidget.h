#ifndef FFMEXPCONTROLWIDGET_H
#define FFMEXPCONTROLWIDGET_H

#include <QWidget>
#include<QtGlobal>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "../multicontrol_stage/multicontrol_stage.h"
#include "../alv_autocorrelator5000/alv_autocorrelator5000.h"
#include "../scanner2000_nicounter/scanner2000nicounter.h"
//#include ""
#include "qfstagecombobox.h"
#include "qfextensionmanager.h"
#include "ffmautocorrelator.h"
#include <QDebug>
#include <limits>
/// TEST
//#include "../alv_autocorrelator5000/alvsma5edataout.h"
// TEST

class FFMExpControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FFMExpControlWidget(QWidget *parent = 0);
    //FFMExpControlWidget(MulticontrolStage *tstage, QWidget *parent = 0);
    ~FFMExpControlWidget();

    void setLog(QtLogFile *logfile);
protected:
    QGridLayout *layout;
    QPushButton *SingleImageButton;
    QPushButton *ImageStackButton;
    QPushButton *FCSButton;
    QtLogFile *log;


    QPushButton *scannertest; //testwidget
    QPushButton *Stagetest; //testwidget
    QPushButton *Alvtest; // testwidget



    AlvAutocorrelator *autocorrelator;
    //ScanController *scancontroller;
    Scanner2000NICounter *scancounter;

    QFStageComboBox *stagecombobox;
    MulticontrolStage *stage;
   // MCStageController *stagecontroller; // test




    // TEST WIDGET FOR STAGE CONTROL
private:
        QWidget *testwidget;
        QGridLayout *stage_layout;
        //QSpinBox *z_axis, *y_axis, *x_axis;
        QLineEdit *z_axis, *y_axis, *x_axis;
        QDoubleValidator *doubleValidator;
        QPushButton *moveButtonX;
        QPushButton *moveButtonY;
        QPushButton *moveButtonZ;
        QPushButton *connectButton;
        QPushButton *disconnectButton;
public slots: // for testButtons
        void connect_stage();
        void movex();
        void movey();
        void movez();
        void disconnect_stage();

// END TEST WIDGET STAGE

// TEST WIDGET SCAN CONTROL
protected:
        // test stuff
        QWidget *scantestwidget; //test
        QGridLayout *scanlayout; //test
        QSpinBox *scanx_axis, *scany_axis; //test
        QPushButton *scaninitButton; //test
        QPushButton *scanmoveButton; //test
        QPushButton *scanconnectButton;
        QPushButton *scandisconnectButton;
public slots:
        void moveScannerSlot();
        void connectScannerSlot();
        void disconnectScannerSlot();
        void initScannerSlot();
        // END TEST WIDGET SCAN CONTROL



signals:

public slots:
    void openscannertest();
    void openStagetest();
    void openAlvtest();


protected:

    virtual void log_text(QString message);

    virtual void log_warning(QString message);

    virtual void log_error(QString message);



};

#endif // FFMEXPCONTROLWIDGET_H
