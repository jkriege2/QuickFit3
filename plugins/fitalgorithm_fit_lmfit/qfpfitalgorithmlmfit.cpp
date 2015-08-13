/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
    All rights reserved.

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice, this
       list of conditions and the following disclaimer in the documentation and/or other
       materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors may
       be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
    OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
    AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
    IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include "qfpfitalgorithmlmfit.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "qffitalgorithmlmfit.h"
#include "qffitalgorithmlmfitbox.h"
#include "qffitalgorithmlmfitirls.h"


QStringList QFPFitAlgorithmLMFitPlugin::getIDs() const {
	QStringList sl;
    sl<<"fit_lmfit";
    sl<<"fit_lmfit_nobox";
    sl<<"fit_irls_lmfit";
    return sl;
}

QFFitAlgorithm* QFPFitAlgorithmLMFitPlugin::get(const QString& id) const {
    if (id=="fit_lmfit") return new QFFitAlgorithmLMFitBox();
    if (id=="fit_lmfit_nobox") return new QFFitAlgorithmLMFit();
    if (id=="fit_irls_lmfit") return new QFFitAlgorithmLMFitIRLS();
    return NULL;
}

Q_EXPORT_PLUGIN2(fit_lmfit, QFPFitAlgorithmLMFitPlugin)
