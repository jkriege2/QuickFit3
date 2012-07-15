#ifndef QFEACQUISITIONTESTDIALOG_H
#define QFEACQUISITIONTESTDIALOG_H
#include "qfpluginservices.h"
#include <QWidget>

namespace Ui {
    class QFEAcquisitiontestDialog;
}

class QFEAcquisitiontestDialog : public QWidget, public QFPluginLogService
{
        Q_OBJECT
        Q_INTERFACES(QFPluginLogService)
    public:
        explicit QFEAcquisitiontestDialog(QWidget *parent = 0);
        ~QFEAcquisitiontestDialog();
        
        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);
    protected slots:
        void on_btnConnect_clicked(bool pressed);
    private:
        Ui::QFEAcquisitiontestDialog *ui;
};

#endif // QFEACQUISITIONTESTDIALOG_H
