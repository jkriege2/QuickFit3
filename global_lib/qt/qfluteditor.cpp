/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "qfluteditor.h"
#include <QGraphicsSceneMouseEvent>
#include <QRadialGradient>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QPainter>
#include <QRadialGradient>
#include <QMouseEvent>
#include <QDialog>
#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#define JKQTPIsOKFloat(v) (std::isfinite(v)&&(!std::isinf(v))&&(!std::isnan(v)))


QFLUTEditor::QFLUTEditor(QWidget *parent):
    QFrame(parent)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    setMinimumWidth(270);

    logHistogram=false;
    lut_value_min=0;
    lut_value_max=255;

    alphaMode=false;

    LUTPalette=JKQTPMathImageGRAY;

    currentNode=-1;
    gradientWidth=10;

    lut_min=0;
    lut_max=255;

    lutTimer=new QTimer(this);



    addNode(QPointF(lut_min, lut_value_min));
    addNode(QPointF(lut_max, lut_value_max));
    addNode(QPointF((lut_min+lut_max)/2.0, (lut_value_min+lut_value_max)/2.0));

    ensureLutNodePositions();

    setMouseTracking(true);
    setToolTip(tr("<u>LUT-Editor</u><br>"
                  "Edit the LUT (lookup-table) by moving creating and deleting nodes.<br>"
                  "possible mouse actions:<ul>"
                  "<li>Double-clicking in empty space will create a new node</li>"
                  "<li>Double-clicking an existing node will delete the node</li>"
                  "<li>nodes can be moved by dragging it with the mouse (left-button down)</li>"
                  "</ul>"
                  "The LUT is linearly interpolated between the nodes."
                  ));
    //setFrameShape(QFrame::StyledPanel);
    //setFrameShadow(QFrame::Sunken);
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setMinimumSize(255, 55);


    lutTimer->setInterval(250);
    lutTimer->setSingleShot(true);
    connect(lutTimer, SIGNAL(timeout()), this, SLOT(emitLUTChanged()));

    actResetLUT=new QAction(tr("reset LUT to linear"), this);
    connect(actResetLUT, SIGNAL(triggered()), this, SLOT(resetLUT()));
    addAction(actResetLUT);
    actGetLUTInfo=new QAction(tr("show LUT info"), this);
    connect(actGetLUTInfo, SIGNAL(triggered()), this, SLOT(showLUTInfo()));
    addAction(actGetLUTInfo);
}


QString QFLUTEditor::doubleToQString(double value, int prec, char f, QChar decimalSeparator) const {
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    QString res=loc.toString(value, f, prec);
    if (loc.decimalPoint()!=decimalSeparator) {
        res=res.replace(loc.decimalPoint(), decimalSeparator);
    }
    return res;
}

QString QFLUTEditor::doubleVecToQString(const QVector<double>& value, int prec, char f, QChar decimalSeparator, const QString itemSeparator) const {
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    QString out="";
    for (int i=0; i<value.size(); i++) {
        if (i>0) out+=itemSeparator;
        QString res=loc.toString(value[i], f, prec);
        if (loc.decimalPoint()!=decimalSeparator) {
            res=res.replace(loc.decimalPoint(), decimalSeparator);
        }
        out+=res;
    }
    return out;
}

double QFLUTEditor::QStringToDouble(QString value) const {
    QString v=value;
    if (value.contains(',')) {
        v=value.replace(',', '.');
    }
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    return loc.toDouble(v) ;
}


QVector<double> QFLUTEditor::QStringToDoubleVec(QString value, const QString itemSeparator) const {
    QStringList sl=value.split(itemSeparator);
    QVector<double> res;
    for (int i=0; i<sl.size(); i++) {
        res<<QStringToDouble(sl[i]);
    }
    return res;
}

