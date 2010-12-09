#ifndef QFPEVALFCSFIT_H
#define QFPEVALFCSFIT_H

#include "qfpluginevaluation.h"
/*!
    \defgroup qf3evalp_fcsfit FCS Least Squares Curve Fitting Plugin
    \ingroup qf3evaluationplugins
*/
/*! \brief plugin class for FCS least-square fits
    \ingroup qf3evalp_fcsfit
*/
class QFPEvalFCSFit : public QObject, public QFPluginEvaluationItemBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginEvaluationItem)
    public:
        /** Default constructor */
        QFPEvalFCSFit(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFPEvalFCSFit();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFEvaluationItem* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() { return tr("fcs_fit"); };

        /** \brief name for the plugin */
        virtual QString getName() { return tr("FCS Correlation Curve Fitting Plugin"); };

        /** \brief short description for the plugin */
        virtual QString getDescription() { return tr("fit Fluorescence Correlations Spectroscopy (FCS) curve data."); };

        /** \brief author the plugin */
        virtual QString getAuthor() { return tr("Jan Krieger"); };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() { return tr("(c) 2010 by Jan Krieger"); };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() { return tr(""); };

        /** \brief icon file for the plugin (from resource system or a separate file) */
        virtual QString getIconFilename() { return QString(":/fcs_fit_logo.png"); };


    protected slots:
        /** \brief insert FCS data from file*/
        void insertFCSFit();

    private:
};

#endif // QFPEVALFCSFIT_H
