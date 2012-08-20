#ifndef QFRDRNUMBERANDBRIGHTNESS_H
#define QFRDRNUMBERANDBRIGHTNESS_H

#include "qfpluginrawdata.h"
/*!
    \defgroup qf3rdrdp_number_and_brightness Raw Data Record Plugin
    \ingroup qf3rawdataplugins
*/

/*! \brief raw data record plugin class
    \ingroup qf3rdrdp_number_and_brightness
    */

class QFRDRNumberAndBrightnessPlugin : public QObject, public QFPluginRawDataRecordBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginRawDataRecord)
    public:
        /** Default constructor */
        QFRDRNumberAndBrightnessPlugin(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFRDRNumberAndBrightnessPlugin();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFRawDataRecord* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("number_and_brightness"); };

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr(""); };

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr(""); };

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr(""); };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr(""); };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr(""); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

        /** \brief icon for the plugin */
        virtual QString getIconFilename() const  { return QString(":/number_and_brightness/qfrdrnumberandbrightness.png"); };
    protected slots:
        /** \brief insertdata from file*/
        void insertRecord();
};

#endif // QFRDRNUMBERANDBRIGHTNESS_H