void QFLUTEditor::getRangeForNode(QFLUTEditorNode *node, double &min, double &max) const
{
    int idx=lut_nodes.indexOf(node);
    min=lut_min;
    max=lut_max;
    if (idx==0) {
        min=lut_min;
        //if (lut_nodes.size()>1) max=lut_nodes[1]->pos().x();
        max=lut_min;
    } else if (idx==lut_nodes.size()-1) {
        min=lut_max;
        max=lut_max;
        //if (lut_nodes.size()>1) min=lut_nodes[lut_nodes.size()-2]->pos().x();

    } else if (idx>0 && idx<lut_nodes.size()-1) {
        min=qBound(lut_min, lut_nodes[idx-1]->pos().x(), lut_max);
        max=qBound(lut_min, lut_nodes[idx+1]->pos().x(), lut_max);
    }

    //qDebug()<<"getRangeForNode( idx="<<idx<<"  min="<<min<<"  max="<<max<<" )";
}

QPointF QFLUTEditor::getPrevNodePos(QFLUTEditorNode *node) const
{
    int idx=lut_nodes.indexOf(node);
    if (idx==0) {
        return node->pos();
    } else if (idx>0 && idx<lut_nodes.size()) {
        return lut_nodes[idx-1]->pos();
    } else {
        return node->pos();
    }
}

QFLUTEditorNode *QFLUTEditor::addNode(QPointF pos, int lut)
{
    QFLUTEditorNode* n=new QFLUTEditorNode(lut, QColor("darkgrey"), this);
    n->setPos(pos);
    int idx=lut_nodes.size();
    for (int i=0; i<lut_nodes.size(); i++) {
        //qDebug()<<"    "<<pos.x()<<" <? "<<lut_nodes[i]->pos().x();
        if (pos.x()<lut_nodes[i]->pos().x()) {
            idx=qMax(0, i);
            break;
        }
    }
    lut_nodes.insert(idx, n);
    currentNode=-1;
    updateLUT();
    return n;
}

void QFLUTEditor::deleteNode(int idx)
{
    if (idx>0 && idx<lut_nodes.size()-1 && lut_nodes.size()>2) {
        QFLUTEditorNode* n=lut_nodes[idx];
        lut_nodes.removeAt(idx);
        delete n;
        updateLUT();
    }
}

QRectF QFLUTEditor::map(const QRectF &r) const
{
    return QRectF(map(r.topLeft()), map(r.bottomRight()));
}

QPointF QFLUTEditor::map(const QPointF &r) const
{
    const double wh=height()-borderWidthBoth();
    const double ww=width()-borderWidthBoth();
    const double wxo=borderWidthWithGradient();
    const double wyo=height()-borderWidthWithGradient();

    const double xx=wxo+(r.x()-lut_min)/(lut_max-lut_min)*ww;
    const double yy=wyo-(r.y()-lut_value_min)/(lut_value_max-lut_value_min)*wh;
    return QPointF(xx,yy);
}

QRectF QFLUTEditor::invMap(const QRectF &r) const
{
    return QRectF(invMap(r.topLeft()), invMap(r.bottomRight()));
}

QPointF QFLUTEditor::invMap(const QPointF &r) const
{
    const double wh=height()-borderWidthBoth();
    const double ww=width()-borderWidthBoth();
    const double wxo=borderWidthWithGradient();
    const double wyo=height()-borderWidthWithGradient();

    const double xx=lut_min+(r.x()-wxo)/ww*(lut_max-lut_min);
    const double yy=lut_value_min-(r.y()-wyo)/wh*(lut_value_max-lut_value_min);
    return QPointF(xx,yy);
}

