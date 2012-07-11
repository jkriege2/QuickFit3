#include "qfrdrtable.h"
#include "dlgcsvparameters.h"
#include <QtXml>
#include "qfrdrtableeditor.h"
#include "qfrdrploteditor.h"
#include "qftools.h"

QFRDRTable::GraphInfo::GraphInfo() {
    title="";
    type=QFRDRTable::gtLines;
    xcolumn=-1;
    ycolumn=-1;
    xerrorcolumn=-1;
    yerrorcolumn=-1;
    style=Qt::SolidLine;
    color=QColor("red");
    errorColor=color.darker();
    linewidth=1;
    symbol=JKQTPnoSymbol;
    symbolSize=5;
    errorStyle=JKQTPnoError;
}

QFRDRTable::PlotInfo::PlotInfo()
{
    title="";
    xlabel="x";
    ylabel="y";
    xlog=false;
    ylog=false;
    xmin=0;
    xmax=10;
    ymin=0;
    ymax=10;
    showKey=true;
    grid=true;
    fontName=QApplication::font().family();
    keyFontSize=10;
    axisFontSize=10;
    axisLabelFontSize=12;
    labelFontSize=16;
}







QFRDRTable::QFRDRTable(QFProject* parent/*, QString name, QString inputFile*/):
    QFRawDataRecord(parent)//, name, QStringList(inputFile))
{
    datamodel=NULL;
}


QFRDRTable::~QFRDRTable()
{
    //dtor
}

QFTablePluginModel* QFRDRTable::model() {
    if (!datamodel) {
        datamodel=new QFTablePluginModel(this);
        connect(datamodel, SIGNAL(modelReset()), this, SLOT(trawDataChanged()));
        connect(datamodel, SIGNAL(dataChanged( const QModelIndex & , const QModelIndex &  )), this, SLOT(tdataChanged( const QModelIndex & , const QModelIndex &  )));
    }
    return datamodel;
};

QVariant QFRDRTable::getModelData(quint16 row, quint16 column) {
    if (!datamodel) {
        datamodel=new QFTablePluginModel(this);
        connect(datamodel, SIGNAL(modelReset()), this, SLOT(trawDataChanged()));
        connect(datamodel, SIGNAL(dataChanged( const QModelIndex & , const QModelIndex &  )), this, SLOT(tdataChanged( const QModelIndex & , const QModelIndex &  )));
    }
    return datamodel->data(datamodel->index(row, column), Qt::DisplayRole);
}

int QFRDRTable::getPlotCount() const
{
    return plots.size();
}

QFRDRTable::PlotInfo QFRDRTable::getPlot(int i) const
{
    return plots[i];
}

void QFRDRTable::addPlot() {
    PlotInfo info;
    info.title="new graph";
    plots.append(info);
    trawDataChanged();
}

void QFRDRTable::addPlot(QFRDRTable::PlotInfo plotInfo)
{
    plots.append(plotInfo);
    trawDataChanged();
}

void QFRDRTable::setPlot(int i, QFRDRTable::PlotInfo plotInfo)
{
    if (i>=0 && i<plots.size()) {
        plots[i]=plotInfo;
        trawDataChanged();
    }
}

void QFRDRTable::deletePlot(int i)
{
    if (i>=0 && i<plots.size()) {
        plots.removeAt(i);
        trawDataChanged();
    }
}

int QFRDRTable::getEditorCount()
{
    return 2;
}

QString QFRDRTable::getEditorName(int i)
{
    if (i==0) return tr("Data Table");
    if (i==1) return tr("Plots");
    return QString("");
}

QFRawDataEditor *QFRDRTable::createEditor(QFPluginServices *services, QFRawDataPropertyEditor *propEditor, int i, QWidget *parent)
{
    if (i==0) return new QFRDRTableEditor(services, propEditor, parent);
    if (i==1) return new QFRDRPlotEditor(services, propEditor, parent);
    return NULL;
};


