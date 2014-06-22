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
#include "datatools.h"
#include "qffitalgorithmparameterstorage.h"
class QFRDRTableEditor; // forward

/*! \brief this class is used to manage a table of values (strings/numbers)
    \ingroup qf3rdrdp_table

    The data is stored in a QFTablePluginModel object which is also externally accessible for data access.
 */

class QFRDRTable : public QFRawDataRecord, public QFRDRTableInterface, public QFRDRColumnGraphsInterface, public QFFitAlgorithmParameterStorage {
        Q_OBJECT
        Q_INTERFACES(QFRDRTableInterface QFRDRColumnGraphsInterface)
    public:
        enum {
            TableExpressionRole = Qt::UserRole+1,
            TableCommentRole = Qt::UserRole+2,
            ColumnExpressionRole = Qt::UserRole+10,
            ColumnCommentRole = Qt::UserRole+11,
            ColumnImagePixWidth = Qt::UserRole+12,
            ColumnImagePixHeight = Qt::UserRole+13,
            ColumnImageWidth = Qt::UserRole+14,
            ColumnImageHeight = Qt::UserRole+15,
            ColumnImageX = Qt::UserRole+16,
            ColumnImageY = Qt::UserRole+17
        };


        enum GraphType {
            gtLines=0,
            gtImpulsesVertical,
            gtImpulsesHorizontal,
            gtFilledCurveX,
            gtFilledCurveY,
            gtStepsHorizontal,
            gtStepsVertical,
            gtBarsHorizontal,
            gtBarsVertical,
            gtBoxplotX,
            gtBoxplotY,
            gtImage,
            gtRGBImage,
            gtMaskImage,
            gtFunction,
            gtHorizontalRange,
            gtVerticalRange,
            gtParametrizedScatter
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
                case gtBoxplotX: return QString("boxx");
                case gtBoxplotY: return QString("boxy");
                case gtImage: return QString("image");
                case gtRGBImage: return QString("rgbimage");
                case gtMaskImage: return QString("maskimage");
                case gtFunction: return QString("function");
                case gtHorizontalRange: return QString("hrange");
                case gtVerticalRange: return QString("vrange");
                case gtParametrizedScatter: return QString("pscatter");
                default: return QString("");
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
                case gtBoxplotX: return QIcon(":/table/icons/plot_boxplotsx.png");
                case gtBoxplotY: return QIcon(":/table/icons/plot_boxplotsy.png");
                case gtImage: return QIcon(":/table/icons/plot_image.png");
                case gtRGBImage: return QIcon(":/table/icons/plot_rgbimage.png");
                case gtMaskImage: return QIcon(":/table/icons/plot_maskimage.png");
                case gtFunction: return QIcon(":/table/icons/plot_function.png");
                case gtHorizontalRange: return QIcon(":/table/icons/plot_hrange.png");
                case gtVerticalRange: return QIcon(":/table/icons/plot_vrange.png");
                case gtParametrizedScatter: return QIcon(":/table/icons/plot_pscatter.png");
                default: return QIcon();
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
            if (s=="boxx") return gtBoxplotX;
            if (s=="boxy") return gtBoxplotY;
            if (s=="image") return gtImage;
            if (s=="rgbimage") return gtRGBImage;
            if (s=="maskimage") return gtMaskImage;
            if (s=="function") return gtFunction;
            if (s=="vrange") return gtVerticalRange;
            if (s=="hrange") return gtHorizontalRange;
            if (s=="pscatter") return gtParametrizedScatter;
            return gtLines;
        }


        enum DataSelectOperation {
            dsoEquals=0,
            dsoUnequal=1,
            dsoGreaterOrEqual=2,
            dsoSmallerOrEqual=3,
            dsoGreater=4,
            dsoSmaller=5
        };

        static QString DataSelectOperation2String(DataSelectOperation type) {
            switch(type) {
                case dsoUnequal: return QString("!=");
                case dsoGreaterOrEqual: return QString(">=");
                case dsoSmallerOrEqual: return QString("<=");
                case dsoGreater: return QString(">");
                case dsoSmaller: return QString("<");
                default:
                case dsoEquals: return QString("==");
            }

        }
        static DataSelectOperation String2DataSelectOperation(QString type) {
            if(type=="!=") return dsoUnequal;
            if(type==">=") return dsoGreaterOrEqual;
            if(type=="<=") return dsoSmallerOrEqual;
            if(type==">") return dsoGreater;
            if(type=="<") return dsoSmaller;
            return dsoEquals;
        }



