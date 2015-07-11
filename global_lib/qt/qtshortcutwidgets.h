#ifndef QTSHORTCUTWIDGETS_H_INCLUDED
#define QTSHORTCUTWIDGETS_H_INCLUDED

#include <QtGui>
#include "numberedit.h"
#include "../lib_imexport.h"


/** \brief this struct represents one shortcut used by ShortcutQSPinBox, ShortcutQDoubleSpinBox, ShortcutQComboBox, ...
 *  \ingroup tools_qt
 */
class LIB_EXPORT myShortcut {
    private:
        /** \brief returns the Qt key code of the key described by \a part */
        int partToKey(QString part);
    public:
        /** \brief the key that has to be pressed */
        int key;
        /** \brief possible modifiers */
        Qt::KeyboardModifiers modifiers;
        /** \brief the shortcut as string, e.g. ALT+0, ... */
        QString ID;

        /** \brief class constructor: init the struct from the given \a keys (e.g. Enter, Alt+0, ...).
         *         The internal ID is set to \a id or \a keys if \a id is empty */
        myShortcut(QString keys, QString id=QString(""));

        /** \brief compare the struct with a QKeyEvent */
        bool matches(QKeyEvent* event);
} ;




/** \brief this specialized QSpinBox allows to catch keyboard events
 *  \ingroup tools_qt
 */
class LIB_EXPORT ShortcutQSpinBox: public QSpinBox {
        Q_OBJECT
    protected:
        /** \brief reimplementation that catches some of the keyboard shortcuts */
        virtual void keyPressEvent (QKeyEvent* event);

        /** \brief list that stores the registered shortcuts */
        QList<myShortcut> shortcuts;
    signals:
        /** \brief emitted when one of the shortcuts is found */
        void keyPressed(QString keyID);
    public:
        /** \brief class constructor */
        ShortcutQSpinBox(QWidget* parent=NULL): QSpinBox(parent) { clearShortcuts(); };

        /** \brief register a new shortcut */
        inline void addShortcut(QString keys, QString id=QString("")) {
            myShortcut sc=myShortcut(keys, id);
            shortcuts.append(sc);
        };

        /** \brief clear all registered shortcuts */
        inline void clearShortcuts() {
            shortcuts.clear();
        };

};





/** \brief this specialized QDoubleSpinBox allows to catch keyboard events
 *  \ingroup tools_qt
 */
class LIB_EXPORT ShortcutQDoubleSpinBox: public QDoubleSpinBox {
        Q_OBJECT
    protected:
        /** \brief reimplementation that catches some of the keyboard shortcuts */
        virtual void keyPressEvent (QKeyEvent* event);

        /** \brief list that stores the registered shortcuts */
        QList<myShortcut> shortcuts;
    signals:
        /** \brief emitted when one of the shortcuts is found */
        void keyPressed(QString keyID);
    public:
        /** \brief class constructor */
        ShortcutQDoubleSpinBox(QWidget* parent=NULL): QDoubleSpinBox(parent) { clearShortcuts(); };

        /** \brief register a new shortcut */
        inline void addShortcut(QString keys, QString id=QString("")) {
            myShortcut sc=myShortcut(keys, id);
            shortcuts.append(sc);
        };

        /** \brief clear all registered shortcuts */
        inline void clearShortcuts() {
            shortcuts.clear();
        };

};





/** \brief this specialized QLineEdit allows to catch keyboard events
 *  \ingroup tools_qt
 */
class LIB_EXPORT ShortcutQLineEdit: public QLineEdit {
        Q_OBJECT
    protected:
        /** \brief reimplementation that catches some of the keyboard shortcuts */
        virtual void keyPressEvent (QKeyEvent* event);

        /** \brief list that stores the registered shortcuts */
        QList<myShortcut> shortcuts;
    signals:
        /** \brief emitted when one of the shortcuts is found */
        void keyPressed(QString keyID);
    public:
        /** \brief class constructor */
        ShortcutQLineEdit(QWidget* parent=NULL): QLineEdit(parent) { clearShortcuts(); };

        /** \brief register a new shortcut */
        inline void addShortcut(QString keys, QString id=QString("")) {
            myShortcut sc=myShortcut(keys, id);
            shortcuts.append(sc);
        };

        /** \brief clear all registered shortcuts */
        inline void clearShortcuts() {
            shortcuts.clear();
        };

};





/** \brief this specialized DoubleEdit allows to catch keyboard events
 *  \ingroup tools_qt
 */
class LIB_EXPORT ShortcutNumberEdit: public NumberEdit {
        Q_OBJECT
    protected:
        /** \brief reimplementation that catches some of the keyboard shortcuts */
        virtual void keyPressEvent (QKeyEvent* event);

        /** \brief list that stores the registered shortcuts */
        QList<myShortcut> shortcuts;
    signals:
        /** \brief emitted when one of the shortcuts is found */
        void keyPressed(QString keyID);
    public:
        /** \brief class constructor */
        ShortcutNumberEdit(QWidget* parent=NULL):NumberEdit(parent) { clearShortcuts(); };

        /** \brief register a new shortcut */
        inline void addShortcut(QString keys, QString id=QString("")) {
            myShortcut sc=myShortcut(keys, id);
            shortcuts.append(sc);
        };

        /** \brief clear all registered shortcuts */
        inline void clearShortcuts() {
            shortcuts.clear();
        };

};






/** \brief this specialized QComboBox allows to catch keyboard events
 *  \ingroup tools_qt
 */
class LIB_EXPORT ShortcutQComboBox: public QComboBox {
        Q_OBJECT
    protected:
        /** \brief reimplementation that catches some of the keyboard shortcuts */
        virtual void keyPressEvent (QKeyEvent* event);

        /** \brief list that stores the registered shortcuts */
        QList<myShortcut> shortcuts;
    signals:
        /** \brief emitted when one of the shortcuts is found */
        void keyPressed(QString keyID);
    public:
        /** \brief class constructor */
        ShortcutQComboBox(QWidget* parent=NULL): QComboBox(parent) { clearShortcuts(); };

        /** \brief register a new shortcut */
        inline void addShortcut(QString keys, QString id=QString("")) {
            myShortcut sc=myShortcut(keys, id);
            shortcuts.append(sc);
        };

        /** \brief clear all registered shortcuts */
        inline void clearShortcuts() {
            shortcuts.clear();
        };

};

#endif // QTSHORTCUTWIDGETS_H_INCLUDED
