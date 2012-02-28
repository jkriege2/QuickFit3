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
class QFFCSMaxEntEvaluation : public QObject, public QFPluginEvaluationItemBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginEvaluationItem)
    public:
        /** Default constructor */
        QFFCSMaxEntEvaluation(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFFCSMaxEntEvaluation();

        /** \brief Create a new QFEvaluationItem in the given project \a parent. Also adds the record to the project. */
        virtual QFEvaluationItem* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("fcs_maxent"); }

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("FCS: MaxEnt"); }

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("maximum entropy data evaluation plugin for FCS data"); }

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Niko Schnellbächer"); }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2012 by Niko Schnellbächer"); }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }

        /** \brief icon file for the plugin (from resource system or a separate file) */
        virtual QString getIconFilename() const  { return QString(":/fcs_maxent/qffcsmaxentevaluation.png"); }

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
