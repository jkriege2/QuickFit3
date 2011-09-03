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
    public:
		QF_PLUGIN
	
        /** Default constructor */
        QFPFitAlgorithmLMFitPlugin(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPFitAlgorithmLMFitPlugin() {}



        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("lmfit: Levenberg-Marquard fit (without box constraints)");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("Levenberg-Marquard fit (without box constraints) using the lmfit library");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("plugin: (c) 2011 Jan Krieger<br/>lmfit: (c) 2009-2010  Joachim Wuttke <j.wuttke@fz-juelich.de>");
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
            minor=0;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitAlgorithm* get(QString id, QObject* parent) const;
    private:
};

#endif // QFPFITALGORITHMLMFIT_H
