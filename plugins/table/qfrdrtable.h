#ifndef QFRDRTABLE_H
#define QFRDRTABLE_H

#include "qftablepluginmodel.h"

#include "qfrawdatarecord.h"
#include "qfrawdatarecordfactory.h"
#include "qfplotter.h"
#include "jkqtptools.h"
#include "qftools.h"
#include "qfrdrtableinterface.h"
#include "qfrdrcolumngraphsinterface.h"
#include "qfmathparser.h"
#include <QPointer>
#include <QMultiMap>
class QFRDRTableEditor; // forward

/*! \brief this class is used to manage a table of values (strings/numbers)
    \ingroup qf3rdrdp_table

    The data is stored in a QFTablePluginModel object which is also externally accessible for data access.
 */

class QFRDRTable : public QFRawDataRecord, public QFRDRTableInterface, public QFRDRColumnGraphsInterface {
        Q_OBJECT
        Q_INTERFACES(QFRDRTableInterface QFRDRColumnGraphsInterface)
    public:
        enum {
            TableExpressionRole = Qt::UserRole+1,
            TableCommentRole = Qt::UserRole+2,
            ColumnExpressionRole = Qt::UserRole+10,
            ColumnCommentRole = Qt::UserRole+11
        };


        enum GraphType {
            gtLines,
            gtImpulsesVertical,
            gtImpulsesHorizontal,
            gtFilledCurveX,
            gtFilledCurveY,
            gtStepsHorizontal,
            gtStepsVertical,
            gtBarsHorizontal,
            gtBarsVertical,
            gtImage,
            gtRGBImage,
            gtMaskImage,
            gtFunction
        };

        enum GTFunctionType {
            gtfString=0,
            gtfPolynomial=1,
            gtfExponential=2,
            gtfPowerLaw=3,
            gtfQFFunction=4
        };

        static QString GTFunctionType2String(GTFunctionType type) {
            switch(type) {
                case gtfQFFunction: return QString("qffunction");
                case gtfPolynomial: return QString("poly");
                case gtfExponential: return QString("exp");
                case gtfPowerLaw: return QString("pow");
                default:
                case gtfString:
                    return QString("string");
            }

        }
        static GTFunctionType String2GTFunctionType(QString type) {
            if(type=="qffunction") return gtfQFFunction;
            if(type=="poly") return gtfPolynomial;
            if(type=="exp") return gtfExponential;
            if(type=="pow") return gtfPowerLaw;
            return gtfString;
        }

