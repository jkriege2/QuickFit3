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

clear;
          
             
%datadir= '.\2012_05_15\radhard2\beads_40nm_0.001_5mW\results\';
%datafile = 'test.32x32.raw_corrmtauallmon_backnone_bin1_bleachnone_seg1_1.video.tif';
datadir='.\';
datafile='test.110713.2500.raw_corrmtauallmon_backnone_bin1_bleachnone_seg1_1.video.tif';
datarange=1:400;      % which images to take from the stack
plotpos = [ 17 14 ];         % longitudinal pixel-size in nanometers
deltat = 20e-6;
deltaI = 1/(2*10e-6);
Isum=2;
avgMin=0;
avgMax=1;
[data, dataT]=load_tiff_sequence(datafile, datadir);

data=Isum*(data(:,:,datarange)/65535*(avgMax-avgMin)+avgMin);
dataT=dataT(datarange);

spp=size(plotpos);

figure(1)
for cnt=1:spp(1)
    ts=squeeze(data(plotpos(cnt,1),plotpos(cnt,2),:)*deltaI);
    t=(1:(length(datarange)));
    t1=(1:2:(length(datarange)-1));
    ts1=[];
    for c=1:2:(length(datarange)-1)
        ts1(ceil(c/2))=ts(c)+ts(c+1);
        c
    end
    plot(t1*deltat*1000, ts1/2000)
    if cnt==1
        hold on
    end
 end
hold off
xlabel('time [ms]');
ylabel('count rate [kHz]');

