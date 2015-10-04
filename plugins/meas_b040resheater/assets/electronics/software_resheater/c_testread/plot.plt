set terminal pdfcairo color font "Arial,8" linewidth 2 size 15cm,10cm
set output 'test_D30_I1_IM100.pdf'
plot 'test_D30_I1_IM100.dat' using 1:(($2)/10.0) axes x1y1 title 'temperature' with lines,\
     'test_D30_I1_IM100.dat' using 1:(($5)/255.0*100.0) axes x1y2 title 'output' with lines
set xlabel 'time [s]'
set ylabel 'temperature [°C]'
set y2label 'output [%]'
set ytics border nomirror autofreq
set y2tics border nomirror autofreq
set output 
set term wxt
replot

