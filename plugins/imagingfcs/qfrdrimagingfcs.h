#ifndef QFRDRIMAGINGFCS_H
#define QFRDRIMAGINGFCS_H

#include "qfpluginrawdata.h"
/*!
    \defgroup qf3rdrdp_imaging_fcs Raw Data Record Plugin
    \ingroup qf3rawdataplugins
*/

/*! \brief raw data record plugin class
    \ingroup qf3rdrdp_imaging_fcs
    */

class QFRDRImagingFCSPlugin : public QObject, public QFPluginRawDataRecordBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginRawDataRecord)
    public:
        /** Default constructor */
        QFRDRImagingFCSPlugin(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFRDRImagingFCSPlugin();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFRawDataRecord* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("imaging_fcs"); };

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("Imaging FCS Data"); };

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("manages FCS correlation curves from an imaging FCS experiment (SPIM-FCS, TIR-FCS ...)"); };

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2011 by Jan W. Krieger"); };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

        /** \brief icon for the plugin */
        virtual QString getIconFilename() const  { return QString(":/imaging_fcs/qfrdrimagingfcs.png"); };
    protected slots:
        /** \brief insertdata from file*/
        void insertRecord();
};

#endif // QFRDRIMAGINGFCS_H
