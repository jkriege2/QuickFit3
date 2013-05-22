#ifndef QFRDRIMAGESTACKDATA_H
#define QFRDRIMAGESTACKDATA_H

#include <QVariant>
#include <QList>
#include <QObject>
#include <QMap>
#include <QProgressBar>
#include <QStatusBar>
#include <QWidget>
#include <QMenu>
#include <QPair>
#include <QList>
#include <QAction>
#include "qfrawdatarecord.h"
#include "qfrdrimagestack_dataeditor.h"
#include "qfrawdatarecordfactory.h"
#include "qfrdrimagestackinterface.h"
#include "qfimporter.h"
#include "qfimporterimageseries.h"
#include "qfimportermanager.h"
#include "tools.h"
#include "qfrdrimagemasktools.h"

/*! \brief QFRawDataRecord implementation that manages a set of 4D image stacks (implemeting QFRDRImageStackInterface)
    \ingroup qf3rdrdp_image_stack

*/
class QFRDRImageStackData : public QFRawDataRecord, public QFRDRImageStackInterface, public QFRDRImageMaskTools {
        Q_OBJECT
        Q_INTERFACES(QFRDRImageStackInterface QFRDRImageMaskInterface)
    public:


        /** Default constructor */
        QFRDRImageStackData(QFProject* parent);
        /** Default destructor */
        virtual ~QFRDRImageStackData();

        /** \brief return type (short type string) */
        virtual QString getType() const { return "image_stack"; }
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("image stack"); }
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/image_stack/qfrdrimagestack_small.png"); }
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr(""); }
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/image_stack/qfrdrimagestack.png"); }
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount() { return 1; }
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i) {
            if (i==0) return tr("image stack editor");
            return QString("");
        }
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(QFPluginServices* services,  QFRawDataPropertyEditor *propEditor, int i=0, QWidget* parent=NULL) {
            if (i==0) return new QFRDRImageStackDataEditor(services, propEditor, parent);
            return NULL;
        };
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename)const ;
        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportFiletypes() */
        QStringList getExportFiletypes() {
            QStringList sl;
            return sl;
        };
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle() { return tr(""); }
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes() { return tr(""); }
















        /** \copydoc QFRDRImageStackInterface::getImageStackCount() */
        virtual int getImageStackCount() const;
        /** \copydoc QFRDRImageStackInterface::getImageStackFrames() */
        virtual uint32_t getImageStackFrames(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackWidth() */
        virtual int getImageStackWidth(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackHeight() */
        virtual int getImageStackHeight(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackChannels() */
        virtual int getImageStackChannels(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStack() */
        virtual double* getImageStack(int stackID, uint32_t frame=0, uint32_t channel=0) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackXUnitFactor() */
        virtual double getImageStackXUnitFactor(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackXUnitName() */
        virtual QString getImageStackXUnitName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackXName() */
        virtual QString getImageStackXName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackYUnitFactor() */
        virtual double getImageStackYUnitFactor(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackYUnitName() */
        virtual QString getImageStackYUnitName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackYName() */
        virtual QString getImageStackYName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackTUnitFactor() */
        virtual double getImageStackTUnitFactor(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackTUnitName() */
        virtual QString getImageStackTUnitName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackTName() */
        virtual QString getImageStackTName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackCUnitFactor() */
        virtual double getImageStackCUnitFactor(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackCUnitName() */
        virtual QString getImageStackCUnitName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackCName() */
        virtual QString getImageStackCName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackDescription() */
        virtual QString getImageStackDescription(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackChannelName() */
        virtual QString getImageStackChannelName(int stack, int channel) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackTimepointName() */
        virtual QString getImageStackTimepointName(int stack, int t) const;

        static QStringList getImageFilterList(QFPluginServices *pluginservices) ;
        static QStringList getImageFormatNameList(QFPluginServices *pluginservices) ;
        static QFImporterImageSeries *getImageReader(int idx, QFPluginServices *pluginservices) ;
        static int getImageReaderCount(QFPluginServices *pluginservices);
        static QStringList getImageReaderIDList(QFPluginServices *pluginservices);

        /*! \brief returns the property \a prop with an optionally added number \a stack

            This function returns \a prop with the appended number \a stack (if the number is >=0). If this property does not exists,
            it returns the property \a prop (without an added number) and if also this does not exist, it returns the given
            \a defaultValue.
         */
        QVariant getEnumeratedProperty(const QString& prop, int stack, QVariant defaultValue=QVariant()) const;
        /*! \brief returns the property \a prop with an optionally added number \a stack and a second index \a channel

            This function works like getEnumeratedProperty(), but the property is double-enumerated with \a stack and \a channel.
            It checkes these variants of the property (in the given order):
              - \c PROP<stack>_<channel>
              - \c PROP_<channel>
              - \c PROP<stack>
              - \c PROP
            .
            An index (\a stack or \a channel) is only used, if its value is >=0.
         */
        QVariant getDoubleEnumeratedProperty(const QString &prop, int stack, int channel, QVariant defaultValue) const;

    protected:
        /** \brief write the contents of the object to a XML file */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL);



        double memsize;

        enum loadMode {
            lmGetSize,
            lmReadData
        };
        enum DualViewMode {
            dvNone=0,
            dvHorizontal=1,
            dvVertical=2
        };
        struct ImageStack {
            ImageStack();
            double* data;
            uint32_t frames;
            int width;
            int height;
            int channels;
            int file;
            DualViewMode dvMode;
        };

        /*! \brief load imagestack from file

            The data is stored in the given ImageStack \a stack. If \a mode is lmGetSize, only the image stack sizes are stored in
            stack, if \a mode is lmReadData the image stack is read from the file.
         */
        bool loadImageFile(ImageStack &stack, QString filename, loadMode mode, int channel=0, QFImageHalf whichHalfToLoad=qfihAny);
        QList<ImageStack> stacks;
        /** \brief allocate memory for all entries in stacks */
        bool allocateMemory();
        /** \brief clear all memory allocated in stacks, does NOT clear stacks!!! */
        void clearMemory();

        void loadImageStacks(const QString& stacktype, const QString& maskS=QString(""));

};


#endif // QFRDRIMAGESTACKDATA_H
