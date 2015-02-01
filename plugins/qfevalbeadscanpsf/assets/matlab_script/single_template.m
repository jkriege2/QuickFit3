clear
clc
addpath('D:\Matlab\spim\'); % path to the rest of the SPIM-FCS evaluation scripts


% plug in the path and file (multi-frame TIFF) name of the measurement:
datadir= '.\';  
datafile = 'stack.tif';
resultoutputnamepart='';

% change this if you don't want to eval the whole stack ... basically you can leave it that way!
datarange=[];      % which images to take from the stack
datarangeX=32:96;
datarangeY=32:96;  %[];

% step in z-direction in nenometers
deltaz = 200;         % longitudinal pixel-size in nanometers

% lateral pixel size in nanometers in the object plane
deltaxy = 400;       % lateral pixelsize in nanometers

% separate the image into two color planes with shift vector
%   e.g. a 128x128 image may be cut into two 64x128 images by 'h'
%   e.g. a 128x128 image may be cut into two 128x64 images by 'v'
%   no cutting is doen for 'n'
dualView='n';

% how many pixels to cut in each direction around a specified bead position (so with 10 it will cut 21x21 subimages from each frame)
cutregion_xy = 5;     % size of the region to cut laterally

% how many pixel to cut along z-direction around every specified bead position (see above)
cutregion_z = 27;     % size of the region to cut longitudinally
max_intensity = 1;

% binning, if you want ... note that width/binning and height/binning have to be integer numbers!
binning=1; % lateral binning

% pixels per image possibly belonging to a bead
beads_per_image=200;
% steps in which to go through the image to find beads
stepsize=round(cutregion_z/4);
% minimum distance between two beads
min_dist=round(cutregion_xy/2);
% maximum size of the beads in the segmented image
max_size=round(cutregion_xy/3);


% fit model in xy/z-direction 'gaussian' (with 1/e^2 widths) or 'pixel' which
% is a rectagle of width fitxy_pixelwidth convolved withg a gaussian
fitmodel_xy='gaussian';
fitxy_pixelwidth=deltaxy*binning;
fitmodel_z='gaussian';
fitz_pixelwidth=deltaz;

% fit gaussian xy-model to additional x-/y-cuts along z-axis
fitxzcolumns=true;


% determine whether to show/save plots of every bead
showsinglebeadplots=true;
showeveryNthSinglebeadPlot=5;

% when comparing the two color channels: how many bins in distance
% histogram?
poshistbins=30;

% when comparing the two color channels: what is the maximal bead position
% distance that still makes sence (in nanometers) in XY- and Z- direction
maxDualcolorBeadDistanceXY=1000;
maxDualcolorBeadDistanceZ=1000;



[datafilepathstr, datafilename, datafileext] = fileparts(datafile) ;
% where to save the results ... the code below will save it in subdirs (that you'll have to create in advance) of the data directory that are calle 'psf_results_binBB' where BB stands for the binning
prefix=[datadir 'psf_results' resultoutputnamepart '_bin' num2str(binning) '_fx' fitmodel_xy '_fz' fitmodel_z '\' datafilename '_'];
mkdir(datadir, [ 'psf_results' resultoutputnamepart '_bin' num2str(binning) '_fx' fitmodel_xy '_fz' fitmodel_z ])

% ranges for the histograms given in annometers
% note all results outside these ranges will be excluded when calculating
% the statistics/histograms!!!
histwidthxyrange=[100 1500];
histwidthxybinwidth=30;
histwidthzrange=[100 2500];
histwidthzbinwidth=30;

% file to save the bead positions to
posfile=[datafile '.beadpos.dat'];




% load data:
disp(['LOADING Z-STACK']);
[data1, dataT]=load_tiff_sequence(datafile, datadir);
data1=single(data1);
if ~isempty(datarangeX)
    data1=data1(datarangeX,:,:);
end
if ~isempty(datarangeY)
    data1=data1(:,datarangeY,:);
end
if (binning>1)
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

if (strcmp(dualView, 'h'))
    datal=data(1:floor(w/2),:,:);
    datar=data((floor(w/2)+1):w,:,:);    
elseif (strcmp(dualView, 'v'))
    datal=data(:, 1:(floor(h/2)),:);
    datar=data(:, (floor(h/2)+1):h,:);    
else
    datal=data;
    datar=[];
end
clear data;
[wl,hl,Nl]=size(datal);
[wr,hr,Nr]=size(datar);

disp('DATA SIZE');
disp(['  left:     ' num2str(wl) ' x ' num2str(hl) ' pixels,   ' num2str(Nl) ' frames']);
disp(['  right:    ' num2str(wr) ' x ' num2str(hr) ' pixels,   ' num2str(Nr) ' frames']);

% determine bead positions
disp('FINDING BEADS');
psf_positions=findBeads3D(datal, beads_per_image, stepsize, min_dist, max_size);
disp(['  found ' num2str(length(psf_positions)) ' beads in left image']);
csvwrite(posfile, psf_positions)








% 
% psf_positions= dlmread([datadir posfile]);
% 
% 
% 
xmi=max(1,min(psf_positions(:,1))-cutregion_xy-2);
xma=min(wl, max(psf_positions(:,1))+cutregion_xy+2);
ymi=max(1,min(psf_positions(:,2))-cutregion_xy-2);
yma=min(hl, max(psf_positions(:,2))+cutregion_xy+2);
zmi=max(1,min(psf_positions(:,3))-cutregion_z-2);
zma=min(Nl, max(psf_positions(:,3))+cutregion_z+2);

% cut down data
datal=datal(xmi:xma,ymi:yma,zmi:zma);
if (~isempty(datar)) 
    datar=datar(xmi:xma,ymi:yma,zmi:zma);
end
% modify psf_positions
for cnt=1:length(psf_positions)
    psf_positions(cnt,:)=psf_positions(cnt,:)-[xmi-1 ymi-1 zmi-1];
end


if dualView=='n'
    dataout=perform_evalzstack(datal, psf_positions, prefix, 'datarange',datarange, 'deltaz',deltaz, 'deltaxy', deltaxy,...
                       'cutregion_xy', cutregion_xy, 'cutregion_z',cutregion_z, 'max_intensity',max_intensity,...
                       'histwidthxyrange', histwidthxyrange, 'histwidthzrange',histwidthzrange, ...
                       'histwidthxybinwidth', histwidthxybinwidth, 'histwidthzbinwidth',histwidthzbinwidth, ...
                       'fitmodel_xy',fitmodel_xy, 'fitxy_pixelwidth', fitxy_pixelwidth, 'fitxzcolumns', fitxzcolumns, ...
                       'fitmodel_z',fitmodel_z, 'fitz_pixelwidth', fitz_pixelwidth, 'showsinglebeadplots', showsinglebeadplots, 'showeveryNthSinglebeadPlot', showeveryNthSinglebeadPlot);
else
    dataout=perform_evaldualcolorzstack(datal, datar, psf_positions, prefix, 'datarange',datarange, 'deltaz',deltaz, 'deltaxy', deltaxy,...
                       'cutregion_xy', cutregion_xy, 'cutregion_z',cutregion_z, 'max_intensity',max_intensity,...
                       'histwidthxyrange', histwidthxyrange, 'histwidthzrange',histwidthzrange, ...
                       'histwidthxybinwidth', histwidthxybinwidth, 'histwidthzbinwidth',histwidthzbinwidth, ...
                       'fitmodel_xy',fitmodel_xy, 'fitxy_pixelwidth', fitxy_pixelwidth, 'fitxzcolumns', fitxzcolumns, ...
                       'fitmodel_z',fitmodel_z, 'fitz_pixelwidth', fitz_pixelwidth, 'showsinglebeadplots', showsinglebeadplots, 'showeveryNthSinglebeadPlot', showeveryNthSinglebeadPlot);

    evaluate_dualcolorzstackresults(dataout, prefix,...
                       'histwidthxyrange', histwidthxyrange, 'histwidthzrange',histwidthzrange, ...
                       'histwidthxybinwidth', histwidthxybinwidth, 'histwidthzbinwidth',histwidthzbinwidth, ...
                       'poshistbins', poshistbins, 'maxdualcolorbeaddistancexy', maxDualcolorBeadDistanceXY, ...
                       'maxdualcolorbeaddistancez', maxDualcolorBeadDistanceZ);
end