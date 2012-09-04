#ifndef QFRDRTABLE_H
#define QFRDRTABLE_H

#include "qftablepluginmodel.h"

#include "qfrawdatarecord.h"
#include "qfrawdatarecordfactory.h"
#include "jkqtplotter.h"
#include "jkqtptools.h"
#include "qftools.h"


/*! \brief this class is used to manage a table of values (strings/numbers)
    \ingroup qf3rdrdp_table

    The data is stored in a QFTablePluginModel object which is also externally accessible for data access.
 */

class QFRDRTable : public QFRawDataRecord {
        Q_OBJECT
    public:
        enum {
            TableExpressionRole = Qt::UserRole+1
        };


        enum GraphType {
            gtLines,
            gtImpulsesVertical,
            gtImpulsesHorizontal,
            gtFilledCurveX,
            gtFilledCurveY,
            gtStepsHorizontal,
            gtStepsVertical,
            gtbarsHorizontal,
            gtbarsVertical,
            gtImage,
            gtMaskImage
        };

        static QString GraphType2String(GraphType type) {
            switch(type) {
                case gtLines: return QString("lines");
                case gtImpulsesVertical: return QString("impulsesv");
                case gtImpulsesHorizontal: return QString("impulsesh");
                case gtFilledCurveX: return QString("filledx");
                case gtFilledCurveY: return QString("filledy");
                case gtStepsHorizontal: return QString("stepsh");
                case gtStepsVertical: return QString("stepsv");
                case gtbarsHorizontal: return QString("barsh");
                case gtbarsVertical: return QString("barsv");
                case gtImage: return QString("image");
                case gtMaskImage: return QString("maskimage");
            }
            return QString("");
        }

        static QIcon GraphType2Icon(GraphType type) {
            switch(type) {
                case gtLines: return QIcon(":/table/icons/plot_pointslines.png");
                case gtImpulsesVertical: return QIcon(":/table/icons/plot_vimpulses.png");
                case gtImpulsesHorizontal: return QIcon(":/table/icons/plot_himpulses.png");
                case gtFilledCurveX: return QIcon(":/table/icons/plot_xfilledcurve.png");
                case gtFilledCurveY: return QIcon(":/table/icons/plot_yfilledcurve.png");
                case gtStepsHorizontal: return QIcon(":/table/icons/plot_hsteps.png");
                case gtStepsVertical: return QIcon(":/table/icons/plot_vsteps.png");
                case gtbarsHorizontal: return QIcon(":/table/icons/plot_hbars.png");
                case gtbarsVertical: return QIcon(":/table/icons/plot_vbars.png");
                case gtImage: return QIcon(":/table/icons/plot_image.png");
                case gtMaskImage: return QIcon(":/table/icons/plot_maskimage.png");
            }
            return QIcon();
        }

        static GraphType String2GraphType(QString type) {
            QString s=type.trimmed().toLower();
            if (s=="lines") return gtLines;
            if (s=="impulsesv") return gtImpulsesVertical;
            if (s=="impulsesh") return gtImpulsesHorizontal;
            if (s=="filledx") return gtFilledCurveX;
            if (s=="filledy") return gtFilledCurveY;
            if (s=="stepsh") return gtStepsHorizontal;
            if (s=="stepsv") return gtStepsVertical;
            if (s=="barsh") return gtbarsHorizontal;
            if (s=="barsv") return gtbarsVertical;
            if (s=="image") return gtImage;
            if (s=="maskimage") return gtMaskImage;
            return gtLines;
        }

        struct GraphInfo {
            GraphInfo();
            GraphType type;
            QString title;
            int xcolumn;
            int ycolumn;
            int xerrorcolumn;
            int yerrorcolumn;
            Qt::PenStyle style;
            QColor color;
            double colorTransparent;
            QColor errorColor;
            QColor fillColor;
            double errorColorTransparent;
            double fillColorTransparent;
            double linewidth;
            JKQTPgraphSymbols symbol;
            double symbolSize;
            JKQTPerrorPlotstyle errorStyle;
            bool drawLine;

            QColor imageTrueColor;
            double imageTrueTransparent;
            QColor imageFalseColor;
            double imageFalseTransparent;
            int imagePixelWidth;
            double imageX;
            double imageY;
            double imageWidth;
            double imageHeight;
            JKQTPMathImage::ColorPalette imagePalette;
            double imageMin;
            double imageMax;
            bool imageAutoRange;
            QString imageLegend;
            bool imageColorbarRight;
            bool imageColorbarTop;
            int colorbarWidth;
            double colorbarRelativeHeight;

        };

        struct PlotInfo {
            PlotInfo();
            QString title;
            QString xlabel;
            QString ylabel;
            double xmin, xmax, ymin, ymax;
            bool xlog, ylog;
            bool grid;
            bool showKey;

            bool keepAxisAspectRatio;
            bool keepDataAspectRatio;
            double dataAspectRatio;
            double axisAspectRatio;

            /*bool showKeyBorder;
            double keyBorderWidth;
            QColor keyBorderColor;
            QColor keyBackgroundColor;
            QColor backgroundColor;
            QColor gridColor;
            Qt::PenStyle gridStyle;
            double gridWidth;*/

            QColor axisColor;
            double axisWidth;
            QString fontName;
            double keyFontSize;
            double labelFontSize;
            double axisFontSize;
            double axisLabelFontSize;
            double keyTransparency;
            JKQTPkeyPosition keyPosition;
            JKQTPkeyLayout keyLayout;
            QList<GraphInfo> graphs;
        };


        /** \brief class constructor, initialises as specified, the ID is obtained from the parent project */
        QFRDRTable(QFProject* parent);

        /** Default destructor */
        virtual ~QFRDRTable();

        /** \brief returns the used datamodel */
        QFTablePluginModel* model() ;
        /** \brief returns the table contents at the given position */
        QVariant getModelData(quint16 row, quint16 column);

        int getPlotCount() const;
        PlotInfo getPlot(int i) const;
        void addPlot();
        void addPlot(PlotInfo plotInfo);
        void setPlot(int i, PlotInfo plotInfo);
        void deletePlot(int i);


        /** \brief return type (short type string) */
        virtual QString getType() const { return "table"; };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("Data Table"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/table/projecttree_tablefile.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("data table"); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/table/projecttree_tablefile.png"); };
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount();
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i);
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(QFPluginServices* services,  QFRawDataPropertyEditor *propEditor, int i=0, QWidget* parent=NULL);
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename)const ;
        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportFiletypes() */
        QStringList getExportFiletypes() {
            QStringList sl;
            sl << "CSV" << "SSV" << "GERMANEXCEL" << "SYLK";
            return sl;
        };
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle() { return tr("Export Data Table ..."); };
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes() { return tr("Comma Separated Value Files (*.csv, *.txt);;Semicolon Separated Value Files (*.csv, *.txt);;Semicolon Separated Value Files [german Excel] (*.csv, *.txt);;SYLK File (*.sylk, *.slk)"); };
    protected slots:
        void tdataChanged( const QModelIndex & tl, const QModelIndex & br ) {
            emit rawDataChanged();
        }
        void trawDataChanged() {
            emit rawDataChanged();
        }

    protected:
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL);

        /** \brief stores a table of QVariants */
        QFTablePluginModel* datamodel;
        QList<PlotInfo> plots;
    private:
};

#endif // QFRDRTABLE_H