QVector<int> QFLUTEditor::getCurrentLUT(int items) const
{
    if (items<=0) return QVector<int>();
    QVector<int> lut(items, 0);
    if (lut_nodes.size()<2) return lut;
    int j0=0;
    int j1=1;
    QImage iy=JKQTPMathImageGetPaletteImage(LUTPalette, 256);
    for (int i=0; i<items; i++) {
        double x=lut_min+double(i)*(lut_max-lut_min)/double(items);
        while (j1+1<lut_nodes.size() && x>lut_nodes[j1]->pos().x()) {
            j0++;
            j1++;
        }
        double y=lut_nodes[j0]->pos().y()+(x-lut_nodes[j0]->pos().x())/(lut_nodes[j1]->pos().x()-lut_nodes[j0]->pos().x())*(lut_nodes[j1]->pos().y()-lut_nodes[j0]->pos().y());
        //qDebug()<<x<<y;
        int yy=qBound(0, (int)round((y-lut_value_min)/(lut_value_max-lut_value_min)*255), 255);
        lut[i]=iy.pixel(yy,0);

    }
    return lut;
}

QVector<int> QFLUTEditor::getCurrentAlphaLUT(int items) const
{
    if (items<=0) return QVector<int>();
    QVector<int> lut(items, 0);
    if (lut_nodes.size()<2) return lut;
    int j0=0;
    int j1=1;
    //QImage iy=JKQTPMathImageGetPaletteImage(LUTPalette, 256);
    for (int i=0; i<items; i++) {
        double x=lut_min+double(i)*(lut_max-lut_min)/double(items);
        while (j1+1<lut_nodes.size() && x>lut_nodes[j1]->pos().x()) {
            j0++;
            j1++;
        }
        double y=lut_nodes[j0]->pos().y()+(x-lut_nodes[j0]->pos().x())/(lut_nodes[j1]->pos().x()-lut_nodes[j0]->pos().x())*(lut_nodes[j1]->pos().y()-lut_nodes[j0]->pos().y());
        //qDebug()<<x<<y;
        int yy=qBound(0, (int)round((y-lut_value_min)/(lut_value_max-lut_value_min)*255), 255);
        lut[i]=yy;

    }
    return lut;
}

QVector<int> QFLUTEditor::getLUT(const QString &config, int items, JKQTPMathImageColorPalette palette)
{
    QString old_conf=getConfig();
    QList<QFLUTEditorNode*> t_lut_nodes=lut_nodes;
    lut_nodes.clear();
    double t_lut_min=lut_min;
    double t_lut_max=lut_max;
    double t_lut_value_min=lut_value_min;
    double t_lut_value_max=lut_value_max;
    JKQTPMathImageColorPalette t_LUTPalette=LUTPalette;
    LUTPalette=palette;
    setConfig(config, false);

    QVector<int> l=getCurrentLUT(items);

    qDeleteAll(lut_nodes);
    lut_nodes=t_lut_nodes;
    lut_min=t_lut_min;
    lut_max=t_lut_max;
    lut_value_min=t_lut_value_min;
    lut_value_max=t_lut_value_max;
    LUTPalette=t_LUTPalette;
    setConfig(old_conf, false);
    return l;
}

QVector<int> QFLUTEditor::getAlphaLUT(const QString &config, int items, JKQTPMathImageColorPalette palette)
{
    QString old_conf=getConfig();
    QList<QFLUTEditorNode*> t_lut_nodes=lut_nodes;
    lut_nodes.clear();
    double t_lut_min=lut_min;
    double t_lut_max=lut_max;
    double t_lut_value_min=lut_value_min;
    double t_lut_value_max=lut_value_max;
    JKQTPMathImageColorPalette t_LUTPalette=LUTPalette;
    LUTPalette=palette;
    setConfig(config, false);

    QVector<int> l=getCurrentAlphaLUT(items);

    qDeleteAll(lut_nodes);
    lut_nodes=t_lut_nodes;
    lut_min=t_lut_min;
    lut_max=t_lut_max;
    lut_value_min=t_lut_value_min;
    lut_value_max=t_lut_value_max;
    LUTPalette=t_LUTPalette;
    setConfig(old_conf, false);
    return l;


}

void QFLUTEditor::setHistogram(const QVector<double> &histogramY)
{
    this->histogramY=histogramY;


    normalizeHistogram();
    update();
}

