#ifndef HEADER_H_H
#define HEADER_H_H

#include "qfpluginevaluation.h"
/*!
    \defgroup qf3rdrdp_GROUPNAME Evaluation Item Plugin
    \ingroup qf3evaluationplugins
*/
/*! \brief Evaluation Item Plugin class
    \ingroup qf3rdrdp_GROUPNAME
*/
class QFEVAL : public QObject, public QFPluginEvaluationItemBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginEvaluationItem)
    public:
        /** Default constructor */
        QFEVAL(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFEVAL();

        /** \brief Create a new QFEvaluationItem in the given project \a parent. Also adds the record to the project. */
        virtual QFEvaluationItem* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("target_id"); };

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("Evaluation Plugin"); };

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("Description"); };

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr(""); };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c)"); };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr(""); };

        /** \brief icon file for the plugin (from resource system or a separate file) */
        virtual QString getIconFilename() const  { return QString(":/target_id/qfeval.png"); };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

    protected slots:
        /** \brief insert FCS data from file*/
        void insertEvaluation();

    private:
};

#endif // HEADER_H_H
