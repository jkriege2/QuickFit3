#include "qfpluginoptionsdialog.h"
#include "programoptions.h"

QFPluginOptionsWidget::QFPluginOptionsWidget(QObject *pluginObject, QWidget *parent) :
    QWidget(parent)
{
    this->pluginObject=pluginObject;
    this->pluginPlugin=qobject_cast<QFPlugin*>(pluginObject);
    this->pluginOptions=qobject_cast<QFPluginOptionsDialogInterface*>(pluginObject);

    if (this->pluginOptions) {
        setWindowTitle(this->pluginOptions->pluginOptionsName());
        setWindowIcon(this->pluginOptions->pluginOptionsIcon());
    }
}
