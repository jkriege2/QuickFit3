#include "qfparametercorrelationservice.h"

QFParameterCorrelationService* QFParameterCorrelationService::instance=NULL;

QFParameterCorrelationService::QFParameterCorrelationService()
{
    if (!instance) instance=this;
}

QFParameterCorrelationService::~QFParameterCorrelationService()
{
    if (instance==this) instance=NULL;
}
