#ifndef QFPEVALIMFCSFIT_H
#define QFPEVALIMFCSFIT_H

#include "qfpluginevaluation.h"
/*!
    \defgroup qf3evalp_imfcsfit FCS Least Squares Curve Fitting Plugin for imaging FCS
    \ingroup qf3evaluationplugins

    This plugin implements more or less the same functionality as the fcsfit plugin (see QFFCSFitEvaluationEditor and QFFCSFitEvaluation). The difference is the
    data storage scheme: Where fcsfit stores data with a separate resultID for every index/run, this plugin stores data as matrices/vectors with one resultID for all
    runs/indices for each parameter.

*/
/*! \brief plugin class for imaging FCS least-square fits
    \ingroup qf3evalp_imfcsfit
*/
class QFPEvalIMFCSFit : public QObject, public QFPluginEvaluationItemBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginEvaluationItem)
    public:
        /** Default constructor */
        QFPEvalIMFCSFit(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFPEvalIMFCSFit();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFEvaluationItem* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("imfcs_fit"); }

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("imagingFCS Correlation Curve Fi Plugin"); }

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("fit imaging Fluorescence Correlations Spectroscopy (imagingFCS) curve data."); }

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan Krieger"); }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2012 by Jan Krieger"); }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }

        /** \brief icon file for the plugin (from resource system or a separate file) */
        virtual QString getIconFilename() const  { return QString(":/imfcs_fit_logo.png"); }

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        }

    protected slots:
        /** \brief insert FCS data from file*/
        void insertFCSFit();

    private:
};

#endif // QFPEVALIMFCSFIT_H
