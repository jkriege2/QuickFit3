function [bb]=erroreg(x,y,sigmay)

sX2=sum(x.*x./(sigmay.^2));
sX=sum(x./(sigmay.^2));
sSIGMA=sum(1./(sigmay.^2));
sXY=sum(x.*y./(sigmay.^2));
sY=sum(y./(sigmay.^2));

A=[sX2 2*sX; 2*sX 2*sSIGMA];
b=[2*sXY; 2*sY];

bb=pinv(A)*b;

a1=bb(1);
b1=bb(2);

bb=[b1,a1];