void QFRDRTable::exportData(const QString& format, const QString& filename)const  {
    if (!datamodel) return;
    QString f=format.toUpper();
    if (f=="CSV") {
        datamodel->saveCSV(filename);
    } else if (f=="SSV") {
        datamodel->saveCSV(filename, ";", '.');
    } else if (f=="GERMANEXCEL") {
        datamodel->saveCSV(filename, ";", ',');
    } else if (f=="SLK" || f=="SYLK") {
        datamodel->saveSYLK(filename);
    }
}



void QFRDRTable::intReadData(QDomElement* e) {
    if (!datamodel) {
        datamodel=new QFTablePluginModel(this);
        connect(datamodel, SIGNAL(modelReset()), this, SLOT(trawDataChanged()));
        connect(datamodel, SIGNAL(dataChanged( const QModelIndex & , const QModelIndex &  )), this, SLOT(tdataChanged( const QModelIndex & , const QModelIndex &  )));
    }
    datamodel->clear();
    datamodel->setReadonly(false);
    if (files.size()>0) {
        //std::cout<<"    reading CSV\n";
        QString s=getProperty("column_separator", ",").toString();
        char column_separator=(s.size()>0)?s[0].toAscii():',';
        s=getProperty("decimal_separator", ".").toString();
        char decimal_separator=(s.size()>0)?s[0].toAscii():'.';
        s=getProperty("header_start", "#!").toString();
        QString header_start=s;
        s=getProperty("coment_start", "#").toString();
        char comment_start=(s.size()>0)?s[0].toAscii():'#';

        //std::cout<<"column_separator="<<column_separator<<"  decimal_separator="<<decimal_separator<<"   header_start="<<header_start.toStdString()<<"   comment_start="<<comment_start<<std::endl;

        datamodel->readCSV(files[0], column_separator, decimal_separator, header_start, comment_start);
        datamodel->setReadonly(true);
        datamodel->resetChanged();
    } else if (e) {
        QDomElement te=e->firstChildElement("column");
        quint16 rows=0, columns=0;
        datamodel->setReadonly(false);
        if (!te.isNull()) {
            while (!te.isNull()) {
                columns++;
                QString n=te.attribute("title");
                QDomElement re=te.firstChildElement("row");
                quint16 r=0;
                //std::cout<<"resize("<<rows<<", "<<columns<<")\n";
                datamodel->resize(rows, columns);
                //std::cout<<"resize("<<rows<<", "<<columns<<") DONE\n";
                while (!re.isNull()) {
                    QString t=re.attribute("type").toLower();
                    QVariant v=re.attribute("value");
                    if (r+1>rows) {
                        rows=r+1;
                        //std::cout<<"resize("<<rows<<", "<<columns<<")\n";
                        datamodel->resize(rows, columns);
                        //std::cout<<"resize("<<rows<<", "<<columns<<") DONE\n";
                    }
                    if (t=="bool") { v.convert(QVariant::Bool); }
                    else if (t=="char") { v.convert(QVariant::Char); }
                    else if (t=="date") { v.convert(QVariant::Date); }
                    else if (t=="datetime") { v.convert(QVariant::DateTime); }
                    else if (t=="double") { v.convert(QVariant::Double); }
                    else if (t=="int") { v.convert(QVariant::Int); }
                    else if (t=="longlong") { v.convert(QVariant::LongLong); }
                    else if (t=="string") { v.convert(QVariant::String); }
                    else if (t=="stringlist") { v.convert(QVariant::StringList); }
                    else if (t=="uint") { v.convert(QVariant::UInt); }
                    else if (t=="ulonglong") { v.convert(QVariant::ULongLong); }
                    else if (t=="time") { v.convert(QVariant::Time); }
                    //std::cout<<"setCell("<<r<<", "<<columns-1<<", '"<<v.toString().toStdString()<<"' ["<<v.typeName()<<"])\n";
                    datamodel->setCell(r, columns-1, v);


                    re = re.nextSiblingElement("row");
                    r++;
                }
                datamodel->setColumnTitle(columns-1, n);
                te = te.nextSiblingElement("column");
            }
        } else {
            datamodel->resize(1,1);
        }
        datamodel->resetChanged();
    }

    if (e) {
        QDomElement te=e->firstChildElement("plots");
        plots.clear();
        if (!te.isNull()) {
            te=te.firstChildElement("plot");
            while (!te.isNull()) {
                PlotInfo plot;
                plot.title=te.attribute("title", tr("graph title"));
                plot.xlabel=te.attribute("xlabel", "x");
                plot.ylabel=te.attribute("ylabel", "y");
                plot.xlog=QStringToBool( te.attribute("xlog", "false"));
                plot.ylog=QStringToBool( te.attribute("ylog", "false"));
                plot.grid=QStringToBool( te.attribute("grid", "true"));
                plot.showKey=QStringToBool( te.attribute("showkey"));
                plot.xmin=CQStringToDouble(te.attribute("xmin", "0"));
                plot.xmax=CQStringToDouble(te.attribute("xmax", "10"));
                plot.ymin=CQStringToDouble(te.attribute("ymin", "0"));
                plot.ymax=CQStringToDouble(te.attribute("ymax", "10"));
                plot.fontName=te.attribute("fontname", "Arial");
                plot.keyFontSize=te.attribute("keyfontsize", "12").toDouble();
                plot.axisFontSize=te.attribute("axisfontsize", "10").toDouble();
                plot.axisLabelFontSize=te.attribute("axislabelfontsize", "12").toDouble();
                plot.labelFontSize=te.attribute("labelFontSize", "16").toDouble();

                QDomElement ge=te.firstChildElement("graph");
                while (!ge.isNull()) {
                    GraphInfo graph;
                    graph.type=String2GraphType(ge.attribute("type"));
                    graph.title=ge.attribute("title");
                    graph.drawLine= QStringToBool(ge.attribute("drawline", "true"));
                    graph.xcolumn=ge.attribute("xcolumn", "-1").toInt();
                    graph.ycolumn=ge.attribute("ycolumn", "-1").toInt();
                    graph.xerrorcolumn=ge.attribute("xerrorcolumn", "-1").toInt();
                    graph.yerrorcolumn=ge.attribute("yerrorcolumn", "-1").toInt();
                    graph.linewidth=ge.attribute("linewidth", "1").toDouble();
                    graph.symbolSize=ge.attribute("symbolSize", "1").toDouble();
                    graph.color=QColor(ge.attribute("color", "blue"));
                    graph.errorColor=QColor(ge.attribute("errorcolor", "darkblue"));
                    graph.fillColor=QColor(ge.attribute("fillcolor", "blue"));
                    graph.style=String2QPenStyle(ge.attribute("style", "solid"));
                    graph.symbol=String2JKQTPgraphSymbols(ge.attribute("symbol", "symbol_cross"));
                    graph.errorStyle=String2JKQTPerrorPlotstyle(ge.attribute("errorStyle", "error_none"));
                    graph.colorTransparent=ge.attribute("color_trans", "1").toDouble();
                    graph.errorColorTransparent=ge.attribute("errorcolor_trans", "1").toDouble();
                    graph.fillColorTransparent=ge.attribute("fillcolor_trans", "0.3").toDouble();
                    plot.graphs.append(graph);
                    ge = ge.nextSiblingElement("graph");
                }

                plots.append(plot);
                te = te.nextSiblingElement("plot");
            }
        }
    }
}