void QFLUTEditor::setHistogram(const double *data, int dataSize, int bins)
{
    if (!data || dataSize<=0 || bins<=0) {
        histogramX.clear();
        histogramY.clear();
        update();
        return;
    }
    //QVector<double> hX(bins, 0.0);
    QVector<double> hY(bins, 0.0);

    double mi=data[0];
    double ma=data[0];
    for (int i=1; i<dataSize; i++) {
        if (data[i]<mi) mi=data[i];
        if (data[i]>ma) ma=data[i];
    }
    double bw=(ma-mi)/double(bins);
    for (int b=0; b<bins; b++) {
        //hX[b]=double(b)*bw+mi;
        hY[b]=0;
    }
    for (int i=1; i<dataSize; i++) {
        int idx=qBound(0, (int)floor(data[i]/bw), bins-1);
        hY[idx]++;
    }

    lut_min=mi;
    lut_max=ma;

    setHistogram(hY);
}

void QFLUTEditor::setHistogram(const float *data, int dataSize, int bins)
{
    if (!data || dataSize<=0 || bins<=0) {
        histogramX.clear();
        histogramY.clear();
        update();
        return;
    }
    //QVector<double> hX(bins, 0.0);
    QVector<double> hY(bins, 0.0);

    double mi=data[0];
    double ma=data[0];
    for (int i=1; i<dataSize; i++) {
        if (data[i]<mi) mi=data[i];
        if (data[i]>ma) ma=data[i];
    }
    double bw=(ma-mi)/double(bins);
    for (int b=0; b<bins; b++) {
        //hX[b]=double(b)*bw+mi;
        hY[b]=0;
    }
    for (int i=1; i<dataSize; i++) {
        int idx=qBound(0, (int)floor(data[i]/bw), bins-1);
        hY[idx]++;
    }

    lut_min=mi;
    lut_max=ma;



    setHistogram(hY);
}

void QFLUTEditor::clearHistogram()
{
    histogramX.clear();
    histogramY.clear();
    update();
}

QString QFLUTEditor::getConfig() const
{
    QVector<double> n;
    n<<lut_min<<lut_max<<lut_value_min<<lut_value_max;
    for (int i=0; i<lut_nodes.size(); i++) {
        n<<lut_nodes[i]->pos().x();
        n<<lut_nodes[i]->pos().y();
    }
    return doubleVecToQString(n);
}

QString QFLUTEditor::getDefaultConfig() const
{
    QVector<double> n;
    n<<lut_min<<lut_max<<lut_value_min<<lut_value_max;
    if (alphaMode) {
        n<<lut_min;
        n<<lut_value_min;//lut_value_max;
        n<<lut_max;
        n<<lut_value_max;
    } else {
        n<<lut_min;
        n<<lut_value_min;
        n<<lut_max;
        n<<lut_value_max;
    }
    return doubleVecToQString(n);
}

void QFLUTEditor::setConfig(const QString &config, bool update)
{
    QVector<double> n=QStringToDoubleVec(config);
    int nodes=(n.size()-4)/2;
    if (nodes>=2) {
        lut_min=n[0];
        lut_max=n[1];
        lut_value_min=n[2];
        lut_value_max=n[3];

        if (nodes<lut_nodes.size()) {
            while (nodes<lut_nodes.size()) {
                delete lut_nodes.last();
                lut_nodes.pop_back();
            }
        }
        if (nodes>lut_nodes.size()) {
            while (nodes>lut_nodes.size()) {
                QFLUTEditorNode* nn=new QFLUTEditorNode(0, QColor("darkgrey"), this);
                lut_nodes.append(nn);
            }
        }
        int j=0;
        for (int i=4; i<n.size(); i=i+2) {
            lut_nodes[j]->setPos(QPointF(n[i], n[i+1]));
            j++;
        }
        ensureLutNodePositions();
        if (update) updateLUT();
    }
}

void QFLUTEditor::resetLUT()
{
    setConfig(getDefaultConfig());
}

