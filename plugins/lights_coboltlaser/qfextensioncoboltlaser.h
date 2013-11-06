#ifndef QFEXTENSIONCOBOLTLASER_H
#define QFEXTENSIONCOBOLTLASER_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include <QVector>
#include <QList>
#include "qfcoboltlaserprotocolhandler.h"
#include "../base_classes/qf3comportmanager.h"
#include "../../../../../LIB/trunk/jkserialconnection.h"

#include "qfextensionlightsource.h"

/*!
    \defgroup qf3ext_lights_cobolt QFExtensionLightSource implementation to control Cobolt Lasers (MLD+DPSS, series 3-5)
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensionLightSource implementation to control Cobolt Lasers (MLD+DPSS, series 3-5)
    \ingroup qf3ext_lights_cobolt
 */
class QFExtensionCoboltLaser : public QObject, public QFExtensionBase, public QFExtensionLightSource {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionLightSource)
    public:
        /** Default constructor */
        QFExtensionCoboltLaser(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionCoboltLaser();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("lights_coboltlaser"); }
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Cobolt Laser"); }
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("control Cobolt DPSS and MLD lasers (series 3-5)"); }
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); }
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2013 by Jan W. Krieger"); }
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/lights_coboltlaser/lights_coboltlaser.png"); }
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();

        /////////////////////////////////////////////////////////////////////////////
        // QFExtensionLightSource routines
        /////////////////////////////////////////////////////////////////////////////
        /*! \copydoc QFExtensionLightSource::getLightSourceCount() */
        virtual unsigned int getLightSourceCount()  ;
        /*! \copydoc QFExtensionLightSource::lightSourceConnect() */
        virtual void lightSourceConnect(unsigned int lightSource);
        /*! \copydoc QFExtensionLightSource::lightSourceDisonnect() */
        virtual void lightSourceDisonnect(unsigned int lightSource);
        /*! \copydoc QFExtensionLightSource::setLightSourceLogging() */
        virtual void setLightSourceLogging(QFPluginLogService* logService);
        /*! \copydoc QFExtensionLightSource::isLightSourceConnected() */
        virtual bool isLightSourceConnected(unsigned int lightSource) ;
        /*! \copydoc QFExtensionLightSource::getLightSourceLineCount() */
        virtual unsigned int getLightSourceLineCount(unsigned int lightSource) ;
        /*! \copydoc QFExtensionLightSource::getLightSourceLineDescription() */
        virtual QString getLightSourceLineDescription(unsigned int lightSource, unsigned int wavelengthLine) ;
        /*! \copydoc QFExtensionLightSource::getLightSourceLinePowerRange() */
        virtual void getLightSourceLinePowerRange(unsigned int lightSource, unsigned int wavelengthLine, double& minimum, double& maximum) ;
        /*! \copydoc QFExtensionLightSource::getLightSourceLinePowerUnit() */
        virtual QString getLightSourceLinePowerUnit(unsigned int lightSource, unsigned int wavelengthLine) ;
        /*! \copydoc QFExtensionLightSource::setLightSourcePower() */
        virtual void setLightSourcePower(unsigned int lightSource, unsigned int wavelengthLine, double power) ;
        /*! \copydoc QFExtensionLightSource::getLightSourceCurrentSetPower() */
        virtual double getLightSourceCurrentSetPower(unsigned int lightSource, unsigned int wavelengthLine) ;
        /*! \copydoc QFExtensionLightSource::getLightSourceCurrentMeasuredPower() */
        virtual double getLightSourceCurrentMeasuredPower(unsigned int lightSource, unsigned int wavelengthLine) ;
        /*! \copydoc QFExtensionLightSource::setLightSourceLineEnabled() */
        virtual void setLightSourceLineEnabled(unsigned int lightSource, unsigned int wavelengthLine, bool enabled) ;
        /*! \copydoc QFExtensionLightSource::getLightSourceLineEnabled() */
        virtual bool getLightSourceLineEnabled(unsigned int lightSource, unsigned int wavelengthLine) ;
        /*! \copydoc QFExtensionLightSource::isLastLightSourceActionFinished() */
        virtual bool isLastLightSourceActionFinished(unsigned int lightSource);
        /*! \copydoc QFExtensionLightSource::getLightSourceDescription() */
        virtual QString getLightSourceDescription(unsigned int lightSource) ;
        /*! \copydoc QFExtensionLightSource::getLightSourceShortName() */
        virtual QString getLightSourceShortName(unsigned int lightSource) ;
        /*! \copydoc QFExtensionLightSource::showLightSourceSettingsDialog() */
        virtual void showLightSourceSettingsDialog(unsigned int lightSource, QWidget* parent=NULL);

    protected:
        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);
        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();
        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);
        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings);



        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);

	protected:
        QFPluginLogService* logService;

        QF3ComPortManager ports;

        enum CoboltLaserTypes {
            cltDPSS4=0,
            cltDPSS5=1,
            cltDPSS6=2,
            cltMLD6=3,


            cltGeneric=cltDPSS4
        };
        static QString coboltLaserType2HumanReadableString(CoboltLaserTypes type);
        static QString coboltLaserType2String(CoboltLaserTypes type);
        static CoboltLaserTypes string2CoboltLaserType(const QString& type);
        static double decodeWavelengthAndLasername(const QString& wl);

        struct LIGHTSOURCE {
            QFCoboltLaserProtocolHandler* serial;
            int port;
            QTime lastAction;
            double setPower;
            QString label;
            double wavelength;
            double max_power;
            /** \brief any power given to setLightSourcePower() is multiplied by this factor before beeing sent to the controller (usually 0.001 to go from mW to W) */
            double powerFactor_ParamToSend;
            double powerFactor_ParamRead;
            CoboltLaserTypes type;
            QString serialNumber;
            bool line_enabled;

        };

        QList<LIGHTSOURCE> sources;

};

#endif // QFEXTENSIONCOBOLTLASER_H
