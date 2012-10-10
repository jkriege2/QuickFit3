reset
set xlabel 'concentration [M]'
set ylabel 'viscosity [Pa*s]'
a=1.002
b=-67.697998
c=65.237
d=-115.747002
e=2080.655029

model1(conc,a,b,c,d,e)=a+b*1e-3*sqrt(conc)+c*1e-2*conc+d*1e-3*conc*conc+e*1e-3*conc*conc*conc
fit [0:] model1(x,a,b,c,d,e) 'solutioncomponent002_1.dat' using 1:4 via a,b,c,d,e
fit [0:] model1(x,a,b,c,d,e) 'solutioncomponent002_1.dat' using 1:4 via a,b,c,d,e
fit [0:] model1(x,a,b,c,d,e) 'solutioncomponent002_1.dat' using 1:4 via a,b,c,d,e
set multiplot layout 3,1
set origin 0,0.33
set size 1,0.66
set samples 5000
plot 'solutioncomponent002_1.dat' using 1:4 title 'data', model1(x,a,b,c,d,e) title sprintf('fit a=%f, b=%f, c=%f, d=%f, e=%f', a,b,c,d,e) with lines
set size 1,0.33
set origin 0,0
set xzeroaxis
set ylabel 'rel. residuals [%]'
set samples 5000
plot 'solutioncomponent002_1.dat' using ($1):((($4)-model1(($1),a,b,c,d,e))*100.0/($4)) title 'rel. residuals [%]' with linespoints
unset multiplot