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

function [ output_args ] = perform_evalxzstack( data, psf_positions, output_prefix, varargin )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

    optargin = size(varargin,2);

    datarange=[];
    deltaz = 1000;         % longitudinal pixel-size in nanometers
    deltaxy = 200;       % lateral pixelsize in nanometers
    deltapixel=5;
    pixsizexy = 412;
    stepsxy=26;
    dirpixels=[-1 0];
    maxfunevals=3000;
    maxiter=1000;
    showsinglebeadplots=true;
    showeveryNthSinglebeadPlot=1;
    hasfitxy_pixelwidth=false;
    
    fitmodel_xy='gaussian';
    fitxy_pixelwidth=deltaxy;

    maxxwidth=2500;
    maxywidth=2500;
    maxzwidth=2500;
    histgammarange=[0.1 6];
    histgammabinwidth=0.1;
    histwidthxyrange=[400 1500];
    histwidthzrange=[400 1500];
    histwidthxybinwidth=10;
    histwidthzbinwidth=10;
    

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
        elseif (strcmp(name, 'histgammabinwidth') )
            histgammabinwidth=val;
        elseif (strcmp(name, 'histwidthxybinwidth') )
            histwidthxybinwidth=val;
        elseif (strcmp(name, 'histwidthzbinwidth') )
            histwidthzbinwidth=val;
        elseif (strcmp(name, 'deltaz') )
            deltaz=val;
        elseif (strcmp(name, 'deltaxy') )
            deltaxy=val;
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
        elseif (strcmp(name, 'pixsizexy') )
            pixsizexy=val;  
        elseif (strcmp(name, 'stepsxy') )
            stepsxy=val;  
        elseif (strcmp(name, 'dirpixels') )
            dirpixels=val;  
        elseif (strcmp(name, 'deltapixel') )
            deltapixel=val;  
        elseif (strcmp(name, 'fitmodel_xy') )
            fitmodel_xy=val;  
        elseif (strcmp(name, 'fitxy_pixelwidth') )
            hasfitxy_pixelwidth=true;
            fitxy_pixelwidth=val;  
        elseif (strcmp(name, 'showsinglebeadplots') )
            showsinglebeadplots=val;
        elseif (strcmp(name, 'showeveryNthSinglebeadPlot') )
            showeveryNthSinglebeadPlot=val;
        else
            error(['unknown parameter "' name '" to perform_evalxzstack()']);
        end
    end
    
    if ~hasfitxy_pixelwidth
        fitxy_pixelwidth=deltaxy;
    end

    
    if length(histgammarange)==2
        histgammaX=histgammarange(1):histgammabinwidth:histgammarange(2);
    elseif length(histgammarange)>2
        histgammaX=histgammarange;
    else
        error('histgammarange has to have at least 2 entries');
    end
    
    if length(histwidthxyrange)==2
        histwidthxyX=histwidthxyrange(1):histwidthxybinwidth:histwidthxyrange(2);
    elseif length(histwidthxyrange)>2
        histwidthxyX=histwidthxyrange;
    else
        error('histwidthxyrange has to have at least 2 entries');
    end

    if length(histwidthzrange)==2
        histwidthzX=histwidthzrange(1):histwidthzbinwidth:histwidthzrange(2);
    elseif length(histwidthzrange)>2
        histwidthzX=histwidthzrange;
    else
        error('histwidthzrange has to have at least 2 entries');
    end
    

    

    [w,h,N]=size(data);
    if (isempty(datarange)) 
        datarange=1:N;
    end

    data=data(:,:,datarange);

    options = optimset('lsqcurvefit');
    options = optimset(options, 'MaxFunEvals',maxfunevals);
    options = optimset(options, 'MaxIter',maxiter);
    options = optimset(options, 'Display','off');

    
    datamin=min(min(min(data)));
    datamax=max(max(max(data)));
    
    %psf_positions=psf_positions+1;
    spsf=size(psf_positions);
    
    dataout.datarange=datarange;
    dataout.deltaz = deltaz;
    dataout.deltaxy = deltaxy;
    dataout.maxfunevals=maxfunevals;
    dataout.maxiter=maxiter;
    
    dataout.histgammarange=histgammarange;
    dataout.histgammabinwidth=histgammabinwidth;
    dataout.histwidthxyrange=histwidthxyrange;
    dataout.histwidthzrange=histwidthzrange;
    dataout.histwidthxybinwidth=histwidthxybinwidth;
    dataout.histwidthzbinwidth=histwidthzbinwidth;
    
    dataout.maxxwidth=maxxwidth;
    dataout.maxywidth=maxywidth;
    dataout.maxzwidth=maxzwidth;
    dataout.datamin=datamin;
    dataout.datamax=datamax;
    dataout.psf_positions=psf_positions;
    dataout.w=w;
    dataout.h=h;
    dataout.N=N;
    dataout.avgIntensity=mean(data(:));
    
    
    cnt=1;
    for cnt1=1:spsf(1)
        x=psf_positions(cnt1,1);
        y=psf_positions(cnt1,2);
        z=psf_positions(cnt1,3);
        %p=psf_positions(cnt1,:);
        datasize=size(data);
            
        disp(['LOOKING AT BEAD ' num2str(cnt1) ':   [' num2str(x) ', ' num2str(y) ', ' num2str(z) ']']);
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % cut bead series from the raw data array
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            pixcoords=[];
            pixcnt=1;
            zn=z;
            zcorrection=(deltapixel*pixsizexy-(stepsxy*deltaxy))/deltaxy;
            %disp(['   w=' num2str(w) ', h=' num2str(h) ', N=' num2str(N) ]);
            for xxx=1:floor(max(w,h)*2/deltapixel)
                xn=x+(xxx-1)*dirpixels(1)*deltapixel;
                yn=y+(xxx-1)*dirpixels(2)*deltapixel;
                for nnn=1:stepsxy
                    %disp(['   [' num2str(xn) ', ' num2str(yn) ', ' num2str(round(zn)) ']:']);
                    if (xn>0 && xn<=w && yn>0 && yn<=h && round(zn)<=N && round(zn)>0)
                        %disp(['        OK!']);
                        pixcoords(pixcnt,1)=xn;
                        pixcoords(pixcnt,2)=yn;
                        pixcoords(pixcnt,3)=round(zn);
                        pixcnt=pixcnt+1;
                    end
                    zn=zn+1;
                end
                
                zn=zn+zcorrection;
            end
            pixidxs=sub2ind(datasize, pixcoords(:,1), pixcoords(:,2), pixcoords(:,3));
            cut=double(data(pixidxs));
            cut=reshape(cut(1:(floor(length(cut)/stepsxy)*stepsxy)), stepsxy, floor(length(cut)/stepsxy));
            xvec=((1:stepsxy)-1)*deltaxy/1000;
            zvec=((1:size(cut,2))-1)*deltaz/1000;

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % collect some basic data on the current bead
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            beadout.position=[x y z];
            beadout.id=cnt1;
            beadout.psf=cut;
            beadout.x=xvec;
            beadout.z=zvec;
            beadout.pixcoords=pixcoords;

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % perform an x-fit to each cut through the bead
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            beadout.widths=[];
            if strcmp(fitmodel_xy, 'pixel')
                ff=@(p,x) fit_rectconvgauss1D([p(1) p(2) p(3) p(4) fitxy_pixelwidth],x);
                NaNFitResults=[NaN, NaN, NaN, NaN];
            else
                ff=@(p,x) fit_gauss1D([p(1) p(2) p(3) p(4)],x);
                NaNFitResults=[NaN, NaN, NaN, NaN];
            end
            for pcnt=1:size(cut,2)
                if sum(isnan(cut(:,pcnt)))+sum(isinf(cut(:,pcnt)))==0
                    [maxcut, maxcutI]=max(cut(:,pcnt));
                    p0=[max(cut(:,pcnt))-min(cut(:,pcnt)), xvec(maxcutI(1)), max(xvec)/10, min(cut(:,pcnt))];
                    p = lsqcurvefit(ff, double(p0), double(xvec), double(cut(:,pcnt)') ,[],[], options);
                    beadout.fitresults(pcnt,:)=p;
                    beadout.widths(pcnt)=p(3)*1000;
                else
                    beadout.widths(pcnt)=NaN;
                    beadout.fitresults(pcnt,:)=NaNFitResults;
                end
            end
            
            
            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % perform an z-fit through brightest pixel
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            [maxcut, maxcutI]=max(cut(:));
            [maxcutI, maxcutJ]=ind2sub(size(cut), maxcutI(1));
            p0=double([max(cut(maxcutI,:))-min(cut(maxcutI,:)), zvec(maxcutJ), max(zvec)/10, min(cut(maxcutI,:))]);
            useIndexes= ~isnan(double(cut(maxcutI,:)));
            p = lsqcurvefit(ff, double(p0), double(zvec(useIndexes)), double(cut(maxcutI,useIndexes)) ,[],[], options);
            beadout.zfitresults=p;
            beadout.zfitcut=cut(maxcutI,:);
            beadout.zfitcut_pos=[maxcutI maxcutJ];
            
            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % perform an xz-fit to the acquired cut
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            [ fx0, fsigmas, fangle, foffset, famplitude ]=performfit_gaussian2Dr(double(cut), 'w', [deltaxy, deltaz], 'anglerange', [-5/180*pi, 5/180*pi], 'sangle', 0, 'ssigmas', [max(300,min(beadout.widths)) 5*deltaz]);
            beadout.fx0=fx0;
            beadout.fsigmas=fsigmas;
            beadout.fangle=fangle;
            beadout.foffset=foffset;
            beadout.famplitude=famplitude;
            disp('   2D-fit: ');
            disp([' x0: [' num2str(fx0) ']']);
            disp([' sigmas: [' num2str(fsigmas) ']' ]);
            disp([' angle: ' num2str(fangle/pi*180) '°' ]);
            disp([' offset: ' num2str(foffset) ]);
            disp([' amplitude: ' num2str(famplitude) ]);
            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % create plots for this bead and save them. This also saves the
            % data of this bead as .mat-file!
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            if showsinglebeadplots && (mod(cnt-1, showeveryNthSinglebeadPlot)==0)

                xvecfine=((1:0.05:stepsxy)-1)*deltaxy/1000;
                zvecfine=((1:0.01:size(cut,2))-1)*deltaz/1000;
                hh=figure(1);
                subplot(4,2,1)
                imagesc(zvec*1000, xvec*1000, cut);
                colormap(Jet)
                hold on
                daspect([1 1 1]);
                xlabel('z [nm]');
                ylabel('x [nm]');
                set(gca,'YDir','normal');
                plot_gaussianprops_image( foffset, famplitude, fx0, fsigmas, fangle);
                title(['INTENSITY OF BEAD ' num2str(cnt1) ':   [' num2str(x) ', ' num2str(y) ', ' num2str(z) ']  2D gaussian widths: [' num2str(fsigmas(1)) ', ' num2str(fsigmas(2)) '] nm,   gamma=' num2str(fsigmas(2)/fsigmas(1))]);
                hold off

                subplot(4,2,2)
                imagesc(zvec*1000, xvec*1000, log(cut));
                colormap(Jet)
                hold on
                daspect([1 1 1]);
                xlabel('z [nm]');
                ylabel('x [nm]');
                set(gca,'YDir','normal');
                plot_gaussianprops_image( foffset, famplitude, fx0, fsigmas, fangle);
                title('log(intensity)');
                hold off

                subplot(4,2,3)
                for pcnt=1:size(cut,2)
                    col=[(pcnt-1)/(size(cut,2)), 0, 1-(pcnt-1)/(size(cut,2)-1)];
                    plot(xvec, cut(:,pcnt), '-', 'Color', col)
                    if (pcnt==1)
                        hold on;
                    end
                end
                title('x-cuts (blue: low z, red: high z)');
                xlabel('x [µm]');
                ylabel('intensity [A.U.]');
                hold off;
                subplot(4,2,4)
                for pcnt=1:size(cut,2)
                    col=[(pcnt-1)/(size(cut,2)), 0, 1-(pcnt-1)/(size(cut,2)-1)];
                    semilogy(xvec, cut(:,pcnt), '-', 'Color', col)
                    if (pcnt==1)
                        hold on;
                    end
                end
                title('log. x-cuts (blue: low z, red: high z)');
                xlabel('x [µm]');
                ylabel('intensity [A.U.]');
                hold off;
                subplot(4,2,5)
                for pcnt=1:size(cut,2)
                    if sum(isnan(cut(:,pcnt)))+sum(isinf(cut(:,pcnt)))==0
                        col=[(pcnt-1)/(size(cut,2)), 0, 1-(pcnt-1)/(size(cut,2)-1)];
                        plot(xvec, cut(:,pcnt), 'x:', 'Color', col)
                        if (pcnt==1)
                            hold on;
                        end
                        plot(xvecfine, ff(beadout.fitresults(pcnt,:),xvecfine), '-', 'Color', col)
                    end
                end
                title('x-cuts+fits (blue: low z, red: high z)');
                xlabel('x [µm]');
                ylabel('intensity [A.U.]');
                hold off;
                subplot(4,2,6);
                plot(zvec, beadout.zfitcut, 'rx:');
                hold on
                plot(zvecfine, ff(beadout.zfitresults, zvecfine), 'r-');
                hold off
                title(['z-cut+fit    width=' num2str(beadout.zfitresults(3)*1000) ' nm']);
                xlabel('z [µm]');
                ylabel('intensity [A.U.]');
            
                subplot(4,2,8)
                cla;
                plot(zvec, beadout.widths, 'r+:');
                xlabel('z [µm]');
                ylabel('width of gaussian [nm]');
                title(['width of gaussian   max. intensity width = ' num2str(beadout.widths(beadout.zfitcut_pos(2))) ' nm    (z=' num2str(zvec(beadout.zfitcut_pos(2))) 'µm)    gamma=' num2str(beadout.zfitresults(3)*1000/beadout.widths(beadout.zfitcut_pos(2)))]); 
                ylim(histwidthxyrange)
                
                drawnow;

                saveas(hh, [output_prefix '.psf' num2str(cnt) '.pdf']);
                saveas(hh, [output_prefix '.psf' num2str(cnt) '.fig']);
                save([output_prefix '.psf' num2str(cnt) '.mat'], 'beadout');
                
                reply = input('Fit OK? Y/N [N]: ', 's');
                if isempty(reply)
                    reply = 'N';
                end
                if (reply=='y' || reply == 'Y')
                    dataout.resulttable(cnt,:)=[cnt1,  x, y, z,  beadout.widths(beadout.zfitcut_pos(2)), beadout.zfitresults(3)*1000, beadout.zfitresults(3)*1000/beadout.widths(beadout.zfitcut_pos(2)), fsigmas(1), fsigmas(2), fsigmas(2)/fsigmas(1), famplitude-foffset];
                    cnt=cnt+1;
                end
            end

    end
          
    csvwrite([output_prefix '.fitresults.dat'], dataout.resulttable);
    
    hh=figure(22)
    subplot(3,3,1)
    hist(dataout.resulttable(:,5), 25);
    title(['distribution of 1D-minimum x-widths (N=' num2str(size(dataout.resulttable,1)) ')']);
    subplot(3,3,2)
    hist(dataout.resulttable(:,6), 25);
    title('distribution of 1D z-widths');
    subplot(3,3,3)
    hist(dataout.resulttable(:,7), 25);
    title('distribution of 1D gammas');
    

    subplot(3,3,4)
    hist(dataout.resulttable(:,8), 25);
    title('distribution of 2D x-widths');
    subplot(3,3,5)
    hist(dataout.resulttable(:,9), 25);
    title('distribution of 2D z-widths');
    subplot(3,3,6)
    hist(dataout.resulttable(:,10), 25);
    title('distribution of 2D gammas');

    subplot(3,3,7)
    plot(dataout.resulttable(:,11),dataout.resulttable(:,8), 'xr');
    xlabel('fit amplitude');
    ylabel('x-width');
    subplot(3,3,8)
    plot(dataout.resulttable(:,11),dataout.resulttable(:,9), 'xr');
    xlabel('fit amplitude');
    ylabel('y-width');
    subplot(3,3,9)
    plot(dataout.resulttable(:,11),dataout.resulttable(:,10), 'xr');
    xlabel('fit amplitude');
    ylabel('gamma');

    saveas(hh, [output_prefix '.histograms.pdf']);
    saveas(hh, [output_prefix '.histograms.fig']);
    save([output_prefix '.results.mat'], 'dataout');
  
    output_args=dataout;

end

