#ifndef DLGQFFITALGORITHMPROGRESSDIALOG_H
#define DLGQFFITALGORITHMPROGRESSDIALOG_H

#include <QDialog>
#include "qffitalgorithm.h"
#include ".uis/ui_dlg_fitalgorithmprogress.h"




/*! \brief a progress dialog for QFFitAlgorithm objects
    \ingroup qf3lib_fitting

    This dialog implements the methods, as in the interface QFFitAlgorithmReporter. In addition this dialog allows to display
    a super message and progress which will be displayed above the default status message (see images below) using
      - reportSuperStatus()
      - setSuperProgressMax()
      - setSuperProgress()
      - incSuperProgress()
    .

    \note you can NOT directly use this dialog as a QFFitAlgorithmReporter as this won't work over thread boundries. Instead
          use dlgQFFitAlgorithmProgressDialogReporter, by first instaciating dlgQFFitAlgorithmProgressDialog and then
          dlgQFFitAlgorithmProgressDialogReporter. Finally you may use dlgQFFitAlgorithmProgressDialogReporter as a QFFitAlgorithmReporter
          savely even over thread boundaries!

    \image html dlg_fitalgorithmprogress.png
    \image latex dlg_fitalgorithmprogress.png

    After initializing the dialog, you may display it using display() and then call done() to close it again.
    Note that this dialog implements a delay for displaying. So if you set this delay (in milliseconds) using setDisplayDelay()
    to a positive value, display() will return immediately and the dialog will be displayed with the given delay.
    Initially the delay is 500 milliseconds. If done() is called before the delay is over the dialog won't be displayed at all.

    Using setAllowCancel() you may en-/disable the cancel Button.


*/
class dlgQFFitAlgorithmProgressDialog : public QDialog, public Ui::QFFitAlgorithmProgressDlg//, public QFFitAlgorithmReporter
{
        Q_OBJECT
    public:
        dlgQFFitAlgorithmProgressDialog(QWidget* parent=NULL);
        virtual ~dlgQFFitAlgorithmProgressDialog();


        /** \brief return \c true, if the user has canceled the fit procedure */
        bool isCanceled() { return m_canceled; };

        /** \brief set the display delay in milliseconds */
        void setDisplayDelay(double delay_in_msecs) { m_displayDelay=delay_in_msecs; }
        /** \brief enable or disable the cancel button. */
        void setAllowCancel(bool enable);

    public slots:
        /** \brief use this to display the widget */
        void display();
        /** \brief call when the dialog should be hidden */
        void done();


        /** \brief report a status message */
        void reportStatus(const QString& message);
        /** \brief set the progress maximum to \a max */
        void setProgressMax(int max=100);
        /** \brief set the current progress to the given value */
        void setProgress(int value);
        /** \brief set the current progress to 100% */
        void setProgressFull();
        /** \brief increment the current progress */
        void incProgress(int increment=1);

        /** \brief report a super status message */
        void reportSuperStatus(const QString& message);
        /** \brief set the super progress maximum to \a max */
        void setSuperProgressMax(int max=100);
        /** \brief set the current super progress to the given value */
        void setSuperProgress(int value);
        /** \brief increment the current super progress */
        void incSuperProgress(int increment=1);


    private:
        bool m_canceled;
        bool m_display;
        double m_displayDelay;
    private slots:
        void on_btnCancel_clicked();
        void doDisplay();
};




/*! \brief this class translates QFFitAlgorithmReporter calls over thread borders to a dlgQFFitAlgorithmProgressDialog
    \ingroup qf3lib_fitting
*/
class dlgQFFitAlgorithmProgressDialogReporter : public QObject, public QFFitAlgorithmReporter
{
        Q_OBJECT
    public:
        dlgQFFitAlgorithmProgressDialogReporter(dlgQFFitAlgorithmProgressDialog* parent);

        /** \brief report a status message */
        virtual void reportStatus(const QString& message);
        /** \brief set the progress maximum to \a max */
        virtual void setProgressMax(int max=100);
        /** \brief set the current progress to the given value */
        virtual void setProgress(int value);
        /** \brief set the current progress to 100% */
        virtual void setProgressFull();
        /** \brief increment the current progress */
        virtual void incProgress(int increment=1);
        /** \brief return \c true, if the user has canceled the fit procedure */
        virtual bool isCanceled();
    signals:
        void s_reportStatus(const QString& message);
        void s_setProgressMax(int max);
        void s_setProgress(int value);
        void s_setProgressFull();
        void s_incProgress(int increment);
    private:
        dlgQFFitAlgorithmProgressDialog* m_parent;
};

#endif // DLGQFFITALGORITHMPROGRESSDIALOG_H