        static QString GraphType2String(GraphType type) {
            switch(type) {
                case gtLines: return QString("lines");
                case gtImpulsesVertical: return QString("impulsesv");
                case gtImpulsesHorizontal: return QString("impulsesh");
                case gtFilledCurveX: return QString("filledx");
                case gtFilledCurveY: return QString("filledy");
                case gtStepsHorizontal: return QString("stepsh");
                case gtStepsVertical: return QString("stepsv");
                case gtBarsHorizontal: return QString("barsh");
                case gtBarsVertical: return QString("barsv");
                case gtImage: return QString("image");
                case gtRGBImage: return QString("rgbimage");
                case gtMaskImage: return QString("maskimage");
                case gtFunction: return QString("function");
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
                case gtBarsHorizontal: return QIcon(":/table/icons/plot_hbars.png");
                case gtBarsVertical: return QIcon(":/table/icons/plot_vbars.png");
                case gtImage: return QIcon(":/table/icons/plot_image.png");
                case gtRGBImage: return QIcon(":/table/icons/plot_rgbimage.png");
                case gtMaskImage: return QIcon(":/table/icons/plot_maskimage.png");
                case gtFunction: return QIcon(":/table/icons/plot_function.png");
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
            if (s=="barsh") return gtBarsHorizontal;
            if (s=="barsv") return gtBarsVertical;
            if (s=="image") return gtImage;
            if (s=="rgbimage") return gtRGBImage;
            if (s=="maskimage") return gtMaskImage;
            if (s=="function") return gtFunction;
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

            int stride;
            int strideStart;
            bool isStrided;

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
            QString imageLegendG;
            QString imageLegendB;
            QString imageLegendMod;
            bool imageColorbarRight;
            bool imageColorbarTop;
            int colorbarWidth;
            double colorbarRelativeHeight;
            JKQTPMathImage::ModifierMode modifierMode;

            QString function;
            GTFunctionType functionType;
            QVector<double> functionParameters;

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
            QColor keyBackgroundColor;*/
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



            QColor backgroundColor;
            QColor gridColor;
            Qt::PenStyle gridStyle;
            double gridWidth;
            JKQTPlabelPosition xlabelPos;
            JKQTPCAlabelType xlabelType;
            JKQTPCAdrawMode xlabelMode1;
            JKQTPCAdrawMode xlabelMode2;
            int xdigits;
            int xminTicks;
            int xminorTicks;
            JKQTPlabelPosition ylabelPos;
            JKQTPCAlabelType ylabelType;
            JKQTPCAdrawMode ylabelMode1;
            JKQTPCAdrawMode ylabelMode2;
            int ydigits;
            int yminTicks;
            int yminorTicks;
            bool x0axis;
            bool y0axis;

        };


        /** \brief class constructor, initialises as specified, the ID is obtained from the parent project */
        QFRDRTable(QFProject* parent);

        /** Default destructor */
        virtual ~QFRDRTable();

        /** \brief returns the used datamodel */
        QFTablePluginModel* model() ;
        /** \brief returns the table contents at the given position */
        QVariant getModelData(quint16 row, quint16 column);


        virtual QVariant tableGetData(quint16 row, quint16 column) const;
        virtual void tableSetData(quint16 row, quint16 column, const QVariant& data);
        virtual void tableSetColumnData(quint16 column, QList<QVariant> data);
        virtual void tableSetColumnTitle( quint16 column, const QString& data);
        virtual QString tableGetColumnTitle(quint16 column) const;
        virtual bool tableSupportsExpressions() const;
        virtual void tableSetExpression(quint16 row, quint16 column, const QString& expression=QString(""));
        virtual QString tableGetExpression(quint16 row, quint16 column) const;
        virtual void tableReevaluateExpressions();
        virtual int tableGetColumnCount() const;
        virtual int tableGetRowCount() const;
        virtual bool tableIsReadonly() const;






        virtual void colgraphAddPlot(int graph, int columnX, int columnY, ColumnGraphTypes type, const QString&  title);
        virtual void colgraphAddErrorPlot(int graph, int columnX, int columnXError, int columnY, int columnYError, ColumnGraphTypes type, const QString&  title, ErrorGraphTypes errorStyle=egtBars);
        virtual void colgraphAddGraph(const QString&  title, const QString& xLabel=QString("x"), const QString& yLabel=QString("y"), bool logX=false, bool logY=false);
        virtual int colgraphGetPlotCount(int graph) const;
        virtual int colgraphGetGraphCount() const;
        virtual void colgraphRemoveGraph(int graph) ;
        virtual void colgraphRemovePlot(int graph, int plot);
        virtual void colgraphSetPlotColor(int graph,  int plot, QColor color);
        virtual void colgraphSetPlotColor(int graph,  int plot, QColor  color, QColor  fillColor);
        virtual void colgraphSetPlotColor(int graph,  int plot, QColor  color, QColor  fillColor, QColor errorColor);
        virtual void colgraphSetGraphTitle(int graph, const QString&  title);
        virtual QString colgraphGetGraphTitle(int graph);
        virtual void colgraphSetGraphXAxisProps(int graph, const QString& xLabel=QString("x"), bool logX=false);
        virtual void colgraphSetGraphYAxisProps(int graph, const QString& yLabel=QString("y"), bool logY=false);
        virtual void colgraphsetXRange(int graph, double xmin, double xmax);
        virtual void colgraphsetYRange(int graph, double xmin, double xmax);

        virtual void colgraphSetPlotTitle(int graph,  int plot,  const QString& title);
        virtual void colgraphSetPlotType(int graph,  int plot,  ColumnGraphTypes type);
        virtual void colgraphSetPlotErrorStyle(int graph,  int plot,  ErrorGraphTypes errorStyle);
        virtual void colgraphSetPlotErrorColor(int graph,  int plot, QColor errorColor);
        virtual void colgraphSetPlotFillColor(int graph,  int plot, QColor fillColor);
        virtual void colgraphSetPlotErrorTransparency(int graph,  int plot, double errorT);
        virtual void colgraphSetPlotFillTransparency(int graph,  int plot, double fillT);
        virtual void colgraphSetPlotPlotTransparency(int graph,  int plot, double trans);
        virtual void colgraphSetPlotSymbol(int graph,  int plot,  ColumnGraphSymbols symbol, double symbolSize=10.0);

        void colgraphToolsSetGraphtype(QFRDRTable::GraphInfo& g, QFRDRColumnGraphsInterface::ColumnGraphTypes type);


        int getPlotCount() const;
        PlotInfo getPlot(int i) const;
        void addPlot();
        void addPlot(PlotInfo plotInfo);
        void setPlot(int i, PlotInfo plotInfo);
        void deletePlot(int i);
        void swapPlots(int i, int j);


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

        QVariant evaluateExpression(QFMathParser &mp, QFMathParser::qfmpNode *n, QModelIndex cell, bool *ok, const QString &expression, QString *error, bool columnMode=false);
    protected slots:
        void tdataChanged( const QModelIndex & tl, const QModelIndex & br ) {
            emit rawDataChanged();
        }
        void trawDataChanged() {
            emit rawDataChanged();
        }

        void emitRebuildPlotWidgets();

        void didRebuildPlotWidgets();
    protected:
        bool emittedRebuildPlotWidgets;
    signals:
        void rebuildPlotWidgets();


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
        QList<QColor> autocolors;

    private:
};

#endif // QFRDRTABLE_H
