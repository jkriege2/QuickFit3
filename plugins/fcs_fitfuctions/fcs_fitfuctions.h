#ifndef FCS_FITFUCTIONS_H
#define FCS_FITFUCTIONS_H

#include "qfpluginfitfunction.h"
/*!
    \defgroup qf3fitfunp_fcs FCS Fitting Function Plugin
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin class for FCS Normal Diffusion Fit Functions
    \ingroup qf3fitfunp_fcs
*/
class QFFitFunctionFCSDiff: public QFFitFunction {
    public:
        QFFitFunctionFCSDiff();
        virtual ~QFFitFunctionFCSDiff() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("FCS: Normal Diffusion 3D"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_diff"); };
        /*! \copydoc QFFitFunction::helpFile()   */
        virtual QString helpFile() const { return id()+".html"; };

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double t, const double* data) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* data, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, double* data) const;
        /*! \copydoc QFFitFunction::getAdditionalPlotCount()   */
        virtual unsigned int getAdditionalPlotCount(const double* params);

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()   */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params);

};

/*! \brief plugin class for FCS Anomalous Diffusion Fit Functions
    \ingroup qf3fitfunp_fcs
*/
class QFFitFunctionFCSADiff: public QFFitFunction {
    public:
        QFFitFunctionFCSADiff();
        virtual ~QFFitFunctionFCSADiff() {}
        /** \brief return a name for the model */
        virtual QString name() const { return QString("FCS: Anomalous Diffusion 3D"); };
        /** \brief return a short unique model ID string */
        virtual QString id() const { return QString("fcs_adiff"); };
        /** \brief return a HTML file to be displayed as model help */
        virtual QString helpFile() const { return id()+".html"; };

        /** \brief evaluate the fitting function with the given parameter vector */
        virtual double evaluate(double t, const double* data) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* data, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, double* data) const;
        /*! \copydoc QFFitFunction::getAdditionalPlotCount()   */
        virtual unsigned int getAdditionalPlotCount(const double* params);

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()   */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params);

};






/*! \brief plugin class for FCS Fitting Functions
    \ingroup qf3fitfunp_fcs
*/
class QFPFitFunctionFCS : public QObject, public QFPluginFitFunction {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction)
    public:
        /** Default constructor */
        QFPFitFunctionFCS(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPFitFunctionFCS() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("FCS fit functions");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("a basic set of FCS fit functions");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2011 by Jan Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("");
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* get(QString id, QObject* parent) const ;
    private:
};

#endif // FCS_FITFUCTIONS_H
