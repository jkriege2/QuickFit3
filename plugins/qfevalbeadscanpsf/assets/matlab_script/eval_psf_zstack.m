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
% datadir= '.\2012_05_22\';
% datafile = 'emccd_beads100nm_phytagel_zstack_001.tif';
% datarange=1:500;      % which images to take from the stack
% deltaz = 100;         % longitudinal pixel-size in nanometers
% deltaxy = 400;        % lateral pixelsize in nanometers
% cutregion_xy = 5;     % size of the region to cut laterally
% cutregion_z = 33;     % size of the region to cut longitudinally
% 
% % array with the pixel positions of the PSFs to evaluate
% psf_positions= [ 50 77 52 ; 70 54 56 ; 65 105 55 ; 82 82 71 ; 46 113 72 ; 22 97 72 ; 74 76 80; ...
%                  95 48 93 ; 39 92 46 ; 70 54 46 ; 42 58 46  ; 94 39 46 ; 67 24 127; 45 113 66;...
%                  66 68 171 ; 70 85 234 ; 73 76 70; 11 75 130; 82 82 66; 55 16 63; 71 18 63;...
%                  95 48 84; 117 54 84; 61 78 84; 111 107 84; 95 40 113; 33 43 113; 25 83 113; ...
%                  56 70 154; 48 89 154; 41 19 154; 36 11 154; 82 19 154; 114 83 154; 57 46 184; ...
%                  51 28 184; 49 19 184; 113 52 189; 65 113 166; 99 97 167; 63 23 167; 99 9 167];
% 
%              
             
% datadir= '.\2012_05_22\';
% datafile = 'radhard2_beads100nm_phytagel_zstack_001.tif';
% datarange=1:500;      % which images to take from the stack
% deltaz = 100;         % longitudinal pixel-size in nanometers
% deltaxy = 1000;       % lateral pixelsize in nanometers
% cutregion_xy = 2;     % size of the region to cut laterally
% cutregion_z = 29;     % size of the region to cut longitudinally
% max_intensity = 0.95;
% 
% % array with the pixel positions of the PSFs to evaluate
% psf_positions= [ 20 15 57; 24 14 34; 9 10 73; 14 20 161; 12 9 34; ...
%                  17 12 182; 11 11 182; 13 7 213;...
%                  15 21 352; 9 20 281; 19 12 290; 20 15 58; 9 6 222;...
%                  9 10 63; 17 12 182; 23 6 145; 6 20 205; 23 6 141;
%                  23 28 209;
%                  22 9 348; 12 7 347];
%              



datadir= '.\2012_06_12\';
datafile = 'beadsingel_zstack15mW_001.lightpath1.tif';
datarange=1:1000;      % which images to take from the stack
deltaz = 100;         % longitudinal pixel-size in nanometers
deltaxy = 412;       % lateral pixelsize in nanometers
cutregion_xy = 3;     % size of the region to cut laterally
cutregion_z = 25;     % size of the region to cut longitudinally
max_intensity = 0.95;

% array with the pixel positions of the PSFs to evaluate
posfile='beadsingel_zstack15mW_001.lightpath1.beadpos.dat';
psf_positions= dlmread([datadir posfile]);


[data, dataT]=load_tiff_sequence(datafile, datadir);

data=data(:,:,datarange);
dataT=dataT(datarange);

temp=mean(data,3);
qi=quantile(temp(:), max_intensity);
ds=size(data);
a=temp<qi;
for cnt=1:ds(3)
    data(:,:,cnt)=data(:,:,cnt).*double(qi);
end


datamin=min(min(min(data)));
datamax=max(max(max(data)));
data8=uint8((data-datamin)*255/(datamax-datamin));

