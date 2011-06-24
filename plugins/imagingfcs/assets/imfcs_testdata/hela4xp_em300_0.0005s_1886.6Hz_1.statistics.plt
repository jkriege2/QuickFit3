set terminal pdf color font "Arial,10" size 29cm,21cm
set output 'hela4xp_em300_0.0005s_1886.6Hz_1.statistics.pdf'
set xlabel 'frame'
set ylabel 'pixel value'
set title "Statistics D:\\kriegerj\\daten\\spim\\2011_05_25\\hela4xp_em300_0.0005s_1886.6Hz_1.tif" noenhanced
plot 'hela4xp_em300_0.0005s_1886.6Hz_1.statistics.dat' using 1:2 title 'mean +/- stddev' with lines lt 1, 'hela4xp_em300_0.0005s_1886.6Hz_1.statistics.dat' using 1:(($2)-(($3)/2.0)) notitle with lines lt 1, 'hela4xp_em300_0.0005s_1886.6Hz_1.statistics.dat' using 1:(($2)+(($3)/2.0)) notitle with lines lt 1
set title "Statistics D:\\kriegerj\\daten\\spim\\2011_05_25\\hela4xp_em300_0.0005s_1886.6Hz_1.tif" noenhanced
plot 'hela4xp_em300_0.0005s_1886.6Hz_1.statistics.dat' using 1:2 title 'mean' with lines, 'hela4xp_em300_0.0005s_1886.6Hz_1.statistics.dat' using 1:4 title 'min' with lines, 'hela4xp_em300_0.0005s_1886.6Hz_1.statistics.dat' using 1:5 title 'max' with lines
set title "Statistics D:\\kriegerj\\daten\\spim\\2011_05_25\\hela4xp_em300_0.0005s_1886.6Hz_1.tif" noenhanced
plot 'hela4xp_em300_0.0005s_1886.6Hz_1.statistics.dat' using 1:2 title 'mean' with lines
set title "Statistics D:\\kriegerj\\daten\\spim\\2011_05_25\\hela4xp_em300_0.0005s_1886.6Hz_1.tif" noenhanced
plot 'hela4xp_em300_0.0005s_1886.6Hz_1.statistics.dat' using 1:3 title 'stddev' with lines
set output
