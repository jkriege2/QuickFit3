set terminal pdf color font "Arial,3" size 48cm,48cm
set output 'hela4xp_em300_0.0005s_1785.7Hz_2.pdf'
set key inside right top samplen 1 spacing .5 font "Arial,2" box
set logscale x
set xrange [*:11.200000]
set yrange [0.999239:1.009239]
set ytics nomirror
set y2tics
set y2range [0.999041:1.009041]
set multiplot layout 16,16 title "correlations D:\kriegerj\daten\spim\2011_05_25\hela4xp_em300_0.0005s_1785.7Hz_2.tif"
set title 'frame [0, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 1 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 3 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 5 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 7 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 9 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 10 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 11 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 12 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 13 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 14 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 15 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 16 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 17 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 18 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 19 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 20 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 21 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 22 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 23 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 24 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 25 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 26 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 27 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 28 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 29 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 30 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 31 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 32 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 33 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 34 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 35 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 36 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 37 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 38 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 39 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 40 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 41 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 42 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 43 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 44 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 45 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 46 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 47 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 48 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 49 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 50 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 51 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 52 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 53 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 54 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 55 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 56 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 57 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 58 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 59 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 60 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 61 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 62 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 63 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 64 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 65 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 66 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 67 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 68 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 69 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 70 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 71 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 72 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 73 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 74 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 75 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 76 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 77 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 78 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 79 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 80 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 81 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 82 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 83 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 84 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 85 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 86 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 87 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 88 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 89 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 90 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 91 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 92 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 93 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 94 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 95 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 96 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 97 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 98 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 99 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 100 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 101 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 102 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 103 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 104 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 105 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 106 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 107 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 108 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 109 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 110 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 111 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 112 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 113 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 114 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 115 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 116 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 117 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 118 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 119 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 120 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 121 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 122 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 123 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 124 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 125 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 126 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 127 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 128 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 129 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 130 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 131 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 132 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 133 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 134 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 135 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 136 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 137 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 138 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 139 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 140 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 141 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 142 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 143 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 144 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 145 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 146 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 147 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 148 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 149 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 150 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 151 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 152 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 153 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 154 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 155 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 156 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 157 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 158 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 159 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 160 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 161 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 162 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 163 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 164 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 165 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 166 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 167 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 168 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 169 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 170 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 171 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 172 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 173 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 174 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 175 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 176 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 177 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 178 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 179 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 180 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 181 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 182 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 183 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 184 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 185 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 186 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 187 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 188 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 189 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 190 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 191 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 192 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 193 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 194 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 195 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 196 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 197 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 198 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 199 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 200 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 201 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 202 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 203 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 204 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 205 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 206 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 207 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 208 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 209 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 210 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 211 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 212 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 213 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 214 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 215 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 216 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 217 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 218 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 219 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 220 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 221 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 222 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 223 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 224 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 225 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 226 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 227 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 228 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 229 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 230 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 231 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 232 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 233 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 234 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 235 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 236 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 237 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 238 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 239 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 240 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 241 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 242 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 243 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 244 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 245 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 246 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 247 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 248 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 249 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 250 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 251 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 252 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 253 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 254 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 255 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:5 axes x1y2 title 'ccf down' with lines
unset multiplot
set yrange [1:1.009083]
set ytics nomirror
set y2tics
set y2range [1:1.008485]
set multiplot layout 16,16 title "correlations D:\kriegerj\daten\spim\2011_05_25\hela4xp_em300_0.0005s_1785.7Hz_2.tif"
set title 'frame [0, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 1 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 3 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 5 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 7 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 9 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 10 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 11 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 12 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 13 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 14 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 15 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 16 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 17 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 18 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 19 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 20 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 21 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 22 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 23 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 24 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 25 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 26 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 27 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 28 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 29 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 30 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 31 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 32 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 33 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 34 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 35 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 36 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 37 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 38 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 39 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 40 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 41 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 42 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 43 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 44 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 45 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 46 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 47 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 48 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 49 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 50 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 51 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 52 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 53 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 54 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 55 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 56 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 57 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 58 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 59 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 60 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 61 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 62 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 63 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 64 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 65 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 66 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 67 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 68 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 69 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 70 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 71 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 72 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 73 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 74 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 75 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 76 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 77 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 78 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 79 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 80 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 81 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 82 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 83 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 84 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 85 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 86 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 87 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 88 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 89 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 90 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 91 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 92 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 93 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 94 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 95 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 96 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 97 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 98 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 99 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 100 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 101 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 102 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 103 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 104 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 105 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 106 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 107 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 108 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 109 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 110 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 111 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 112 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 113 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 114 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 115 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 116 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 117 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 118 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 119 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 120 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 121 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 122 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 123 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 124 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 125 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 126 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 127 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 128 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 129 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 130 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 131 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 132 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 133 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 134 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 135 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 136 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 137 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 138 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 139 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 140 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 141 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 142 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 143 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 144 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 145 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 146 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 147 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 148 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 149 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 150 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 151 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 152 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 153 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 154 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 155 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 156 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 157 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 158 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 159 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 160 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 161 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 162 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 163 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 164 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 165 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 166 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 167 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 168 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 169 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 170 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 171 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 172 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 173 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 174 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 175 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 176 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 177 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 178 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 179 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 180 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 181 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 182 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 183 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 184 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 185 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 186 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 187 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 188 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 189 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 190 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 191 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 192 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 193 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 194 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 195 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 196 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 197 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 198 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 199 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 200 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 201 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 202 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 203 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 204 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 205 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 206 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 207 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 208 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 209 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 210 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 211 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 212 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 213 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 214 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 215 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 216 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 217 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 218 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 219 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 220 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 221 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 222 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 223 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 224 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 225 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 226 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 227 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 228 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 229 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 230 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 231 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 232 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 233 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 234 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 235 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 236 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 237 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 238 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 239 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 240 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 241 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 242 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 243 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 244 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 245 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 246 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 247 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 248 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 249 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 250 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 251 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 252 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 253 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 254 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 255 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:5 axes x1y2 title 'ccf down' with lines
unset multiplot
set yrange [*:*]
set ytics nomirror
set y2tics
set y2range [*:*]
set multiplot layout 16,16 title "correlations D:\kriegerj\daten\spim\2011_05_25\hela4xp_em300_0.0005s_1785.7Hz_2.tif"
set title 'frame [0, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 1 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 3 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 5 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 7 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 9 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 10 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 11 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 12 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 13 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 14 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 15 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 16 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 17 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 18 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 19 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 20 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 21 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 22 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 23 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 24 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 25 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 26 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 27 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 28 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 29 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 30 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 31 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 32 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 33 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 34 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 35 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 36 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 37 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 38 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 39 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 40 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 41 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 42 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 43 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 44 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 45 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 46 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 47 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 48 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 49 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 50 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 51 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 52 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 53 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 54 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 55 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 56 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 57 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 58 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 59 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 60 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 61 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 62 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 63 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 64 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 65 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 66 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 67 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 68 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 69 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 70 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 71 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 72 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 73 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 74 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 75 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 76 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 77 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 78 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 79 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 80 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 81 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 82 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 83 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 84 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 85 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 86 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 87 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 88 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 89 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 90 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 91 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 92 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 93 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 94 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 95 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 96 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 97 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 98 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 99 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 100 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 101 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 102 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 103 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 104 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 105 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 106 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 107 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 108 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 109 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 110 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 111 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 112 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 113 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 114 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 115 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 116 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 117 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 118 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 119 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 120 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 121 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 122 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 123 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 124 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 125 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 126 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 127 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 128 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 129 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 130 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 131 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 132 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 133 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 134 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 135 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 136 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 137 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 138 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 139 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 140 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 141 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 142 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 143 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 144 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 145 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 146 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 147 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 148 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 149 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 150 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 151 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 152 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 153 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 154 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 155 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 156 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 157 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 158 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 159 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 160 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 161 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 162 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 163 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 164 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 165 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 166 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 167 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 168 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 169 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 170 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 171 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 172 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 173 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 174 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 175 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 176 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 177 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 178 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 179 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 180 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 181 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 182 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 183 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 184 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 185 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 186 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 187 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 188 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 189 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 190 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 191 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 192 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 193 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 194 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 195 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 196 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 197 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 198 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 199 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 200 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 201 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 202 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 203 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 204 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 205 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 206 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 207 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 208 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 209 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 210 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 211 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 212 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 213 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 214 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 215 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 216 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 217 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 218 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 219 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 220 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 221 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 222 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 223 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 224 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 225 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 226 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 227 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 228 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 229 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 230 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 231 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 232 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 233 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 234 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 235 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 236 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 237 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 238 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 239 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 240 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 241 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 242 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 243 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 244 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 245 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 246 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 247 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 248 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 249 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 250 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 251 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 252 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 253 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 254 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 255 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:5 axes x1y2 title 'ccf down' with lines
unset multiplot
set yrange [1:*]
set ytics nomirror
set y2tics
set y2range [1:*]
set multiplot layout 16,16 title "correlations D:\kriegerj\daten\spim\2011_05_25\hela4xp_em300_0.0005s_1785.7Hz_2.tif"
set title 'frame [0, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 0 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 1 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 1 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 2 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 3 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 3 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 4 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 5 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 5 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 6 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 7 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 7 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 8 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 9 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 9 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 10 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 10 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 11 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 11 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 12 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 12 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 13 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 13 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 14 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 14 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 15 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 15 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 16 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 16 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 17 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 17 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 18 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 18 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 19 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 19 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 20 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 20 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 21 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 21 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 22 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 22 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 23 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 23 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 24 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 24 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 25 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 25 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 26 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 26 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 27 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 27 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 28 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 28 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 29 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 29 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 30 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 30 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 31 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 31 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 32 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 32 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 33 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 33 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 34 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 34 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 35 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 35 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 36 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 36 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 37 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 37 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 38 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 38 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 39 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 39 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 40 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 40 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 41 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 41 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 42 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 42 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 43 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 43 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 44 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 44 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 45 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 45 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 46 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 46 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 47 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 47 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 48 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 48 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 49 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 49 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 50 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 50 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 51 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 51 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 52 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 52 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 53 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 53 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 54 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 54 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 55 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 55 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 56 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 56 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 57 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 57 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 58 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 58 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 59 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 59 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 60 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 60 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 61 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 61 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 62 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 62 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 63 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 63 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 64 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 64 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 65 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 65 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 66 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 66 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 67 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 67 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 68 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 68 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 69 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 69 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 70 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 70 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 71 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 71 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 72 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 72 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 73 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 73 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 74 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 74 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 75 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 75 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 76 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 76 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 77 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 77 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 78 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 78 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 79 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 79 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 80 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 80 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 81 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 81 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 82 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 82 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 83 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 83 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 84 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 84 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 85 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 85 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 86 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 86 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 87 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 87 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 88 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 88 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 89 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 89 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 90 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 90 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 91 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 91 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 92 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 92 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 93 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 93 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 94 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 94 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 95 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 95 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 96 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 96 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 97 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 97 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 98 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 98 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 99 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 99 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 100 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 100 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 101 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 101 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 102 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 102 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 103 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 103 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 104 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 104 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 105 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 105 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 106 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 106 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 107 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 107 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 108 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 108 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 109 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 109 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 110 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 110 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 111 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 111 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 112 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 112 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 113 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 113 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 114 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 114 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 115 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 115 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 116 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 116 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 117 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 117 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 118 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 118 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 119 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 119 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 120 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 120 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 121 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 121 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 122 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 122 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 123 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 123 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 124 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 124 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 125 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 125 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 126 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 126 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 127 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 127 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 128 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 128 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 129 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 129 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 130 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 130 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 131 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 131 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 132 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 132 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 133 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 133 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 134 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 134 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 135 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 135 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 136 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 136 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 137 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 137 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 138 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 138 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 139 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 139 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 140 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 140 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 141 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 141 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 142 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 142 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 143 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 143 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 144 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 144 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 145 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 145 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 146 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 146 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 147 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 147 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 148 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 148 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 149 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 149 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 150 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 150 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 151 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 151 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 152 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 152 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 153 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 153 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 154 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 154 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 155 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 155 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 156 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 156 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 157 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 157 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 158 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 158 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 159 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 159 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 160 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 160 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 161 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 161 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 162 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 162 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 163 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 163 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 164 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 164 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 165 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 165 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 166 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 166 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 167 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 167 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 168 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 168 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 169 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 169 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 170 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 170 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 171 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 171 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 172 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 172 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 173 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 173 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 174 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 174 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 175 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 175 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 176 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 176 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 177 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 177 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 178 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 178 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 179 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 179 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 180 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 180 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 181 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 181 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 182 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 182 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 183 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 183 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 184 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 184 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 185 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 185 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 186 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 186 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 187 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 187 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 188 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 188 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 189 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 189 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 190 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 190 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 191 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 191 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 192 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 192 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 193 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 193 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 194 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 194 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 195 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 195 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 196 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 196 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 197 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 197 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 198 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 198 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 199 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 199 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 200 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 200 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 201 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 201 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 202 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 202 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 203 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 203 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 204 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 204 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 205 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 205 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 206 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 206 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 207 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 207 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 208 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 208 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 209 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 209 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 210 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 210 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 211 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 211 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 212 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 212 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 213 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 213 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 214 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 214 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 215 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 215 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 216 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 216 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 217 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 217 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 218 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 218 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 219 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 219 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 220 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 220 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 221 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 221 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 222 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 222 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 223 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 223 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 224 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 224 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 225 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 225 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 226 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 226 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 227 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 227 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 228 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 228 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 229 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 229 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 230 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 230 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 231 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 231 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 232 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 232 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 233 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 233 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 234 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 234 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 235 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 235 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 236 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 236 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 237 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 237 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 238 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 238 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 239 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 239 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [0, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 240 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 240 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [1, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 241 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 241 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [2, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 242 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 242 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [3, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 243 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 243 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [4, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 244 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 244 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [5, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 245 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 245 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [6, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 246 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 246 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [7, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 247 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 247 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [8, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 248 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 248 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [9, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 249 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 249 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [10, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 250 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 250 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [11, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 251 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 251 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [12, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 252 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 252 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [13, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 253 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 253 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [14, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 254 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 254 using 1:5 axes x1y2 title 'ccf down' with lines
set title 'frame [15, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 255 axes x1y1 title 'acf' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:2 axes x1y2 title 'ccf left' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:3 axes x1y2 title 'ccf right' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:4 axes x1y2 title 'ccf up' with lines,\
     'hela4xp_em300_0.0005s_1785.7Hz_2.crosscorrelation.dat' index 255 using 1:5 axes x1y2 title 'ccf down' with lines
