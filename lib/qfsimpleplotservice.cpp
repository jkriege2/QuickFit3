#include "qfsimpleplotservice.h"



QFSimplePlotService* QFSimplePlotService::instance=NULL;

QFSimplePlotService::QFSimplePlotService()
{
    if (!instance) instance=this;
}

QFSimplePlotService::~QFSimplePlotService()
{
    if (instance==this) instance=NULL;
}