void QFLUTEditor::showLUTInfo()
{
    QDialog* dlg=new QDialog(this);
    dlg->setWindowTitle(tr("LUT Info"));
    QVBoxLayout* lay=new QVBoxLayout();
    dlg->setLayout(lay);
    QPlainTextEdit* edt=new QPlainTextEdit(dlg);
    lay->addWidget(edt);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
    connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    lay->addWidget(buttonBox);
    QString txt;
    txt+=tr("lut_min = %1\n").arg(lut_min);
    txt+=tr("lut_max = %1\n").arg(lut_max);
    txt+=tr("lut_value_min = %1\n").arg(lut_value_min);
    txt+=tr("lut_value_max = %1\n").arg(lut_value_max);
    txt+=tr("alphaMode = %1\n").arg(alphaMode);
    txt+=QString("\nlut_nodes = [\n");
    for (int i=0; i<lut_nodes.size(); i++) {
        if (lut_nodes[i]) {
            txt+=tr("    %1: (%2, %3)\n").arg(i).arg(lut_nodes[i]->pos().x()).arg(lut_nodes[i]->pos().y());
        }
    }
    QVector<int> lu=getCurrentLUT();
    if (alphaMode) lu=getCurrentAlphaLUT();
    txt+=QString("\nlut = [\n");
    for (int i=0; i<lut_nodes.size(); i++) {
        if (lut_nodes[i]) {
            txt+=tr("    %1: %2\n").arg(i).arg(lu[i]);
        }
    }
    txt+=QString("]\n\n");
    edt->setPlainText(txt);
    dlg->exec();
    delete dlg;
}

void QFLUTEditor::setAlphaMode(bool a)
{
    alphaMode=a;
    while (lut_nodes.size()>2) {
        delete lut_nodes[0];
        lut_nodes.removeAt(0);
    }
    while (lut_nodes.size()<2) {
        lut_nodes.append(new QFLUTEditorNode(0, QColor("darkgrey"), this));
    }
    lut_nodes[0]->setPos(lut_min, lut_value_max);
    lut_nodes[1]->setPos(lut_max, lut_value_max);
    updateLUT();
}

QSize QFLUTEditor::minimumSizeHint() const
{
    return QSize(270,270);
}

QSize QFLUTEditor::sizeHint() const
{
    return QSize(270,270);
}

void QFLUTEditor::setColorPalette(JKQTPMathImageColorPalette LUTPalette)
{
    this->LUTPalette=LUTPalette;
    updateLUT();
}

void QFLUTEditor::setLogHistogram(bool lh)
{
    logHistogram=lh;
    update();
}

void QFLUTEditor::emitLUTChanged()
{
    if (alphaMode) emit lutChanged(getCurrentAlphaLUT());
    else emit lutChanged(getCurrentLUT());
}

void QFLUTEditor::ensureLutNodePositions()
{
    for (int i=0; i<lut_nodes.size(); i++) {
        QPointF p=lut_nodes[i]->pos();
        double mi, ma;
        getRangeForNode(lut_nodes[i], mi, ma);
        p.setX(qBound(mi, p.x(), ma));
        p.setY(qBound(lut_value_min, p.y(), lut_value_max));
        if (p!=lut_nodes[i]->pos()) lut_nodes[i]->setPos(p);
    }

    //scene()->setSceneRect(lut_min, lut_value_min, lut_max, lut_value_max);
}

void QFLUTEditor::normalizeHistogram()
{
    if (histogramY.size()<=0) return;
    double cnt=histogramY[0];
    for (int i=1; i<histogramY.size(); i++) {
        if (histogramY[i]>cnt) cnt=histogramY[i];
    }
    for (int i=0; i<histogramY.size(); i++) {
        histogramY[i]=histogramY[i]/cnt;
    }
    histogramX=histogramY;
    double bw=(lut_max-lut_min)/double(histogramX.size());
    for (int i=0; i<histogramX.size(); i++) {
        histogramX[i]=lut_min+double(i)*bw;
    }
}