        struct GraphInfo {
            GraphInfo();
            //void setBoxplotColumns(int position, int minC, int q25C, int medianC, int meanC, int q75C, int maxC);
            GraphType type;
            QString title;
            int xcolumn;
            int ycolumn;
            int xerrorcolumn;
            int yerrorcolumn;
            int meancolumn;
            int q75column;
            int maxcolumn;
            double width;
            double shift;
            Qt::PenStyle style;
            Qt::PenStyle whiskerStyle;
            Qt::BrushStyle fillStyle;
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

            double errorWidth;
            Qt::PenStyle errorLineStyle;
            double errorBarSize;

            double offset;
            int xerrorcolumnlower;
            int yerrorcolumnlower;
            bool xerrorsymmetric;
            bool yerrorsymmetric;



            int stride;
            int strideStart;
            bool isStrided;

            bool isDataSelect;
            int dataSelectColumn;
            DataSelectOperation dataSelectOperation;
            double dataSelectCompareValue;

            int imageTicks;
            int imageModTicks;
            double imageColorbarFontsize;
            double imageColorbarTicklength;

            JKQTPCAlabelType imageColorbarLabelType;
            int imageColorbarLabelDigits;

            QColor imageTrueColor;
            double imageTrueTransparent;
            QColor imageFalseColor;
            double imageFalseTransparent;
            int imagePixelWidth;
            double imageX;
            double imageY;
            double imageWidth;
            double imageHeight;
            JKQTPMathImageColorPalette imagePalette;
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

            double rangeStart;
            double rangeEnd;
            double rangeCenter;
            bool rangeInverted;
            bool rangeFill;
            QColor rangeCenterColor;
            double rangeCenterColorTransparent;
            Qt::PenStyle rangeCenterStyle;
            double rangeCenterWidth;
            bool rangeDrawCenter;

            bool errorColorAuto;
            bool fillColorAuto;
            bool centerColorAuto;

            QMap<QString, QVariant> moreProperties;

        };



        struct AxisInfo {
            AxisInfo();
            QString label;
            double min, max;
            bool log;
            QColor axisColor;
            double axisWidth;
            double axisFontSize;
            double axisLabelFontSize;
            JKQTPlabelPosition labelPos;
            JKQTPCAlabelType labelType;
            int digits;
            JKQTPCAdrawMode labelMode1;
            JKQTPCAdrawMode labelMode2;

            int minTicks;
            int minorTicks;
            bool axis0;

            double TickSpacing;
            bool AutoTicks;
            double AxisTickWidth;
            double AxisMinorTickWidth;
            bool AxisInverted;
            double TickInsideLength;
            double TickOutsideLength;
            double MinorTickInsideLength;
            double MinorTickOutsideLength;

            int columnNamedTickNames;
            int columnNamedTickValues;

        };



        struct PlotInfo {
            PlotInfo();
            QString title;
            bool grid;
            bool showKey;
            bool showTitle;

            bool keepAxisAspectRatio;
            bool keepDataAspectRatio;
            double dataAspectRatio;
            double axisAspectRatio;

            QColor axisColor;
            double axisWidth;
            QString fontName;
            double keyFontSize;
            double keyBoxLineWidth;
            QColor keyBackgroundColor;
            bool keyBox;            
            QColor keyLineColor;

            double keyXMargin;
            double keyYMargin;
            double keyXOffset;
            double keyYOffset;
            double keyXSeparation;
            double keyYSeparation;
            double key_line_length;

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

            AxisInfo xAxis;
            AxisInfo yAxis;

            bool graphAutosize;
            int graphWidth;
            int graphHeight;

        };


        /** \brief class constructor, initialises as specified, the ID is obtained from the parent project */
        QFRDRTable(QFProject* parent);

        /** Default destructor */
        virtual ~QFRDRTable();

        /** \brief returns the used datamodel */
        QFTablePluginModel* model() ;
        /** \brief returns the table contents at the given position */
        QVariant getModelData(quint32 row, quint32 column);


