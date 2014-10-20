#include "qfhelpaction.h"
#include "qfpluginservices.h"

QFHelpAction::QFHelpAction(QObject *parent) :
    QAction(parent)
{
    setHelp("quickfit.html");
    connect(this, SIGNAL(triggered()), this, SLOT(showHelp()));
}

QFHelpAction::QFHelpAction(const QString &plugin, const QString &filename, QObject *parent) :
    QAction(parent)
{
    setHelp(plugin, filename);
    connect(this, SIGNAL(triggered()), this, SLOT(showHelp()));
}

QFHelpAction::QFHelpAction(const QString &filename, QObject *parent) :
    QAction(parent)
{
    setHelp(filename);
    connect(this, SIGNAL(triggered()), this, SLOT(showHelp()));
}

void QFHelpAction::setHelp(const QString &plugin, const QString &filename)
{
    mainHelp=false;
    m_plugin=plugin;
    m_filename=filename;
}

void QFHelpAction::setMainHelp(const QString &filename)
{
    mainHelp=true;
    m_plugin="";
    m_filename=filename;
}

void QFHelpAction::setHelp(const QString &filename)
{
    mainHelp=false;
    m_plugin="";
    m_filename=filename;
}

void QFHelpAction::showHelp()
{
    if (m_plugin.isEmpty())  {
        if (mainHelp) QFPluginServices::getInstance()->displayMainHelpWindow(m_filename);
        else QFPluginServices::getInstance()->displayHelpWindow(m_filename);
    } else  QFPluginServices::getInstance()->displayPluginHelpWindow(m_plugin, m_filename);
}
