#ifndef QFRADHARD2FLASHTOOL_H
#define QFRADHARD2FLASHTOOL_H

#include <QDialog>
#include "../base_classes/radhard2flash.h"
#include "programoptions.h"

namespace Ui {
    class QFRadhard2Flashtool; // forward
}

class QFExtensionCameraRadhard2; // forward

/*! \brief Flash tool dialog for Radhard2
    \ingroup qf3ext_radhard2

*/
class QFRadhard2Flashtool : public QDialog{
        Q_OBJECT
    public:
        explicit QFRadhard2Flashtool(QFExtensionCameraRadhard2 *ext, QWidget* parent = 0);
        ~QFRadhard2Flashtool();

        void setBitfile(QString bitfile);
        void setAutoBitfile(QString bitfile);
        void setAutoFlash(bool v);
        void setRetries(int v);
        void setRetryDelayMS(int milliseconds);

        int retryDelayMS() const;
        int retries() const;
        bool autoflash() const;
        QString bitfile() const;
        QString autoBitfile() const;
    protected slots:
        void on_btnLoad_clicked();
        void on_btnLoad1_clicked();
        void on_btnFlash_clicked();
    private:
        Ui::QFRadhard2Flashtool *ui;
        QFExtensionCameraRadhard2* radhard2extension;
};

#endif // QFRADHARD2FLASHTOOL_H
