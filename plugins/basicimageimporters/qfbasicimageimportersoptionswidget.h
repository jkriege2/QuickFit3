#ifndef QFBASICIMAGEIMPORTERSOPTIONSWIDGET_H
#define QFBASICIMAGEIMPORTERSOPTIONSWIDGET_H

#include <QWidget>
#include "qfpluginoptionsdialog.h"

namespace Ui {
    class QFBasicImageImportersOptionsWidget;
}

class QFBasicImageImportersOptionsWidget : public QFPluginOptionsWidget
{
        Q_OBJECT
        
    public:
        explicit QFBasicImageImportersOptionsWidget(QObject* pluginObject, QWidget *parent = 0);
        ~QFBasicImageImportersOptionsWidget();
        
        /** \brief read the widget contents from the program options */
        void readSettings(ProgramOptions* options);
        /** \brief write the widget contents to the program options */
        void writeSettings(ProgramOptions* options);
    protected slots:
        void on_btnClearLibTIFFLog_clicked();
    private:
        Ui::QFBasicImageImportersOptionsWidget *ui;
};

#endif // QFBASICIMAGEIMPORTERSOPTIONSWIDGET_H
