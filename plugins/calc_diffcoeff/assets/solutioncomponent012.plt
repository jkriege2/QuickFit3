reset
set xlabel 'concentration [M]'
set ylabel 'viscosity [Pa*s]'
a=1.012
b=-67.697998
c=65.237
d=-115.747012
e=2080.655029
f=100
g=100
h=100
j=100
model1(conc,a,b,c,d,e,f,g,h,j)=a+b*1e-3*sqrt(conc)+c*1e-2*conc+d*1e-3*conc*conc+e*1e-3*conc*conc*conc+f*1e-3*conc*conc*conc*conc+g*1e-3*conc*conc*conc*conc*conc+h*1e-3*conc*conc*conc*conc*conc*conc+j*1e-3*conc*conc*conc*conc*conc*conc*conc
fit [0:] model1(x,a,b,c,d,e,f,g,h,j) 'solutioncomponent012.dat' using 1:4 via a,b,c,d,e,f,g,h,j
fit [0:] model1(x,a,b,c,d,e,f,g,h,j) 'solutioncomponent012.dat' using 1:4 via a,b,c,d,e,f,g,h,j
fit [0:] model1(x,a,b,c,d,e,f,g,h,j) 'solutioncomponent012.dat' using 1:4 via a,b,c,d,e,f,g,h,j
set multiplot layout 3,1
set origin 0,0.33
set size 1,0.66
set samples 5000
plot 'solutioncomponent012.dat' using 1:4 title 'data', model1(x,a,b,c,d,e,f,g,h,j) title sprintf('fit a=%f, b=%f, c=%f, d=%f, e=%f, f=%f, g=%f, h=%f, j=%f', a,b,c,d,e,f,g,h,j) with lines
set size 1,0.33
set origin 0,0
set xzeroaxis
set ylabel 'rel. residuals [%]'
set samples 5000
plot 'solutioncomponent012.dat' using ($1):((($4)-model1(($1),a,b,c,d,e,f,g,h,j))*100.0/($4)) title 'rel. residuals [%]' with linespoints
unset multiplot