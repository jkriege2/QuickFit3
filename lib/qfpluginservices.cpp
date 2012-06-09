#include "qfpluginservices.h"

QFPluginServices* QFPluginServices::inst=NULL;

QFPluginServices::QFPluginServices()
{
    if (inst==NULL) inst=this;
}

QFPluginServices::~QFPluginServices()
{
    if (inst==this) inst=NULL;
}

