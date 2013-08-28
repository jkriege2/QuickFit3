#ifndef QFRDRRUNSELECTIONEDITTOOLS_H
#define QFRDRRUNSELECTIONEDITTOOLS_H

#include <QObject>
#include "qfrdrrunselection.h"
#include "qfrdrrunselectiontools.h"
#include <QAction>
#include <QWidget>
#include <QPointer>
#include "qfrawdatarecord.h"


class QFRDRRunSelectionEditTools: public QObject {
        Q_OBJECT
    public:
        QFRDRRunSelectionEditTools(QWidget *parentWidget = 0, const QString& settingsPrefix="");


        virtual void setRDR(QFRawDataRecord* rdr);
        virtual void registerMaskToolsToMenu(QMenu* menu) const;

        QAction* get_actCopyMask() const { return actCopyMask; }
        QAction* get_actPasteMask() const { return actPasteMask; }
        QAction* get_actLoadMask() const { return actLoadMask; }
        QAction* get_actSaveMask() const { return actSaveMask; }
        QAction* get_actClearMask() const { return actClearMask; }
        QAction* get_actInvertMask() const { return actInvertMask; }
        QAction* get_actUndoMask() const { return actUndoMask; }
        QAction* get_actRedoMask() const { return actRedoMask; }
        QAction* get_actMaskBorder() const { return actMaskBorder; }


    public slots:
        void loadMask();
        void pasteMask();
        void saveMask();
        void copyMask();
        void clearMask();
        void invertMask();
        void undoMask();
        void redoMask();
        void copyMaskToGroup();
    signals:
        void rawDataChanged();

    protected slots:
        void setLeaveout(QList<int> leaveout, bool clearOld=false);

        void signalMaskChanged(bool delayed, bool updateUndoRedo=true);
        void updateUndoActions();
    protected:
        QWidget* parentWidget;
        QPointer<QFRawDataRecord> rdr;
        QAction* actCopyMask;
        QAction* actPasteMask;
        QAction* actLoadMask;
        QAction* actSaveMask;
        QAction* actClearMask;
        QAction* actInvertMask;
        QAction* actCopyMaskToGroup;

        QAction* actUndoMask;
        QAction* actRedoMask;



        QFRDRRunSelectionsInterface* runselection;

        QStringList undos;
        int undoPos;
        QString settingsPrefix;

};

#endif // QFRDRRUNSELECTIONEDITTOOLS_H