void QFLUTEditor::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // draw gradients
    QTransform t;
    t.rotate(-90);
    QImage ix=JKQTPMathImageGetPaletteImage(JKQTPMathImageGRAY, 255);
    QImage iy, ix2;
    QVector<int> lut=getCurrentLUT(255);
    QVector<int> initLut(255);
    for (int i=0; i<255; i++) initLut[i]=i;

    if (alphaMode) {
        iy=JKQTPMathImageGetAlphaPaletteImage(initLut.data(), initLut.size()-1, 256, gradientWidth).transformed(t);
        ix2=JKQTPMathImageGetAlphaPaletteImage(lut.data(), lut.size()-1, 256, gradientWidth);
    } else {
        iy=JKQTPMathImageGetPaletteImage(LUTPalette, 255).transformed(t);
        ix2=JKQTPMathImageGetPaletteImage(lut.data(), lut.size()-1, 256);
    }
    painter.drawImage(QRectF(borderWidth()+gradientWidth, borderWidth(), gradientWidth-2, height()-borderWidthBoth()), iy);
    painter.drawImage(QRectF(borderWidthWithGradient(), height()+2-borderWidthWithGradient(), width()-borderWidthBoth(), gradientWidth-2), ix);
    painter.drawImage(QRectF(borderWidthWithGradient(), height()+2-borderWidthWithGradient()+gradientWidth, width()-borderWidthBoth(), gradientWidth-2), ix2);

    // draw background
    QRectF r(borderWidthWithGradient(), borderWidth(), width()-borderWidthBoth(), height()-borderWidthBoth());
    QPen pls=QPen(QColor("black"));
    pls.setWidthF(1);
    painter.setPen(pls);
    painter.fillRect(r, QColor("lightgrey"));
    painter.drawLine(r.bottomLeft(), r.bottomRight());
    painter.drawLine(r.bottomLeft(), r.topLeft());

    // draw histogram
    QPen plh=QPen(QColor("darkgrey"));
    plh.setWidthF(2);
    painter.setPen(plh);
    if (logHistogram) {
        QVector<double> lhy;
        double minl=0;
        double maxl=0;
        bool first=true;
        for (int i=0; i<histogramY.size(); i++) {
            double lh=NAN;
            if (histogramY[i]>0) {
                lh=log10(histogramY[i]);
                if (first) {
                    minl=maxl=lh;
                    first=false;
                } else {
                    minl=qMin(lh, minl);
                    maxl=qMax(lh, maxl);
                }
            }
            lhy<<lh;
        }
        for (int i=0; i<histogramY.size(); i++) {
            if (JKQTPIsOKFloat(lhy[i])) {
                lhy[i]=(lhy[i]-minl)/(maxl-minl);
            } else {
                lhy[i]=0;
            }
        }
        for (int i=0; i<qMin(histogramX.size(), lhy.size()); i++) {
            double hx=lut_min+double(i)*(lut_max-lut_min)/double(qMin(histogramX.size(), lhy.size()));
            double hy=lut_value_min+lhy[i]*(lut_value_max-lut_value_min);
            painter.drawLine(map(QPointF(hx, lut_value_min)), map(QPointF(hx, hy)));
        }
    } else {
        for (int i=0; i<qMin(histogramX.size(), histogramY.size()); i++) {
            double hx=lut_min+double(i)*(lut_max-lut_min)/double(qMin(histogramX.size(), histogramY.size()));
            double hy=lut_value_min+histogramY[i]*(lut_value_max-lut_value_min);
            painter.drawLine(map(QPointF(hx, lut_value_min)), map(QPointF(hx, hy)));
        }
    }

    // draw box
    painter.setPen(pls);
    painter.drawLine(r.bottomLeft(), r.bottomRight());
    painter.drawLine(r.bottomLeft(), r.topLeft());

    // draw lines
    for (int i=1; i<lut_nodes.size(); i++) {
        QPointF p=lut_nodes[i-1]->pos();
        QPen pl=QPen(lut_nodes[i-1]->getColor().darker());
        pl.setWidthF(1.5);
        painter.setPen(pl);
        painter.drawLine(map(p), map(lut_nodes[i]->pos()));
    }

    // draw nodes
    for (int i=0; i<lut_nodes.size(); i++) {
        lut_nodes[i]->paint(&painter, currentNode==i);
    }

}

