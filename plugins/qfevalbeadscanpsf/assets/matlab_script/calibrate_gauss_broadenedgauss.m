%%
%% B040 SPIM Beadscan evaluation scripts
%%  (C) 2011-2013 Jan W. Krieger <j.krieger@dkfz.de, jan@jkrieger.de>
%%
%% This file is part of B040 SPIM Beadscan evaluation scripts.
%%
%% B040 SPIM Beadscan evaluation scripts is free software: you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published by
%% the Free Software Foundation, either version 3 of the License, or
%% (at your option) any later version.
%%
%% B040 SPIM Beadscan evaluation scripts is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty of
%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
%% GNU General Public License for more details.
%%
%% You should have received a copy of the GNU General Public License
%% along with Copyright Header. If not, see <http:%%www.gnu.org/licenses/>.
%%

clear all;
xx=-10000:5:10000;
ww=50:10:1000;
fitxy_pixelwidth=[115 200 300 400 500];

options = optimset('lsqcurvefit');
options = optimset(options, 'Display','off');


ww_out=zeros(length(ww),length(fitxy_pixelwidth));
cnt=1;
figure(1)
cla;
l={};
pcnt=1;
for pix=fitxy_pixelwidth
    cnt=1;
    for w=ww
        pg=[1 0 w 0];
        gg=fit_rectconvgauss1D([ pg pix], xx);
        ffxy=@(p,x) fit_gauss1D([p(1) 0 p(2) 0],x);
        pout= lsqcurvefit(ffxy, [1 w], xx, gg,[],[],options);
        ww_out(cnt, pix)=pout(2);
        cnt=cnt+1;
    end
    %ww_out(ww<pix,pix)=NaN;
    col=[pcnt/length(fitxy_pixelwidth) 0 0];
    plot(ww, ww_out(:, pix), '-', 'Color', col);
    if (pix==fitxy_pixelwidth(1))
        hold on
    end
    l{pcnt}=['pixelsize: ' num2str(pix) 'nm'];
    pcnt=pcnt+1;
    drawnow
end
hold off;
legend(l);
xlabel('width of broadened gaussian [nm]');
ylabel('width of fit simple gaussian [nm]');







ww_out=zeros(length(ww),length(fitxy_pixelwidth));
cnt=1;
figure(2)
cla;
l={};
pcnt=1;
for pix=fitxy_pixelwidth
    cnt=1;
    for w=ww
        pg=[1 0 w 0];
        gg=fit_gauss1D( pg, xx);
        ffxy=@(p,x) fit_rectconvgauss1D([p(1) 0 p(2) 0 pix],x);
        pout= lsqcurvefit(ffxy, [1 w], xx, gg,[],[],options);
        ww_out(cnt, pix)=pout(2);
        cnt=cnt+1;
    end
    %ww_out(ww<pix,pix)=NaN;
    col=[pcnt/length(fitxy_pixelwidth) 0 0];
    plot(ww, ww_out(:, pix), '-', 'Color', col);
    if (pix==fitxy_pixelwidth(1))
        hold on
    end
    l{pcnt}=['pixelsize: ' num2str(pix) 'nm'];
    pcnt=pcnt+1;
    drawnow
end
hold off;
legend(l);
xlabel('width of simple gaussian gaussian [nm]');
ylabel('width of fit broadened gaussian [nm]');

