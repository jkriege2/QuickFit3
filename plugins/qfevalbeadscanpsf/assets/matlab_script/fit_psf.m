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

% This script loads an image sequence and then fits a 2D gaussian to every
% frame. The results are returned and plottet (as cuts along x- and y-axis
% and as images)
%

% image sequence to load
filename='2010_10_1x\EMCCD\2010_10_08\psf_determination\101008_invitrogen_fs_yg_20nm_15nmol_psf2.tif';

% directory for the output
outdir='2010_10_1x\EMCCD\2010_10_08\psf_determination\101008_invitrogen_fs_yg_20nm_15nmol_psf2\';

% width of one pixel in microns
pixelwidth=30/30;

seq=load_tiff_sequence(filename);
s=size(seq);
N=s(3);

disp(['loaded ' num2str(N) ' images with ' num2str(s(1)) 'x' num2str(s(2)) ' pixels']);

% pixel width in high-resolution graph display
deltax=0.05;
% pixel width in high-resolution image display
delta_image=0.2;

results=zeros(N,7);
resultfit=zeros(s(1),s(2),N);
for idx=1:N
    image=seq(:,:,idx);
    maxpix=max(max(image));
    [maxx, maxy]=find(image==maxpix);
    x0(1)=mean2(image);
    x0(2)=maxpix-x0(1);
    x0(3)=maxx(1);
    x0(4)=maxy(1);
    x0(5)=5;
    x0(6)=5;
    fitfun = @(x)Fit_Gaussian2D(x(1), x(2), x(3), x(4), x(5), x(6), image);

    [x, res]=lsqnonlin(fitfun, x0);
    disp([num2str(idx) ': ' num2str(x) ' ' num2str(res) ]);
    results(idx,:)=[x res];
    lr_result=Gaussian2D(x(1), x(2), x(3), x(4), x(5), x(6), s(1), s(2),delta_image,delta_image);
    hr_result=Gaussian2D(x(1), x(2), x(3), x(4), x(5), x(6), s(1), s(2),deltax,deltax);
    
    clf;
    h=figure(1);
    title(filename);
    subplot(2,2,1);
    imagesc(image);
    xlabel('x [pixel]');
    ylabel('y [pixel]');
    title(sprintf('image frame %d\n%s', idx, filename));
    colorbar;
    subplot(2,2,2);
    imagesc(lr_result);
    xlabel('x [pixel]');
    ylabel('y [pixel]');
    title(sprintf('fit for frame %d', idx));
    colorbar;
    
    subplot(2,2,3);
    
    cutpos=round(x(4));
    if (cutpos<1) cutpos=1; end
    if (cutpos>s(2)) cutpos=s(2); end
    
    plot((1:s(1)).*pixelwidth, image(:,cutpos), 'r+-');
    hold on;
    plot((1:deltax:s(1)).*pixelwidth, hr_result(:,1+(cutpos-1)/deltax), 'b-');
    hold off;
    title(sprintf('cut and fit along x axis (1/e² width = %f µm)', x(5)*pixelwidth));
    xlabel('x [µm]');
    ylabel('intensity [A.U.]');
    legend('data', 'fit')

    
    
    subplot(2,2,4);
    
    cutpos=round(x(3));
    if (cutpos<1) cutpos=1; end
    if (cutpos>s(1)) cutpos=s(1); end
    
    plot((1:s(2)).*pixelwidth, image(cutpos,:), 'r+-');
    hold on;
    plot((1:deltax:s(2)).*pixelwidth, hr_result(1+(cutpos-1)/deltax,:), 'b-');
    hold off;
    title(sprintf('cut and fit along y axis (1/e² width = %f µm)', x(6)*pixelwidth));
    xlabel('y [µm]');
    ylabel('intensity [A.U.]');
    legend('data', 'fit')
    
    saveas(h, sprintf('%s%0d.pdf',outdir,idx));
    pause(1)
end
csvwrite(sprintf('%sresults.txt',outdir), results)