void QFLUTEditor::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPointF p=invMap(event->pos());
    if (event->button()==Qt::LeftButton && p.x()>=lut_min && p.x()<lut_max
        && p.y()>=lut_value_min && p.y()<lut_value_max) {

        int idx=-1;
        for (int i=0; i<lut_nodes.size(); i++) {
            QPointF d=(map(lut_nodes[i]->pos())-QPointF(event->pos()));
            if (d.x()*d.x()+d.y()*d.y()<lut_nodes[i]->getNodeSize()*lut_nodes[i]->getNodeSize()) {
                idx=i;
                break;
            }
        }

        if (idx<0) addNode(p);
        else deleteNode(idx);
    }
}

void QFLUTEditor::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug()<<"move( cn="<<currentNode<<" but="<<event->buttons();
    if (event->buttons()==Qt::LeftButton) {
        if (currentNode>=0 && currentNode<lut_nodes.size()) {
            lut_nodes[currentNode]->setPosCorrected(invMap(QPointF(event->pos())));
            updateLUT();
        } else {
            currentNode=-1;
            updateLUT();
        }
        event->accept();
    } else {
        QFrame::mouseMoveEvent(event);
    }
}

void QFLUTEditor::mousePressEvent(QMouseEvent *event)
{
    int idx=-1;
    if (event->button()==Qt::LeftButton) {
        for (int i=0; i<lut_nodes.size(); i++) {
            QPointF d=(map(lut_nodes[i]->pos())-QPointF(event->pos()));
            if (d.x()*d.x()+d.y()*d.y()<lut_nodes[i]->getNodeSize()*lut_nodes[i]->getNodeSize()) {
                idx=i;
                break;
            }
        }
    }
    //qDebug()<<idx<<event->button();
    currentNode=idx;
    updateLUT();
    event->accept();
    //QFrame::mousePressEvent(event);
}

void QFLUTEditor::mouseReleaseEvent(QMouseEvent *event)
{
    currentNode=-1;
    updateLUT();
    event->accept();
    //QFrame::mouseReleaseEvent(event);
}



int QFLUTEditor::borderWidth() const
{
    return 5;
}

int QFLUTEditor::borderWidthWithGradient() const
{
    return gradientWidth*2+borderWidth();
}

int QFLUTEditor::borderWidthBoth() const
{
    return gradientWidth*2+2*borderWidth();
}

void QFLUTEditor::updateLUT()
{
    update();
    lutTimer->stop();
    lutTimer->start();
}


QFLUTEditorNode::QFLUTEditorNode(int lut, QColor color, QFLUTEditor *graphWidget)
{
    this->color=color;
    this->lut=lut;
    this->graph=graphWidget;
    this->nodeSize=5;
}

void QFLUTEditorNode::setPosCorrected(const QPointF &pos)
{
    double mi, ma;
    graph->getRangeForNode(this, mi, ma);
    QPointF p;
    p.setX(qBound(mi, pos.x(), ma));
    p.setY(qBound(graph->getLutValueMin(), pos.y(), graph->getLutValueMax()));
    setPos(p);
}

void QFLUTEditorNode::paint(QPainter *painter, bool activated)
{
    qreal penWidth = 1;
    qreal radius=nodeSize/2.0;

    QPen p=QPen(color.darker());
    p.setWidthF(penWidth);
    if (activated) {
        p.setColor(color.lighter());
        p.setWidthF(penWidth*2.0);
    }
    QBrush b=QBrush(color);

    painter->setPen(p);
    painter->setBrush(b);
    painter->drawEllipse(graph->map(pos()), radius, radius);


}



