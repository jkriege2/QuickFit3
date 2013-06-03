#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include <QWidget>
#include "qfpluginoptionsdialog.h"

namespace Ui {
    class OptionsWidget;
}

class OptionsWidget : public QFPluginOptionsWidget
{
        Q_OBJECT
        
    public:
        explicit OptionsWidget(QObject* pluginObject, QWidget *parent = 0);
        ~OptionsWidget();
        
        /** \brief read the widget contents from the program options */
        void readSettings(ProgramOptions* options);
        /** \brief write the widget contents to the program options */
        void writeSettings(ProgramOptions* options);
    private:
        Ui::OptionsWidget *ui;
};

#endif // OPTIONSWIDGET_H
