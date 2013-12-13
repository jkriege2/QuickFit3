#include "qfespimb040opticssetupbase.h"
#include "qfespimb040acquisitiontools.h"

QFESPIMB040OpticsSetupBase::QFESPIMB040OpticsSetupBase(QWidget *parent):
    QWidget(parent)
{
    acqTools=NULL;
}

QFESPIMB040OpticsSetupBase::~QFESPIMB040OpticsSetupBase()
{
}

void QFESPIMB040OpticsSetupBase::setAcquisitionTools(QFESPIMB040AcquisitionTools *acqTools)
{
    this->acqTools=acqTools;
}

QFESPIMB040AcquisitionTools *QFESPIMB040OpticsSetupBase::getAcquisitionTools() const
{
    return acqTools;
}
