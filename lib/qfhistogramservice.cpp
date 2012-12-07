#include "qfhistogramservice.h"

QFHistogramService* QFHistogramService::instance=NULL;


QFHistogramService::QFHistogramService()
{
    if (!instance) instance=this;
}

QFHistogramService::~QFHistogramService()
{
    if (instance==this) instance=NULL;
}
