set xlabel 'time [seconds]'
set ylabel 'pixel grey value'
set title "Background Statistics, DV 2 '../plugins/imfccsfit/examples/test_simulated2cdiffusion.tif'" noenhanced
set style fill transparent solid 0.5 noborder
set multiplot layout 1,2
plot 'test_simulated2cdiffusion_corrdirectavg_backimgoffset_bin01_.backstatistics_dv2.dat' using 1:(($2)-(($3)/2.0)):(($2)+(($3)/2.0)) title '+/- stddev' with filledcu, 'test_simulated2cdiffusion_corrdirectavg_backimgoffset_bin01_.backstatistics_dv2.dat' using 1:2 title 'mean' with lines lt 1
plot 'test_simulated2cdiffusion_corrdirectavg_backimgoffset_bin01_.backstatistics_dv2.dat' using 1:4:5 title 'min/max' with filledcu, 'test_simulated2cdiffusion_corrdirectavg_backimgoffset_bin01_.backstatistics_dv2.dat' using 1:(($2)-(($3)/2.0)):(($2)+(($3)/2.0)) title '+/- stddev' with filledcu, 'test_simulated2cdiffusion_corrdirectavg_backimgoffset_bin01_.backstatistics_dv2.dat' using 1:2 title 'mean' with lines lt 1
unset multiplot
pause -1
set multiplot layout 2,2
plot 'test_simulated2cdiffusion_corrdirectavg_backimgoffset_bin01_.backstatistics_dv2.dat' using 1:2 title 'mean' with lines lt 1
plot 'test_simulated2cdiffusion_corrdirectavg_backimgoffset_bin01_.backstatistics_dv2.dat' using 1:3 title 'stddev' with lines lt 1
plot 'test_simulated2cdiffusion_corrdirectavg_backimgoffset_bin01_.backstatistics_dv2.dat' using 1:4 title 'min' with lines lt 1
plot 'test_simulated2cdiffusion_corrdirectavg_backimgoffset_bin01_.backstatistics_dv2.dat' using 1:5 title 'max' with lines lt 1
unset multiplot
pause -1
f(t)=a0+a1*exp(-t/t1)
t1=50
a0=0
a1=500
fit f(x) 'test_simulated2cdiffusion_corrdirectavg_backimgoffset_bin01_.backstatistics_dv2.dat' using 1:2 via a0, a1, t1
set label sprintf('1exp: a0=%f a1=%f t1=%f', a0, a1, t1) at graph 0.5,0.9
plot 'test_simulated2cdiffusion_corrdirectavg_backimgoffset_bin01_.backstatistics_dv2.dat' using 1:2 title 'mean' with points, f(x) title 'exponential fit
pause -1



