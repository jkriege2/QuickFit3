#include <QtGui>
#include "../lib/tools.h"
#include "numberedit.h"
#include <iostream>


#ifndef QFTOOLWIDGETS_H
#define QFTOOLWIDGETS_H

/** \brief two sliders in a box, aligned left and right
 *  \ingroup quickfit2_widgets
 *
 * The widget looks like this:
 *  \image html datacutsliders.png
 * You can supply a lower and an upper bound which is valid for both integer edit fields (QSpinBoxes).
 * The left edit allows to set the lower bound and the right one the upper bound. So the left box starts at
 * value=min and the right one at value=max. It is always asserted that leftValue<rightValue.
 *
 */
class DataCutSliders : public QWidget
{
    Q_OBJECT

    public:
        /** \brief class constructor */
        DataCutSliders(QWidget* parent=0);

        GetSetMacroI(int, min, update());
        GetSetMacroI(int, max, update());
        GetSetMacroI(int, userMin, update());
        GetSetMacroI(int, userMax, update());
        GetSetMacro(bool, allowCopyToAll);
        GetMacro(bool, sliderSignals);

        /** \brief switches off the slidersChanged() signal. */
        void disableSliderSignals() {
            sliderSignals=false;
        };
        /** \brief switches on the slidersChanged() signal. */
        void enableSliderSignals() {
            sliderSignals=true;
        };

    protected:

        /** \brief displays a context menu, if contextMenu is not \c NULL */
        void contextMenuEvent(QContextMenuEvent *event);

    private slots:
        /** \brief this slot is bound to the sliderLow and is called whenever its value changes. It updates the internal variables and label */
        void sliderLowValueChanged(int value);
        /** \brief this slot is bound to the sliderHigh and is called whenever its value changes. It updates the internal variables and label */
        void sliderHighValueChanged(int value);

        /** \brief this emity copyUserMinToAll() */
        void copyUserMinClicked() { emit copyUserMinToAll(userMin); };

        /** \brief this emity copyUserMaxToAll() */
        void copyUserMaxClicked() { emit copyUserMaxToAll(userMax); };

        /** \brief called when actFitAll was clicked */
        void fitAllClicked() { emit fitAll(); };

        /** \brief called when actFitCurrent was clicked */
        void fitCurrentClicked() { emit fitCurrent(); };

    signals:
        /** \brief this signal is emitted by the DataCutSlider widget, whenever the user moved the sliders or set new values by
        *          the set_userMin(), set_userMax() and get_min() as well as get_max() methods. You can switch off this signal by
        *          using disableSliderSignals() (and enableSliderSignals() to reacivate it).
        */
        void slidersChanged(int userMin, int userMax, int min, int max);

        /** \brief this signal is emited when the user clicks the "copy to all" context menu of the min widget item. */
        void copyUserMinToAll(int userMin);

        /** \brief this signal is emited when the user clicks the "copy to all" context menu of the max widget item. */
        void copyUserMaxToAll(int userMax);

        /** \brief tells the application to fit the current file again */
        void fitCurrent();

        /** \brief tells the application to fit all files */
        void fitAll();

    public slots:
        /** \brief sets all the data and constraints of the widget at once */
        void setValues(int userMin, int userMax, int min, int max);
    private:
        /** \brief update the widgets with the values in the private data members */
        void update();

        /** \brief indicates whether to emit the signalChanged() signal, wen the values change
         *
         * This property is set with enableSliderSignals() and disableSliderSignals() and accessed by
         * get_sliderSignals()
         */
        bool sliderSignals;

        /** \brief QLineEdit which displays and edits the current lower cut-off */
        QSpinBox* editLow;
        /** \brief QLineEdit which displays and edits the current upper cut-off */
        QSpinBox* editHigh;

        /** \brief this signal is emited when the user clicks the "copy min to all" context menu item. */
        QAction* actCopyMin;
        /** \brief this signal is emited when the user clicks the "copy max to all" context menu item. */
        QAction* actCopyMax;
        /** \brief this signal is emited when the user clicks the "copymin+maxto all" context menu item. */
        QAction* actCopyBoth;

        /** \brief "fit current file" action inside contextMenu */
        QAction* actFitCurrent;

        /** \brief "fit all files" action inside contextMenu */
        QAction* actFitAll;

        /** \brief if set \c true this widget may emit a copyToAll() signal */
        bool allowCopyToAll;

        /** \brief context menu for the widget */
        QMenu* contextMenu;

        /** \brief overall minimum value */
        int min;
        /** \brief overall maximum value */
        int max;
        /** \brief user set minimum value */
        int userMin;
        /** \brief user set maximum value */
        int userMax;
};


/** \brief this struct represents one shortcut used by ShortcutQSPinBox, ShortcutQDoubleSpinBox, ShortcutQComboBox, ...
 *  \ingroup quickfit2_widgets
 */
class myShortcut {
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
 *  \ingroup quickfit2_widgets
 */
class ShortcutQSpinBox: public QSpinBox {
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
 *  \ingroup quickfit2_widgets
 */
class ShortcutQDoubleSpinBox: public QDoubleSpinBox {
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
 *  \ingroup quickfit2_widgets
 */
class ShortcutQLineEdit: public QLineEdit {
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
 *  \ingroup quickfit2_widgets
 */
class ShortcutDoubleEdit: public DoubleEdit {
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
        ShortcutDoubleEdit(QWidget* parent=NULL): DoubleEdit(parent) { clearShortcuts(); };

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
 *  \ingroup quickfit2_widgets
 */
class ShortcutQComboBox: public QComboBox {
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



#endif // QUICKFITTOOLWIDGETS_H