psf_positions=psf_positions+1;
spsf=size(psf_positions);
cnt=1;
for cnt1=1:spsf(1)
    x=psf_positions(cnt1,1);
    y=psf_positions(cnt1,2);
    z=psf_positions(cnt1,3);
    p=psf_positions(cnt1,:);
    datasize=size(data);
    if (y-cutregion_xy>0 && x-cutregion_xy>0 && z-cutregion_z>0 && y-cutregion_xy<=datasize(1) && x-cutregion_xy<=datasize(2) && z-cutregion_z<=datasize(3))
        psf=data((y-cutregion_xy):(y+cutregion_xy), (x-cutregion_xy):(x+cutregion_xy), (z-cutregion_z):(z+cutregion_z));
        psf_min=min(min(min(psf)));
        psf_max=max(max(max(psf)));
        psf8=uint8((psf-psf_min)*255/(psf_max-psf_min));
        %figure(1)
        %implay(psf8);

        temp=find(psf==psf_max);
        if (length(temp)>=1)
            [psf_maxpos(1) psf_maxpos(2) psf_maxpos(3)]=ind2sub([2*cutregion_xy+1, 2*cutregion_xy+1, 2*cutregion_z+1], temp(1+floor(length(temp)/2)));
        else
            psf_maxpos(1)=cutregion_xy+1;
            psf_maxpos(2)=cutregion_xy+1;
            psf_maxpos(3)=cutregion_z+1;
        end


        h=figure(2)
        cutx=squeeze(psf8(psf_maxpos(1),:,:));
        cuty=squeeze(psf8(:,psf_maxpos(2),:));
        subplot(3,2,1)
        image(cuty);
        %daspect([deltaz/deltaxy 1 1 ]);
        ylabel('x');
        xlabel('z');
        subplot(3,2,2)
        image(cutx);
        %daspect([deltaz/deltaxy 1 1 ]);
        ylabel('y');
        xlabel('z');

        subplot(3,2,4)
        cut=squeeze(psf(psf_maxpos(1),psf_maxpos(2),:));
        cutZaxis=((1:length(cut))'-1).*deltaz;
        gparam0=[psf_max-psf_min max(cutZaxis)/2 3*deltaz ]; %psf_min];
        temp=find(cut==max(cut));
        if (length(temp)>=1)
            gparam0(2)=cutZaxis(temp(1+floor(length(temp)/2)));
        end
        offset=mean(cut((end-3):end));
        ff=@(p,x) fit_gauss1D([p(1) p(2) p(3) offset],x);
        gparam = lsqcurvefit(ff, gparam0, cutZaxis, cut);
        gparam=[gparam offset];
        cut_fitZaxis=(0:0.2:length(cut)).*deltaz;
        cut_fit=fit_gauss1D(gparam, cut_fitZaxis);
        plot(cutZaxis, cut, 'r+', cut_fitZaxis, cut_fit, 'b-');
        text(0.7,0.9,['wz = ' num2str(gparam(3)) ' nm' ], 'Units', 'normalized');
        text(0.7,0.7,['(x,y) = (' num2str(psf_maxpos(1)) ', ' num2str(psf_maxpos(2)) ') px' ], 'Units', 'normalized', 'FontSize', 6);
        gparamz(cnt,:)=0;
        gparamz(cnt,1:length(gparam))=gparam;
        zmaxpos=uint16(round(gparam(2)/deltaz)+1);

        subplot(3,2,1)
        rectangle('Position', [0.5 psf_maxpos(1)-0.5 2*cutregion_z+1 1], 'EdgeColor', 'white');
        subplot(3,2,2)
        rectangle('Position', [0.5 psf_maxpos(2)-0.5 2*cutregion_z+1 1], 'EdgeColor', 'white');


        subplot(3,2,3)
        spsf8=size(psf8);
        cutz=squeeze(psf8(:,:,min(zmaxpos, spsf8(3))));
        image(cutz);
        rectangle('Position', [ psf_maxpos(1)-0.5 psf_maxpos(2)-0.5 1 1], 'EdgeColor', 'white');
        daspect([1 1 1]);
        ylabel('x');
        xlabel('y');

        subplot(3,2,5)
        spsf=size(psf);
        cut=squeeze(psf(:,min(spsf(2),psf_maxpos(2)),min(spsf(3),zmaxpos)));
        cutZaxis=((1:length(cut))'-1).*deltaxy;
        gparam0=[psf_max-psf_min max(cutZaxis)/2 deltaxy psf_min];
        temp=find(cut==max(cut));
        if (length(temp)>=1)
            gparam0(2)=cutZaxis(temp(1+floor(length(temp)/2)));
        end
        ff=@(p,x) fit_gauss1D(p,x);
        gparam = lsqcurvefit(ff, gparam0, cutZaxis, cut);
        cut_fitZaxis=(0:0.2:length(cut)).*deltaxy;
        cut_fit=fit_gauss1D(gparam, cut_fitZaxis);
        plot(cutZaxis, cut, 'r+', cut_fitZaxis, cut_fit, 'b-');
        text(0.7,0.9,['wx = ' num2str(gparam(3)) ' nm' ], 'Units', 'normalized');
        text(0.7,0.7,['zmaxpos = ' num2str(zmaxpos) ' px' ], 'Units', 'normalized', 'FontSize', 6);
        gparamx(cnt,:)=gparam;

        subplot(3,2,6)
        cut=squeeze(psf(min(spsf(1),psf_maxpos(1)),:,min(spsf(3),zmaxpos)));
        cutZaxis=((1:length(cut))-1).*deltaxy;
        gparam0=[psf_max-psf_min max(cutZaxis)/2 deltaxy psf_min];
        temp=find(cut==max(cut));
        if (length(temp)>=1)
            gparam0(2)=cutZaxis(temp(1+floor(length(temp)/2)));
        end
        ff=@(p,x) fit_gauss1D(p,x);
        gparam = lsqcurvefit(ff, gparam0, cutZaxis, cut);
        cut_fitZaxis=(0:0.2:length(cut)).*deltaxy;
        cut_fit=fit_gauss1D(gparam, cut_fitZaxis);
        plot(cutZaxis, cut, 'r+', cut_fitZaxis, cut_fit, 'b-');
        text(0.7,0.9,['wy = ' num2str(gparam(3)) ' nm' ], 'Units', 'normalized');
        text(0.7,0.7,['zmaxpos = ' num2str(zmaxpos) ' px' ], 'Units', 'normalized', 'FontSize', 6);
        gparamy(cnt,:)=gparam;

        saveas(h, [datadir filesep 'psf_results' filesep datafile '.psf' num2str(cnt) '.pdf']);

        cnt
        psf_positions(cnt,:)
        pause(0.1)
    end
    cnt=cnt+1;
end


wz=abs(gparamz(:,3));
wx=abs(gparamx(:,3));
wy=abs(gparamy(:,3));

disp(['avg(wx) = ' num2str(mean(wx)) '    median(wx) = ' num2str(median(wx)) '    SD(wx) = ' num2str(std(wx))  ]);
disp(['avg(wy) = ' num2str(mean(wy)) '    median(wy) = ' num2str(median(wy)) '    SD(wy) = ' num2str(std(wy))  ]);
disp(['avg(wz) = ' num2str(mean(wz)) '    median(wz) = ' num2str(median(wz)) '    SD(wz) = ' num2str(std(wz))  ]);

disp(['avg(gamma_zx) = ' num2str(mean(wz./wx)) '    median(gamma_zx) = ' num2str(median(wz./wx)) '    SD(gamma_zx) = ' num2str(std(wz./wx))  ]);
disp(['avg(gamma_zy) = ' num2str(mean(wz./wy)) '    median(gamma_zy) = ' num2str(median(wz./wy)) '    SD(gamma_zy) = ' num2str(std(wz./wy))  ]);

h=figure(3)
subplot(3,3,1)
hist(wx, 400:10:1500);
title([ datafile '   N_PSF = ' num2str(length(psf_positions))], 'Interpreter', 'none')
text(0.5,0.94,['avg(wx) = ' num2str(mean(wx))  ], 'Units', 'normalized', 'FontSize', 6);
text(0.5,0.87,['median(wx) = ' num2str(median(wx))  ], 'Units', 'normalized', 'FontSize', 6);
text(0.5,0.8,['SD(wx) = ' num2str(std(wx))  ], 'Units', 'normalized', 'FontSize', 6);
subplot(3,3,2)
hist(wy, 400:10:1500);
text(0.5,0.94,['avg(wy) = ' num2str(mean(wy))  ], 'Units', 'normalized', 'FontSize', 6);
text(0.5,0.87,['median(wy) = ' num2str(median(wy))  ], 'Units', 'normalized', 'FontSize', 6);
text(0.5,0.8,['SD(wY) = ' num2str(std(wy))  ], 'Units', 'normalized', 'FontSize', 6);
subplot(3,3,3)
hist(wz, 500:30:2000);
text(0.5,0.94,['avg(wz) = ' num2str(mean(wz))  ], 'Units', 'normalized', 'FontSize', 6);
text(0.5,0.87,['median(wz) = ' num2str(median(wz))  ], 'Units', 'normalized', 'FontSize', 6);
text(0.5,0.8,['SD(wz) = ' num2str(std(wz))  ], 'Units', 'normalized', 'FontSize', 6);
subplot(3,3,6)
plot(psf_positions(:,3), wx, 'r+');
hold on
plot(psf_positions(:,3), wy, 'g+');
plot(psf_positions(:,3), wz, 'b+');
hold off
legend('x-width', 'y-width', 'z-width');
xlabel('z-position of psf measurement [pixel]');
ylabel('width of psf [pixel]');
subplot(3,3,4)
plot(psf_positions(:,1), wx, 'r+');
hold on
plot(psf_positions(:,1), wy, 'g+');
plot(psf_positions(:,1), wz, 'b+');
hold off
legend('x-width', 'y-width', 'z-width');
xlabel('x-position of psf measurement [pixel]');
ylabel('width of psf [pixel]');
subplot(3,3,5)
plot(psf_positions(:,2), wx, 'r+');
hold on
plot(psf_positions(:,2), wy, 'g+');
plot(psf_positions(:,2), wz, 'b+');
hold off
legend('x-width', 'y-width', 'z-width');
xlabel('y-position of psf measurement [pixel]');
ylabel('width of psf [pixel]');

subplot(3,3,7)
hist(wz./wx, 0.5:0.1:3);
text(0.5,0.94,['avg(gamma_zx) = ' num2str(mean(wz./wx))  ], 'Units', 'normalized', 'FontSize', 6);
text(0.5,0.87,['median(gamma_zx) = ' num2str(median(wz./wx))  ], 'Units', 'normalized', 'FontSize', 6);
text(0.5,0.8,['SD(gamma_zx) = ' num2str(std(wz./wx))  ], 'Units', 'normalized', 'FontSize', 6);
subplot(3,3,8)
hist(wz./wy, 0.5:0.1:3);
text(0.5,0.94,['avg(gamma_zy) = ' num2str(mean(wz./wy))  ], 'Units', 'normalized', 'FontSize', 6);
text(0.5,0.87,['median(gamma_zy) = ' num2str(median(wz./wy))  ], 'Units', 'normalized', 'FontSize', 6);
text(0.5,0.8,['SD(gamma_zy) = ' num2str(std(wz./wy))  ], 'Units', 'normalized', 'FontSize', 6);




saveas(h, [datadir filesep datafile '.psfhistogram.pdf']);
save([datadir filesep datafile '.psfhistogram.mat'])
M=[psf_positions(:,1)';psf_positions(:,2)';psf_positions(:,3)';wx';wy';wz']';
dlmwrite([datadir filesep datafile '.psfwidths.dat'], M);


