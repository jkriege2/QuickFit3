#ifndef DLGPOLOLUMAESTROTESTER_H
#define DLGPOLOLUMAESTROTESTER_H

#include <QDialog>
#include "jkserialconnection.h"
#include "programoptions.h"
#include "qfbinaryserialprotocolhandler.h"

namespace Ui {
    class DlgPololuMaestroTester;
}

class DlgPololuMaestroTester : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DlgPololuMaestroTester(QWidget *parent = 0);
        ~DlgPololuMaestroTester();
        
    protected slots:
        void updateRead();
        void on_spinSet_valueChanged(int value);
        void on_btnConnect_clicked(bool checked);
        void on_cmbBaudrate_currentIndexChanged(int index);
    private:
        Ui::DlgPololuMaestroTester *ui;
        JKSerialConnection com;
        QFBinarySerialProtocolHandler* serial;
};

#endif // DLGPOLOLUMAESTROTESTER_H
