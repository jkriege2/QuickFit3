#ifndef QFFCSMAXENTEVALUATION_H
#define QFFCSMAXENTEVALUATION_H

#include "qfpluginevaluation.h"
/*!
    \defgroup qf3rdrdp_GROUPNAME Evaluation Item Plugin
    \ingroup qf3evaluationplugins
*/
/*! \brief Evaluation Item Plugin class
    \ingroup qf3rdrdp_GROUPNAME
*/
class QFFCSMSDEvaluation : public QObject, public QFPluginEvaluationItemBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginEvaluationItem)
    public:
        /** Default constructor */
        QFFCSMSDEvaluation(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFFCSMSDEvaluation();

        /** \brief Create a new QFEvaluationItem in the given project \a parent. Also adds the record to the project. */
        virtual QFEvaluationItem* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("fcs_msd"); }

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("FCS: MSD"); }

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("extracts the mean squared displacement from FCS autocorrelation curves"); }

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2012 by Jan W. Krieger"); }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }

        /** \brief icon file for the plugin (from resource system or a separate file) */
        virtual QString getIconFilename() const  { return QString(":/fcsmsdevaluation/fcsmsdevaluation.png"); }

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        }

    protected slots:
        /** \brief insert FCS data from file*/
        void insertEvaluation();

    private:
};

#endif // QFFCSMAXENTEVALUATION_H
