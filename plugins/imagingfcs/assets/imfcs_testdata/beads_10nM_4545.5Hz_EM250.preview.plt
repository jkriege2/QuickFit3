set terminal pdf color font "Arial,3" size 14cm,3cm
set output 'beads_10nM_4545.5Hz_EM250.preview8.pdf'
set logscale x
set xrange [*:2.640000]
set key inside right top samplen 1 spacing .5 font "Arial,2" box
set yrange [0.997703:1.018723]
set ytics nomirror
set y2tics
set y2range [0.997555:1.013288]
set multiplot layout 1,4 title "Correlations D:\kriegerj\daten\spim\2011_05_11\beads_10nM_4545.5Hz_EM250.tif"
set title 'frame [0, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:5 axes x1y2 title 'ccf down' with lines
unset multiplot
set yrange [1:1.018723]
set ytics nomirror
set y2tics
set y2range [1:1.013288]
set multiplot layout 1,4 title "Correlations D:\kriegerj\daten\spim\2011_05_11\beads_10nM_4545.5Hz_EM250.tif"
set title 'frame [0, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:5 axes x1y2 title 'ccf down' with lines
unset multiplot
set yrange [*:*]
set ytics nomirror
set y2tics
set y2range [*:*]
set multiplot layout 1,4 title "Correlations D:\kriegerj\daten\spim\2011_05_11\beads_10nM_4545.5Hz_EM250.tif"
set title 'frame [0, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:5 axes x1y2 title 'ccf down' with lines
unset multiplot
set yrange [1:*]
set ytics nomirror
set y2tics
set y2range [1:*]
set multiplot layout 1,4 title "Correlations D:\kriegerj\daten\spim\2011_05_11\beads_10nM_4545.5Hz_EM250.tif"
set title 'frame [0, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:5 axes x1y2 title 'ccf down' with lines
unset multiplot
set yrange [*:*]
set ytics nomirror
set y2tics
set multiplot layout 1,4 title "Correlations D:\kriegerj\daten\spim\2011_05_11\beads_10nM_4545.5Hz_EM250.tif"
set title 'frame [0, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines
set title 'frame [2, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines
set title 'frame [4, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines
set title 'frame [6, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines
set title 'frame [8, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines
unset multiplot
set terminal pdf color font "Arial,3" size 27cm,3cm
set output 'beads_10nM_4545.5Hz_EM250.preview16.pdf'
set logscale x
set xrange [*:2.640000]
set key inside right top samplen 1 spacing .5 font "Arial,2" box
set yrange [0.997703:1.018723]
set ytics nomirror
set y2tics
set y2range [0.997555:1.013288]
set multiplot layout 1,9 title "Correlations D:\kriegerj\daten\spim\2011_05_11\beads_10nM_4545.5Hz_EM250.tif"
set title 'frame [0, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 1 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 3 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 5 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 7 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:5 axes x1y2 title 'ccf down' with lines
unset multiplot
set yrange [1:1.018723]
set ytics nomirror
set y2tics
set y2range [1:1.013288]
set multiplot layout 1,9 title "Correlations D:\kriegerj\daten\spim\2011_05_11\beads_10nM_4545.5Hz_EM250.tif"
set title 'frame [0, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 1 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 3 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 5 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 7 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:5 axes x1y2 title 'ccf down' with lines
unset multiplot
set yrange [*:*]
set ytics nomirror
set y2tics
set y2range [*:*]
set multiplot layout 1,9 title "Correlations D:\kriegerj\daten\spim\2011_05_11\beads_10nM_4545.5Hz_EM250.tif"
set title 'frame [0, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 1 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 3 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 5 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 7 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:5 axes x1y2 title 'ccf down' with lines
unset multiplot
set yrange [1:*]
set ytics nomirror
set y2tics
set y2range [1:*]
set multiplot layout 1,9 title "Correlations D:\kriegerj\daten\spim\2011_05_11\beads_10nM_4545.5Hz_EM250.tif"
set title 'frame [0, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 0 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 1 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 1 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 2 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 3 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 3 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 4 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 5 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 5 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 6 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 7 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 7 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'beads_10nM_4545.5Hz_EM250.crosscorrelation.dat' index 8 using 1:5 axes x1y2 title 'ccf down' with lines
unset multiplot
set yrange [*:*]
set ytics nomirror
set y2tics
set multiplot layout 1,9 title "Correlations D:\kriegerj\daten\spim\2011_05_11\beads_10nM_4545.5Hz_EM250.tif"
set title 'frame [0, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines
set title 'frame [1, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 1 axes x1y1 title 'acf' with lines
set title 'frame [2, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines
set title 'frame [3, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 3 axes x1y1 title 'acf' with lines
set title 'frame [4, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines
set title 'frame [5, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 5 axes x1y1 title 'acf' with lines
set title 'frame [6, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines
set title 'frame [7, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 7 axes x1y1 title 'acf' with lines
set title 'frame [8, 0]'
plot 'beads_10nM_4545.5Hz_EM250.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines
unset multiplot
set output
