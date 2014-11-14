/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFPROPERTIES_H
#define QFPROPERTIES_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QVariant>
#include <QStringList>
#include "qftools.h"
#include "lib_imexport.h"

class QDomElement; // forward
class QXmlStreamWriter;
class QFPropertiesPrivate;

/*! \brief implements a property system, together with storage to/read from XML files
    \ingroup qf3lib_project

*/
class QFLIB_EXPORT QFProperties {
    public:
        /** Default constructor */
        QFProperties();
        /** Default destructor */
        virtual ~QFProperties();

        /** \brief clear all properties */
        void clearProperties() ;
        /** \brief return the value of the specified property */
        QVariant getProperty( const QString& p) const ;
        /** \brief return the value of the specified property or the supplied default value */
        QVariant getProperty(const QString& p, const QVariant& defaultValue) const;
        /** \brief return the value of the specified property */
        inline QVariant getQFProperty( const QString& p) const {
            return getProperty(p);
        }
        /** \brief return the value of the specified property or the supplied default value */
        inline QVariant getQFProperty(const QString& p, const QVariant& defaultValue) const {
            return getProperty(p, defaultValue);
        }
        /** \brief return the number of properties in the object */
        unsigned int getPropertyCount() const;
        /** \brief return the number of visible properties in the object */
        unsigned int getVisiblePropertyCount() const;
        /** \brief returna list of all visible properties */
        QStringList getVisibleProperties() const;
        /** \brief get the name of the i-th visible property */
        QString getVisibleProperty(unsigned int i) const;
        /** \brief returns a QStringList which contains the names of all properties */
        QStringList getPropertyNames() const ;
        /** \brief returns the name of the i-th property */
        QString getPropertyName(int i) const ;
        /** \brief returns whether the given property is visible */
        bool isPropertyVisible(const QString& property) const;
        /** \brief returns whether the given property is user editable */
        bool isPropertyUserEditable(const QString& property) const ;
        /** \brief delete the given property */
        void deleteProperty(const QString& n);

        /** \brief returns true if the specified property exists */
        bool propertyExists(const QString& p) const;

        /** \brief set property to the specified value */
        void setQFProperty(const QString& p, QVariant value, bool usereditable=true, bool visible=true);
        /** \brief set property to the specified value, if it is not user editable or does not exist */
        void setQFPropertyIfNotUserEditable(const QString& p, QVariant value, bool usereditable=true, bool visible=true);
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
        inline void setStringProperty(const QString& p, const QString& value, bool usereditable=true, bool visible=true) {
            setQFProperty(p, QVariant(value), usereditable, visible);
        }

        /** \brief add all properties from another QFProperties object to this object */
        void addProperties(const QFProperties& other);

        void propReadLock() const;
        void propWriteLock() const;
        void propReadUnLock() const;
        void propWriteUnLock() const;
    protected:
        /** \brief struct to store a property */
        struct propertyItem {
            QVariant data;
            bool usereditable;
            bool visible;
        };
        /** \brief internal store for the objectproperties */
        QHash<QString, propertyItem> props;
        QFPropertiesPrivate* p;


        /** \brief called when the project properties (name, description, ...) changed
         *
         *  As there are problems when deriving from two classes with the same base class
         *  (here QObject), this class here may NOT be derived from QObject and therefore
         *  does NOT support the meta-object compiler of Qt. So tro still beeing able to emit
         *  signals, we have to use a method that will then be implemented by the inheriting
         *  class.
         */
        virtual void emitPropertiesChanged(const QString& property=QString(""), bool visible=true ) {};
        /** \brief issue an error report/set object into error state
         *
         * This should be reimplemented by the inheriting object, to comply woth its error
         * reporting system!
         */
        virtual void setPropertiesError(const QString& message) {};

        /*! \brief write the properties to an XML, using the given QXmlStreamWriter object

            This function uses the given XML writer to store all properties in a XML file.
            Note that you have to enter a XML node before calling thius and close it afterwards,
            so you can choose in which subnode (e.g. \verbatim <properties>\endverbatim ) everything is saved.
            Here is some example code for calling it:
            \code
            w.writeStartElement("properties");
            storeProperties(w);
            w.writeEndElement();
            \endcode
        */
        void storeProperties(QXmlStreamWriter& w) const;

        /*! \brief read properties from a given QDomElement, which were written using storeProperties before */
        void readProperties(QDomElement& e);
};


DEFINE_READWRITE_LOCKERS(QFProperties, propReadLock, propWriteLock, propReadUnLock, propWriteUnLock)

#endif // QFPROPERTIES_H