        virtual QVariant tableGetData(quint32 row, quint32 column) const;
        virtual void tableSetData(quint32 row, quint32 column, const QVariant& data);
        virtual void tableSetColumnTitle( quint32 column, const QString& data);
        virtual QString tableGetColumnTitle(quint32 column) const;
        virtual bool tableSupportsExpressions() const;
        virtual void tableSetExpression(quint32 row, quint32 column, const QString& expression=QString(""));
        virtual void tableSetColumnExpression(quint32 column, const QString& expression=QString(""));
        virtual void tableSetColumnComment(quint32 column, const QString& comment=QString(""));
        virtual void tableSetComment(quint32 row, quint32 column, const QString& comment=QString(""));
        virtual QString tableGetExpression(quint32 row, quint32 column) const;
        virtual void tableReevaluateExpressions();
        virtual int tableGetColumnCount() const;
        virtual int tableGetRowCount() const;
        virtual bool tableIsReadonly() const;
        virtual void tableSetColumnData(quint32 column, const QList<QVariant>& data);
        virtual QList<QVariant> tableGetColumnData(quint32 column);
        virtual void tableSetColumnDataAsDouble(quint32 column, const QVector<double>& data);
        virtual QVector<double> tableGetColumnDataAsDouble(int column);
        virtual bool tablesGetDoEmitSignals() const;
        virtual void tablesSetDoEmitSignals(bool doEmit);




        virtual bool colgraphGetDoEmitSignals() const;
        virtual void colgraphSetDoEmitSignals(bool doEmit);
        virtual void colgraphAddBoxPlot(int plotid, Orientation orientation, int columnX, int columnMin, int columnQ25, int columnMedian, int columnMean, int columnQ75, int columnMax, const QString&  title);
        virtual void colgraphAddGraph(int plotid, int columnX, int columnY, ColumnGraphTypes type, const QString&  title);
        virtual void colgraphAddFunctionGraph(int plotid, const QString& expression, ColumnGraphTypes type, const QString&  title, int columnParam=-1);
        virtual void colgraphAddFunctionGraph(int plotid, const QString& expression, ColumnGraphTypes type, const QString&  title, const QVector<double>& params);
        virtual void colgraphSetFunctionGraph(int plotid, int graphid, const QString& expression, ColumnGraphTypes type, const QString&  title, int columnParam=-1);
        virtual void colgraphSetFunctionGraph(int plotid, int graphid, const QString& expression, ColumnGraphTypes type, const QString&  title, const QVector<double>& params);
        virtual void colgraphAddErrorGraph(int plotid, int columnX, int columnXError, int columnY, int columnYError, ColumnGraphTypes type, const QString&  title, ErrorGraphTypes errorStyle=egtBars);
        virtual void colgraphSetErrorGraphProperties(int plot, int graphid, int columnXError, int columnYError, ErrorGraphTypes errorStyle=egtBars);
        virtual void colgraphSetErrorGraphErrorColumnX(int plot, int graphid, int columnXError);
        virtual void colgraphSetErrorGraphErrorColumnY(int plot, int graphid, int columnYError);
        virtual void colgraphSetErrorGraphStyle(int plot, int graphid, ErrorGraphTypes errorStyle);
        virtual void colgraphSetErrorGraphErrorColumnXAsymmetric(int plot, int graphid, bool xErrorAsyymetric, int columnXErrorAsymmetric=-1);
        virtual void colgraphSetErrorGraphErrorColumnYAsymmetric(int plot, int graphid, bool yErrorAsyymetric, int columnYErrorAsymmetric=-1);

        virtual void colgraphAddImageGraph(int plotid, int imageColumn, ImageColorPalette palette, double x, double y, double width, double height, int Nx, const QString& title);
        virtual void colgraphAddImageMaskGraph(int plotid, int imageColumn, double x, double y, double width, double height, int Nx, const QString& title, QColor trueColor=QColor("black"), QColor falseColor=QColor("transparent"));
        virtual void colgraphAddRGBImageGrph(int plotid, int imageRColumn, int imageGColumn, int imageBColumn, double x, double y, double width, double height, int Nx, const QString& title);
        virtual void colgraphAddRangeGraph(int plotid, Orientation orientation, double rangeStart, double rangeEnd, double rangeCenter, const QString& title, bool invertRange=false, bool fillrange=true, bool drawRangeLines=true, bool drawRangeCenter=true, QColor centerColor=QColor("red"),  Qt::PenStyle centerStyle=Qt::SolidLine, double centerWidth=1.0);
        virtual void colgraphSetImageGraphModifier(int plotid, int graphid, int imageModifierColumn, ImageModifierMode mode);
        virtual void colgraphSetImageGraphRange(int plotid, int graphid, ImageGraphRangeChannel channel, bool autoRange, double min=0, double max=0);
        virtual void colgraphSetGraphProperty(int plotid, int graphid, const QString& name, const QVariant& value);
        virtual QVariant colgraphGetGraphProperty(int plotid, int raphid, const QString& name, const QVariant& defaultValue=QVariant());

