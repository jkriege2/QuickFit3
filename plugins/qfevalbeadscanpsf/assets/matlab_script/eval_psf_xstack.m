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
  datadir= '.\2012_06_12\';
datafile = 'beadsingel_ystack15mW_001.lightpath1.tif';
datarange=1:500;      % which images to take from the stack
deltaz = 100;         % longitudinal pixel-size in nanometers
cutregion_z = 20;      % size of the region to cut longitudinally
direction=1;          % 1: y stack, 2: x stack

% array with the pixel positions of the PSFs to evaluate
posfile='beadsingel_ystack15mW_001.lightpath1.beadpos.dat';
psf_positions= dlmread([datadir posfile]);


% datadir= '.\2012_05_22\';
% datafile = 'radhard2_beads100nm_phytagel_ystack_000.tif';
% datarange=1:500;      % which images to take from the stack
% deltaz = 100;         % longitudinal pixel-size in nanometers
% cutregion_z = 20;      % size of the region to cut longitudinally
% direction=1;          % 1: y stack, 2: x stack
% 
% % array with the pixel positions of the PSFs to evaluate
% psf_positions= [ 5 10 147; 21 16 91; 21 10 141; 21 8 173; 21 20 53; 5 15 97; 13 28 81; 13 25 109; ...
%                  5 14 109; 21 17 82; 30 13 86; 11 18 102; 3 23 115; 11 15 133; 11 25 35; 30 18 36; ...
%                  10 1 47; 21 19 63; 21 11 142; 4 19 162; 3 8 172; 21 7 182; 21 11 142; 11 7 213;
%                  23 17 245; 23 15 265; 3 9 265; 2 22 305; 25 30 332; 25 25 352; 25 23 399; 25 20 399;
%                  25 13 471; 25 16 471; 2 5 479; 17 17 436; 10 15 436];
             
% datadir= '.\2012_05_22\';
% datafile = 'emccd_beads100nm_phytagel_ystack_000.tif';
% datarange=1:500;      % which images to take from the stack
% deltaz = 100;         % longitudinal pixel-size in nanometers
% cutregion_z = 20;      % size of the region to cut longitudinally
% direction=1;          % 1: y stack, 2: x stack
% 
% % array with the pixel positions of the PSFs to evaluate
% psf_positions= [ 53 53 39; 33 94 39; 108 15 39; 99 109 39; 59 53 62; 5 55 62; 22 71 62; ...
%                  39 94 62; 114 15 62; 7 90 96; 60 77 96; 113 109 96; 3 86 96; 14 90 128; ...
%                  38 71 128; 75 54 125; 21 56 125; 40 48 125; 22 90 158; 46 71 158; ...
%                  56 74 158; 48 48 158; 81 9 158; 83 54 158; 43 47 158; 50 86 287; ...
%                   78 71 287; 124 31 287; 11 64 453; 14 49 453; 62 42 453; 68 62 453; ...
%                  70 42 453];
%              
[data, dataT]=load_tiff_sequence(datafile, datadir);

data=data(:,:,datarange);
dataT=dataT(datarange);

dirname{1}='y';
dirname{2}='x';
dirname{3}='z';


datamin=min(min(min(data)));
datamax=max(max(max(data)));
data8=uint8((data-datamin)*255/(datamax-datamin));

psf_positions=psf_positions+1;
spsf=size(psf_positions);
for cnt=1:spsf(1)
    x=psf_positions(cnt,1);
    y=psf_positions(cnt,2);
    z=psf_positions(cnt,3);
    p=psf_positions(cnt,:);
    
    psf=squeeze(data(y,x, (z-cutregion_z):(z+cutregion_z)));
    
    psf_min=min(psf);
    psf_max=max(psf);
   
    
    h=figure(2)
    subplot(1,1,1)
    cutZaxis=((1:length(psf))'-1).*deltaz;
    gparam0=[psf_max-psf_min max(cutZaxis)/2 3*deltaz psf_min];
    temp=find(psf==max(psf));
    if (length(temp)>=1)
        gparam0(2)=cutZaxis(temp(1+floor(length(temp)/2)));
    end
    ff=@(p,x) fit_gauss1D(p,x);
    gparam = lsqcurvefit(ff, gparam0, cutZaxis, psf);
    cut_fitZaxis=(1:0.2:length(psf)).*deltaz;
    cut_fit=fit_gauss1D(gparam, cut_fitZaxis);
    plot(cutZaxis, psf, 'r+', cut_fitZaxis, cut_fit, 'b-');
    text(0.7,0.9,['w' dirname{direction} ' = ' num2str(gparam(3)) ' nm' ], 'Units', 'normalized');
    gparamz(cnt,:)=gparam;
    zmaxpos=uint16(round(gparam(2)/deltaz)+1);

    ylabel('intensity');
    xlabel(dirname{direction});

    
    
    saveas(h, [datadir filesep 'psf_results' filesep datafile '.psf' num2str(cnt) '.pdf']);

    cnt
    psf_positions(cnt,:)
    %pause;%(0.1)
end


wz=abs(gparamz(:,3));

disp(['avg(w' dirname{direction} ') = ' num2str(mean(wz)) '    median(w' dirname{direction} ') = ' num2str(median(wz)) '    SD(w' dirname{direction} ') = ' num2str(std(wz))  ]);

h=figure(3)
subplot(1,2,1)
hist(wz, 50);
title([ datafile '   N_PSF = ' num2str(length(psf_positions))], 'Interpreter', 'none')
text(0.5,0.94,['avg(w' dirname{direction} ') = ' num2str(mean(wz))  ], 'Units', 'normalized', 'FontSize', 6);
text(0.5,0.87,['median(w' dirname{direction} ') = ' num2str(median(wz))  ], 'Units', 'normalized', 'FontSize', 6);
text(0.5,0.8,['SD(w' dirname{direction} ') = ' num2str(std(wz))  ], 'Units', 'normalized', 'FontSize', 6);
subplot(1,2,2)
plot(psf_positions(:,3), wz, 'r+');
legend([dirname{direction} '-width']);
xlabel([ dirname{direction} '-position of psf measurement [pixel]']);
ylabel('width of psf [pixel]');


saveas(h, [datadir filesep datafile '.psfhistogram.pdf']);
save([datadir filesep datafile '.psfhistogram.mat'])
M=[psf_positions(:,1)';psf_positions(:,2)';psf_positions(:,3)';wz']';
dlmwrite([datadir filesep datafile '.psfwidths.dat'], M);


