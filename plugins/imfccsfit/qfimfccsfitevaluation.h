#ifndef QFIMFCCSFITEVALUATION_H
#define QFIMFCCSFITEVALUATION_H

#include "qfpluginevaluation.h"
#include "qfpluginoptionsdialog.h"
#include "qfpluginservices.h"

class QFImFCCSRelativeCCFDialog;//forward

/*!
    \defgroup qf3rdrdp_imfccsfit Evaluation Item Plugin for imaging Fluorescence Crosscorrelation Spectroscopy Fits
    \ingroup qf3evaluationplugins
*/
/*! \brief imFCCS Evaluation Item Plugin class
    \ingroup qf3rdrdp_imfccsfit
*/
class QFImFCCSFitEvaluation : public QObject, public QFPluginEvaluationItemBase, public QFPluginOptionsDialogInterface {
        Q_OBJECT
        Q_INTERFACES(QFPluginEvaluationItem QFPluginOptionsDialogInterface)
    public:
        /** Default constructor */
        QFImFCCSFitEvaluation(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFImFCCSFitEvaluation();

        /** \brief Create a new QFEvaluationItem in the given project \a parent. Also adds the record to the project. */
        virtual QFEvaluationItem* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        virtual void init();

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("imfccs_fit"); };

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("ImagingFCCS Fit Plugin"); };

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("Plugin for fitting imaging Fluorescence Crosscorrelation Spectroscopy (imFCCS) data"); };

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2013 by Jan W. Krieger"); };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };

        /** \brief icon file for the plugin (from resource system or a separate file) */
        virtual QString getIconFilename() const  { return QString(":/imfccs_fit_logo.png"); };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };




        QString pluginOptionsName() const;
        QIcon pluginOptionsIcon() const;
        QFPluginOptionsWidget *createOptionsWidget(QWidget *parent);

    protected slots:
        /** \brief insert FCS data from file*/
        void insertEvaluation();

        /** \brief calculates the fraction of CCF amplitude */
        void calcRelativeCCF();

    private:

        QPointer<QFImFCCSRelativeCCFDialog> dlgRelCCF;
};

#endif // QFIMFCCSFITEVALUATION_H
