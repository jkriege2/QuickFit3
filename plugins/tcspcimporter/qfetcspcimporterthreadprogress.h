#ifndef QFETCSPCImporterTHREADPROGRESS_H
#define QFETCSPCImporterTHREADPROGRESS_H

#include <QWidget>

namespace Ui {
    class QFETCSPCImporterThreadProgress;
}

/*! \brief progress widget for the correlation progress
    \ingroup qf3ext_tcspcimporter
*/
class QFETCSPCImporterThreadProgress : public QWidget {
        Q_OBJECT

    public:
        explicit QFETCSPCImporterThreadProgress(QWidget *parent = 0);
        ~QFETCSPCImporterThreadProgress();
        bool wasCanceled() const;
        bool isDone() const;
        void resetWidget();
        int getRangeMax() const;
        int getRangeMin() const;
        int getProgress() const;
        int getStatus() const;
    public slots:
        void setName(QString name);
        /*! \brief set the status displayed

              - \a status == 0: waiting
              - \a status == 1: running
              - \a status == 2: done
              - \a status == -1: error
          */
        void setStatus(int status=0);
        void setMessage(QString message);
        void setProgress(int p);
        void incProgress(int pi);
        void setRange(int min, int max);
    signals:
        void cancelClicked();
        void closeClicked();
    protected slots:
        void on_btnCancel_clicked();
    private:
        Ui::QFETCSPCImporterThreadProgress *ui;
        bool m_wasCanceled;
        bool m_isDone;
        int m_status;
};

#endif // QFETCSPCImporterTHREADPROGRESS_H
