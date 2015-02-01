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

function [ output_args ] = perform_evalzstack_3dfits( data, psf_positions, output_prefix, varargin )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

    optargin = size(varargin,2);

    datarange=[];
    deltaz = 100;         % longitudinal pixel-size in nanometers
    deltaxy = 412;       % lateral pixelsize in nanometers
    cutregion_xy = 3;     % size of the region to cut laterally
    cutregion_z = 25;     % size of the region to cut longitudinally
    max_intensity = 0.95;
    maxfunevals=3000;
    maxiter=1000;
    
    maxxwidth=2500;
    maxywidth=2500;
    maxzwidth=2500;
    histgammarange=0.1:0.1:6;
    histwidthxyrange=400:10:1500;
    histwidthzrange=400:10:1500;
    

    for a=1:2:optargin
        name= varargin{a};
        val= varargin{a+1};

        if (strcmp(name, 'datarange') )
            datarange=val;
        elseif (strcmp(name, 'histgammarange') )
            histgammarange=val;
        elseif (strcmp(name, 'histwidthxyrange') )
            histwidthxyrange=val;
        elseif (strcmp(name, 'histwidthzrange') )
            histwidthzrange=val;
        elseif (strcmp(name, 'deltaz') )
            deltaz=val;
        elseif (strcmp(name, 'deltaxy') )
            deltaxy=val;
        elseif (strcmp(name, 'cutregion_xy') )
            cutregion_xy=val;
        elseif (strcmp(name, 'cutregion_z') )
            cutregion_z=val;
        elseif (strcmp(name, 'max_intensity') )
            max_intensity=val;
        elseif (strcmp(name, 'maxxwidth') )
            maxxwidth=val;
        elseif (strcmp(name, 'maxywidth') )
            maxywidth=val;
        elseif (strcmp(name, 'maxzwidth') )
            maxzwidth=val;
        elseif (strcmp(name, 'maxfunevals') )
            maxfunevals=val;
        elseif (strcmp(name, 'maxiter') )
            maxiter=val;  
        else
            error(['unknown parameter "' name '" to perform_evalzstack()']);
        end
    end

    %data=frames;

    [w,h,N]=size(data);
    if (isempty(datarange)) 
        datarange=1:N;
    end

    data=data(:,:,datarange);

    
    
    temp=mean(data,3);
    qi=quantile(temp(:), max_intensity);
    ds=size(data);
    %a=temp<qi;
    for cnt=1:ds(3)
        data(:,:,cnt)=data(:,:,cnt).*double(qi);
    end


    datamin=min(min(min(data)));
    datamax=max(max(max(data)));
    
    psf_positions=psf_positions+1;
    spsf=size(psf_positions);
    
    dataout.datarange=datarange;
    dataout.deltaz = deltaz;
    dataout.deltaxy = deltaxy;
    dataout.cutregion_xy = cutregion_xy;
    dataout.cutregion_z = cutregion_z;
    dataout.max_intensity = max_intensity;
    dataout.maxfunevals=maxfunevals;
    dataout.maxiter=maxiter;
    
    dataout.maxxwidth=maxxwidth;
    dataout.maxywidth=maxywidth;
    dataout.maxzwidth=maxzwidth;
    dataout.datamin=datamin;
    dataout.datamax=datamax;
    dataout.psf_positions=psf_positions;
    dataout.w=w;
    dataout.h=h;
    dataout.N=N;
    
    cnt=1;
    for cnt1=1:spsf(1)
        x=psf_positions(cnt1,1);
        y=psf_positions(cnt1,2);
        z=psf_positions(cnt1,3);
        %p=psf_positions(cnt1,:);
        datasize=size(data);
        if (y-cutregion_xy>0 && x-cutregion_xy>0 && z-cutregion_z>0 && x+cutregion_xy<=datasize(1) && y+cutregion_xy<=datasize(2) && z+cutregion_z<=datasize(3))
            psf=double(data((x-cutregion_xy):(x+cutregion_xy), (y-cutregion_xy):(y+cutregion_xy), (z-cutregion_z):(z+cutregion_z)));
            psf_min=min(min(min(psf)));
            psf_max=max(max(max(psf)));
            psf8=uint8(round((psf-psf_min)*250.0/(psf_max-psf_min)));
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


            h=figure(2);
            set(h, 'PaperOrientation', 'landscape');
            cutx=squeeze(psf(psf_maxpos(1),:,:));
            cuty=squeeze(psf(:,psf_maxpos(2),:));
            subplot(3,3,1)
            imagesc(cuty);
            daspect([deltaxy/deltaz 1 1 ]);
            ylabel('x [pixel]');
            xlabel('z [pixel]');
            title(['x=' num2str(psf_maxpos(1)) ' cut / point ' num2str(cnt) ': (' num2str(x) ', ' num2str(y) ', ' num2str(z) ')']);
            subplot(3,3,2)
            imagesc(cutx);
            daspect([deltaxy/deltaz 1 1 ]);
            ylabel('y [pixel]');
            xlabel('z [pixel]');
            title(['y=' num2str(psf_maxpos(2)) ' cut']);

            subplot(3,3,4)
            imagesc(log(double(cuty))/log(10));
            daspect([deltaxy/deltaz 1 1 ]);
            ylabel('x [pixel]');
            xlabel('z [pixel]');
            title(['log(y=' num2str(psf_maxpos(2)) ' cut)']);
            subplot(3,3,5)
            imagesc(log(double(cutx))/log(10));
            daspect([deltaxy/deltaz 1 1 ]);
            ylabel('y [pixel]');
            xlabel('z [pixel]');
            title(['log(x=' num2str(psf_maxpos(1)) ' cut)']);
            
            subplot(3,3,7)
            cut=squeeze(psf(psf_maxpos(1),psf_maxpos(2),:));
            cutZaxis=((1:length(cut))'-1).*deltaz;
            gparam0=[psf_max-psf_min max(cutZaxis)/2 3*deltaz psf_min];
            temp=find(cut==max(cut));
            if (length(temp)>=1)
                gparam0(2)=cutZaxis(temp(1+floor(length(temp)/2)));
            end
            
            ff=@(p,x) fit_gauss1D([p(1) p(2) p(3) p(4)],x);
            options = optimset('lsqcurvefit');
            options = optimset(options, 'MaxFunEvals',maxfunevals);
            options = optimset(options, 'MaxIter',maxiter);
            gparam = lsqcurvefit(ff, gparam0, cutZaxis, cut,[],[], options);
            zmaxpos=uint16(round(gparam(2)/deltaz)+1);

            subplot(3,3,1)
            rectangle('Position', [0.5 psf_maxpos(1)-0.5 2*cutregion_z+1 1], 'EdgeColor', 'white');
            rectangle('Position', [double(zmaxpos)-0.5 psf_maxpos(1)-0.5 1 1], 'EdgeColor', 'white');
            subplot(3,3,2)
            rectangle('Position', [0.5 psf_maxpos(2)-0.5 2*cutregion_z+1 1], 'EdgeColor', 'white');
            rectangle('Position', [double(zmaxpos)-0.5 psf_maxpos(2)-0.5 1 1], 'EdgeColor', 'white');
            subplot(3,3,4)
            rectangle('Position', [0.5 psf_maxpos(1)-0.5 2*cutregion_z+1 1], 'EdgeColor', 'white');
            rectangle('Position', [double(zmaxpos)-0.5 psf_maxpos(1)-0.5 1 1], 'EdgeColor', 'white');
            subplot(3,3,5)
            rectangle('Position', [0.5 psf_maxpos(2)-0.5 2*cutregion_z+1 1], 'EdgeColor', 'white');
            rectangle('Position', [double(zmaxpos)-0.5 psf_maxpos(2)-0.5 1 1], 'EdgeColor', 'white');


            subplot(3,3,3)
            spsf8=size(psf);
            cutz=squeeze(psf(:,:,max(1,min(zmaxpos, spsf8(3))))');
            imagesc(cutz);
            rectangle('Position', [ psf_maxpos(1)-0.5 psf_maxpos(2)-0.5 1 1], 'EdgeColor', 'white');
            daspect([1 1 1]);
            ylabel('x [pixel]');
            xlabel('y [pixel]');
            title(['z=' num2str(max(1,min(zmaxpos, spsf8(3)))) ' cut']);
            
            subplot(3,3,6)
            imagesc(log(double(cutz))/log(10));
            rectangle('Position', [ psf_maxpos(1)-0.5 psf_maxpos(2)-0.5 1 1], 'EdgeColor', 'white');
            daspect([1 1 1]);
            ylabel('x [pixel]');
            xlabel('y [pixel]');
            title(['log(z=' num2str(max(1,min(zmaxpos, spsf8(3)))) ' cut)']);


            
            
            
            
            
            saveas(h, [output_prefix '.psf' num2str(cnt) '.pdf']);
            saveas(h, [output_prefix '.psf' num2str(cnt) '.fig']);

            cnt
            psf_positions(cnt,:)
            pause(0.1)
        end
        cnt=cnt+1;
    end

    dataout.gparamx=gparamx;
    dataout.gparamy=gparamy;
    dataout.gparamz=gparamz;
    
    wz=abs(gparamz(:,3));
    wx=abs(gparamx(:,3));
    wy=abs(gparamy(:,3));
    
    idx=find(wx>0 & wy>0 & wz>0);
    wx=wx(idx);
    wy=wy(idx);
    wz=wz(idx);

    disp(['avg(wx) = ' num2str(mean(wx)) '    median(wx) = ' num2str(median(wx)) '    SD(wx) = ' num2str(std(wx))  ]);
    disp(['avg(wy) = ' num2str(mean(wy)) '    median(wy) = ' num2str(median(wy)) '    SD(wy) = ' num2str(std(wy))  ]);
    disp(['avg(wz) = ' num2str(mean(wz)) '    median(wz) = ' num2str(median(wz)) '    SD(wz) = ' num2str(std(wz))  ]);

    disp(['avg(gamma_zx) = ' num2str(mean(wz./wx)) '    median(gamma_zx) = ' num2str(median(wz./wx)) '    SD(gamma_zx) = ' num2str(std(wz./wx))  ]);
    disp(['avg(gamma_zy) = ' num2str(mean(wz./wy)) '    median(gamma_zy) = ' num2str(median(wz./wy)) '    SD(gamma_zy) = ' num2str(std(wz./wy))  ]);

    h=figure(3);
    subplot(3,3,1)
    hist(wx, histwidthxyrange);
    xlabel('x width [nm]');
    ylabel('frequency');
    title([ 'N_PSF = ' num2str(length(psf_positions))], 'Interpreter', 'none')
    text(0.5,0.94,['avg(wx) = ' num2str(mean(wx))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.87,['median(wx) = ' num2str(median(wx))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.8,['SD(wx) = ' num2str(std(wx))  ], 'Units', 'normalized', 'FontSize', 6);
    subplot(3,3,2)
    hist(wy, histwidthxyrange);
    xlabel('y width [nm]');
    ylabel('frequency');
    text(0.5,0.94,['avg(wy) = ' num2str(mean(wy))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.87,['median(wy) = ' num2str(median(wy))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.8,['SD(wY) = ' num2str(std(wy))  ], 'Units', 'normalized', 'FontSize', 6);
    subplot(3,3,3)
    hist(wz, histwidthzrange);
    xlabel('z width [nm]');
    ylabel('frequency');
    text(0.5,0.94,['avg(wz) = ' num2str(mean(wz))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.87,['median(wz) = ' num2str(median(wz))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.8,['SD(wz) = ' num2str(std(wz))  ], 'Units', 'normalized', 'FontSize', 6);
    subplot(3,3,6)
    plot(psf_positions(idx,3), wx, 'r+');
    hold on
    plot(psf_positions(idx,3), wy, 'g+');
    plot(psf_positions(idx,3), wz, 'b+');
    hold off
    legend('x-width', 'y-width', 'z-width');
    xlabel('z-position of psf measurement [pixel]');
    ylabel('width of psf [µm]');
    subplot(3,3,4)
    plot(psf_positions(idx,1), wx, 'r+');
    hold on
    plot(psf_positions(idx,1), wy, 'g+');
    plot(psf_positions(idx,1), wz, 'b+');
    hold off
    legend('x-width', 'y-width', 'z-width');
    xlabel('x-position of psf measurement [pixel]');
    ylabel('width of psf [µm]');
    subplot(3,3,5)
    plot(psf_positions(idx,2), wx, 'r+');
    hold on
    plot(psf_positions(idx,2), wy, 'g+');
    plot(psf_positions(idx,2), wz, 'b+');
    hold off
    legend('x-width', 'y-width', 'z-width');
    xlabel('y-position of psf measurement [pixel]');
    ylabel('width of psf [µm]');

    subplot(3,3,7)
    hist(wz./wx, histgammarange);
    xlabel('axial ratio w_z/w_x');
    ylabel('frequency');
    text(0.5,0.94,['avg(gamma_zx) = ' num2str(mean(wz./wx))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.87,['median(gamma_zx) = ' num2str(median(wz./wx))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.8,['SD(gamma_zx) = ' num2str(std(wz./wx))  ], 'Units', 'normalized', 'FontSize', 6);
    subplot(3,3,8)
    hist(wz./wy, histgammarange);
    xlabel('axial ratio w_z/w_y');
    ylabel('frequency');
    text(0.5,0.94,['avg(gamma_zy) = ' num2str(mean(wz./wy))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.87,['median(gamma_zy) = ' num2str(median(wz./wy))  ], 'Units', 'normalized', 'FontSize', 6);
    text(0.5,0.8,['SD(gamma_zy) = ' num2str(std(wz./wy))  ], 'Units', 'normalized', 'FontSize', 6);


    dataout.wx=wx;
    dataout.wy=wy;
    dataout.wz=wz;
    dataout.used_psf_positions=psf_positions(idx,:);



    saveas(h, [output_prefix '.psfhistogram.pdf']);
    save([output_prefix '.psfhistogram.mat'], 'dataout');
    M=[psf_positions(idx,1)';psf_positions(idx,2)';psf_positions(idx,3)';wx';wy';wz']';
    dlmwrite([output_prefix '.psfwidths.dat'], M);

    output_args=dataout;

end

