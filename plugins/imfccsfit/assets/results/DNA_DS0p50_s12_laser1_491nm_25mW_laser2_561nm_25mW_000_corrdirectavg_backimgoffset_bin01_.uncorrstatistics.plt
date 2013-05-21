set xlabel 'time [seconds]'
set ylabel 'pixel grey value'
set title "Uncorrected Statistics 'D:/dkfz_data/DNA_DS0p50_s12_laser1_491nm_25mW_laser2_561nm_25mW_000.tif'" noenhanced
set style fill transparent solid 0.5 noborder
set multiplot layout 1,2
plot 'DNA_DS0p50_s12_laser1_491nm_25mW_laser2_561nm_25mW_000_corrdirectavg_backimgoffset_bin01_.uncorrstatistics.dat' using 1:(($2)-(($3)/2.0)):(($2)+(($3)/2.0)) title '+/- stddev' with filledcu, 'DNA_DS0p50_s12_laser1_491nm_25mW_laser2_561nm_25mW_000_corrdirectavg_backimgoffset_bin01_.uncorrstatistics.dat' using 1:2 title 'mean' with lines lt 1
plot 'DNA_DS0p50_s12_laser1_491nm_25mW_laser2_561nm_25mW_000_corrdirectavg_backimgoffset_bin01_.uncorrstatistics.dat' using 1:4:5 title 'min/max' with filledcu, 'DNA_DS0p50_s12_laser1_491nm_25mW_laser2_561nm_25mW_000_corrdirectavg_backimgoffset_bin01_.uncorrstatistics.dat' using 1:(($2)-(($3)/2.0)):(($2)+(($3)/2.0)) title '+/- stddev' with filledcu, 'DNA_DS0p50_s12_laser1_491nm_25mW_laser2_561nm_25mW_000_corrdirectavg_backimgoffset_bin01_.uncorrstatistics.dat' using 1:2 title 'mean' with lines lt 1
unset multiplot
pause -1
set multiplot layout 2,2
plot 'DNA_DS0p50_s12_laser1_491nm_25mW_laser2_561nm_25mW_000_corrdirectavg_backimgoffset_bin01_.uncorrstatistics.dat' using 1:2 title 'mean' with lines lt 1
plot 'DNA_DS0p50_s12_laser1_491nm_25mW_laser2_561nm_25mW_000_corrdirectavg_backimgoffset_bin01_.uncorrstatistics.dat' using 1:3 title 'stddev' with lines lt 1
plot 'DNA_DS0p50_s12_laser1_491nm_25mW_laser2_561nm_25mW_000_corrdirectavg_backimgoffset_bin01_.uncorrstatistics.dat' using 1:4 title 'min' with lines lt 1
plot 'DNA_DS0p50_s12_laser1_491nm_25mW_laser2_561nm_25mW_000_corrdirectavg_backimgoffset_bin01_.uncorrstatistics.dat' using 1:5 title 'max' with lines lt 1
unset multiplot
pause -1
f(t)=a0+a1*exp(-t/t1)
t1=50
a0=0
a1=500
fit f(x) 'DNA_DS0p50_s12_laser1_491nm_25mW_laser2_561nm_25mW_000_corrdirectavg_backimgoffset_bin01_.uncorrstatistics.dat' using 1:2 via a0, a1, t1
set label sprintf('1exp: a0=%f a1=%f t1=%f', a0, a1, t1) at graph 0.5,0.9
plot 'DNA_DS0p50_s12_laser1_491nm_25mW_laser2_561nm_25mW_000_corrdirectavg_backimgoffset_bin01_.uncorrstatistics.dat' using 1:2 title 'mean' with points, f(x) title 'exponential fit
pause -1