        virtual void colgraphAddPlot(const QString&  title, const QString& xLabel=QString("x"), const QString& yLabel=QString("y"), bool logX=false, bool logY=false);
        virtual int colgraphGetGraphCount(int plotid) const;
        virtual int colgraphGetPlotCount() const;
        virtual void colgraphRemovePlot(int plotid) ;
        virtual void colgraphRemoveGraph(int plotid, int graphid);

        virtual void colgraphSetGraphWhiskerLineStyle(int plot,  int graph,  Qt::PenStyle  style);
        virtual void colgraphSetGraphLineStyle(int plotid,  int graphid,  Qt::PenStyle  style);
        virtual void colgraphSetGraphLineWidth(int plotid,  int graphid,  double width);
        virtual void colgraphSetGraphFillStyle(int plotid,  int graphid,  Qt::BrushStyle  style);
        virtual void colgraphSetGraphWidth(int plot,  int graph,  double width);
        virtual void colgraphSetGraphShift(int plot,  int graph,  double shift);


        virtual void colgraphSetGraphColor(int plotid,  int graphid, QColor color);
        virtual void colgraphSetGraphColor(int plotid,  int graphid, QColor  color, QColor  fillColor);
        virtual void colgraphSetGraphColor(int plotid,  int graphid, QColor  color, QColor  fillColor, QColor errorColor);
        virtual void colgraphSetPlotTitle(int plotid, const QString&  title);
        virtual QString colgraphGetPlotTitle(int plotid);
        virtual void colgraphSetPlotXAxisProps(int plotid, const QString& xLabel=QString("x"), bool logX=false);
        virtual void colgraphSetPlotYAxisProps(int plotid, const QString& yLabel=QString("y"), bool logY=false);
        virtual void colgraphSetPlotXRange(int plotid, double xmin, double xmax);
        virtual void colgraphSetPlotYRange(int plotid, double xmin, double xmax);

        virtual void colgraphSetGraphTitle(int plotid,  int graphid,  const QString& title);
        virtual void colgraphSetGrphType(int plotid,  int graphid,  ColumnGraphTypes type);
        virtual void colgraphSetGraphErrorStyle(int plotid,  int graphid,  ErrorGraphTypes errorStyle);
        virtual void colgraphSetGraphErrorColor(int plotid,  int graphid, QColor errorColor);
        virtual void colgraphSetGraphFillColor(int plotid,  int graphid, QColor fillColor);
        virtual void colgraphSetGraphErrorTransparency(int plotid,  int graphid, double errorT);
        virtual void colgraphSetGraphFillTransparency(int plotid,  int graphid, double fillT);
        virtual void colgraphSetGraphTransparency(int plotid,  int graphid, double trans);
        virtual void colgraphSetGraphSymbol(int graph,  int plot,  ColumnGraphSymbols symbol, double symbolSize=10.0);

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
            sl << "QFTABLEXML";
            QStringList f=QFDataExportHandler::getFormats();
            for (int i=0; i<f.size(); i++) sl<<QString("F%1").arg(i);
            return sl;

        }
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle() { return tr("Export Data Table ..."); }
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes() { return tr("Table XML file (*.qftxml)")+";;"+QFDataExportHandler::getFormats().join(";;"); }

        QVariant evaluateExpression(QFMathParser &mp, QFMathParser::qfmpNode *n, QModelIndex cell, bool *ok, const QString &expression, QString *error, bool columnMode=false);
        void readAxisInfo(AxisInfo &plot, const QString &axisName, QDomElement te);
        void readPlotInfo(PlotInfo &plot, QDomElement te);
        void readGraphInfo(GraphInfo &graph, QDomElement te);
        void writeAxisInfo(QXmlStreamWriter& w, const AxisInfo &plot, const QString &axisName) const;
        void writePlotInfo(QXmlStreamWriter& w, const PlotInfo &plot, bool writeGraphs=true) const;
        void writeGraphInfo(QXmlStreamWriter& w, const GraphInfo &graph) const;
        void loadColumnToComboBox(QComboBox *combo);
    public slots:
        void emitRebuildPlotWidgets();

        void columnsInserted(int start, int count, bool emitRebuild=true);
        void columnsRemoved(int start, int count, bool emitRebuild=true);

    protected slots:
        void tdataChanged( const QModelIndex & tl, const QModelIndex & br ) {
            emit rawDataChanged();
        }
        void trawDataChanged() {
            emit rawDataChanged();
        }


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

        bool emitColGraphChangedSignals;

    private:
};


#endif // QFRDRTABLE_H
