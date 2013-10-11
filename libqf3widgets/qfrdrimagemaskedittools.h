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
#include <QWidgetAction>
#include <QToolBar>
#include "libwid_imexport.h"


class QFWIDLIB_EXPORT QFRDRImageMaskEditTools: public QObject {
        Q_OBJECT
    public:
        QFRDRImageMaskEditTools(QWidget *parentWidget = 0, const QString& settingsPrefix="");


        virtual void setRDR(QFRawDataRecord* rdr);
        virtual void registerMaskToolsToMenu(QMenu* menu) const;
        virtual void registerMaskToolsToToolbar(QToolBar* menu) const;

        QAction* get_actCopyMask() const { return actCopyMask; }
        QAction* get_actPasteMask() const { return actPasteMask; }
        QAction* get_actLoadMask() const { return actLoadMask; }
        QAction* get_actSaveMask() const { return actSaveMask; }
        QAction* get_actClearMask() const { return actClearMask; }
        QAction* get_actInvertMask() const { return actInvertMask; }
        QAction* get_actUndoMask() const { return actUndoMask; }
        QAction* get_actRedoMask() const { return actRedoMask; }
        QAction* get_actMaskBorder() const { return actMaskBorder; }
        QAction* get_actMaskSelected() const { return actMaskSelected; }
        QAction* get_actUnmaskSelected() const { return actUnmaskSelected; }

        void registerPlotter(JKQtPlotter* plot);
        void unregisterPlotter(JKQtPlotter* plot);
        virtual void registerPlotterMaskToolsToToolbar(QToolBar *menu) const;
        void setUseDelay(bool use);

        void setAllowEditSelection(bool enabled, bool* selectionArray=NULL, int width=0, int height=0);
        bool getAllowEditSelection() const;
        void setMaskEditing(bool enabled);
        bool getMaskEditing() const;
        void copyMask(bool* data, int maskSize);
        void setMask(bool* data, int maskSize);
        void setEditingMode(int idx);
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
        void maskSelected();
        void unmaskSelected();

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

        void cmbModeChanged(int index);

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
        QAction* actMaskSelected;
        QAction* actUnmaskSelected;
        QAction* actImagesZoom;
        QAction* actImagesDrawPoints;
        QAction* actImagesDrawRectangle;
        QAction* actImagesDrawCircle;
        QAction* actImagesDrawEllipse;
        QAction* actImagesDrawLine;
        QAction* actImagesScribble;
        QActionGroup* agImageSelectionActions;
        QComboBox* cmbMode;
        QWidgetAction* actMode;

        QMenu* menuSpecials;


        QList<QPointer<JKQtPlotter> > plotters;

        QFRDRImageMaskInterface* imagemask;
        QFRDRRunSelectionsInterface* runselection;

        QStringList undos;
        int undoPos;
        QString settingsPrefix;
        QTimer* timUpdateAfterClick;
        bool useDelay;

        bool* selection;
        int selectionWidth;
        int selectionHeight;
        bool selectionEditing;
        bool maskEditing;

};

#endif // QFRDRIMAGEMASKEDITTOOLS_H