unset multiplot
set yrange [*:*]
set ytics nomirror
set y2tics
set multiplot layout 16,16 title "correlations D:\kriegerj\daten\spim\2011_05_25\hela4xp_em300_0.0005s_1785.7Hz_2.tif"
set title 'frame [0, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 0 axes x1y1 title 'acf' with lines
set title 'frame [1, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 1 axes x1y1 title 'acf' with lines
set title 'frame [2, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 2 axes x1y1 title 'acf' with lines
set title 'frame [3, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 3 axes x1y1 title 'acf' with lines
set title 'frame [4, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 4 axes x1y1 title 'acf' with lines
set title 'frame [5, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 5 axes x1y1 title 'acf' with lines
set title 'frame [6, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 6 axes x1y1 title 'acf' with lines
set title 'frame [7, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 7 axes x1y1 title 'acf' with lines
set title 'frame [8, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 8 axes x1y1 title 'acf' with lines
set title 'frame [9, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 9 axes x1y1 title 'acf' with lines
set title 'frame [10, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 10 axes x1y1 title 'acf' with lines
set title 'frame [11, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 11 axes x1y1 title 'acf' with lines
set title 'frame [12, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 12 axes x1y1 title 'acf' with lines
set title 'frame [13, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 13 axes x1y1 title 'acf' with lines
set title 'frame [14, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 14 axes x1y1 title 'acf' with lines
set title 'frame [15, 0]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 15 axes x1y1 title 'acf' with lines
set title 'frame [0, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 16 axes x1y1 title 'acf' with lines
set title 'frame [1, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 17 axes x1y1 title 'acf' with lines
set title 'frame [2, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 18 axes x1y1 title 'acf' with lines
set title 'frame [3, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 19 axes x1y1 title 'acf' with lines
set title 'frame [4, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 20 axes x1y1 title 'acf' with lines
set title 'frame [5, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 21 axes x1y1 title 'acf' with lines
set title 'frame [6, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 22 axes x1y1 title 'acf' with lines
set title 'frame [7, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 23 axes x1y1 title 'acf' with lines
set title 'frame [8, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 24 axes x1y1 title 'acf' with lines
set title 'frame [9, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 25 axes x1y1 title 'acf' with lines
set title 'frame [10, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 26 axes x1y1 title 'acf' with lines
set title 'frame [11, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 27 axes x1y1 title 'acf' with lines
set title 'frame [12, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 28 axes x1y1 title 'acf' with lines
set title 'frame [13, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 29 axes x1y1 title 'acf' with lines
set title 'frame [14, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 30 axes x1y1 title 'acf' with lines
set title 'frame [15, 1]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 31 axes x1y1 title 'acf' with lines
set title 'frame [0, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 32 axes x1y1 title 'acf' with lines
set title 'frame [1, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 33 axes x1y1 title 'acf' with lines
set title 'frame [2, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 34 axes x1y1 title 'acf' with lines
set title 'frame [3, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 35 axes x1y1 title 'acf' with lines
set title 'frame [4, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 36 axes x1y1 title 'acf' with lines
set title 'frame [5, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 37 axes x1y1 title 'acf' with lines
set title 'frame [6, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 38 axes x1y1 title 'acf' with lines
set title 'frame [7, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 39 axes x1y1 title 'acf' with lines
set title 'frame [8, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 40 axes x1y1 title 'acf' with lines
set title 'frame [9, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 41 axes x1y1 title 'acf' with lines
set title 'frame [10, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 42 axes x1y1 title 'acf' with lines
set title 'frame [11, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 43 axes x1y1 title 'acf' with lines
set title 'frame [12, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 44 axes x1y1 title 'acf' with lines
set title 'frame [13, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 45 axes x1y1 title 'acf' with lines
set title 'frame [14, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 46 axes x1y1 title 'acf' with lines
set title 'frame [15, 2]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 47 axes x1y1 title 'acf' with lines
set title 'frame [0, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 48 axes x1y1 title 'acf' with lines
set title 'frame [1, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 49 axes x1y1 title 'acf' with lines
set title 'frame [2, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 50 axes x1y1 title 'acf' with lines
set title 'frame [3, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 51 axes x1y1 title 'acf' with lines
set title 'frame [4, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 52 axes x1y1 title 'acf' with lines
set title 'frame [5, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 53 axes x1y1 title 'acf' with lines
set title 'frame [6, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 54 axes x1y1 title 'acf' with lines
set title 'frame [7, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 55 axes x1y1 title 'acf' with lines
set title 'frame [8, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 56 axes x1y1 title 'acf' with lines
set title 'frame [9, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 57 axes x1y1 title 'acf' with lines
set title 'frame [10, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 58 axes x1y1 title 'acf' with lines
set title 'frame [11, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 59 axes x1y1 title 'acf' with lines
set title 'frame [12, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 60 axes x1y1 title 'acf' with lines
set title 'frame [13, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 61 axes x1y1 title 'acf' with lines
set title 'frame [14, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 62 axes x1y1 title 'acf' with lines
set title 'frame [15, 3]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 63 axes x1y1 title 'acf' with lines
set title 'frame [0, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 64 axes x1y1 title 'acf' with lines
set title 'frame [1, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 65 axes x1y1 title 'acf' with lines
set title 'frame [2, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 66 axes x1y1 title 'acf' with lines
set title 'frame [3, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 67 axes x1y1 title 'acf' with lines
set title 'frame [4, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 68 axes x1y1 title 'acf' with lines
set title 'frame [5, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 69 axes x1y1 title 'acf' with lines
set title 'frame [6, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 70 axes x1y1 title 'acf' with lines
set title 'frame [7, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 71 axes x1y1 title 'acf' with lines
set title 'frame [8, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 72 axes x1y1 title 'acf' with lines
set title 'frame [9, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 73 axes x1y1 title 'acf' with lines
set title 'frame [10, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 74 axes x1y1 title 'acf' with lines
set title 'frame [11, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 75 axes x1y1 title 'acf' with lines
set title 'frame [12, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 76 axes x1y1 title 'acf' with lines
set title 'frame [13, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 77 axes x1y1 title 'acf' with lines
set title 'frame [14, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 78 axes x1y1 title 'acf' with lines
set title 'frame [15, 4]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 79 axes x1y1 title 'acf' with lines
set title 'frame [0, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 80 axes x1y1 title 'acf' with lines
set title 'frame [1, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 81 axes x1y1 title 'acf' with lines
set title 'frame [2, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 82 axes x1y1 title 'acf' with lines
set title 'frame [3, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 83 axes x1y1 title 'acf' with lines
set title 'frame [4, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 84 axes x1y1 title 'acf' with lines
set title 'frame [5, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 85 axes x1y1 title 'acf' with lines
set title 'frame [6, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 86 axes x1y1 title 'acf' with lines
set title 'frame [7, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 87 axes x1y1 title 'acf' with lines
set title 'frame [8, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 88 axes x1y1 title 'acf' with lines
set title 'frame [9, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 89 axes x1y1 title 'acf' with lines
set title 'frame [10, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 90 axes x1y1 title 'acf' with lines
set title 'frame [11, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 91 axes x1y1 title 'acf' with lines
set title 'frame [12, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 92 axes x1y1 title 'acf' with lines
set title 'frame [13, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 93 axes x1y1 title 'acf' with lines
set title 'frame [14, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 94 axes x1y1 title 'acf' with lines
set title 'frame [15, 5]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 95 axes x1y1 title 'acf' with lines
set title 'frame [0, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 96 axes x1y1 title 'acf' with lines
set title 'frame [1, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 97 axes x1y1 title 'acf' with lines
set title 'frame [2, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 98 axes x1y1 title 'acf' with lines
set title 'frame [3, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 99 axes x1y1 title 'acf' with lines
set title 'frame [4, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 100 axes x1y1 title 'acf' with lines
set title 'frame [5, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 101 axes x1y1 title 'acf' with lines
set title 'frame [6, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 102 axes x1y1 title 'acf' with lines
set title 'frame [7, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 103 axes x1y1 title 'acf' with lines
set title 'frame [8, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 104 axes x1y1 title 'acf' with lines
set title 'frame [9, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 105 axes x1y1 title 'acf' with lines
set title 'frame [10, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 106 axes x1y1 title 'acf' with lines
set title 'frame [11, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 107 axes x1y1 title 'acf' with lines
set title 'frame [12, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 108 axes x1y1 title 'acf' with lines
set title 'frame [13, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 109 axes x1y1 title 'acf' with lines
set title 'frame [14, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 110 axes x1y1 title 'acf' with lines
set title 'frame [15, 6]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 111 axes x1y1 title 'acf' with lines
set title 'frame [0, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 112 axes x1y1 title 'acf' with lines
set title 'frame [1, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 113 axes x1y1 title 'acf' with lines
set title 'frame [2, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 114 axes x1y1 title 'acf' with lines
set title 'frame [3, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 115 axes x1y1 title 'acf' with lines
set title 'frame [4, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 116 axes x1y1 title 'acf' with lines
set title 'frame [5, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 117 axes x1y1 title 'acf' with lines
set title 'frame [6, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 118 axes x1y1 title 'acf' with lines
set title 'frame [7, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 119 axes x1y1 title 'acf' with lines
set title 'frame [8, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 120 axes x1y1 title 'acf' with lines
set title 'frame [9, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 121 axes x1y1 title 'acf' with lines
set title 'frame [10, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 122 axes x1y1 title 'acf' with lines
set title 'frame [11, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 123 axes x1y1 title 'acf' with lines
set title 'frame [12, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 124 axes x1y1 title 'acf' with lines
set title 'frame [13, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 125 axes x1y1 title 'acf' with lines
set title 'frame [14, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 126 axes x1y1 title 'acf' with lines
set title 'frame [15, 7]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 127 axes x1y1 title 'acf' with lines
set title 'frame [0, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 128 axes x1y1 title 'acf' with lines
set title 'frame [1, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 129 axes x1y1 title 'acf' with lines
set title 'frame [2, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 130 axes x1y1 title 'acf' with lines
set title 'frame [3, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 131 axes x1y1 title 'acf' with lines
set title 'frame [4, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 132 axes x1y1 title 'acf' with lines
set title 'frame [5, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 133 axes x1y1 title 'acf' with lines
set title 'frame [6, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 134 axes x1y1 title 'acf' with lines
set title 'frame [7, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 135 axes x1y1 title 'acf' with lines
set title 'frame [8, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 136 axes x1y1 title 'acf' with lines
set title 'frame [9, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 137 axes x1y1 title 'acf' with lines
set title 'frame [10, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 138 axes x1y1 title 'acf' with lines
set title 'frame [11, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 139 axes x1y1 title 'acf' with lines
set title 'frame [12, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 140 axes x1y1 title 'acf' with lines
set title 'frame [13, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 141 axes x1y1 title 'acf' with lines
set title 'frame [14, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 142 axes x1y1 title 'acf' with lines
set title 'frame [15, 8]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 143 axes x1y1 title 'acf' with lines
set title 'frame [0, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 144 axes x1y1 title 'acf' with lines
set title 'frame [1, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 145 axes x1y1 title 'acf' with lines
set title 'frame [2, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 146 axes x1y1 title 'acf' with lines
set title 'frame [3, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 147 axes x1y1 title 'acf' with lines
set title 'frame [4, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 148 axes x1y1 title 'acf' with lines
set title 'frame [5, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 149 axes x1y1 title 'acf' with lines
set title 'frame [6, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 150 axes x1y1 title 'acf' with lines
set title 'frame [7, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 151 axes x1y1 title 'acf' with lines
set title 'frame [8, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 152 axes x1y1 title 'acf' with lines
set title 'frame [9, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 153 axes x1y1 title 'acf' with lines
set title 'frame [10, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 154 axes x1y1 title 'acf' with lines
set title 'frame [11, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 155 axes x1y1 title 'acf' with lines
set title 'frame [12, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 156 axes x1y1 title 'acf' with lines
set title 'frame [13, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 157 axes x1y1 title 'acf' with lines
set title 'frame [14, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 158 axes x1y1 title 'acf' with lines
set title 'frame [15, 9]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 159 axes x1y1 title 'acf' with lines
set title 'frame [0, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 160 axes x1y1 title 'acf' with lines
set title 'frame [1, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 161 axes x1y1 title 'acf' with lines
set title 'frame [2, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 162 axes x1y1 title 'acf' with lines
set title 'frame [3, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 163 axes x1y1 title 'acf' with lines
set title 'frame [4, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 164 axes x1y1 title 'acf' with lines
set title 'frame [5, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 165 axes x1y1 title 'acf' with lines
set title 'frame [6, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 166 axes x1y1 title 'acf' with lines
set title 'frame [7, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 167 axes x1y1 title 'acf' with lines
set title 'frame [8, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 168 axes x1y1 title 'acf' with lines
set title 'frame [9, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 169 axes x1y1 title 'acf' with lines
set title 'frame [10, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 170 axes x1y1 title 'acf' with lines
set title 'frame [11, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 171 axes x1y1 title 'acf' with lines
set title 'frame [12, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 172 axes x1y1 title 'acf' with lines
set title 'frame [13, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 173 axes x1y1 title 'acf' with lines
set title 'frame [14, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 174 axes x1y1 title 'acf' with lines
set title 'frame [15, 10]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 175 axes x1y1 title 'acf' with lines
set title 'frame [0, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 176 axes x1y1 title 'acf' with lines
set title 'frame [1, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 177 axes x1y1 title 'acf' with lines
set title 'frame [2, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 178 axes x1y1 title 'acf' with lines
set title 'frame [3, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 179 axes x1y1 title 'acf' with lines
set title 'frame [4, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 180 axes x1y1 title 'acf' with lines
set title 'frame [5, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 181 axes x1y1 title 'acf' with lines
set title 'frame [6, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 182 axes x1y1 title 'acf' with lines
set title 'frame [7, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 183 axes x1y1 title 'acf' with lines
set title 'frame [8, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 184 axes x1y1 title 'acf' with lines
set title 'frame [9, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 185 axes x1y1 title 'acf' with lines
set title 'frame [10, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 186 axes x1y1 title 'acf' with lines
set title 'frame [11, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 187 axes x1y1 title 'acf' with lines
set title 'frame [12, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 188 axes x1y1 title 'acf' with lines
set title 'frame [13, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 189 axes x1y1 title 'acf' with lines
set title 'frame [14, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 190 axes x1y1 title 'acf' with lines
set title 'frame [15, 11]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 191 axes x1y1 title 'acf' with lines
set title 'frame [0, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 192 axes x1y1 title 'acf' with lines
set title 'frame [1, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 193 axes x1y1 title 'acf' with lines
set title 'frame [2, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 194 axes x1y1 title 'acf' with lines
set title 'frame [3, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 195 axes x1y1 title 'acf' with lines
set title 'frame [4, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 196 axes x1y1 title 'acf' with lines
set title 'frame [5, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 197 axes x1y1 title 'acf' with lines
set title 'frame [6, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 198 axes x1y1 title 'acf' with lines
set title 'frame [7, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 199 axes x1y1 title 'acf' with lines
set title 'frame [8, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 200 axes x1y1 title 'acf' with lines
set title 'frame [9, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 201 axes x1y1 title 'acf' with lines
set title 'frame [10, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 202 axes x1y1 title 'acf' with lines
set title 'frame [11, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 203 axes x1y1 title 'acf' with lines
set title 'frame [12, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 204 axes x1y1 title 'acf' with lines
set title 'frame [13, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 205 axes x1y1 title 'acf' with lines
set title 'frame [14, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 206 axes x1y1 title 'acf' with lines
set title 'frame [15, 12]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 207 axes x1y1 title 'acf' with lines
set title 'frame [0, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 208 axes x1y1 title 'acf' with lines
set title 'frame [1, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 209 axes x1y1 title 'acf' with lines
set title 'frame [2, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 210 axes x1y1 title 'acf' with lines
set title 'frame [3, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 211 axes x1y1 title 'acf' with lines
set title 'frame [4, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 212 axes x1y1 title 'acf' with lines
set title 'frame [5, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 213 axes x1y1 title 'acf' with lines
set title 'frame [6, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 214 axes x1y1 title 'acf' with lines
set title 'frame [7, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 215 axes x1y1 title 'acf' with lines
set title 'frame [8, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 216 axes x1y1 title 'acf' with lines
set title 'frame [9, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 217 axes x1y1 title 'acf' with lines
set title 'frame [10, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 218 axes x1y1 title 'acf' with lines
set title 'frame [11, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 219 axes x1y1 title 'acf' with lines
set title 'frame [12, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 220 axes x1y1 title 'acf' with lines
set title 'frame [13, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 221 axes x1y1 title 'acf' with lines
set title 'frame [14, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 222 axes x1y1 title 'acf' with lines
set title 'frame [15, 13]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 223 axes x1y1 title 'acf' with lines
set title 'frame [0, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 224 axes x1y1 title 'acf' with lines
set title 'frame [1, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 225 axes x1y1 title 'acf' with lines
set title 'frame [2, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 226 axes x1y1 title 'acf' with lines
set title 'frame [3, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 227 axes x1y1 title 'acf' with lines
set title 'frame [4, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 228 axes x1y1 title 'acf' with lines
set title 'frame [5, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 229 axes x1y1 title 'acf' with lines
set title 'frame [6, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 230 axes x1y1 title 'acf' with lines
set title 'frame [7, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 231 axes x1y1 title 'acf' with lines
set title 'frame [8, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 232 axes x1y1 title 'acf' with lines
set title 'frame [9, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 233 axes x1y1 title 'acf' with lines
set title 'frame [10, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 234 axes x1y1 title 'acf' with lines
set title 'frame [11, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 235 axes x1y1 title 'acf' with lines
set title 'frame [12, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 236 axes x1y1 title 'acf' with lines
set title 'frame [13, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 237 axes x1y1 title 'acf' with lines
set title 'frame [14, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 238 axes x1y1 title 'acf' with lines
set title 'frame [15, 14]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 239 axes x1y1 title 'acf' with lines
set title 'frame [0, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 240 axes x1y1 title 'acf' with lines
set title 'frame [1, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 241 axes x1y1 title 'acf' with lines
set title 'frame [2, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 242 axes x1y1 title 'acf' with lines
set title 'frame [3, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 243 axes x1y1 title 'acf' with lines
set title 'frame [4, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 244 axes x1y1 title 'acf' with lines
set title 'frame [5, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 245 axes x1y1 title 'acf' with lines
set title 'frame [6, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 246 axes x1y1 title 'acf' with lines
set title 'frame [7, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 247 axes x1y1 title 'acf' with lines
set title 'frame [8, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 248 axes x1y1 title 'acf' with lines
set title 'frame [9, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 249 axes x1y1 title 'acf' with lines
set title 'frame [10, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 250 axes x1y1 title 'acf' with lines
set title 'frame [11, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 251 axes x1y1 title 'acf' with lines
set title 'frame [12, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 252 axes x1y1 title 'acf' with lines
set title 'frame [13, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 253 axes x1y1 title 'acf' with lines
set title 'frame [14, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 254 axes x1y1 title 'acf' with lines
set title 'frame [15, 15]'
plot 'hela4xp_em300_0.0005s_1785.7Hz_2.autocorrelation.dat' index 255 axes x1y1 title 'acf' with lines
unset multiplot
set output
