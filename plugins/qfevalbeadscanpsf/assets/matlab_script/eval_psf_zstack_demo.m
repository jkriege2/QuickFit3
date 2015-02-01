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

clear
%addpath('../');


% plug in the path and file (multi-frame TIFF) name of the measurement:
datadir= '.\2012_06_27\beads_scan\';  
datafile = 'beads_scan.tif';

% change this if you don't want to eval the whole stack ... basically you can leave it that way!
datarange=[];      % which images to take from the stack

% step in z-direction in nenometers
deltaz = 200;         % longitudinal pixel-size in nanometers

% lateral pixel size in nanometers in the object plane
deltaxy = 113;       % lateral pixelsize in nanometers

% how many pixels to cut in each direction around a specified bead position (so with 10 it will cut 21x21 subimages from each frame)
cutregion_xy = 10;     % size of the region to cut laterally

% how many pixel to cut along z-direction around every specified bead position (see above)
cutregion_z = 10;     % size of the region to cut longitudinally
max_intensity = 1;

% binning, if you want ... note that width/binning and height/binning have to be integer numbers!
binning=1; % lateral binning

% where to save the results ... the code below will save it in subdirs (that you'll have to create in advance) of the data directory that are calle 'psf_results_binBB' where BB stands for the binning
prefix=[datadir 'psf_results_bin' num2str(binning) '\beads_scan'];

% ranges for the histograms given in annometers
histwidthxyrange=100:50:1000;
histwidthzrange=100:50:1500;

% array with the pixel positions of the PSFs to evaluate
posfile='beads_scan.beadpos.dat';
% this file should contain the coordinates of the beads as read from the point tool in ImageJ/FIJI. 
% So open the multi-frame-TIFF in FIJI and find the brightest pixel of each of the beads (roughly) and write the coordinates into this file
% it then should look like this (x, y and z-coordinate as given by ImageJ):
%
% 1138 901 11
% 1300 1081 11
% 1148 1403 11
% 1236 1541 11
% 1260 477 19
% 1192 499 19
% 1232 673 19
% .....
%
% Note: you'll have to keep (cutregion_xy+1) pixels distance from the left/right, top/bottom borders of the image and 
%       only start at frame (cutregion_z+1) and stop (cutregion_z+1) frames before the last one ... or the analysed volumes
%       of size (2*cutregion_xy+1)X(2*cutregion_xy+1)X(2*cutregion_z+1) can not be cut out.
%
% This is how the script works:
%  - It goes through all the coordinates you gave to it in the posfile
%    - For each coordinate it cuts a region of size (2*cutregion_xy+1)X(2*cutregion_xy+1)X(2*cutregion_z+1) pixels from the full
%      3D image
%    - for each region:
%        1. find the brightest pixel
%        2. make three 1D cuts along the x-, y- and z- axis through the region
%        3. fit a 1D gaussian to each of these cuts and output a .fig+.pdf image of the results
%    - finally it plots histograms of all parameters and outputs some statistics (mean, median S.D.)





psf_positions= dlmread([datadir posfile]);
[data1, dataT]=load_tiff_sequence(datafile, datadir);
data1=single(data1);

if (binning>1)
    psf_positions(:,1)=round(psf_positions(:,1)/binning);
    psf_positions(:,2)=round(psf_positions(:,2)/binning);
    deltaxy=deltaxy*binning;
    for cnt=1:length(dataT)
        data(:,:,cnt)=single(downsamp2d(data1(:,:,cnt), [binning binning]));
    end
else
    data=data1;
end
clear data1;


min(min(min(data)))
max(max(max(data)))

[w,h,N]=size(data);

xmi=max(1,min(psf_positions(:,1))-cutregion_xy-2);
xma=min(w, max(psf_positions(:,1))+cutregion_xy+2);
ymi=max(1,min(psf_positions(:,2))-cutregion_xy-2);
yma=min(h, max(psf_positions(:,2))+cutregion_xy+2);
zmi=max(1,min(psf_positions(:,3))-cutregion_z-2);
zma=min(N, max(psf_positions(:,3))+cutregion_z+2);

% cut down data
data=data(xmi:xma,ymi:yma,zmi:zma);
% modify psf_positions
for cnt=1:length(psf_positions)
    psf_positions(cnt,:)=psf_positions(cnt,:)-[xmi-1 ymi-1 zmi-1];
end

perform_evalzstack(data, psf_positions, prefix, 'datarange',datarange, 'deltaz',deltaz, 'deltaxy', deltaxy,...
                   'cutregion_xy', cutregion_xy, 'cutregion_z',cutregion_z, 'max_intensity',max_intensity,...
                   'histwidthxyrange', histwidthxyrange, 'histwidthzrange',histwidthzrange);
               

%perform_evalzstack_3dfits(data, psf_positions, prefix3d, 'datarange',datarange, 'deltaz',deltaz, 'deltaxy', deltaxy,...
%                   'cutregion_xy', cutregion_xy, 'cutregion_z',cutregion_z, 'max_intensity',max_intensity,...
%                   'histwidthxyrange', histwidthxyrange, 'histwidthzrange',histwidthzrange);               