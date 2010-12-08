#ifndef QFPRDRFCS_H
#define QFPRDRFCS_H

#include "qfpluginrawdata.h"


class QFPRDRFCS : public QObject, public QFPluginRawDataRecordBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginRawDataRecord)
    public:
        /** Default constructor */
        QFPRDRFCS(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFPRDRFCS();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFRawDataRecord* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() { return tr("fcs"); };

        /** \brief name for the plugin */
        virtual QString getName() { return tr("FCS Correlation Curve Plugin"); };

        /** \brief short description for the plugin */
        virtual QString getDescription() { return tr("manages Fluorescence Correlations Spectroscopy (FCS) curve data."); };
    protected slots:
        /** \brief insert FCS data from file*/
        void insertFCS();

    private:
};

#endif // QFPRDRFCS_H
