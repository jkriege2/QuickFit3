#ifndef FFMAUTOCORRELATOR_H
#define FFMAUTOCORRELATOR_H

#include <QObject>
#include <QSharedMemory>
#include <QtGui>
#include <qtlogfile.h>

/*














  OUTDATED CLASS, CONTINUE IN THE CORRESPONDING PLUGIN





























  */

/*
  This class is developed for combined use with the ALV-5000/E Software.
  which exist for Windows systems only; Therefore, use of this class is limited to Windows XP;
  If someone manages running the ALV-software under Linux etc., this class might have to be adopted.
  */

class FFMAutoCorrelator : public QObject
{
    Q_OBJECT
public:
    explicit FFMAutoCorrelator(QObject *parent = 0);
    ~FFMAutoCorrelator();

    QtLogFile *logger;
    /*
      |----------------------------- -|
      | public programming interface  |
      |-------------------------------|
    */
   // void setLog(QtLogFile *logfile);




    // TEST FUNCTIONS
        //
private:

    // properties
    QProcess *AlvSoftware;
    QSharedMemory *sm_data_out, *sm_data_in, *sm_fitdata_out, *sm_fitdata_in;

    /*
      |-----------------------------------------------|
      | internal functions to talk to the AVL program |
      |-----------------------------------------------|
     */

signals:

public slots:

    void startAlv();
    void prepare_shared_memory();
};

#endif // FFMAUTOCORRELATOR_H