void QFRDRTable::intWriteData(QXmlStreamWriter& w) {
    if (!datamodel) {
        datamodel=new QFTablePluginModel(this);
        connect(datamodel, SIGNAL(modelReset()), this, SLOT(trawDataChanged()));
        connect(datamodel, SIGNAL(dataChanged( const QModelIndex & , const QModelIndex &  )), this, SLOT(tdataChanged( const QModelIndex & , const QModelIndex &  )));
    }
    if (files.size()>0) {
        if (datamodel->hasChanged()) datamodel->saveCSV(files[0]);
    } else {
        for (quint16 c=0; c<datamodel->columnCount(); c++) {
            w.writeStartElement("column");
            w.writeAttribute("title", datamodel->columnTitle(c));
            for (quint16 r=0; r<datamodel->rowCount(); r++) {
                w.writeStartElement("row");
                w.writeAttribute("type", QString(datamodel->cell(r, c).typeName()));
                w.writeAttribute("value", datamodel->cell(r, c).toString());
                w.writeEndElement();
            }
            w.writeEndElement();
        }
    }

    w.writeStartElement("plots");
    for (int i=0; i<plots.size(); i++) {
        w.writeStartElement("plot");
        w.writeAttribute("title", plots[i].title);
        w.writeAttribute("xlabel", plots[i].xlabel);
        w.writeAttribute("ylabel", plots[i].ylabel);
        w.writeAttribute("showkey", boolToQString(plots[i].showKey));
        w.writeAttribute("grid", boolToQString(plots[i].grid));
        w.writeAttribute("xlog", boolToQString(plots[i].xlog));
        w.writeAttribute("ylog", boolToQString(plots[i].ylog));
        w.writeAttribute("xmin", CDoubleToQString(plots[i].xmin));
        w.writeAttribute("xmax", CDoubleToQString(plots[i].xmax));
        w.writeAttribute("ymin", CDoubleToQString(plots[i].ymin));
        w.writeAttribute("ymax", CDoubleToQString(plots[i].ymax));
        w.writeAttribute("keyfontsize", CDoubleToQString(plots[i].keyFontSize));
        w.writeAttribute("axisfontsize", CDoubleToQString(plots[i].axisFontSize));
        w.writeAttribute("axislabelfontsize", CDoubleToQString(plots[i].axisLabelFontSize));
        w.writeAttribute("labelfontsize", CDoubleToQString(plots[i].labelFontSize));
        w.writeAttribute("fontname", plots[i].fontName);

        for (int g=0; g<plots[i].graphs.size(); g++) {
            w.writeStartElement("graph");
            w.writeAttribute("type", GraphType2String(plots[i].graphs[g].type));
            w.writeAttribute("title", plots[i].graphs[g].title);
            w.writeAttribute("drawline", boolToQString( plots[i].graphs[g].drawLine));
            w.writeAttribute("xcolumn", QString::number(plots[i].graphs[g].xcolumn));
            w.writeAttribute("ycolumn", QString::number(plots[i].graphs[g].ycolumn));
            w.writeAttribute("xerrorcolumn", QString::number(plots[i].graphs[g].xerrorcolumn));
            w.writeAttribute("yerrorcolumn", QString::number(plots[i].graphs[g].yerrorcolumn));
            w.writeAttribute("linewidth", QString::number(plots[i].graphs[g].linewidth));
            w.writeAttribute("symbolSize", QString::number(plots[i].graphs[g].symbolSize));
            w.writeAttribute("style", QPenStyle2String(plots[i].graphs[g].style));
            w.writeAttribute("color", QColor2String(plots[i].graphs[g].color));
            w.writeAttribute("errorcolor", QColor2String(plots[i].graphs[g].errorColor));
            w.writeAttribute("fillcolor", QColor2String(plots[i].graphs[g].fillColor));
            w.writeAttribute("symbol", JKQTPgraphSymbols2String(plots[i].graphs[g].symbol));
            w.writeAttribute("errorStyle", JKQTPerrorPlotstyle2String(plots[i].graphs[g].errorStyle));
            w.writeAttribute("color_trans", QString::number(plots[i].graphs[g].colorTransparent));
            w.writeAttribute("errorcolor_trans", QString::number(plots[i].graphs[g].errorColorTransparent));
            w.writeAttribute("fillcolor_trans", QString::number(plots[i].graphs[g].fillColorTransparent));
            w.writeEndElement();
        }
        w.writeEndElement();
    }
    w.writeEndElement();
}
