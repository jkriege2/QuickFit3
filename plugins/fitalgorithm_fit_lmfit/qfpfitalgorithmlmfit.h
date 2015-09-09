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
#ifndef QFPFITALGORITHMLMFIT_H
#define QFPFITALGORITHMLMFIT_H

#include "qfpluginfitalgorithm.h"

/*!
    \defgroup qf3fitfunp_fit_lmfit LMFit Fitting Algorithm Plugin
    \ingroup qf3fitalgplugins
*/



/*! \brief plugin class for Levmar Fit Algorithm
    \ingroup qf3fitalgp_lmfit
*/
class QFPFitAlgorithmLMFitPlugin : public QObject, public QFPluginFitAlgorithm {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitAlgorithm)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFPFitAlgorithmLMFitPlugin")
#endif

    public:
		QF_PLUGIN
	
        /** Default constructor */
        QFPFitAlgorithmLMFitPlugin(QObject* parent=NULL): QObject(parent) {}
        /** Default destructor */
        virtual ~QFPFitAlgorithmLMFitPlugin() {}



        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("lmfit: Levenberg-Marquard fit");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("Levenberg-Marquard fit using the lmfit library");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("plugin: (c) 2011-2015 Jan Krieger<br/>lmfit: (c) 2009-2013  Joachim Wuttke <j.wuttke@fz-juelich.de>");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("");
        };
		

        /** \brief plugin ID  */
        virtual QString getID() const {
            return "fit_lmfit";
        };

        /** \brief plugin icon  */
        virtual QString getIconFilename() const {
            return ":/lib/fitalg_icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=1;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitAlgorithm* get(const QString &id) const;
    private:
};

#endif // QFPFITALGORITHMLMFIT_H
