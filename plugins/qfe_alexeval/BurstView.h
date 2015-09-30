/*
Copyright (c) 2012-2015 by Sebastian Isbaner

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BURSTVIEW_H
#define BURSTVIEW_H

#include <QWidget>
#include "alexdata.h"
#include "jkqtplotter.h"


namespace Ui {
class BurstView;
}

/**
 * \brief Widget that displays individual bursts.
 * Bursts are loaded into a QTableView and plotted when double-clicked. The user can chosse between intenisty display with varying binwidth
 * and TTTR display that shows singel photon arrival times. The marker events are automatically plotted as excitation periods.
 * It is possible to show an offset of photon data before and after the burst. Additionally, the correlation function
 * of the dispayed data (including offset) is calculated and plotted.
 * Settings can be stored and restored with the functions storeSettings() and loadSettings().
 */
class BurstView : public QWidget
{
    Q_OBJECT
    
public:
    explicit BurstView(QWidget *parent = 0, AlexData* data = NULL);
    ~BurstView();

    /// store the settings of this widget in \a settings with prefix \a prefix (e.g. group)
    void storeSettings(QSettings& settings, const QString& prefix=QString("")) const;
    /// load the widget's settings from \a settings. the \a prefix must be the same as for storeSettings
    void loadSettings(QSettings& settings, const QString& prefix=QString(""));

    /// refill table
    void refresh();

private slots:

    /// connected to \a cellDoubleClicked()
    void cellSelected( int row, int column );

private:
    Ui::BurstView *ui;
    AlexData* ad;
    QStringList m_BurstTableHeader;

    /**
     * @brief fillTable reads AlexData.burstVectorDual and adds all bursts to the \a QTableView.
     */
    void fillTable();
    /**
     * @brief plotIntensity plots the intensity timetrace of a burst including offsets and marker events (by calling \verbatim plotMarker() \endverbatim )
     */
    void plotIntensity();
    /**
     * @brief plotTTTR plots single photon arrival times as impulse plot.
     */
    void plotTTTR();
    /**
     * @brief plotMarker plots the markers as filled color in a specified range
     * It uses a binary search algorithm to search for the marker entries corresponding to the burst.
     * @param plot the plotting widget
     * @param startTime
     * @param endTime
     * @param height height of the markers from 0 to +- \a height
     */
    void plotMarker(JKQtPlotter *plot, const double startTime, const double endTime, const double height=1);
    /**
     * @brief plotBurstCorr plots the correlation function of a single burst.
     * It uses the photons specified by \a burst.startIdx and \a burst.numberOfPhotons
     * @param burst
     */
    void plotBurstCorr(const BurstDual &burst);

    /// old implementation, not used
    /// it implements both a binary and a linear search for the marker events to check if they have the same result.
    void plotMarkerOld();
};


/**
 * @brief The JKQTPbarHorizontalGraphEdges class
 * This class implements a horizontal bar graph where you can specify the edges of the bins.
 * The range of the i^th bin is from x_i to x_{i+1}.
 * Note that you need N+1 x-values for N y-values.
 */
class JKQTPbarHorizontalGraphEdges: public JKQTPbarHorizontalGraph {
        Q_OBJECT
    public:
        /** \brief class constructor */
        JKQTPbarHorizontalGraphEdges(JKQtBasePlotter* parent=NULL);

        /** \brief plots the graph to the plotter object specified as parent */
        virtual void draw(JKQTPEnhancedPainter &painter);
        /** \brief get the maximum and minimum y-value of the graph
         *
         * The result is given in the two parameters which are call-by-reference parameters!
         */
        virtual bool getYMinMax(double& miny, double& maxy, double& smallestGreaterZero);

        /** \brief get the maximum and minimum x-value of the graph
         *
         * The result is given in the two parameters which are call-by-reference parameters!
         */
        virtual bool getXMinMax(double& minx, double& maxx, double& smallestGreaterZero);

        GetSetMacro(double,baseline);

    private:
        /** \brief offset of the bars w.r.t the y-axis in world coordinates  */
        double baseline;
};


#endif // BURSTVIEW_H
