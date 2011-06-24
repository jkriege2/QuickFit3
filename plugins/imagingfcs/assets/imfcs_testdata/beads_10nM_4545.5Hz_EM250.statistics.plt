set terminal pdf color font "Arial,10" size 29cm,21cm
set output 'beads_10nM_4545.5Hz_EM250.statistics.pdf'
set xlabel 'frame'
set ylabel 'pixel value'
set title "Statistics D:\\kriegerj\\daten\\spim\\2011_05_11\\beads_10nM_4545.5Hz_EM250.tif" noenhanced
plot 'beads_10nM_4545.5Hz_EM250.statistics.dat' using 1:2 title 'mean +/- stddev' with lines lt 1, 'beads_10nM_4545.5Hz_EM250.statistics.dat' using 1:(($2)-(($3)/2.0)) notitle with lines lt 1, 'beads_10nM_4545.5Hz_EM250.statistics.dat' using 1:(($2)+(($3)/2.0)) notitle with lines lt 1
set title "Statistics D:\\kriegerj\\daten\\spim\\2011_05_11\\beads_10nM_4545.5Hz_EM250.tif" noenhanced
plot 'beads_10nM_4545.5Hz_EM250.statistics.dat' using 1:2 title 'mean' with lines, 'beads_10nM_4545.5Hz_EM250.statistics.dat' using 1:4 title 'min' with lines, 'beads_10nM_4545.5Hz_EM250.statistics.dat' using 1:5 title 'max' with lines
set title "Statistics D:\\kriegerj\\daten\\spim\\2011_05_11\\beads_10nM_4545.5Hz_EM250.tif" noenhanced
plot 'beads_10nM_4545.5Hz_EM250.statistics.dat' using 1:2 title 'mean' with lines
set title "Statistics D:\\kriegerj\\daten\\spim\\2011_05_11\\beads_10nM_4545.5Hz_EM250.tif" noenhanced
plot 'beads_10nM_4545.5Hz_EM250.statistics.dat' using 1:3 title 'stddev' with lines
set output
