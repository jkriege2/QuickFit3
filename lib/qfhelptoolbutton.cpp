#include "qfhelptoolbutton.h"
#include "qfpluginservices.h"

QFHelpToolButton::QFHelpToolButton(QWidget *parent) :
    QToolButton(parent)
{
    setHelp("quickfit.html");
    initWidget();
}

QFHelpToolButton::QFHelpToolButton(const QString &plugin, const QString &filename, QWidget *parent) :
    QToolButton(parent)
{
    setHelp(plugin, filename);
    initWidget();
}

QFHelpToolButton::QFHelpToolButton(const QString &filename, QWidget *parent) :
    QToolButton(parent)
{
    setHelp(filename);
    initWidget();
}

void QFHelpToolButton::setHelp(const QString &plugin, const QString &filename)
{
    m_plugin=plugin;
    m_filename=filename;
}

void QFHelpToolButton::setHelp(const QString &filename)
{
    m_plugin="";
    m_filename=filename;
}

void QFHelpToolButton::showHelp()
{
    if (m_plugin.isEmpty()) QFPluginServices::getInstance()->displayMainHelpWindow(m_filename);
    else  QFPluginServices::getInstance()->displayPluginHelpWindow(m_plugin, m_filename);
}

void QFHelpToolButton::initWidget()
{
    connect(this, SIGNAL(clicked()), this, SLOT(showHelp()));
    setText("");
    setIcon(QIcon(":/lib/help.png"));
    setToolTip(tr("show online-help ..."));
}
