#ifndef QFRDRIMAGEMASKEDITTOOLS_H
#define QFRDRIMAGEMASKEDITTOOLS_H

#include <QPointer>
#include <QObject>
#include <QMenu>
#include <QAction>
#include <QSet>
#include <QList>
#include "qfrawdatarecord.h"
#include "qfrdrrunselection.h"
#include "qfrdrfcsdatainterface.h"
#include "qfrdrimagemask.h"
#include <QStack>
#include "jkqtplotter.h"
#include <QToolBar>

class QFRDRImageMaskEditTools: public QObject {
        Q_OBJECT
    public:
        QFRDRImageMaskEditTools(QWidget *parentWidget = 0, const QString& settingsPrefix="");


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

        void registerPlotter(JKQtPlotter* plot);
        void unregisterPlotter(JKQtPlotter* plot);
        virtual void registerPlotterMaskToolsToToolbar(QToolBar *menu) const;
        void setUseDelay(bool use);

    public slots:
        void loadMask();
        void pasteMask();
        void saveMask();
        void copyMask();
        void clearMask();
        void invertMask();
        void undoMask();
        void redoMask();
        void maskBorder();
        void copyMaskToGroup();
    signals:
        void rawDataChanged();

    protected slots:
        void imageClicked(double x, double y, Qt::KeyboardModifiers modifiers);
        void imageScribbled(double x, double y, Qt::KeyboardModifiers modifiers, bool first, bool last);
        void imageRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers);
        void imageLineFinished(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers);
        void imageCircleFinished(double x, double y, double radius, Qt::KeyboardModifiers modifiers);
        void imageEllipseFinished(double x, double y, double radiusX, double radiusY, Qt::KeyboardModifiers modifiers);
        void setLeaveout(QList<int> leaveout, bool clearOld=false);

        void signalMaskChanged(bool delayed, bool updateUndoRedo=true);
        void updateUndoActions();
        void updateAfterClick();
        void setImageEditMode();

    protected:
        QWidget* parentWidget;
        QPointer<QFRawDataRecord> rdr;
        QAction* actCopyMask;
        QAction* actPasteMask;
        QAction* actLoadMask;
        QAction* actSaveMask;
        QAction* actClearMask;
        QAction* actInvertMask;
        QAction* actMaskBorder;
        QAction* actCopyMaskToGroup;

        QAction* actUndoMask;
        QAction* actRedoMask;

        QAction* actImagesZoom;
        QAction* actImagesDrawPoints;
        QAction* actImagesDrawRectangle;
        QAction* actImagesDrawCircle;
        QAction* actImagesDrawEllipse;
        QAction* actImagesDrawLine;
        QAction* actImagesScribble;
        QActionGroup* agImageSelectionActions;


        QList<QPointer<JKQtPlotter> > plotters;

        QFRDRImageMaskInterface* imagemask;
        QFRDRRunSelectionsInterface* runselection;

        QStringList undos;
        int undoPos;
        QString settingsPrefix;
        QTimer* timUpdateAfterClick;
        bool useDelay;

};

#endif // QFRDRIMAGEMASKEDITTOOLS_H
