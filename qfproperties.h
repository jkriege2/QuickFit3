#ifndef QFPROPERTIES_H
#define QFPROPERTIES_H

#include <QObject>
#include <QDomElement>
#include <QXmlStreamWriter>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QStringList>

/*! \brief implements a property system, together with storage to/read from XML files

*/
class QFProperties {
    public:
        /** Default constructor */
        QFProperties();
        /** Default destructor */
        virtual ~QFProperties();

        /** \brief clear all properties */
        inline void clearProperties() { props.clear(); emitPropertiesChanged(); }
        /** \brief return the value of the specified property */
        inline QVariant getProperty( const QString& p) { return props[p].data; };
        /** \brief return the value of the specified property or the supplied default value */
        inline QVariant getProperty(const QString& p, const QVariant& defaultValue) {
            if (props.contains(p)) return props.value(p).data;
            return defaultValue;
        };
        /** \brief return the number of properties in the object */
        inline unsigned int getPropertyCount() { return props.size(); };
        /** \brief return the number of visible properties in the object */
        unsigned int getVisiblePropertyCount();
        /** \brief get the name of the i-th visible property */
        QString getVisibleProperty(unsigned int i);
        /** \brief returns a QStringList which contains the names of all properties */
        inline QStringList getPropertyNames() { return props.keys(); };
        /** \brief returns the name of the i-th property */
        inline QString getPropertyName(int i) { return props.keys().at(i); };
        /** \brief returns whether the given property is visible */
        inline bool isPropertyVisible(QString property) {
            if (!props.contains(property)) return false;
            return props[property].visible;
        };
        /** \brief returns whether the given property is user editable */
        inline bool isPropertyUserEditable(QString property) {
            if (!props.contains(property)) return false;
            return props[property].usereditable;
        };
        /** \brief delete the given property */
        inline void deleteProperty(const QString& n) { props.remove(n); emitPropertiesChanged(); };

        /** \brief returns true if the specified property exists */
        inline bool propertyExists(const QString& p) { return props.contains(p); };

/** \brief set property to the specified value */
        inline void setQFProperty(const QString& p, QVariant value, bool usereditable=true, bool visible=true) {
            propertyItem i;
            i.data=value;
            i.usereditable=usereditable;
            i.visible=visible;
            props[p]=i;
            emitPropertiesChanged();
        }
        /** \brief set property to the specified value */
        inline void setDoubleProperty(const QString& p, double value, bool usereditable=true, bool visible=true) {
            setQFProperty(p, QVariant(value), usereditable, visible);
        }
        /** \brief set property to the specified value */
        inline void setBoolProperty(const QString& p, bool value, bool usereditable=true, bool visible=true) {
            setQFProperty(p, QVariant(value), usereditable, visible);
        }
        /** \brief set property to the specified value */
        inline void setIntProperty(const QString& p, qlonglong value, bool usereditable=true, bool visible=true) {
            setQFProperty(p, QVariant(value), usereditable, visible);
        }
        /** \brief set property to the specified value */
        inline void setStringProperty(const QString& p, QString value, bool usereditable=true, bool visible=true) {
            setQFProperty(p, QVariant(value), usereditable, visible);
        }
    protected:
        /** \brief struct to store a property */
        struct propertyItem {
            QVariant data;
            bool usereditable;
            bool visible;
        };
        /** \brief internal store for the objectproperties */
        QMap<QString, propertyItem> props;

        /** \brief called when the project properties (name, description, ...) changed
         *
         *  As there are problems when deriving from two classes with the same base class
         *  (here QObject), this class here may NOT be derived from QObject and therefore
         *  does NOT support the meta-object compiler of Qt. So tro still beeing able to emit
         *  signals, we have to use a method that will then be implemented by the inheriting
         *  class.
         */
        virtual void emitPropertiesChanged() {};
        /**\brief issue an error report/set object into error state
         *
         * This should be reimplemented by the inheriting object, to comply woth its error
         * reporting system!
         */
        virtual void setPropertiesError(QString message) {};

        /*! \brief write the properties to an XML, using the given QXmlStreamWriter object

            This function uses the given XML writer to store all properties in a XML file.
            Note that you have to enter a XML node before calling thius and close it afterwards,
            so you can choose in which subnode (e.g. \c <properties> ) everything is saved.
            Here is some example code for calling it:
            \code
            w.writeStartElement("properties");
            storeProperties(w);
            w.writeEndElement();
            \endcode
        */
        void storeProperties(QXmlStreamWriter& w);

        /*! \brief read properties from a given QDomElement, which were written using storeProperties before */
        void readProperties(QDomElement& e);
};

#endif // QFPROPERTIES_H
